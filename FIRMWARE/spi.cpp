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

/*	IF ( B(val, 6) == BIT(0) ) THEN // Tx config
		div <= RANGE(val, 23, 16);
		cnt_strobe <= ( BIT(0) & RANGE(val, 23, 17) ); //default
		tsfr_sz := RANGE(val, 10, 8);
		nbytes <= tsfr_sz;
		fifo_wptr_tx <= tsfr_sz; // 0 in case of DMA
		//fifo_wptr_rx <= BIN(000);
		fifo_rptr_rx <= BIN(000);
		dma_tsfr <= BOOL2BIT(tsfr_sz == BIN(000));
		fifo_state <= BIN(00); //
		PORT_BASE(spi_dma_o).rdy <= BIT(0);
		csn_clk_ncycles <= (RANGE(val, 15, 11) & BIT(0));
		is_master <= B(val, 0);
		clk_pol_tmp := B(val, 1);
		clk_rx_pol <= B(val, 2);
		dma_wait_fifo_full <= B(val, 7);
		rx_sampling <= B(val, 4);
		tx_rx <= B(val, 5);
		auto_csn <= B(val, 3);
		clk_pol <= clk_pol_tmp;
	ELSE // configure Rx sampling time and pause duration
		cnt_strobe <= RANGE(val, 23, 16);
		pause_ncycles <= RANGE(val, 5, 0);
	ENDIF
	*/
   // 26: pause_ncycles
	// 16: period
	// 11: csn_delay / 2
	// 8: tsfr_sz
	// 5: tx+rx
	// 3: auto csn
  // 1: clk_pol
  // 0: is_master
  uint32_t val1, val2;
  //				pause	  |    div    | csn_ncyc  |     sz    |   txrx   | autocsn   |   mst
  *spi_tb_conf = ( (10 << 26) | (4 << 16) | (2 << 11) | (3 << 8)  | (1 << 5) |  (0 << 3) | (0 << 0) );
  *spi_conf =    ( (10 << 26) | (4 << 16) | (2 << 11) | (3 << 8)  | (1 << 5) |  (0 << 3) | (1 << 0) );
  uint32_t spi_tx = 0;
  *spi_data = spi_tx;
  for (int i = 0; i < 255; i++)
  {
	  while ((*spi_conf)){} // still busy
	  val1 = *spi_tb_data;
	  val2 = *spi_data;
	  gprintf("#MTB Read  % \n", val1);
	  gprintf("#RDUT Read % \n", val2);
	  spi_tx++;
	  *spi_tb_data = spi_tx+1000;
	  *spi_data = spi_tx;
  }
//  *spi_data = 0x55FF00AA;
//  while ((*spi_conf)){} // still busy
//  val2 = *spi_tb_data;
//  gprintf("#MRead % then %\n", val1, val2);

  //while ((*spi_conf)){} // still busy
	gprintf("#U****************** END OF SIMULATION *******************");




}
