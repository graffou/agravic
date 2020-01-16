#define BIT_REVERSE(a) ( ((a&0x80)>>7) | ((a&0x40)>>5) | ((a&0x20)>>3) | ((a&0x10)>>1) | ((a&8)<<1) | ((a&4)<<3) | ((a&2)<<5) | ((a&1)<<7) )
#define regs (*( struct reg_test_t *)0x40005f00) 
volatile uint32_t *dbg = (volatile uint32_t *) 0x40005ff8; 

volatile uint32_t *var = (volatile uint32_t *) 0x40005ffc;
volatile uint32_t *timer = (volatile uint32_t *) 0x40005ff0;
// simple uart register:
// Write: 16b(division_factor) 7b(0) 1b(rx_not_tx) 8b(tx_byte)
// Read:  16b(division_factor) 7b(0) 1b(ready)     8b(rx_byte) 
volatile uint32_t *uart = (volatile uint32_t *) 0x40007fc0;

// second uart is included in testbench 
// configure it with a -0x400 (or 0x100 in 4B words) offset 
// this verification hack is found in tb.h
volatile uint32_t *tb_uart = (volatile uint32_t *) 0x40007Bc0;

volatile uint32_t *dma = (volatile uint32_t *) 0x40007f80;


#define CLK_FIXED_FREQ_HZ (50ULL * 1000 * 1000)


/**
 * Delay loop executing within 8 cycles on ibex
 */
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
static int usleep_ibex(unsigned long usec) {
  unsigned long usec_cycles;
  usec_cycles = CLK_FIXED_FREQ_HZ * usec / 1000 / 1000 / 8;

  delay_loop_ibex(usec_cycles);
  return 0;
}
static int usleep(unsigned long usec) {
  return usleep_ibex(usec);
}
