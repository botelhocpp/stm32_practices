# Instalando a Newlib

Para podermos usar funções rotineiras do C, como **printf** e **memcpy** precisamos adicionar a **C Standard Library (libc)** ao nosso projeto. Para isso adicioná-la a um projeto com ARM podemos usar a implementação desta biblioteca que vem instalada no nosso toolchain (o arm-none-eabi-gcc), isto é, a **Newlib** (ou sua versão otimizada, a **Newlib-Nano**).

- Libc (C Standard Library): É a especificação (o padrão ISO). Ela define quais funções devem existir (printf, memcpy, strcpy) e como elas devem se comportar.
- Newlib: É uma implementação específica dessa regra, feita sob medida para sistemas embarcados.

No Desktop (Linux), a implementação padrão chama-se Glibc (GNU Libc). Mas por que não usamos a Glibc no STM32? Se você tentar compilar um código usando a Glibc para um STM32, dois problemas acontecerão:

- Falta de Espaço: O código do printf da Glibc é tão complexo e cheio de buffers que provavelmente ocuparia mais memória Flash do que o seu microcontrolador tem inteira.
- Chamadas de Sistema (Syscalls): A Glibc tentaria chamar o kernel do Linux para escrever na tela (write syscall). Como não existe Linux no seu chip, o código travaria imediatamente.

A Newlib foi criada para resolver isso. Ela é modular. Ela implementa toda a lógica de strings (memcpy, strlen) de forma eficiente. Quando precisa falar com o hardware (ler arquivo, escrever na tela), ela para e chama uma função "stub" (toco) que você deve implementar. Posteriormente teremos inclusive que criar a função _write.

Você chama printf("Ola"). A Newlib formata o texto para um buffer. A Newlib chama _write passando esse buffer. O seu código pega o buffer e joga na USART.

Apesar disso, dentro do mundo embarcado, a Newlib ainda era considerada um pouco "inflada" para chips muito pequenos. Por isso criaram a Newlib-Nano (que ativamos com --specs=nano.specs).

- Newlib (Padrão): Otimizada para velocidade em embarcados.
- Newlib-Nano: Otimizada agressivamente para tamanho. Removeu o suporte a float no printf (por padrão) e usa algoritmos de formatação mais lentos, mas que economizam muita memória Flash.

A libc padrão assume que existe um Sistema Operacional (Linux/Windows) para cuidar de arquivos, memória e tela. Como não temos SO precisamos criar uma "ponte" chamada **Retargeting**, ou seja, uma implementação específica de algumas chamadas de sistemas (as syscalls).

## Ajustar as Flags do Compilador e do Linker

Precisamos dizer ao GCC para incluir a versão "Nano" da biblioteca (menor e mais rápida) e dizer que não temos chamadas de sistema padrão. Nos nossos Makefiles (geral e no do BSP) vamos adicionar algumas flags na compilação (CFLAGS) e na linkagem (LDFLAGS):

```
--specs=nano.specs --specs=nosys.specs
```

- **nano.specs**: Usa a Newlib-Nano (economiza MUITA Flash/RAM). Adicione ao CFLAGS de ambos os Makefiles.
- **nosys.specs**: Fornece implementações vazias para funções de sistema para o linker não reclamar (evita erros de "undefined reference to _exit", _sbrk, etc.).  Adicione apenas ao LDFLAGS do Makefile externo.

Lembre-se de remover a flag -nostdlib, pois ela exclui explicitamente o uso da biblioteca padrão.

## A Ponte para o Hardware (Retargeting)

Se tentarmos usar printf, o código vai compilar, mas não vai sair nada. A função printf formata o texto e depois chama uma função de baixo nível chamada _write. Logo, precisamos implementar _write para pegar os bytes e jogá-los na porta serial (ou seja, na USART).

Assim, vamos criar um arquivo novo (vou chamar de syscalls.c):

``` 
#define _GNU_SOURCE

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "usart.h"

#define STDIN   (0)
#define STDOUT  (1)
#define STDERR  (2)

int _write(int file, char *ptr, int len) {
    if (file == STDOUT || file == STDERR) {
        for (int i = 0; i < len; i++) {
            usart_write_blocking(USART1, ptr[i]);
        }
        return len;
    }
    
    errno = EBADF;

    return -1;
}

int _close(int file) { return -1; }

int _lseek(int file, int ptr, int dir) { return 0; }

int _read(int file, char *ptr, int len) { return 0; }

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) { return 1; }
```

Note que algumas funções são apenas "stubs" necessários para usar Newlib sem SO. Apenas para o linker ficar feliz, a maioria não faz nada.

## Inicialização da Libc

Em C, a função main não é a primeira coisa a rodar. Antes dela, o ambiente precisa ser preparado. O compilador GCC espera que certas inicializações aconteçam. Se você usar uma função da Newlib (como rand(), malloc() ou funções de tempo) que dependa de uma inicialização interna, e você não chamou o init_array, essa função vai acessar memória não inicializada. O resultado? HardFault aleatório.

Para inicializar a Libc precisamos chamar uma função especial. Ela quem vai inicializa a biblioteca C e construtores: 

```
__libc_init_array();
```

Ela percorre uma lista de ponteiros de função e executa um por um. Para isso, você precisa dizer ao Linker onde guardar a lista de funções que devem ser chamadas. Adicione estas seções (depois da seção .text ou .rodata, e antes da .data):

```
  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > FLASH
```

Devemos chamar a função após copiar o .data e zerar o .bss e antes de chamar a função main. Você também deve declarar o protótipo no topo e adicionar os stubs das funções _init e _fini ao syscalls.c (ou mantê-los no startup.c):

```
// [...]
extern void __libc_init_array(void);

void _init(void) { return; }

void _fini(void) { return; }

extern int main(void);

__attribute__((naked, noreturn))
void _start(void) {

    DISABLE_IRQ();

    /* Copy .data from Flash to SRAM */
    for (uint32_t *p_src = &_sidata, *p_dst = &_sdata; p_dst < &_edata; p_src++, p_dst++) {
        *p_dst = *p_src;
    }

    /* Zero .bss in SRAM */
    for (uint32_t *p_dst = &_sbss; p_dst < &_ebss; p_dst++) {
        *p_dst = 0;
    }

    __libc_init_array();

    ENABLE_IRQ();

    main();

    while(1);
}

// [...]
```

Por que devo adicionar estes dois aqui? Se manter -nostartfiles no Makefile, não será necessário, já que o compilador já irá definir vários arquivos de configuração (a CRT - C Runtime, crti.o, crtn.o, etc), incluindo essas funções. O problema é que o compilador pode incluir "coisa demais" para você desnecessariamente. Portanto, tiramos isso e definimos manualmente, o que não é grande trabalho!

## Modificações na Linkagem

Alguns erros podem acontecer, como as funções que estão no **syscalls.c** não sendo reconhecidas. Isso acontece por que as funções definidas em syscall.c são necessitadas muito depois da análise da biblioteca estática, fazendo com o que não sejam achados. Para resolver isso, adicione **--start-group** e **--end-group** ao Makefile, fazendo o Linker ficar rodando em loop entre a libbsp e a libc até resolver tudo. Portanto, no LDFLAGS adicione:

```
-Wl,--start-group -lbsp -lc -lgcc -Wl,--end-group
```

Outra coisa importante é que ao usar a flag ```--specs=nosys.specs``` é fornecido uma implementação padrão (e burra) da função _sbrk (usada pelo malloc interno do printf). Essa implementação padrão da libnosys assume que existe uma variável (símbolo) no seu Linker Script chamada exatamente end para saber onde a memória RAM livre começa (o início do Heap). O seu Linker Script não tem esse nome.

Abra o seu arquivo bsp/linker.ld. Você precisa adicionar PROVIDE(end = .); no final da memória usável:

```
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > SRAM


  .heap :
  {
    . = ALIGN(8);
    _mem_end = .;
    PROVIDE(end = .);
    _heap_base = .;
    . += HEAP_SIZE;
    _heap_limit = .;
  } > SRAM

  .stack :
  {
    . = ALIGN(8);
    . += STACK_SIZE;
  } > SRAM
```

Veja que definimos uma pilha (stack) e um heap, importante para podermos usar alocação dinâmica (necessária pelo printf)!

## Outros Pontos Importantes

Por padrão, a nano.specs desabilita suporte a %f no printf para economizar memória. Se precisar imprimir floats, adicione esta flag ao Linker (LDFLAGS):

```
-u _printf_float
```

Se você quiser usar alocação dinâmica (malloc), você precisa implementar a função **_sbrk** no syscalls.c e definir onde fica o Heap no seu Linker Script (.ld). Em sistemas embarcados pequenos, evite malloc se puder. Use buffers estáticos ou globais. Usar malloc traz riscos de fragmentação e stack overflow.

## Apêndice A: O Recurso "Constructor" do GCC

Mesmo em C puro (sem C++), você pode criar funções que rodam automaticamente antes do main. Isso é excelente para drivers que precisam se "auto-registrar".

```
// Esta função roda SOZINHA antes do main... 
// MAS SÓ SE você tiver o __libc_init_array configurado!
void __attribute__((constructor)) meu_driver_init(void) {
    // Configura algo crítico
}

int main() {
    // Aqui, meu_driver_init já rodou.
}
```

Sem o __libc_init_array, o atributo constructor é ignorado silenciosamente.

Se amanhã você decidir mudar a extensão do arquivo para .cpp para usar uma classe, e declarar: LED led_status; (como variável global), o construtor dessa classe jamais será chamado. Seu objeto será um bloco de bytes zerados inerte.
