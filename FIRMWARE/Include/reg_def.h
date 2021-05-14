#pragma once
// Mask corresponds to the address bus width, in bytes (15 bits -> 7FFF)
//#define REG_MASK 0x7FFF
#define REG_MASK 0xFFFFFF

// Bits 30:28 define the address span of the block (4 -> 2^4 32-bit words)
// Other bits define the start address
// 0x100000 <=> 1Mword or 4MB
// !!!!!!!!!!!!!!!!!!!!!!!!!
// Due to signed integer format, 4 lsbs lead to a negative number when 4lsbs > 7
// Because of integer division , expected value on blocks is +1 !!!
// So BFFFE000 -> actually 12 bits, not 11
#define CSR_IRQ_REGS BFFFE000
#define SPI_MST_REGS 4FFFFA00
#define SPI_REGS 4FFFFA00
#define SPI_TB_REGS 4FFFFA10
#define I2C_REGS 4FFFFA20
#define I2C_TB_REGS 4FFFFA30
#define UART_TB_REGS 4FFFFEF0
#define DMA_REGS     4FFFFFD0
#define UART_REGS    1FFFFFF0
#define HDMI_REGS    2FFFFFFE
#define SDRAM_REGS   2F100000
