#include "../Include_libs/slv.h"

START_OF_FILE(sUART)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(altera)

// Half-duplex simple UART

ENTITY(sUART,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN), // have to reset the UART for DMA
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(irq_o, BIT_TYPE, OUT),
		PORT(uart_dma_i, d2p_8_t, IN),
		PORT(uart_dma_o, p2d_8_t, OUT),
		PORT(uart_rts_o, BIT_TYPE, OUT),
		PORT(uart_cts_i, BIT_TYPE, IN),
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN)
		)
		, INTEGER generic_int
);

//SIG(cnt, UINT(32));
//SIG(gate_cell, BIT_TYPE);
// does nothing in C++, mandatory for VHDL generation
//DECL_GATED_CLK(clk_g);
SIG(div, UINT(16));
SIG(rx_cnt, UINT(16));
SIG(tx_cnt, UINT(16));
SIG(conf, UINT(32));
SIG(rx_byte, UINT(8));
SIG(tx_byte, UINT(8));
SIG(byte0, UINT(8));
SIG(send_tx_byte, BIT_TYPE);
SIG(get_rx_byte, BIT_TYPE); //  Rx operation
SIG(uart_rx, BIT_TYPE); // prev value
SIG(uart_rxp, BIT_TYPE); // prev value
SIG(uart_tx, BIT_TYPE); // prev value
SIG(rx_started, BIT_TYPE); //
SIG(dma_req, BIT_TYPE); //
SIG(rx_ready, BIT_TYPE);
SIG(tx_ready, BIT_TYPE);
SIG(end_rx_byte, BIT_TYPE);
SIG(boot_mode, BIT_TYPE);
SIG(rx_state, UINT(5)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
SIG(tx_state, UINT(5)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
//SIG(always_tx, UINT(1)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
CONST(reg_base_addr, UINT(LEN(PORT_BASE(core2mem_i).addr))) := TO_UINT(generic_int, LEN(PORT_BASE(core2mem_i).addr));
SIG(base_addr_test, UINT(LEN(PORT_BASE(core2mem_i).addr)));
CONST(reg_addr_lsbs, INTEGER) := ( generic_int / 268435456);
SIG(addr_lsbs_test, UINT(4));

SIG(base_addr_ok, BIT_TYPE);
SIG(addr_ok, BIT_TYPE);
SIG(titi, CPX_INT(16));
SIG(titip, CPX_INT(16));
SIG(toti, CPX_INT(6));
SIG(tata, INT(6));
SIG(tete, UINT(2));
TYPE(fifo_t, ARRAY_TYPE(CPX_INT(16), 4)); // two dma channels
SIG(tutu, fifo_t);
SIG(tito, INT(12));
SIG(tito1, UINT(8));
SIG(tito2, INT(8));
SIG(tito3, INT(8));
SIG(tito4, INT(8));
SIG(tito5, INT(8));
SIG(tito6, INT(8));
SIG(tito7, INT(8));
SIG(tito8, INT(8));
SIG(tito9, CPX_INT(16));
SIG(tito10, UINT(7));
SIG(tito11, UINT(16));

BEGIN


// Is a declaration in C++, instantiation in VHDL(after begin)
//GATED_CLK(clk_g ,clk_peri, gate_cell);


PROCESS(0, clk_peri, reset_n) // ----------------------------------------------------------

VAR(rx_trans, BIT_TYPE);
VAR(val, UINT(32));
VAR(reset_data_en, BIT_TYPE);
VAR(DBG, UINT(8));

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		rx_cnt <= TO_UINT(0, LEN(rx_cnt));
		tx_cnt <= TO_UINT(0, LEN(tx_cnt));
		boot_mode <= BIT(0);
#if 0 //ndef VHDL
	// speedup sim
		div <= TO_UINT((1), LEN(div)); // assume 48Mhz clk and 115200bauds uart default
		//div <= TO_UINT((417), LEN(div)); // assume 48Mhz clk and 115200bauds uart default
#else
		div <= TO_UINT((417), LEN(div)); // assume 48Mhz clk and 115200bauds uart default
#endif
		rx_ready <= BIT(0);
		dma_req <= BIT(0);
		uart_rts_o <= BIT(0);
		tx_ready <= BIT(1);
		uart_tx_o <= BIT(1);
		//state <= BIN(00000);
		rx_state <= TO_UINT(24, LEN(rx_state));
		tx_state <= TO_UINT(24, LEN(tx_state));
		RESET(tx_byte);
		RESET(rx_byte);
		uart_rx <= BIT(1);
		uart_rxp <= BIT(1);
		uart_tx <= BIT(1);
		end_rx_byte <= BIT(0);
		rx_started <= BIT(0);
		get_rx_byte  <= BIT(1);
		send_tx_byte <= BIT(0);
		base_addr_test <= reg_base_addr;
		addr_lsbs_test <= TO_UINT(reg_addr_lsbs, 4);
		irq_o <= BIT(0);
		PORT_BASE(uart_dma_o).rdy <= BIT(1);
		titi <= TO_CPX_INT(0, 0, LEN(titi));
		titip <= TO_CPX_INT(0, 1, LEN(titi));
		toti <= TO_CPX_INT(-4, 4, LEN(toti));
		tata <= SIGNED(TO_UINT(3, LEN(tata)));
		//always_tx <= BIN(1);
	ELSEIF ( EVENT(clk_peri) and (clk_peri == BIT(1)) ) THEN
		//cnt <= cnt - TO_UINT(1, LEN(cnt));
		titip <= titi;
		titi <= CSXT( (titi + TO_CPX_INT(1, 4, LEN(titi))) * toti, LEN(titi) ) + SSXT(tata, LEN(titi)/2) + TO_CPX_INT(2, -1, LEN(titi));
		IF (titi == titip) THEN
			titi <= titi + 1;
		ENDIF
		tete <= tete + 1;
		//tutu(TO_INTEGER(tete)) <= titi;
		tutu <= tutu.shift<ARRAY_LEN(tutu)>(Signed<4>(1));
		tutu(0) <= titi;
		tito <= tito + 1;
		//tito1 <= tito.psat<8>();
		tito1 <= PSAT(tito, 8);//tito.psat<8>();
		tito2 <= tito.pos_sat<8>();
		tito3 <= SAT(tito, 8);//tito.sat<8>();
		tito4 <= SYM_SAT(tito, 8);//tito.sym_sat<8>();
		tito5 <= VAR_TRUNC_STD(tito, TO_UINT(4, 4), 8);//tito.trunc_std<8>(slv<4>(4));
		tito6 <= VAR_SROUND(tito, TO_UINT(4, 4), 8);//tito.sround<8>(slv<4>(4));
		tito7 <= VAR_SROUND_SAT(tito, TO_UINT(4, 4), 8);//tito.sround_sat<8>(slv<4>(4));
		tito8 <= SROUND_SAT(tito, 4);//tito.sround_sat<4>();
		tito9 <= (re(titi) + ii * im(titi));
		tito10 <= tito8.sabs();
		tito11 <= tito9.sabs();

		rx_cnt <= rx_cnt - 1;
		tx_cnt <= tx_cnt - 1;
		reset_data_en := BIT(0);
		boot_mode <= boot_mode_i;
		IF ( (boot_mode == BIT(0)) and (boot_mode_i == BIT(1))) THEN
			reset_data_en := BIT(1); // otherwise the DMA could catch an older data
		ENDIF
		// Registers R/W
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		//IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, data_addr_span - 3, 4) == BIN(111111111) ) ) THEN
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, HI(PORT_BASE(core2mem_i).addr), REG_NBITS) == RANGE( reg_base_addr, HI(reg_base_addr), REG_NBITS) ) ) THEN
			base_addr_ok <= BIT(1);
			// Write registers
			IF (PORT_BASE(core2mem_i).wr_n == BIT(0) ) THEN
			//IF (PORT_BASE(core2mem_i).addr == BIN(1111111110000)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					addr_ok <= BIT(1);
					val := PORT_BASE(core2mem_i).data;
					div <= RANGE(val, 31, 16);
					IF (B(val, 8) == BIT(1)) THEN //config for Rx byte receive
						RESET(rx_cnt);
						rx_state <= TO_UINT(24, LEN(rx_state));
						rx_ready <= BIT(0);
						rx_started <= BIT(0);
						uart_rts_o <= BIT(0);
						end_rx_byte <= BIT(0);

					ELSE
						tx_state <= TO_UINT(24, LEN(tx_state));
						RESET(tx_cnt);
						tx_ready <= BIT(0);
						tx_byte <= RANGE(val, 7, 0);
					ENDIF
					get_rx_byte  <= not B(val, 9); // cancel rx
					send_tx_byte <= not B(val, 8);
					reset_data_en := BIT(1); //PORT_BASE(uart_dma_o).data_en <= BIT(0); // Essential! otherwise DMA tsfr would start with an old data!
				ENDIF
			ELSE
				// Read registers
				//IF (PORT_BASE(core2mem_i).addr == BIN(1111111110000)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( div & TO_UINT(0, 6) & tx_ready & rx_ready & rx_byte );
				ENDIF
			ENDIF
		ENDIF


		//DMA ---------------------------
		// No condition over this: the DMA takes control
		IF ( PORT_BASE(uart_dma_i).data_en == BIT(1) ) THEN
			send_tx_byte <= BIT(1);
			//get_rx_byte <= BIT(0);
			tx_byte <= PORT_BASE(uart_dma_i).data;
			RESET(tx_cnt);
			tx_ready <= BIT(0);
			PORT_BASE(uart_dma_o).rdy <= BIT(0);
		ENDIF

		irq_o <= BIT(0);
		// Tx byte FSM---------------------------------------------------------------------
		IF ( (send_tx_byte == BIT(1)) and (tx_cnt == TO_UINT(0, LEN(tx_cnt))) ) THEN
			SWITCH((tx_state))
				CASE(CASE_BIN(11000)) 
					PORT_BASE(uart_dma_o).rdy <= BIT(0);
					IF ( uart_cts_i == BIT(0) ) THEN
						uart_tx_o <= BIT(0); tx_cnt <= div; tx_state <= BIN(00000); // start bit
					ENDIF	
				CASE(CASE_BIN(01000)) uart_tx_o <= BIT(1); tx_cnt <= div; tx_state <= BIN(10000); // stop bit
				CASE(CASE_BIN(10000)) uart_tx_o <= BIT(1); tx_state <= BIN(11000); send_tx_byte <= BIT(0); tx_ready <= BIT(1); PORT_BASE(uart_dma_o).rdy <= BIT(1); irq_o <= BIT(1);// tx end
				DEFAULT tx_cnt <= div; uart_tx_o <= B(tx_byte,0); tx_state <= tx_state + TO_UINT(1, LEN(tx_state)); tx_byte <= SHIFT_RIGHT(tx_byte, 1);//data bit
			ENDCASE
		ENDIF

		// Uart Rx FSM -----------------------------------------------------------------------
		IF ( (get_rx_byte == BIT(1)) ) THEN
			// no synchronization, is that critical? A little bit of sync finally
			IF (rx_state == BIN(11000)) THEN
				rx_trans := ( (not uart_rx) and uart_rxp );
			ELSE
				rx_trans := ( uart_rx and not uart_rxp); // PORT_BASE(uart_rx_i) );
			ENDIF
			if ( (PORT_BASE(uart_dma_i).grant == BIT(1)) or (reset_data_en == BIT(1)) ) THEN
				PORT_BASE(uart_dma_o).data_en <= BIT(0);
			ENDIF
			//PORT_BASE(uart_dma_o).data_en <= ( PORT_BASE(uart_dma_o).data_en and not PORT_BASE(uart_dma_i).grant and not reset_data_en); // reset data_en as soon as DMA granted
			uart_rx <= uart_rx_i;
			uart_rxp <= uart_rx;
			if ( rx_trans == BIT(1) ) THEN// Waiting for start bit or detecting bit edge
				rx_cnt <= SHIFT_RIGHT(div, 1);
				rx_started <= BIT(1);
				dma_req <=  PORT_BASE(uart_dma_i).req;
				end_rx_byte <= BIT(0);
			ENDIF
			
			// if last rx byte was from dma and req is still active, keep rts to 0, else 1 (end of tsfr)
			IF ( (end_rx_byte == BIT(1)) ) THEN
				uart_rts_o <= ( not (dma_req and PORT_BASE(uart_dma_i).req) );
			ENDIF
			// Byte receive FSM
			IF ( (rx_started == BIT(1)) and (rx_cnt == TO_UINT(0, LEN(rx_cnt))) ) THEN
				SWITCH((rx_state))
					CASE(CASE_BIN(11000)) rx_cnt <= div; rx_state <= BIN(00000); // start bit
					CASE(CASE_BIN(01000)) RESET(rx_cnt); rx_state <= BIN(10000); // stop bit
					CASE(CASE_BIN(10000)) rx_state <= BIN(11000); rx_ready <= BIT(1); PORT_BASE(uart_dma_o).data <= rx_byte; 
											PORT_BASE(uart_dma_o).data_en <= BIT(1); rx_started <= BIT(0); irq_o <= BIT(1);// rx end
											uart_rts_o <= (not PORT_BASE(uart_dma_i).req);
											end_rx_byte <= BIT(1);
					DEFAULT rx_cnt <= div; rx_byte <= (SHIFT_RIGHT(rx_byte, 1) or (uart_rxp & BIN(0000000))); rx_state <= rx_state + TO_UINT(1, LEN(rx_state));
				ENDCASE
			ENDIF

		ENDIF

	ENDIF
END_PROCESS

BLK_END;
