// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <string.h>



#define CLK_FIXED_FREQ_HZ (50ULL * 1000 * 1000)

struct reg_test_t
{
	

	uint32_t en	:1;
	uint32_t en1 	:1;
	uint32_t en2 	:2;
	uint32_t en3 	:1;
	uint32_t 	:8;
	uint32_t	parm1:1;
	uint32_t parm2:5;
	uint32_t parm3:8;
	//uint32_t 	:16;

	volatile uint32_t parm4	:14;
	uint32_t 	:2;
	uint32_t en4 	:1;
	uint32_t			:7;
	uint32_t en5 	:1;
	uint32_t			:7;


} ;




#define INT32_TYPE 0x00000100
#define INT16_TYPE 0x00000200
#define INT8_TYPE 0x00000400
#define UINT32_TYPE 0x80000100
#define UINT16_TYPE 0x80000200
#define UINT8_TYPE 0x80000400
#define BOOL_TYPE 0x80000800
#define COLOR_TYPE 0xFFFFFF00
#define END_PRINT 0x88000000


#ifdef LINUX //----------------------------------------
// To debug this code in linux env.
uint32_t* var = new uint32_t;
reg_test_t regs;
void usleep(int x){};
#include <iostream>
#include <fstream>
std::ofstream f("dbg_file", std::ios::out);
struct dummy
{


    template<class T>
    void operator=(T x)
    {
        f << x;
    }
};
dummy* dbg = new dummy;

#else //----------------------------------------
// Embedded code

// Interesting:
// If volatile, CPU reads before write a second time, making register access slower
// Then, status registers should be volatile, configuration registers should not.
//#define regs (*(volatile struct reg_test_t *)0x40005f00) 
#define regs (*( struct reg_test_t *)0x40005f00) 
volatile uint32_t *dbg = (volatile uint32_t *) 0x40007ff8; 

volatile uint32_t *var = (volatile uint32_t *) 0x40005ffc;

// simple uart register:
// Write: 16b(division_factor) 7b(0) 1b(rx_not_tx) 8b(tx_byte)
// Read:  16b(division_factor) 7b(0) 1b(ready)     8b(rx_byte) 
volatile uint32_t *uart = (volatile uint32_t *) 0x40007fc0;

// second uart is included in testbench 
// configure it with a -0x400 (or 0x100 in 4B words) offset 
// this verification hack is found in tb.h
volatile uint32_t *tb_uart = (volatile uint32_t *) 0x40007Bc0;


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
#endif //----------------------------------------



void dbg_write(const char *ptr)
{
   while( (*ptr != 0) )
   {
    *dbg = *ptr;
    ptr++;
   }
    *dbg = 0; // tag end of char* print
}

struct color_type
{
    char code;
    color_type(char x)
    {
        code = x;
    }
};

// Hoping that all signed and unsigned types will automatically cast to one of these (including char)
void dbg_write(uint32_t x)
{
    *dbg = UINT32_TYPE;
    *dbg = x;
}

void dbg_write(int32_t x)
{
    *dbg = INT32_TYPE;
    *dbg = x;
}

void dbg_write(color_type x)
{
    *dbg = COLOR_TYPE | uint32_t(x.code);
}


void recurse_print(const char* toto)
{
    dbg_write(toto);
    // End of print: tag this
    *dbg = END_PRINT;
}


void till_percent_and_send_color(const char*& toto)
{

    while ( (*toto != '\0') and (*toto != '%') ) // Parse char string
    {
        *dbg = (*toto); // output char
        toto++;
    }  
    if (*toto != 0)
    {
        toto++;
        if (*(toto) > 65) // assume it's color code
        {
            dbg_write(color_type(*(toto++)));
        }    
    }
}

template <class T, class ...Args>
void recurse_print(const char* toto, const T& first, const Args&... args)
{

    till_percent_and_send_color(toto);   
 
    dbg_write(first);

    recurse_print(toto,  args...);
}


#define gprintf(...) recurse_print(__VA_ARGS__)



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

  uint8_t cnt = 0;
  *var = 0xffffffff; 
  titi tata;  

  gprintf("#VGiorno core running %Y %Y\n", *var, uint32_t(127));    

  while (cnt < 10) {
    usleep(1); // 1000 ms
    //std::cerr << var;
    //*var = ~(*var);
    if (cnt & 0x4)
    {

        *var = toto[cnt&3];
        gprintf("Applying array value\n");
    }
    else
    {
        *var = tata.run(4);
        gprintf("Applying value from class method\n");       
    }
    cnt++;

  }
  
  // Play with bit fields
  gprintf("R");
  *var = regs.parm4;
  bool en2 = regs.en2;  
  bool en3 = regs.en3;
  gprintf("W");
  regs.parm4 = 0xCEB;
  regs.en2 = 1;
  regs.en4 = 1;
  regs.en5 = 1;
  gprintf("R\n");
  *var = regs.parm4;
  

  // Test gprintf capabilities (and what it costs in terms of rom / CPU time) 
  gprintf("#VGiorno core code end %Y %Y but %C and %Y\n", *var, uint32_t(127), "GOGO", "TITI");   
  gprintf("#VGiorno core code yo! %Y %Y but %C and %Y\n", "TOTO", uint32_t(127), "GOGO", "TITI"); 
  gprintf("#VSome stupid text %Y %M %Y but %Y and %Y\n", "0", *var, uint32_t(127), "GOGO", "TITI"); 
  gprintf("#VLet's all chant %Y %Y %Y but %Y and %Y\n", int32_t(55), *var, uint32_t(127), "GOGO", "TITI"); 
  gprintf("#VIs it me or you %Y %Y\n", *var, uint32_t(127));     
  gprintf("#VFlonflon pouetpouet %Y %Y %Y but %Y and %Y\n", int32_t(55), *var, uint32_t(127), "GOGO", "TITI");  
     
  gprintf("#VThere is a house in New Orleans %G %G\n", uint32_t(regs.parm4), uint32_t(127));         
  *uart = 0x00B200C5;     
  while ( (*uart & 0x100) == 0);
  //uint32_t zobi = *uart;
  //gprintf("#RUart read %B", zobi);
   *uart = 0x00B20091;     
  while ( (*uart & 0x100) == 0); 
   *uart = 0x00B20026;     
  while ( (*uart & 0x100) == 0); 
  *uart = 0x00B20100;   
  
  // Now test uart rx using the verif. uart in tb
  *tb_uart = 0x00B20091;
  while ( (*uart & 0x100) == 0); 
  gprintf("#VRead %Y from UART\n", uint32_t(*uart & 0xff));  
     
  
  
  
   
}
