// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <string.h>


#define CLK_FIXED_FREQ_HZ (50ULL * 1000 * 1000)

volatile uint32_t *dbg = (volatile uint32_t *) 0x40005ff8; 
int _write(int32_t file, uint8_t *ptr, int32_t len)
{
  
   // return # of bytes written - as best we can tell
   //for (unsigned int i = 0; i < len; i++)
   while(*ptr != 0)
   {
    *dbg = *ptr;
    ptr++;
   }
   return (true);
}

void printf(char str[])
{
    _write(0, reinterpret_cast<uint8_t*>(str), sizeof(str));
}

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

uint32_t toto[4] = {0xF0F0F0F0, 0x0F0F0F0F, 0xAAAAAAAA, 0x55555555};


struct titi
{
    uint32_t val = 0xF3FC0C3F;
    uint32_t run(uint32_t n)
    {
        val = (val << n) | (val >> (32 - n) );
        return val;
    }

};

int main(int argc, char **argv) {
  // Any data written to the stack segment will connect the lowest four bits to
  // the board leds
  volatile uint32_t *var = (volatile uint32_t *) 0x40005ffc;
  uint8_t cnt = 0;
  *var = 0xffffffff; 
  titi tata;  

  printf("Giorno core running\n");    

  while (1) {
    usleep(1); // 1000 ms
    //std::cerr << var;
    //*var = ~(*var);
    if (cnt & 0x4)
    {
        printf("Applying array value\n");
        *var = toto[cnt&3];
    }
    else
    {
        printf("Applying value from class method\n");       
        *var = tata.run(4);
    }
    cnt++;

  }
}
