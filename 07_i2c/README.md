# Driver OLED SSD1306 I2C (STM32F103 - Bare Metal)

## 📌 Contexto
Implementação de driver I2C "Bare Metal" para display OLED SSD1306 usando STM32F103 (Blue Pill).
- **Periférico:** I2C1
- **Pinos (Remapeados):** SCL = PB8, SDA = PB9.
- **Abordagem:** Controle direto de registradores (sem HAL).

## ⚠️ Problemas Enfrentados & Soluções

Durante o desenvolvimento, o sistema travava na função `i2c_start` ou apresentava glitches visuais após resets consecutivos. Abaixo estão as causas raiz e correções.

### 1. Travamento no Start (Bus Lock-up)
**Sintoma:** O código travava no loop `while` esperando a flag `SB` (Start Bit), pois o hardware detectava o barramento como `BUSY`.
**Causa:** Ao resetar o STM32 durante uma transmissão, o OLED (que permanece ligado) continuava segurando a linha **SDA em LOW** (esperando clock). O STM32 reiniciava, via SDA em 0V e bloqueava o hardware I2C.
**Solução:** Implementação de uma rotina de **"Bus Recovery"** via Bit-Banging antes da inicialização do hardware.
* Configura pinos como GPIO Output Open-Drain.
* Gera pulsos manuais no SCL até o escravo soltar o SDA.
* Gera um STOP condition manual.

### 2. Flag BUSY "Fantasma" (STM32 Silicon Errata)
**Sintoma:** Mesmo com o barramento livre, o registrador `SR2` indicava `BUSY` após resets rápidos, impedindo o Start.
**Solução:** Reset nuclear do periférico via RCC.
* Uso do `RCC->APB1RSTR` dentro da função `i2c_init` para resetar a máquina de estados interna do silício I2C.

### 3. Falta de Pull-Ups (Hardware)
**Sintoma:** Travamento imediato.
**Causa:** Ao contrário de PB6/PB7, os pinos **PB8/PB9 não possuem resistores de pull-up** na placa Blue Pill. O I2C (Open-Drain) requer pull-ups externos.
**Solução:** Adição de resistores de 4.7kΩ entre 3.3V e as linhas SCL/SDA.

### 4. Glitch Visual (Memória do OLED)
**Sintoma:** Após resetar a placa, o OLED desenhava em colunas erradas ou com rotação estranha.
**Causa:** O SSD1306 mantém sua configuração interna (Ponteiros de RAM e Modo de Endereçamento) enquanto estiver energizado.
**Solução:** Forçar a reconfiguração completa no `oled_init`:
* Setar Memory Addressing Mode (`0x20`) para Horizontal (`0x00`).
* Resetar ponteiros de coluna (`0x21`) e página (`0x22`) para cobrir a tela toda (0-127, 0-7).

---

## 🛠️ Snippets Críticos

### 1. Rotina de Recuperação (Bus Recovery)
Deve ser chamada **antes** de habilitar o periférico I2C.

```c
void i2c_reset_bus(i2c_handle_t *i2c) {
    // 1. Desabilita o periférico I2C (Libera pinos)
    i2c->bus->CR1 &= ~I2C_CR1_PE;

    // 2. Configura como GPIO Manual (Open-Drain)
    gpio_init(i2c->scl, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_GP_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);
    gpio_init(i2c->sda, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_GP_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);

    // 3. Sequência de pulsos para destravar SDA
    gpio_write(i2c->sda, 1);
    for(int i = 0; i < 12; i++) { // Bate o clock 12 vezes
        gpio_write(i2c->scl, 0);
        delay_us(5);
        gpio_write(i2c->scl, 1);
        delay_us(5);
    }

    // 4. STOP Condition Manual
    gpio_write(i2c->sda, 0);
    delay_us(5);
    gpio_write(i2c->scl, 1);
    delay_us(5);
    gpio_write(i2c->sda, 1);
}
```

### 2. Inicialização do Periférico (Com RCC Reset)


```
void i2c_init(i2c_handle_t *i2c) {
    // 1. Reset do bloco I2C no RCC (Limpa flags presas)
    if (i2c->bus == I2C1) {
        RCC->APB1RSTR |=  (1 << 21);
        delay(100);
        RCC->APB1RSTR &= ~(1 << 21);
    }

    // 2. Configura Pinos para Modo Hardware (Alternate Function)
    gpio_init(i2c->scl, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_AF_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);
    gpio_init(i2c->sda, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_AF_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);

    // 3. Configura Timings (Ex: 36MHz PCLK1)
    i2c->bus->CR1 &= ~I2C_CR1_PE; // Garante desligado
    i2c->bus->CR2 = 36;
    i2c->bus->CCR = 180; // 100kHz
    i2c->bus->TRISE = 37;
    i2c->bus->CR1 |= I2C_CR1_PE;  // Habilita
}
```

### 3. Fluxo de Inicialização Seguro
Lógica de retry no main para evitar travamento infinito.

```
void init_display_safe(void) {
    afio_remap(AFIO_PERIPHERAL_I2C1, AFIO_REMAP);
    
    // Limpeza física inicial
    i2c_reset_bus(&i2c_bus);
    i2c_init(&i2c_bus);

    // Tenta inicializar o OLED com retries
    uint8_t retries = 5;
    while(retries--) {
        if(oled_init(&i2c_bus)) break; // Sucesso
        
        // Falhou? Tenta recuperar e re-iniciar
        i2c_reset_bus(&i2c_bus);
        i2c_init(&i2c_bus);
    }
}
```
