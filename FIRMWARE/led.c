// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
//#include <cmath>



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



/*
#define INT32_TYPE 0x80800100
#define INT16_TYPE 0x80800200
#define INT8_TYPE 0x80800400
#define UINT32_TYPE 0x80000100
#define UINT16_TYPE 0x80000200
#define UINT8_TYPE 0x80000400
#define FLOAT_TYPE 0x80001000
#define BOOL_TYPE 0x80000800
#define COLOR_TYPE 0xFFFFFF00
#define END_PRINT 0x88000000
*/
#define INT32_TYPE  0x80000000
#define INT16_TYPE  0x81000000
#define INT8_TYPE   0x82000000
#define UINT32_TYPE 0x83000000
#define UINT16_TYPE 0x84000000
#define UINT8_TYPE  0x85000000
#define BOOL_TYPE   0x86000000
#define FLOAT_TYPE  0x87000000
#define COLOR_TYPE  0xE0000000
#define END_PRINT   0xF0000000
#define TOGGLE_BLINK   0x03000000
#define PRINT_AT_XY   0x0F000000
#define CLS   0x02000000
#define FONT   0x01000000

//char test_font[12] = {0xAA,0x55,0x11,0x22,0x33,0x44,0xAA,0x55,0x11,0x22,0x33,0x44};
char test_font[12] = {0xfe,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0xfe};

char alien1[12]={
	0b00110000,
	0b00001000,
	0b00011111,
	0b00110111,
	0b01111011,
	0b11111111,
	0b11111100,
	0b00011000,
	0b00100111,
	0b01000000,
	0b10000000,
	0b00000000};

#define BIT_REVERSE(a) ( ((a&0x80)>>7) | ((a&0x40)>>5) | ((a&0x20)>>3) | ((a&0x10)>>1) | ((a&8)<<1) | ((a&4)<<3) | ((a&2)<<5) | ((a&1)<<7) )

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
void dbg_write(float x)
{
    *dbg = FLOAT_TYPE;
    *dbg = *reinterpret_cast<uint32_t*>(&x);
}

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

void dbg_write( char c)
{
    *dbg = c; 
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


#define PRINT_AT(a, b, c) *dbg = (PRINT_AT_XY | (b << 8) | ((a) << 16) | (c));


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
  *uart = 0x00B30100;   
  
 
  
  // Now test uart rx using the verif. uart in tb
  *tb_uart = 0x00B20091;
  while ( (*uart & 0x100) == 0); 
  gprintf("#VRead %Y from UART\n", uint32_t(*uart & 0xff));  
   
  // Test DMA with UART  
   *uart = 0x00B20100;   
   *uart = 0x00040100;   
   
   
   gprintf("CONF DMA\n");
   uint8_t dma_sink[16];
  // Configure DMA
  *dma = 0; // channel 0
  *(dma+1) = reinterpret_cast<uint32_t>(dma_sink);// addr
  *(dma+2) = 16;// tsfr sz
  *(dma+3) = 1000;// no timeout
  *(dma+4) = ( (8) | (0 << 4) | (1 << 8) | (3 << 12) | (0 << 14) | (1 << 16) ) ;//  source (en & src) | sink | addr inc | periph mask | data mask | priority
  
   gprintf("TB UART\n");
 /*
	for (uint32_t i = 0; i < 15; i++)
	{
		//*tb_uart = 0x00B20000 | i;
		*tb_uart = 0x00040000 | i;
		while ( (*tb_uart & 0x100) == 0);//{gprintf("#Vwait");}; 
		
	}
 */
 gprintf("#UDMA transfer result: ");
 	for (uint32_t i = 0; i < 16; i++)
	{
		
		 gprintf("#U	%: ", uint32_t(dma_sink[i]));

	} 
	
  *(dma+1) = reinterpret_cast<uint32_t>(dma_sink);// addr	
  *(dma+4) = ( (0) | (0 << 4) | (1 << 8) | (3 << 12) | (0 << 14) | (1 << 16) ) ;//  source (en & src) | sink | addr inc | periph mask | data mask | priority
  *(dma+2) = 16;// tsfr sz

 gprintf("#VStarting float operations\n");   
  float a,b,c,d;
   
  a = 3.14159E0;
  b = .99;  
  c = a;
  *timer = 10000;
  for (int i = 0; i < 5; i++)
  { 
    c = c * b;
    // This works with include<cmath> and linking with -lm, however this is a +18kB 
    // in rom size and sin(x) takes 650µs with rv32i instruction set
    //d = sin(float(i)/10.0);
    gprintf("Le resultat c = %R\n", c); 
    //*var = 1000 + int32_t(d * 1000);
  }
  	*dbg = TOGGLE_BLINK;
   //while ( (*uart & 0x100) == 0); 
 	//*dbg = CLS;
 	//while ( (*uart & 0x100) == 0); 

	for (int i = 0; i < 12; i++)
 	{
		*dbg = FONT | (((128-32)*12+i) << 8) | test_font[i];
		*dbg = FONT | (((65-32)*12+i) << 8) | alien1[i];
		*dbg = FONT | (((66-32)*12+i) << 8) | BIT_REVERSE(alien1[i]);
	}
 	
 	 //while ( (*uart & 0x100) == 0); 
	gprintf("\x80\x80\x80\n");
  while (1) {
   *uart = 0x01A10100;   
   while ( (*uart & 0x100) == 0); 
	char c = char(*uart);
	if (c == 13) c = 10;
	if (c == 0xA3) 
	{
		*dbg = CLS;
		/*
		for (int i = 0; i < 80;i++)
		{
			for (int j = 0; j < 40;j++)
			{
				*dbg = (PRINT_AT_XY | (i << 8) | ((j) << 16) | (i+j+64));
			}	
		}	
		*/
		PRINT_AT(20,0,128);
	}
	else
	dbg_write(c);
	
	

  /*
   for (int i = 0; i < 80;i++)
   {
		for (int j = 0; j < 40;j++)
		{
			*dbg = (PRINT_AT | (i << 8) | (j << 16) | (i+j));
		}	
	}
	*/
/*
	gprintf("\n");	
	char c = static_cast<char>(*uart);
	for (int i = 7; i >= 0; i--)
		if ( c >>i )
			gprintf("1");
		else
			gprintf("0");
*/	
	//gprintf("popo %\n", char(*uart));
   }
}
