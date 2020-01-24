// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
//#include <cmath>
#include "misc.h"
#include "gprintf.h"


//#define CLK_FIXED_FREQ_HZ (50ULL * 1000 * 1000)

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
//



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
  
#define TEST_DMA 
 #ifdef TEST_DMA  
   gprintf("CONF DMA\n");
   uint8_t dma_sink[16];
  // Configure DMA
  *dma = 0; // channel 0
  *(dma+1) = reinterpret_cast<uint32_t>(dma_sink);// addr
  *(dma+2) = 16;// tsfr sz
  *(dma+3) = 1000;// no timeout
  *(dma+4) = ( (8) | (0 << 4) | (1 << 8) | (3 << 12) | (0 << 14) | (1 << 16) ) ;//  source (en & src) | sink | addr inc | periph mask | data mask | priority
  
   gprintf("TB UART\n");
 
	for (uint32_t i = 0; i < 16; i++)
	{
		//*tb_uart = 0x00B20000 | i;
		*tb_uart = 0x00040000 | i;
		while ( (*tb_uart & 0x100) == 0);//{gprintf("#Vwait");}; 
		
	}
 
 gprintf("#UDMA transfer result: \n");
 	for (uint32_t i = 0; i < 16; i++)
	{
		
		 gprintf("#V	%: %Y\n", i, uint32_t(dma_sink[i]));

	} 
	
  *(dma+1) = reinterpret_cast<uint32_t>(dma_sink);// addr	
  *(dma+4) = ( (0) | (0 << 4) | (1 << 8) | (3 << 12) | (0 << 14) | (1 << 16) ) ;//  source (en & src) | sink | addr inc | periph mask | data mask | priority
  *(dma+2) = 16;// tsfr sz
#endif
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


  SET_CONSOLE(0);

#define TEST_BUF_READ
#ifdef TEST_BUF_READ
  PRINT_AT(10,10,65);
  char C = READ_BUF();
  PRINT_AT(0,0,0);
  gprintf("#UBuffer readback %Y\n", uint32_t(C));
  PRINT_AT(10,10,1);
  C = READ_BUF();
  PRINT_AT(0,0,0);
  gprintf("#UBuffer readback %Y\n", uint32_t(C));
  PRINT_AT(10,10,48);
  C = READ_BUF();
  PRINT_AT(0,0,0);
  gprintf("#UBuffer readback %Y\n", uint32_t(READ_BUF()));
  PRINT_AT(10,10,66);
  C = READ_BUF();
  PRINT_AT(0,0,0);
  gprintf("#UBuffer readback %Y\n", uint32_t(READ_BUF()));
#endif
while(1);
  	//*dbg = TOGGLE_BLINK;
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
