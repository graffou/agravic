#include "../Include_libs/slv.h"

START_OF_FILE(SPI)
INCLUDES
USE_PACKAGE(structures)
//USE_PACKAGE(altera)
USE_PACKAGE(slv_utils)


ENTITY(SPI_master,
DECL_PORTS(
		PORT(clk_120, CLK_TYPE, IN),
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
		PORT(spi_dma_i, d2p_8_t, IN),
		PORT(spi_dma_o, p2d_8_t, OUT),
		PORT(spi_csn_o, BIT_TYPE, OUT),
		PORT(spi_clk_o, BIT_TYPE, OUT),
		PORT(spi_tx_o, BIT_TYPE, OUT),
		PORT(spi_rx_i, BIT_TYPE, IN)
		)
		, INTEGER generic_int
);

TYPE(fifo_t, ARRAY_TYPE(UINT(8), 4)); // two dma channels
SIG(fifo_tx, fifo_t);
SIG(fifo_rx, fifo_t);
SIG(fifo_rptr_rx, UINT(3));
SIG(fifo_wptr_rx, UINT(3));
SIG(fifo_rptr_tx, UINT(3));
SIG(fifo_wptr_tx, UINT(3));

SIG(spi_state, UINT(5)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
SIG(next_state, UINT(5)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
CONST(spi_idle, UINT(LEN(spi_state))) := TO_UINT(0, LEN(spi_state));
CONST(spi_wait, UINT(LEN(spi_state))) := TO_UINT(1, LEN(spi_state));
CONST(dma_init, UINT(LEN(spi_state))) := TO_UINT(2, LEN(spi_state));
CONST(spi_start, UINT(LEN(spi_state))) := TO_UINT(4, LEN(spi_state));
CONST(spi_send, UINT(LEN(spi_state))) := TO_UINT(6, LEN(spi_state));
CONST(spi_stop, UINT(LEN(spi_state))) := TO_UINT(8, LEN(spi_state));
CONST(is_master, BOOLEAN) := true;



// Conf registers
SIG(div, UINT(16));
SIG(trigged_tsfr, BIT_TYPE); // 1 to launch dma-driven tsfrs on external trig signal
SIG(csn_clk_ncycles, UINT(6)); // how many clk cycles from csn low to spi clk start
SIG(nbytes, UINT(3)); // 0 -> undefined (DMA) 1-> 1 byte tsfr, ... 3 -> 4-byte tsfr
SIG(clk_pol, BIT_TYPE);
SIG(clk_rx_pol, BIT_TYPE);
SIG(start_spi, BIT_TYPE);
SIG(reset_spi, BIT_TYPE);
SIG(dma_tsfr, BIT_TYPE);
SIG(tx_rx, BIT_TYPE); // 1 to have rx during tx




CONST(dma_tsfr_nbytes, UINT(LEN(nbytes))) := TO_UINT(0, LEN(nbytes));

// Other signals

SIG(master_clk, BIT_TYPE);
SIG(master_csn, BIT_TYPE);
SIG(cnt, UINT(16));
SIG(tsfr_cnt, UINT(3));
SIG(conf, UINT(32));
SIG(data_tx, UINT(32));
SIG(data_rx, UINT(32));
SIG(tx_byte, UINT(8));
SIG(tx_byte0, UINT(8));
SIG(rx_byte, UINT(8));
SIG(ready, BIT_TYPE);
SIG(spi_tx, BIT_TYPE);
SIG(ext_clk, BIT_TYPE);
SIG(dma_wait_fifo_full, BIT_TYPE);
SIG(dma_wait_for_grant, BIT_TYPE);
SIG(rx_sampling, BIT_TYPE); // 0 for regular sampling, 1 for end of cycle
SIG(sending, BIT_TYPE); // 0 for regular sampling, 1 for end of cycle
SIG(fifo_state, UINT(2));
SIG(cnt_strobe, UINT(16));
SIG(fifo_init, BIT_TYPE);
CONST(reg_base_addr, UINT(LEN(PORT_BASE(core2mem_i).addr))) := TO_UINT(generic_int, LEN(PORT_BASE(core2mem_i).addr));
SIG(base_addr_test, UINT(LEN(PORT_BASE(core2mem_i).addr)));
CONST(reg_addr_lsbs, INTEGER) := ( generic_int / 268435456);
SIG(addr_lsbs_test, UINT(4));

// Signals for pluto input testing
#define PLUTO_TEST
SIG(cnt_test, UINT(8));
SIG(cnt_test2, UINT(8));

BEGIN


// Is a declaration in C++, instantiation in VHDL(after begin)
//GATED_CLK(clk_g ,clk_peri, gate_cell);
//#define LOOPBACK_TEST

PROCESS(3, clk_mcu, reset_n)

VAR(val, UINT(32));
VAR(tsfr_sz, UINT(3)); // in bytes. 0 for dma
VAR(clk_pol_tmp, BIT_TYPE);
VAR(fifo_full_tx , BOOLEAN);
VAR(fifo_empty_tx , BOOLEAN);
VAR(fifo_wptr_tx_tmp, UINT(3));

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		//cnt <= TO_UINT(0, LEN(cnt));
		div <= TO_UINT((1), LEN(div)); // assume 60MHz defualt
		ready <= BIT(1);

		//RESET(fifo_rptr_tx);
		//RESET(fifo_wptr_tx);
		//RESET(fifo_rptr_rx);
		//RESET(fifo_wptr_rx);
		RESET(fifo_wptr_tx);
		RESET(fifo_rptr_rx);
		start_spi <= BIT(0);
		clk_rx_pol <= BIT(0);
		clk_pol <= BIT(1);
		dma_tsfr <= BIT(0);
		dma_wait_for_grant <= BIT(0);
	ELSEIF ( EVENT(clk_mcu) and (clk_mcu == BIT(1)) ) THEN

		start_spi <= BIT(0);
		reset_spi <= BIT(0);

		// Registers R/W
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		//IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, data_addr_span - 3, 4) == BIN(111111101) ) ) THEN
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, HI(PORT_BASE(core2mem_i).addr), REG_NBITS) == RANGE( reg_base_addr, HI(reg_base_addr), REG_NBITS) ) ) THEN
			// Write registers
			IF (PORT_BASE(core2mem_i).wr_n == BIT(0) ) THEN
				// just configure
				//IF (PORT_BASE(core2mem_i).addr == BIN(1111111010000)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					val := PORT_BASE(core2mem_i).data;
					IF ( B(val, 6) == BIT(0) ) THEN // Tx config
						div <= RANGE(val, 31, 16);
						cnt_strobe <= ( BIT(0) & RANGE(val, 31, 17) ); //default
						tsfr_sz := RANGE(val, 10, 8);
						nbytes <= tsfr_sz;
						fifo_wptr_tx <= tsfr_sz; // 0 in case of DMA
						fifo_rptr_rx <= BIN(000);
						dma_tsfr <= BOOL2BIT(tsfr_sz == BIN(000));
						fifo_state <= BIN(00); //
						PORT_BASE(spi_dma_o).rdy <= BIT(0);
						csn_clk_ncycles <= (RANGE(val, 15, 11) & BIT(0));
						//is_master <= B(val, 0);
						clk_pol_tmp := B(val, 1);
						clk_rx_pol <= B(val, 2);
						dma_wait_fifo_full <= B(val, 7);
						rx_sampling <= B(val, 4);
						tx_rx <= B(val, 5);
						clk_pol <= clk_pol_tmp;

					ELSE // configure Rx sampling time
						cnt_strobe <= RANGE(val, 31, 16);
					ENDIF
					reset_spi <= BIT(1);
					dma_wait_for_grant <= BIT(0);

					// add a bit for lsb first (and use as UART Tx)
					// Configure as slave -> release control over clk and csn signals
					// ( B(val, 0) == BIT(0) ) THEN
					//ORT_BASE(spi_clk_io) <= "Z";
					//ORT_BASE(spi_csn_io) <= "Z";
					//SE // init master output signals
						//PORT_BASE(spi_csn_o) <= BIN(1);
						//PORT_BASE(spi_clk_o) <= clk_pol_tmp;
						//spi_tx_o <= BIT(0);
					//DIF
				ENDIF

				//IF (PORT_BASE(core2mem_i).addr == BIN(1111111010001)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(1, REG_NBITS)) THEN
					//data_tx <= PORT_BASE(core2mem_i).data;
					fifo_tx(0) <= RANGE(PORT_BASE(core2mem_i).data, 31, 24); //MSBs first ?
					fifo_tx(1) <= RANGE(PORT_BASE(core2mem_i).data, 23, 16); //MSBs first ?
					// !!!! No Gray order required !!!!!!
					fifo_tx(2) <= RANGE(PORT_BASE(core2mem_i).data, 15, 8); //MSBs first ?
					fifo_tx(3) <= RANGE(PORT_BASE(core2mem_i).data, 7, 0); //MSBs first ?
					start_spi <= BIT(1); // not for DMA
					//spi_state <= spi_start;
					ready <= BIT(0);
					//if (is_master)
				ENDIF
			ELSE
				// Read registers
				//IF (PORT_BASE(core2mem_i).addr == BIN(1111111010000)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( TO_UINT(0, 31) & BOOL2BIT(spi_state == spi_idle) );
				ENDIF
				//IF (PORT_BASE(core2mem_i).addr == BIN(1111111010001)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(1, REG_NBITS)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( fifo_rx(3) & fifo_rx(2) & fifo_rx(1) & fifo_rx(0) );
				ENDIF
			ENDIF
		ENDIF

		// DMA FIFO TX-------------------------------
		IF (dma_tsfr == BIT(1)) THEN
			// evaluate fifo full considering newest data
#ifndef PLUTO_TESTz
			fifo_wptr_tx_tmp := fifo_wptr_tx + (BIN(00) & PORT_BASE(spi_dma_i).data_en);
#else
			fifo_wptr_tx_tmp := fifo_wptr_tx + (BIN(00) & BOOL2BIT(cnt_test == TO_UINT(9, 8)));
#endif

			fifo_empty_tx := ( fifo_wptr_tx == fifo_rptr_tx );
			fifo_full_tx := ( fifo_wptr_tx_tmp == ( fifo_rptr_tx + BIN(100) ) );

			// Tx DMA ------------------
			IF (not fifo_full_tx) THEN
				IF (fifo_state == BIN(10)) THEN // End of tsfr
					PORT_BASE(spi_dma_o).rdy <= BIT(0);
				ELSE
					PORT_BASE(spi_dma_o).rdy <= BIT(1);
				ENDIF
			ELSE
				PORT_BASE(spi_dma_o).rdy <= BIT(0); // Fifo full: not ready anymore
			ENDIF
#ifndef PLUTO_TESTz
			IF ( (PORT_BASE(spi_dma_i).data_en) == BIT(1) ) THEN // REceived new byte from dma
				fifo_tx(TO_INTEGER(RANGE(fifo_wptr_tx, 1, 0))) <= PORT_BASE(spi_dma_i).data;
				fifo_wptr_tx <= fifo_wptr_tx_tmp;// + 1;
			ENDIF
#else
			if (cnt_test == TO_UINT(9, 8)) THEN
				cnt_test <= TO_UINT(0, 8);
				fifo_tx(TO_INTEGER(RANGE(fifo_wptr_tx, 1, 0))) <= cnt_test2;
				fifo_wptr_tx <= fifo_wptr_tx_tmp;
				cnt_test2 <= cnt_test2 + 1;
			ELSE
				cnt_test <= cnt_test + 1;
			ENDIF
#endif

			start_spi <= BOOL2BIT(fifo_full_tx);

			// Rx DMA --------------
			IF ( (not (fifo_wptr_rx == fifo_rptr_rx) ) and (dma_wait_for_grant == BIT(0)) ) THEN// stg in Rx fifo
				PORT_BASE(spi_dma_o).data <= fifo_rx(TO_INTEGER(fifo_rptr_rx));
				PORT_BASE(spi_dma_o).data_en <= BIT(1);
				dma_wait_for_grant <= BIT(1);
			ENDIF

			IF ( (dma_wait_for_grant == BIT(1)) and (PORT_BASE(spi_dma_i).grant == BIT(1)) ) THEN
				fifo_rptr_rx <= fifo_rptr_rx + 1;
				dma_wait_for_grant <= BIT(0);
				PORT_BASE(spi_dma_o).data_en <= BIT(0);

			ENDIF

		ELSE
			PORT_BASE(spi_dma_o).rdy <= BIT(0);
			PORT_BASE(spi_dma_o).data_en <= BIT(0);

		ENDIF

	ENDIF
END_PROCESS




PROCESS(0, clk_120, reset_n) // ----------------------------------------------------------

VAR(rx_trans, BIT_TYPE);
VAR(next_cnt, UINT(16));
VAR(fifo_empty_tx, BOOLEAN);
VAR(fifo_empty_rx, BOOLEAN);
VAR(fifo_full_rx , BOOLEAN);
VAR(rx_byte_tmp, UINT(8));

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		cnt <= TO_UINT(0, LEN(cnt));
		ready <= BIT(1);
		master_clk <= BIT(0);
		master_csn <= BIT(1);
		spi_clk_o <= BIT(0);
		spi_csn_o <= BIT(0);
		spi_tx_o <= BIT(0);
		spi_state <= spi_idle;
		RESET(rx_byte);
		RESET(tx_byte);
		RESET(fifo_rptr_tx);

		RESET(fifo_wptr_rx);
		sending <= BIT(0);
		fifo_init <= BIT(0);


	ELSEIF ( EVENT(clk_120) and ( clk_120 == BIT(1) ) ) THEN

		//Receiver part: out of FSM (gain the send state for rx)
		IF ( (tx_rx == BIT(1)) and (cnt == cnt_strobe) and (sending == BIT(1)) ) THEN
#ifdef LOOPBACK_TEST
			rx_byte_tmp := ( RANGE( rx_byte, 6, 0) & (spi_tx) );
#else
			rx_byte_tmp := ( RANGE( rx_byte, 6, 0) & spi_rx_i );
#endif
			IF ( ( tsfr_cnt == BIN(111) ) ) THEN // last tx bit is last rx bit as well
				fifo_rx( TO_INTEGER( RANGE(fifo_wptr_rx, 1, 0)) ) <= rx_byte_tmp;
				fifo_wptr_rx <= fifo_wptr_rx + 1;
			ENDIF
			rx_byte <= rx_byte_tmp;
		ENDIF

		// SPI FSM ------------------------------------
		IF (spi_state == spi_idle) THEN
			ready <= BIT(1);
			sending <= BIT(0);
			// init fifo after first SPI start, not when entering idle state because of fifo empty (uncontrolled inputs like ADC...)
			IF (fifo_init == BIT(1)) THEN
				fifo_rptr_tx <= BIN(000);
				fifo_wptr_rx <= BIN(000);
				fifo_init <= BIT(0);
			ENDIF
			IF (is_master) THEN
				spi_clk_o <= clk_pol;
				spi_csn_o <= BIT(1);
				spi_tx_o <= BIT(0);
			ENDIF
			// wait either value written to data reg, or fifo full (dma)
			IF ( start_spi == BIT(1) ) THEN
				spi_state <= spi_start;
			ENDIF

		ELSEIF (spi_state == spi_wait) THEN // state used for delays
			next_cnt := cnt - TO_UINT(1, LEN(cnt));
			cnt <= next_cnt;
			IF (next_cnt == TO_UINT(0, LEN(cnt))) THEN
				spi_state <= next_state;
				IF ( RANGE(tsfr_cnt, 2, 0) == BIN(111) ) THEN // must load a new data from fifo
					fifo_empty_tx := (fifo_wptr_tx == fifo_rptr_tx);
					IF (fifo_empty_tx and not (next_state == spi_idle) ) THEN // can't, end tsfr. If next state is idle, the ending processed has already begun
						spi_state <= spi_stop;
					ELSEIF (next_state == spi_send) THEN
						tx_byte <= fifo_tx(TO_INTEGER( RANGE(fifo_rptr_tx, 1, 0)) ); // msb is meaningless (for fifo full / empty disambiguation)
						tx_byte0 <= fifo_tx(TO_INTEGER( RANGE(fifo_rptr_tx, 1, 0)) ); // msb is meaningless (for fifo full / empty disambiguation)
						fifo_rptr_tx <= fifo_rptr_tx + 1;
						tsfr_cnt <= TO_UINT(7, LEN(tsfr_cnt));
					ENDIF
				ELSE
					//tsfr_cnt <= TO_UINT(8, LEN(tsfr_cnt));
				ENDIF
			ENDIF
			// use next_cnt to have better clk shape for low div ratios
			IF ( (next_cnt == EXT(RANGE(div, HI(div), 1), LEN(cnt))) and (sending == BIT(1)) ) THEN // create master spi clk
				master_clk <= (not master_clk);
				spi_clk_o  <= (not master_clk);
			ENDIF
/*
			//Receiver part
			IF ( (tx_rx == BIT(1)) and (next_cnt == cnt_strobe) and (sending == BIT(1)) ) THEN
#ifdef LOOPBACK_TEST
				rx_byte_tmp := ( RANGE( rx_byte, 6, 0) & (spi_tx) );
#else
				rx_byte_tmp := ( RANGE( rx_byte, 6, 0) & spi_rx_i );
#endif
				IF ( ( tsfr_cnt == BIN(111) ) ) THEN // last tx bit is last rx bit as well
					fifo_rx( TO_INTEGER( RANGE(fifo_wptr_rx, 1, 0)) ) <= rx_byte_tmp;
					fifo_wptr_rx <= fifo_wptr_rx + 1;
				ENDIF
				rx_byte <= rx_byte_tmp;
			ENDIF
*/
		ELSEIF (spi_state == spi_start) THEN // csn low, then wait csn_clk_ncycles, then send
			tsfr_cnt <= TO_UINT(0, LEN(tsfr_cnt));
			spi_csn_o <= BIT(0);
			cnt <= EXT(csn_clk_ncycles, LEN(cnt));
			spi_state <= spi_wait;
			next_state <= spi_send;
			// tsfr_cnt is the number of bits to transfer (can be renewed in case of dma tsfr)
			tsfr_cnt <= TO_UINT(7, LEN(tsfr_cnt));

		ELSEIF (spi_state == spi_send) THEN
			sending <= BIT(1);
			cnt <= EXT(div, LEN(cnt));
			spi_tx_o <= B(tx_byte,7);
			spi_tx <= B(tx_byte,7);
			tx_byte <= SHIFT_LEFT(tx_byte, 1);
			master_clk <= clk_pol;
			spi_clk_o <= clk_pol;
			spi_state <= spi_wait;
			next_state <= spi_send;
			tsfr_cnt <= tsfr_cnt - 1;
		ELSEIF (spi_state == spi_stop) THEN
			sending <= BIT(0);
			cnt <= EXT(csn_clk_ncycles, LEN(cnt));
			spi_state <= spi_wait;
			next_state <= spi_idle;
			spi_clk_o <= clk_pol;

		ENDIF

		// Conf register write resets FSM
		IF (reset_spi == BIT(1)) THEN
			spi_state <= spi_idle;
			fifo_init <= BIT(1);
		ENDIF

	ENDIF
END_PROCESS
/*
COMB_PROCESS(1, clk_120)
BEGIN
	ext_clk <= spi_clk_io;

ENDIF
*/
BLK_END;
