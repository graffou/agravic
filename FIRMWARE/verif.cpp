// Copyright lowRISC contributors.

#include <stdint.h>
//#include <cmath>
#include "misc.h"
#include "gprintf.h"



int main(int argc, char **argv) {
  // Any data written to the stack segment will connect the lowest four bits to
  // the board leds

  uint8_t cnt = 0;
  *var = 0xffffffff;


  gprintf("#VGiorno core running %Y %Y\n", *var, uint32_t(127));


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
   uint8_t dma_sink2[16];
   uint8_t big_buf[1024];
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
    // in rom size and sin(x) takes 650us with rv32i instruction set
    //d = sin(float(i)/10.0);
    gprintf("Result is c = %R\n", c);
    //*var = 1000 + int32_t(d * 1000);
  }
  gprintf("#VUART addr % ", uint32_t(uart));

#if 0
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
  }
#endif




	gprintf("#U****************** END OF SIMULATION *******************");




}
