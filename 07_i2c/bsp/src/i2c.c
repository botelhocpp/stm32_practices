#include "i2c.h"

#define I2C_CR1_PE          0x0001  // Peripheral Enable
#define I2C_CR1_SMBUS       0x0002  // SMBus Mode
#define I2C_CR1_START       0x0100  // Start Generation
#define I2C_CR1_STOP        0x0200  // Stop Generation
#define I2C_CR1_ACK         0x0400  // Acknowledge Enable
#define I2C_CR1_POS         0x0800  // Acknowledge/PEC Position (para recepção)
#define I2C_CR1_SWRST       0x8000  // Software Reset
#define I2C_CR2_FREQ_MASK   0x003F  // Frequência do periférico (bits 0-5)
#define I2C_CR2_ITERREN     0x0100  // Error Interrupt Enable
#define I2C_CR2_ITEVTEN     0x0200  // Event Interrupt Enable
#define I2C_CR2_ITBUFEN     0x0400  // Buffer Interrupt Enable
#define I2C_SR1_SB          0x0001  // Start Bit (Start condition gerado)
#define I2C_SR1_ADDR        0x0002  // Address sent (Endereço enviado com sucesso)
#define I2C_SR1_BTF         0x0004  // Byte Transfer Finished (Byte saiu totalmente)
#define I2C_SR1_ADD10       0x0008  // 10-bit header sent
#define I2C_SR1_STOPF       0x0010  // Stop detection
#define I2C_SR1_RXNE        0x0040  // Data Register not Empty (Tem dado para ler)
#define I2C_SR1_TXE         0x0080  // Data Register Empty (Pode escrever novo dado)
#define I2C_SR1_BERR        0x0100  // Bus Error
#define I2C_SR1_ARLO        0x0200  // Arbitration Loss
#define I2C_SR1_AF          0x0400  // Acknowledge Failure (NACK recebido)
#define I2C_SR2_MSL         0x0001  // Master/Slave (1=Master, 0=Slave)
#define I2C_SR2_BUSY        0x0002  // Bus Busy (1=Ocupado)
#define I2C_SR2_TRA         0x0004  // Transmitter/Receiver (1=Tx, 0=Rx)
#define I2C_CCR_FS          0x8000  // I2C Master Mode Selection (0=Standard, 1=Fast)
#define I2C_CCR_DUTY        0x4000  // Fast Mode Duty Cycle
#define I2C_CCR_CCR_MASK    0x0FFF  // Valor do clock

#define WAIT_CYCLES (100)
#define WAIT()      for(volatile int i = 0; i < WAIT_CYCLES; i++) NOP()

static void i2c_sw_reset(i2c_handle_t *i2c) ;

void i2c_init(i2c_handle_t *i2c) {
    i2c_sw_reset(i2c);

    /* Init SCL and SDA pins as open-drain output */
    gpio_init(i2c->scl, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_AF_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);
    gpio_init(i2c->sda, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_AF_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);

    /* Disable I2C to config */
    i2c->bus->CR1 &= ~I2C_CR1_PE;

    // 2. Configura a frequência do clock de entrada (PCLK1) no CR2
    // Assumindo PCLK1 de 36MHz (comum no STM32F1). Ajuste conforme seu clock!
    // Bits 0-5 do CR2 definem a frequência em MHz.
    i2c->bus->CR2 = 36; 

    // 3. Configura o CCR (Clock Control Register)
    // Para 100kHz (Standard Mode) com PCLK1 de 36MHz:
    // Thigh = Tlow = 5us. 
    // CCR = 5us / (1/36MHz) = 180 (0xB4)
    i2c->bus->CCR = 180;

    // 4. Configura o TRISE (Maximum Rise Time)
    // Para 100kHz, TRISE = (1000ns / Tpclk1) + 1
    // TRISE = (1us / 27.7ns) + 1 = 36 + 1 = 37
    i2c->bus->TRISE = 37;

    // 5. Habilita o periférico (PE = 1)
    i2c->bus->CR1 |= I2C_CR1_PE;
}

bool i2c_start(i2c_handle_t *i2c) {
    uint32_t timeout = 100000;
    while(i2c->bus->SR2 & I2C_SR2_BUSY) {
        if (--timeout == 0) {
            // Se der timeout aqui, o barramento travou.
            // A solução seria chamar i2c_recover_and_init() novamente aqui.
            return false; 
        }
    }

    // Habilita o bit de ACK (necessário para receber dados, opcional aqui mas boa prática)
    i2c->bus->CR1 |= I2C_CR1_ACK;

    // Gera o START condition
    i2c->bus->CR1 |= I2C_CR1_START;

    // 3. Espera o Start Bit (SB) com Timeout
    timeout = 100000; 
    while (!(i2c->bus->SR1 & I2C_SR1_SB)) {
        if (--timeout == 0) {
            return false; // Falha grave no hardware
        }
    }

    return true;
}

void i2c_stop(i2c_handle_t *i2c) {
    // Gera o STOP condition
    i2c->bus->CR1 |= I2C_CR1_STOP;
    
    // O bit é limpo automaticamente pelo hardware
}

// Retorna true se sucesso (ACK), false se falha (NACK/Timeout)
bool i2c_write_byte(i2c_handle_t *i2c, uint8_t byte) {
    i2c->bus->DR = byte;

    while (1) {
        uint16_t sr1 = i2c->bus->SR1;

        // 1. VERIFICAÇÃO DE ERRO (O Pulo do Gato)
        // Se recebermos NACK (AF), geramos STOP e saímos para não travar
        if (sr1 & I2C_SR1_AF) {
            i2c->bus->CR1 |= I2C_CR1_STOP; // Envia Stop
            i2c->bus->SR1 &= ~I2C_SR1_AF;  // Limpa flag de erro
            return false;              // Retorna erro
        }

        // 2. Se enviou Endereço (ADDR)
        if (sr1 & I2C_SR1_ADDR) {
            (void)i2c->bus->SR2; // Limpa flag ADDR
            return true;
        }

        // 3. Se enviou Dado (TXE ou BTF)
        if ((sr1 & I2C_SR1_TXE) || (sr1 & I2C_SR1_BTF)) {
            return true;
        }
    }
}

void i2c_reset_bus(i2c_handle_t *i2c) {
    /* Disable I2C */
    i2c->bus->CR1 &= ~I2C_CR1_PE;

    /* Config SCL and SDA pins as open-drain output */
    gpio_init(i2c->scl, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_GP_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);
    gpio_init(i2c->sda, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_GP_OUTPUT_OPEN_DRAIN, DONT_USE_PULL);

    /* Pull SDA up */
    gpio_write(i2c->sda, HIGH);

    /* Forces I2C slave to release SDA */
    for(int i = 0; i < 12; i++) {
        gpio_toggle(i2c->scl);
        WAIT();
    }

    /* Generate a manual STOP condition */
    gpio_write(i2c->sda, LOW);
    WAIT();
    gpio_write(i2c->scl, HIGH);
    WAIT();
    gpio_write(i2c->sda, HIGH);
}

static void i2c_sw_reset(i2c_handle_t *i2c) {
    if (i2c->bus == I2C1) {
        RCC->APB1RSTR |=  (1 << 21);
        WAIT();
        RCC->APB1RSTR &= ~(1 << 21);
    }
}
