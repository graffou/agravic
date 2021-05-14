//TODO: fix autocsn, however this is not of great value (except for 3-wire link)

#include "../Include_libs/slv.h"

START_OF_FILE(SPI)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(altera)
USE_PACKAGE(slv_utils)


ENTITY(SPI,
DECL_PORTS(
		PORT(clk_120, CLK_TYPE, IN),
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
		PORT(spi_dma_i, d2p_8_t, IN),
		PORT(spi_dma_o, p2d_8_t, OUT),
		PORT(spi_csn_io, TRISTATE(1), INOUT),
		PORT(spi_clk_io, TRISTATE(1), INOUT),
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
CONST(spi_pause, UINT(LEN(spi_state))) := TO_UINT(7, LEN(spi_state)); // pause between bytes
CONST(spi_stop, UINT(LEN(spi_state))) := TO_UINT(8, LEN(spi_state));
//CONST(is_master, BOOLEAN) := true;



// Conf registers
SIG(div, UINT(8));
SIG(trigged_tsfr, BIT_TYPE); // 1 to launch dma-driven tsfrs on external trig signal
SIG(csn_clk_ncycles, UINT(6)); // how many clk cycles from csn low to spi clk start
SIG(pause_ncycles, UINT(6)); // how many clk cycles between bytes
SIG(nbytes, UINT(3)); // 0 -> undefined (DMA) 1-> 1 byte tsfr, ... 3 -> 4-byte tsfr
SIG(clk_pol, BIT_TYPE);
SIG(clk_rx_pol, BIT_TYPE);
SIG(start_spi, BIT_TYPE);
SIG(reset_spi, BIT_TYPE);
SIG(dma_tsfr, BIT_TYPE);
SIG(tx_rx, BIT_TYPE);
SIG(auto_csn, BIT_TYPE); // No csn signal: detect clk end of activity instead
SIG(is_master, BIT_TYPE); //




CONST(dma_tsfr_nbytes, UINT(LEN(nbytes))) := TO_UINT(0, LEN(nbytes));

// Other signals

SIG(master_clk, BIT_TYPE);
SIG(master_csn, BIT_TYPE);
SIG(cnt, UINT(8));
SIG(cnt_auto_csn, UINT(6));
SIG(tsfr_cnt, UINT(3));
SIG(conf, UINT(32));
SIG(data_tx, UINT(32));
SIG(data_rx, UINT(32));
SIG(tx_byte, UINT(8));
SIG(tx_byte0, UINT(8));
SIG(rx_byte, UINT(8));
SIG(ready, BIT_TYPE);
SIG(spi_tx, BIT_TYPE);
SIG(spi_rx, BIT_TYPE);
SIG(spi_clkp, BIT_TYPE);
SIG(spi_clk, BIT_TYPE);
SIG(spi_csn, BIT_TYPE);
SIG(spi_csnp, BIT_TYPE);
SIG(eot, BIT_TYPE);
SIG(clk_detected, BIT_TYPE);

SIG(tx_data_available, BIT_TYPE);

SIG(ext_clk, BIT_TYPE);
SIG(dma_wait_fifo_full, BIT_TYPE);
SIG(dma_wait_for_grant, BIT_TYPE);
SIG(rx_sampling, BIT_TYPE); // 0 for regular sampling, 1 for end of cycle
SIG(sending, BIT_TYPE); // 0 for regular sampling, 1 for end of cycle
SIG(fifo_state, UINT(2));
SIG(cnt_strobe, UINT(8));
SIG(fifo_init, BIT_TYPE);
SIG(new_tx_byte, BIT_TYPE);
SIG(first_byte, BIT_TYPE);

// Signals for pluto input testing
//#define PLUTO_TEST
SIG(cnt_test, UINT(8));
SIG(data_test, UINT(8));
SIG(data_en_test, BIT_TYPE);
SIG(rdy_test, BIT_TYPE);
SIG(toggle_test, BIT_TYPE);
SIG(fifo_full_rx, BIT_TYPE);
SIG(busy, BIT_TYPE);

//dbg
SIG(rx_trans_dbg, BIT_TYPE);


//SIG(cnt_test2, UINT(8));
CONST(reg_base_addr, UINT(LEN(PORT_BASE(core2mem_i).addr))) := TO_UINT(generic_int, LEN(PORT_BASE(core2mem_i).addr));
SIG(base_addr_test, UINT(LEN(PORT_BASE(core2mem_i).addr)));
CONST(reg_addr_lsbs, INTEGER) := ( generic_int / 268435456);
SIG(addr_lsbs_test, UINT(4));

BEGIN


PROCESS(3, clk_mcu, reset_n)

VAR(val, UINT(32));
VAR(tsfr_sz, UINT(3)); // in bytes. 0 for dma
VAR(clk_pol_tmp, BIT_TYPE);
VAR(fifo_full_tx , BOOLEAN);
VAR(fifo_empty_tx , BOOLEAN);
VAR(fifo_wptr_tx_tmp, UINT(3));

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		div <= TO_UINT((1), LEN(div)); // assume 60MHz defualt
		//assigned in other process ready <= BIT(1);
		RESET(fifo_wptr_tx);
		RESET(fifo_rptr_rx);
		start_spi <= BIT(0);
		clk_rx_pol <= BIT(0);
		clk_pol <= BIT(1);
		dma_tsfr <= BIT(0);
		dma_wait_for_grant <= BIT(0);
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		tx_rx <= BIT(0);
		auto_csn <= BIT(0);
		RESET(cnt_strobe);
		RESET(nbytes);
		PORT_BASE(spi_dma_o).rdy <= BIT(0);
		reset_spi <= BIT(0);
		busy <= BIT(0);
	ELSEIF ( EVENT(clk_mcu) and (clk_mcu == BIT(1)) ) THEN

		start_spi <= BIT(0);
		reset_spi <= BIT(0);

		// Registers R/W ------------------------------------------------------------------------
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		busy <= (busy and not ready);
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, HI(PORT_BASE(core2mem_i).addr), REG_NBITS) == RANGE( reg_base_addr, HI(reg_base_addr), REG_NBITS) ) ) THEN
			// Write registers
			IF (PORT_BASE(core2mem_i).wr_n == BIT(0) ) THEN
				// just configure
				//IF (PORT_BASE(core2mem_i).addr == BIN(1111111000000)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					val := PORT_BASE(core2mem_i).data;
					IF ( B(val, 6) == BIT(0) ) THEN // Tx config
						pause_ncycles <= RANGE(val, 31, 26);
						div <= RANGE(val, 23, 16);
						cnt_strobe <= ( BIT(0) & RANGE(val, 23, 17) ); //default
						tsfr_sz := RANGE(val, 10, 8);
						nbytes <= tsfr_sz;
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
					ENDIF
					reset_spi <= BIT(1);
				ENDIF

				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(1, REG_NBITS)) THEN
					fifo_tx(3) <= RANGE(PORT_BASE(core2mem_i).data, 31, 24); //MSBs first ? NO!!!!
					fifo_tx(2) <= RANGE(PORT_BASE(core2mem_i).data, 23, 16); //MSBs first ?
					fifo_tx(1) <= RANGE(PORT_BASE(core2mem_i).data, 15, 8); //MSBs first ?
					fifo_tx(0) <= RANGE(PORT_BASE(core2mem_i).data, 7, 0); //MSBs first ? NO!!!! If single byte is send, must be in fifo(0)
					fifo_wptr_tx <= nbytes;
					busy <= BIT(1);
					reset_spi <= BIT(1);
				ENDIF
			ELSE
				// Read registers
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( TO_UINT(0, 31) & busy );
				ENDIF
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(1, REG_NBITS)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( fifo_rx(3) & fifo_rx(2) & fifo_rx(1) & fifo_rx(0) );
					reset_spi <= (not is_master); // prepare for another read w/o configuration; not for master as it would start another Tx!
				ENDIF
			ENDIF
		ENDIF

		// DMA FIFO TX-------------------------------
		IF (dma_tsfr == BIT(1)) THEN
			// evaluate fifo full considering newest data
			fifo_wptr_tx_tmp := fifo_wptr_tx + (BIN(00) & PORT_BASE(spi_dma_i).data_en);
			fifo_empty_tx := ( fifo_wptr_tx == fifo_rptr_tx );
			fifo_full_tx := ( fifo_wptr_tx_tmp == ( fifo_rptr_tx + BIN(100) ) );
			IF (not fifo_full_tx) THEN
					PORT_BASE(spi_dma_o).rdy <= BIT(1);
			ELSE
				PORT_BASE(spi_dma_o).rdy <= BIT(0); // Fifo full: not ready anymore
			ENDIF

			IF ( (PORT_BASE(spi_dma_i).data_en) == BIT(1) ) THEN // REceived new byte from dma
				fifo_tx(TO_INTEGER(RANGE(fifo_wptr_tx, 1, 0))) <= PORT_BASE(spi_dma_i).data;
				fifo_wptr_tx <= fifo_wptr_tx_tmp;// + 1;
			ENDIF

			// Rx DMA --------------
			IF ( (not (fifo_wptr_rx == fifo_rptr_rx) ) and (dma_wait_for_grant == BIT(0)) ) THEN// stg in Rx fifo
				PORT_BASE(spi_dma_o).data <= fifo_rx(TO_INTEGER(RANGE(fifo_rptr_rx, 1, 0)));
				PORT_BASE(spi_dma_o).data_en <= BIT(1);
				dma_wait_for_grant <= BIT(1);
			ENDIF

			IF ( (dma_wait_for_grant == BIT(1)) and (PORT_BASE(spi_dma_i).grant == BIT(1)) ) THEN
				fifo_rptr_rx <= fifo_rptr_rx + 1;
				dma_wait_for_grant <= BIT(0);
				PORT_BASE(spi_dma_o).data_en <= BIT(0);
			ENDIF

			IF (eot == BIT(1)) THEN // Transfer has ended: reset those flags
				dma_tsfr <= BIT(0);
				dma_wait_for_grant <= BIT(0);
			ENDIF
		ELSE
			PORT_BASE(spi_dma_o).rdy <= BIT(0);
			PORT_BASE(spi_dma_o).data_en <= BIT(0);
		ENDIF

	ENDIF
END_PROCESS



// high speed process for acceptable asynchronous Rx speed
PROCESS(0, clk_120, reset_n) // ----------------------------------------------------------

VAR(next_cnt, UINT(LEN(cnt)));
VAR(fifo_empty_tx, BOOLEAN);
VAR(fifo_empty_rx, BOOLEAN);
VAR(rx_byte_tmp, UINT(8));
VAR(rx_trans , BOOLEAN);
VAR(do_rx , BOOLEAN);
VAR(do_tx , BOOLEAN);

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		cnt <= TO_UINT(0, LEN(cnt));
		ready <= BIT(1);
		spi_tx_o <= BIT(0);
		spi_state <= spi_idle;
		RESET(rx_byte);
		RESET(tx_byte);
		RESET(fifo_rptr_tx);
		RESET(fifo_wptr_rx);
		sending <= BIT(0);
		spi_rx <= BIT(0);
		spi_clk <= BIT(0);
		spi_clkp <= BIT(0);
		spi_csn <= BIT(0);
		spi_csnp <= BIT(0);
		cnt_test <= HEX(A5);
		fifo_init <= BIT(0);
		new_tx_byte <= BIT(0);
		clk_detected <= BIT(0);
		eot <= BIT(1);
		tx_data_available <= BIT(0);
	ELSEIF ( EVENT(clk_120) and (clk_120 == BIT(1)) ) THEN

		// Synchronize
		spi_rx <= spi_rx_i;
		spi_clk <= TO_LOGIC(PORT_BASE(spi_clk_io));
		spi_clkp <= spi_clk;
		spi_csnp <= spi_csn;

		do_rx := (not BIT2BOOL(is_master) or BIT2BOOL(tx_rx));
		do_tx := (BIT2BOOL(is_master) or BIT2BOOL(tx_rx));
		tx_data_available <= (not BOOL2BIT(fifo_wptr_tx == fifo_rptr_tx));

		IF (is_master == BIT(0)) THEN
			// Auto csn generation
			IF (auto_csn == BIT(0)) THEN
				spi_csn <= (not BOOL2BIT(PORT_BASE(spi_csn_io) == BIN(0))); //BIN2BIT(PORT_BASE(spi_csn_io));
			ELSEIF (eot == BIT(0)) THEN
				IF (fifo_init == BIT(1)) THEN // init cnt_auto_csn as well
					cnt_auto_csn <= csn_clk_ncycles;
					spi_csn <= BIT(1);
				ENDIF
				IF (spi_csn == BIT(1)) THEN

					cnt_auto_csn <= cnt_auto_csn - 1;
					IF (cnt_auto_csn == TO_UINT(0, LEN(cnt_auto_csn))) THEN
						cnt_auto_csn <= csn_clk_ncycles;
						spi_csn <= BIT(0);
						clk_detected <= BIT(0);

					ENDIF
				ELSE
					IF (clk_detected == BIT(1)) THEN
						cnt_auto_csn <= cnt_auto_csn - 1;
						IF (cnt_auto_csn == TO_UINT(0, LEN(cnt_auto_csn))) THEN
							cnt_auto_csn <= csn_clk_ncycles;
							spi_csn <= BIT(1);
							clk_detected <= BIT(0);
						ENDIF
					ENDIF
				ENDIF
				IF ( not (spi_clk == spi_clkp) ) THEN
					cnt_auto_csn <= csn_clk_ncycles;
					clk_detected <= BIT(1);
				ENDIF
			ENDIF
		ELSE //master

		ENDIF


		rx_trans := ( ( spi_clkp == clk_pol ) and not (spi_clk == spi_clkp) );
		// SPI FSM ------------------------------------
		IF (spi_state == spi_idle) THEN
			sending <= BIT(0);
			IF (fifo_init == BIT(1)) THEN
				fifo_rptr_tx <= BIN(000);
				fifo_wptr_rx <= BIN(000);
				fifo_init <= BIT(0);
			ENDIF
			IF (BIT2BOOL(is_master) and (tx_data_available == BIT(1))) THEN
				spi_csn <= BIT(0);
			ENDIF
			// wait either value written to data reg, or fifo full (dma)
			IF ( (spi_csn == BIT(0)) and (spi_csnp == BIT(1)) and not (eot == BIT(1)) ) THEN
				spi_state <= spi_start;
			ENDIF

		ELSEIF (spi_state == spi_wait) THEN // state used for delays
			next_cnt := cnt - TO_UINT(1, LEN(cnt));
			cnt <= next_cnt;
			// in slave mode, exit on clk toggle
			IF ( ( (cnt == TO_UINT(1, LEN(cnt))) /*and (sending == BIT(0))*/ ) or ( rx_trans and not BIT2BOOL(is_master)) ) THEN
				spi_state <= next_state;
				IF (next_state == spi_idle) THEN
					ready <= BIT(1);
					IF (BIT2BOOL(is_master)) THEN
						spi_csn_io <= BIN(1);					ENDIF
				ENDIF

				IF ( RANGE(tsfr_cnt, 2, 0) == BIN(111) ) THEN // must load a new data from fifo
					fifo_empty_tx := (fifo_wptr_tx == fifo_rptr_tx);

					IF ( do_tx and fifo_empty_tx and not (next_state == spi_idle) ) THEN // can't, end tsfr. If next state is idle, the ending processed has already begun
						spi_state <= spi_stop;
					ELSE
						new_tx_byte <= (not first_byte); // must not block shifting on 1st byte (but on 1st byte of subsequent bursts) //BIT(1);
						first_byte <= BIT(0);
						IF (new_tx_byte == BIT(0)) THEN // no remaining tx_byte from previous spi burst
							tx_byte <= fifo_tx(TO_INTEGER( RANGE(fifo_rptr_tx, 1, 0)) ); // msb is meaningless (for fifo full / empty disambiguation)
							tx_byte0 <= fifo_tx(TO_INTEGER( RANGE(fifo_rptr_tx, 1, 0)) ); // msb is meaningless (for fifo full / empty disambiguation)
							fifo_rptr_tx <= fifo_rptr_tx + 1;
						ENDIF
						tsfr_cnt <= TO_UINT(7, LEN(tsfr_cnt));
					ENDIF
				ELSE
					new_tx_byte <= BIT(0);
				ENDIF
			ENDIF

			// use next_cnt to have better clk shape for low div ratios
			IF ( BIT2BOOL(is_master) and (next_cnt == EXT(RANGE(div, HI(div), 1), LEN(cnt))) and (sending == BIT(1)) ) THEN // create master spi clk
				master_clk <= (not master_clk);
				spi_clk_io  <= BIT2BIN(not master_clk);
			ENDIF

			IF ( spi_csn == BIT(1) ) THEN
				spi_state <= spi_idle;
				cnt <= EXT(csn_clk_ncycles, LEN(cnt));
			ENDIF

		ELSEIF (spi_state == spi_start) THEN // csn low, then wait csn_clk_ncycles, then send
			tsfr_cnt <= TO_UINT(0, LEN(tsfr_cnt));
			IF (BIT2BOOL(is_master)) THEN
				cnt <= EXT(csn_clk_ncycles, LEN(cnt)); // don't care in slv mode: spi exits wait state on clk'event
				spi_csn_io <= BIN(0);
			ELSE
				cnt <= TO_UINT(1, LEN(cnt)); // will exit immediately to send state
			ENDIF

			spi_state <= spi_wait;
			next_state <= spi_send;
			// tsfr_cnt is the number of bits to transfer (can be renewed in case of dma tsfr)
			tsfr_cnt <= TO_UINT(7, LEN(tsfr_cnt));

		ELSEIF (spi_state == spi_send) THEN
			sending <= BIT(1);
			IF (new_tx_byte == BIT(1)) THEN
				cnt <= EXT(div, LEN(cnt)) + EXT(pause_ncycles, LEN(cnt));
			ELSE
				cnt <= EXT(div, LEN(cnt));
			ENDIF
			// When previous burst was interrupted, 1st bit of tx_byte was sent.
			// This condition is true in this only case, and this keeps from shifting once again at burst start.
			IF ( ( (new_tx_byte == BIT(0) ) or (sending == BIT(1)) ) and ((tx_rx or is_master)== BIT(1)) ) THEN
				spi_tx_o <= B(tx_byte,7);
				spi_tx <= B(tx_byte,7);
				tx_byte <= SHIFT_LEFT(tx_byte, 1);
			ENDIF
			spi_state <= spi_wait;
			next_state <= spi_send;
			tsfr_cnt <= tsfr_cnt - 1;
			IF (BIT2BOOL(is_master)) THEN
				spi_clk_io <= BIT2BIN(clk_pol);
				master_clk <= clk_pol;
			ENDIF
		ELSEIF (spi_state == spi_stop) THEN // should not end here (means fifo empty while receiving)
			sending <= BIT(0);
			cnt <= EXT(csn_clk_ncycles, LEN(cnt)); // vestigial of spi master
			spi_state <= spi_wait;
			next_state <= spi_idle;
			eot <= BIT(1);
			IF (BIT2BOOL(is_master)) THEN
				spi_clk_io <= BIT2BIN(clk_pol);
				master_clk <= clk_pol;
			ENDIF
		ENDIF

		//Receiver part
		rx_trans_dbg <= BOOL2BIT(rx_trans);
		//IF ( (sending == BIT(1)) and ( ( (BIT2BOOL(is_master) and (next_cnt == cnt_strobe)) ) or (not BIT2BOOL(is_master) and rx_trans) ) ) THEN//(next_cnt == cnt_strobe) and (sending == BIT(1)) ) THEN
		IF ( (sending == BIT(1)) and ( ( (BIT2BOOL(is_master and tx_rx) and (cnt == cnt_strobe)) ) or (not BIT2BOOL(is_master) and rx_trans) ) ) THEN//(next_cnt == cnt_strobe) and (sending == BIT(1)) ) THEN

			rx_byte_tmp := ( RANGE( rx_byte, 6, 0) & spi_rx );
			fifo_full_rx <= BOOL2BIT(fifo_wptr_rx == (fifo_rptr_rx + BIN(100)));

			IF ( ( tsfr_cnt == BIN(111) ) ) THEN // last tx bit is last rx bit as well
				//fifo_full_rx := (fifo_wptr_rx == (fifo_rptr_rx + BIN(100)));
				IF (not (fifo_full_rx == BIT(1))) THEN
					fifo_rx( TO_INTEGER( RANGE(fifo_wptr_rx, 1, 0)) ) <= rx_byte_tmp;
					fifo_wptr_rx <= fifo_wptr_rx + 1;
				ELSE
					eot <= BIT(1);
					spi_state <= spi_idle;
					spi_csn <= BIT(1);
				ENDIF
			ENDIF
			rx_byte <= rx_byte_tmp;
		ENDIF

		// Conf register write resets FSM
		IF (reset_spi == BIT(1)) THEN
			spi_state <= spi_idle;
			fifo_init <= BIT(1);
			new_tx_byte <= BIT(0);
			spi_csn <= BIT(1);
			eot <= BIT(0);
			fifo_wptr_rx <= BIN(000);
			fifo_full_rx <= BIT(0);
			first_byte <= BIT(1);
			ready <= BIT(0);
			// Configure as slave -> release control over clk and csn signals
			IF ( not BIT2BOOL(is_master) ) THEN
				PORT_BASE(spi_clk_io) <= "Z";
				PORT_BASE(spi_csn_io) <= "Z";
			ELSE // init master output signals
				PORT_BASE(spi_csn_io) <= BIN(1);
				PORT_BASE(spi_clk_io) <= BIT2BIN(clk_pol);
				spi_tx_o <= BIT(0);
			ENDIF
		ENDIF

	ENDIF
END_PROCESS

BLK_END;
