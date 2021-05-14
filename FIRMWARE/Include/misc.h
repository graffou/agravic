
#include <stdint.h>

#include "reg_def.h" // HW description (peripherals addresses and address span)

#define BIT_REVERSE(a) ( ((a&0x80)>>7) | ((a&0x40)>>5) | ((a&0x20)>>3) | ((a&0x10)>>1) | ((a&8)<<1) | ((a&4)<<3) | ((a&2)<<5) | ((a&1)<<7) )
// For rtl addr -> c addr conversion
#define CONCAT(a,b) a ## b
#define CONV_ADDR(a) (((CONCAT(0x,a)<<2)&REG_MASK) | 0x40000000)

#define regs (*( struct reg_test_t *)0x40005f00) 

#if 0

volatile uint32_t *dbg = (volatile uint32_t *) 0x40007ff8; // 1ffe

volatile uint32_t *var = (volatile uint32_t *) 0x40005ffc;
volatile uint32_t *timer = (volatile uint32_t *) 0x40005ff0;
// simple uart register:
// Write: 16b(division_factor) 7b(0) 1b(rx_not_tx) 8b(tx_byte)
// Read:  16b(division_factor) 7b(0) 1b(ready)     8b(rx_byte) 
volatile uint32_t *uart = (volatile uint32_t *) 0x40007fc0;
volatile uint32_t *spi_conf = (volatile uint32_t *) 0x40007f40;
volatile uint32_t *spi_data = (volatile uint32_t *) 0x40007f44;
volatile uint32_t *spi_conf_slv = (volatile uint32_t *) 0x40007f00;
volatile uint32_t *spi_data_slv = (volatile uint32_t *) 0x40007f04;
// second uart is included in testbench 
// configure it with a -0x400 (or 0x100 in 4B words) offset 
// this verification hack is found in tb.h
volatile uint32_t *tb_uart = (volatile uint32_t *) 0x40007Bc0;

volatile uint32_t *dma = (volatile uint32_t *) 0x40007f80;
#else


volatile uint32_t *uart = (volatile uint32_t *) CONV_ADDR(UART_REGS);
//volatile uint32_t *uart = (volatile uint32_t *) 0x40007fc0;

volatile uint32_t *dbg = (volatile uint32_t *) CONV_ADDR(HDMI_REGS); // 1ffe
//volatile uint32_t *dbg = (volatile uint32_t *) 0x40007ff8; // 1ffe

volatile uint32_t *var = (volatile uint32_t *) 0x40005ffc;
volatile uint32_t *timer = (volatile uint32_t *) 0x40005ff0;
// simple uart register:

volatile uint32_t *spi_conf = (volatile uint32_t *) CONV_ADDR(SPI_MST_REGS);
volatile uint32_t *spi_data = (volatile uint32_t *) CONV_ADDR(SPI_MST_REGS)+1;//0x40007f44;
volatile uint32_t *spi_tb_conf = (volatile uint32_t *) CONV_ADDR(SPI_TB_REGS);//0x40007f00;
volatile uint32_t *spi_tb_data = (volatile uint32_t *) CONV_ADDR(SPI_TB_REGS)+1;//0x40007f04;

volatile uint32_t *i2c_conf = (volatile uint32_t *) CONV_ADDR(I2C_REGS);
volatile uint32_t *i2c_data = (volatile uint32_t *) CONV_ADDR(I2C_REGS)+1;//0x40007f44;
volatile uint32_t *i2c_tb_conf = (volatile uint32_t *) CONV_ADDR(I2C_TB_REGS);//0x40007f00;
volatile uint32_t *i2c_tb_data = (volatile uint32_t *) CONV_ADDR(I2C_TB_REGS)+1;//0x40007f04;

// second uart is included in testbench
// configure it with a -0x400 (or 0x100 in 4B words) offset
// this verification hack is found in tb.h
volatile uint32_t *tb_uart = (volatile uint32_t *) CONV_ADDR(UART_TB_REGS);//0x40007Bc0;

volatile uint32_t *dma = (volatile uint32_t *) CONV_ADDR(DMA_REGS);

#endif

// Adding 3 NOPs avoids reworking the CPU pipe-line
// Not clean, but very small overhead on SW Vs. complicating the HW
// temporary fix. Should not be needed with actual IT traps (which flushes the pipe-line)
// With actual IRQ, it seems two is enough.
#define __WFI__ asm("wfi");asm("nop"); asm("nop"); asm("nop");
#define MIT_ENABLE csrrs_noread(CSR_MSTATUS, (1<<MSTATUS_MIE));
#define MIT_DISABLE csrrc_noread(CSR_MSTATUS, (1<<MSTATUS_MIE));
#define RESTORE_IRQ csrrc_noread(CSR_MSTATUS, 0); // This reenables IRQs inside an ISR
#define IT_ENABLE(val) csrrs_noread(CSR_MIE, 1<<val);
#define IT_DISABLE(val) csrrc_noread(CSR_MIE, 1<<val);
#define IT_PENDING(val) ( csrr(CSR_MIP, val) != 0 )
#define IT_CLEAR(val) csrrc_noread(CSR_MIP, (1<<val));
#define SET_TRAP_VECTOR(val) csrrw_noread(CSR_MTVEC, (val | 1) ); // use vectored trap addresses for ITs

#define CLK_FIXED_FREQ_HZ (50ULL * 1000 * 1000)
#define CYCLES_PER_MS (000ULL)

#define CSR_MSTATUS		0x300
#define CSR_MISA		0x301
#define CSR_MIE			0x304
#define CSR_MTVEC		0x305
#define CSR_MSCRATCH		0x340
#define CSR_MEPC		0x341
#define CSR_MCAUSE		0x342
#define CSR_MTVAL		0x343
#define CSR_MIP			0x344
//#define CSR_PMPCFG0		0x3a0
//#define CSR_PMPADDR0		0x3b0
#define CSR_MHARTID		0xf14
// Not in RISC-V spec (timer registers)- GIORNO core specific
#define CSR_MTIME		0xFC0
#define CSR_MTIMEH		0xFC1
#define CSR_MTIMECMP		0xBF0
#define CSR_MTIMECMPH		0xBF1

// define IT enable / status bits
#define MTIME_IT 7

// global IT enable
#define MSTATUS_MIE 3

// Read CSRs through return value
__attribute__((always_inline)) inline uint32_t csrr(uint32_t csr_num)  {
    volatile int result;
    asm volatile ("csrr %0, %1" : "=r"(result) : "i"(csr_num));
    return result; }

// To read/write CSRs using a unique variable - No cycle nor register overhead for read, since csrr* can do that
__attribute__((always_inline)) inline void csrrw(uint32_t csr_num, uint32_t& val)  {
    asm volatile ("csrrw %0, %1, %2" : "=r"(val) : "i"(csr_num), "r"(val) );
     }
__attribute__((always_inline)) inline void csrrs(uint32_t csr_num, uint32_t& val)  {
    asm volatile ("csrrs %0, %1, %2" : "=r"(val) : "i"(csr_num), "r"(val) );
     }
__attribute__((always_inline)) inline void csrrc(uint32_t csr_num, uint32_t& val)  {
    asm volatile ("csrrc %0, %1, %2" : "=r"(val) : "i"(csr_num), "r"(val) );
     }

// To write CSRs only
__attribute__((always_inline)) inline void csrrw_noread(uint32_t csr_num, uint32_t val)  {
    asm volatile ("csrrw x0, %0, %1" :  : "i"(csr_num), "r"(val) );
     }
__attribute__((always_inline)) inline void csrrs_noread(uint32_t csr_num, uint32_t val)  {
    asm volatile ("csrrs x0, %0, %1" :  : "i"(csr_num), "r"(val) );
     }
__attribute__((always_inline)) inline void csrrc_noread(uint32_t csr_num, uint32_t val)  {
    asm volatile ("csrrc x0, %0, %1" :  : "i"(csr_num), "r"(val) );
     }
// 32-bit time 
__attribute__((always_inline)) inline uint32_t time32() {
    return(csrr(CSR_MTIME));  
}

// 64-bit time !!! Dangerous because not protected against CSR_TIME wrap
__attribute__((always_inline)) inline uint64_t time() {
    return(uint64_t(csrr(CSR_MTIMEH)) << 32 | csrr(CSR_MTIME));  
}

/**
 * Delay loop executing within 8 cycles on ibex
 */
#if 0
static void delay_loop_ibex(unsigned long loops) {
  int out; /* only to notify compiler of modifications to |loops| */
  asm volatile(
      "1: nop             \n" // 1 cycle
      "   nop             \n" // 1 cycle
      "   nop             \n" // 1 cycle
      "   nop             \n" // 1 cycle
      "   addi %1, %1, -1 \n" // 1 cycle
      "   bnez %1, 1b     \n" // 3 cycles
      : "=&r" (out)
      : "0" (loops)
  );
}
#else
static void wait_cycles(unsigned long loops) {
	MIT_DISABLE
	uint64_t cur_time;
	cur_time = csrr((CSR_MTIME));
	cur_time |= (uint64_t(csrr(CSR_MTIMEH)) << 32);
	cur_time += uint64_t(loops);
    csrrw_noread(CSR_MTIMECMPH, cur_time >> 32);
    csrrw_noread(CSR_MTIMECMP, cur_time);
    SET_TRAP_VECTOR(0x4)
    MIT_ENABLE
    IT_ENABLE(1<<MTIME_IT)

    __WFI__
	//IT_CLEAR(1<<MTIME_IT)
	//IT_DISABLE(1<<MTIME_IT)
    MIT_DISABLE


}
#endif

// quick conversion to cycles w/o multiplier
static int usleep(unsigned long n) {
	  unsigned long cycles;
	  cycles = (n << 5) + (n << 4) ;

	  wait_cycles(cycles);
	  return 0;
}

// quick conversion to cycles w/o multiplier
static int nsleep(unsigned long n) {
	  unsigned long cycles;
	  cycles = ((n << 5) + (n << 4)) >> 10 ;

	  wait_cycles(cycles);
	  return 0;
}

template <class T, class ...Args>
void recurse_print(const char* toto, const T& first, const Args&... args);

void (*callback)();

extern "C" void timer_irq_handler() __attribute__ ((interrupt ("machine"), nested));

extern "C" void timer_irq_handler()
{

	// clear IRQ by setting timecmp value >> current timer
	csrrw_noread(CSR_MTIMECMPH, 0xFFFFFFFF);
	//csrrw_noread(CSR_MTIMECMP, cur_time);
	IT_DISABLE(1<<MTIME_IT)
		    //RESTORE_IRQ // reenables active IRQs after register save
	//recurse_print("#RIn timer_irq_handler routine %", '!');
	if (callback)
		(*callback)();
	return;
}

extern "C" void dma_irq_handler() __attribute__ ((interrupt ("machine"), nested));

extern "C" void dma_irq_handler()
{
	IT_DISABLE(1<<MTIME_IT)
	recurse_print("#RIn timer_irq_handler routine %", '!');
	if (callback)
		(*callback)();
	return;
}


