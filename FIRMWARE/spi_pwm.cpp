#include <stdint.h>
//#include <cmath>
#include "misc.h"
#include "gprintf.h"



int main(int argc, char **argv) {

    IT_ENABLE(16); // enable DMA IRQ
   uint32_t dma_source[8];
   uint32_t z = 0;
   // create pwm signal
   for (int i = 0; i < 8; i++)
   {
     z = (z << 1) | 1;
     dma_source[i] = z;//(z << 24) | (z << 16) | (z << 8) | z; 
   }
 
 // not txrx: tx only (master and not txrx)
  //				pause	  |    div    | csn_ncyc  |     sz    |   txrx   | autocsn   |   mst
   *spi_conf =    ( (0 << 26) | (4 << 16) | (2 << 11) | (0 << 8)  | (0 << 5) |  (0 << 3) | (1 << 0) );

   
  *dma = 0; // channel 0


  *(dma+3) = 1000;// no timeout
  // No inc, read always same addr. Sink is SPI.
  //*(dma+4) = ( (0) | (1 << 4) | (0 << 8) | (3 << 12) | (0 << 14) | (1 << 16) ) ;//  source (en & src) | sink | addr inc | periph mask | data mask | priority
  while (1)
  for (int i = 0; i < 8; i++)
  {
    *(dma+1) = reinterpret_cast<uint32_t>(dma_source+i);// addr
    *(dma+2) = 1600;// tsfr sz
    //  gprintf("src % wr % \n", reinterpret_cast<uint32_t>(dma_source), reinterpret_cast<uint32_t>(dma_source+i));
    *(dma+4) = ( (1) | (1 << 4) | (0 << 8) | (3 << 12) | (0 << 14) | (1 << 16) ) ;//  source (en & src) | sink | addr inc | periph mask | data mask | priority   
    __WFI__;
    IT_CLEAR(16);
  }
}
