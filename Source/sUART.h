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
		PORT(uart_dma_i, d2p_8_t, IN),
		PORT(uart_dma_o, p2d_8_t, OUT),
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN)
		)
);

//SIG(cnt, UINT(32));
//SIG(gate_cell, BIT_TYPE);
// does nothing in C++, mandatory for VHDL generation
//DECL_GATED_CLK(clk_g);
SIG(div, UINT(16));
SIG(cnt, UINT(16));
SIG(conf, UINT(32));
SIG(byte, UINT(8));
SIG(byte0, UINT(8));
SIG(send_tx_byte, BIT_TYPE);
SIG(get_rx_byte, BIT_TYPE); //  Rx operation
SIG(uart_rx, BIT_TYPE); // prev value
SIG(rx_started, BIT_TYPE); //
SIG(ready, BIT_TYPE);
SIG(boot_mode, BIT_TYPE);
SIG(state, UINT(5)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
//SIG(always_tx, UINT(1)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit

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
		cnt <= TO_UINT(0, LEN(cnt));
		boot_mode <= BIT(0);
#ifndef VHDL
	// speedup sim
		div <= TO_UINT((1), LEN(div)); // assume 48Mhz clk and 115200bauds uart default
		//div <= TO_UINT((417), LEN(div)); // assume 48Mhz clk and 115200bauds uart default
#else
		div <= TO_UINT((417), LEN(div)); // assume 48Mhz clk and 115200bauds uart default
#endif
		ready <= BIT(1);
		uart_tx_o <= BIT(1);
		//state <= BIN(00000);
		state <= TO_UINT(24, LEN(state));
		RESET(byte);
		uart_rx <= BIT(1);
		rx_started <= BIT(0);
		get_rx_byte  <= BIT(1);
		send_tx_byte <= BIT(0);
		//always_tx <= BIN(1);
	ELSEIF ( EVENT(clk_peri) and (clk_peri == BIT(1)) ) THEN
		cnt <= cnt - TO_UINT(1, LEN(cnt));
		reset_data_en := BIT(0);
		boot_mode <= boot_mode_i;
		IF ( (boot_mode == BIT(0)) and (boot_mode_i == BIT(1))) THEN
			reset_data_en := BIT(1); // otherwise the DMA could catch an older data
		ENDIF
		// Registers R/W
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, data_addr_span - 3, 4) == BIN(111111111) ) ) THEN
			// Write registers
			IF (PORT_BASE(core2mem_i).wr_n == BIT(0) ) THEN
				IF (PORT_BASE(core2mem_i).addr == BIN(1111111110000)) THEN
					val := PORT_BASE(core2mem_i).data;
					div <= RANGE(val, 31, 16);
					byte <= RANGE(val, 7, 0);
					get_rx_byte  <= B(val, 8); // cancel rx
					send_tx_byte <= not B(val, 8);
					RESET(cnt);
					state <= TO_UINT(24, LEN(state));
					rx_started <= BIT(0);
					ready <= BIT(0);
					reset_data_en := BIT(1); //PORT_BASE(uart_dma_o).data_en <= BIT(0); // Essential! otherwise DMA tsfr would start with an old data!
				ENDIF
			ELSE
				// Read registers
				IF (PORT_BASE(core2mem_i).addr == BIN(1111111110000)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( div & TO_UINT(0, 7) & ready & byte );
				ENDIF
			ENDIF
		ENDIF

		/*
		// To see whether this thing is alive on pin 13 or 14
		IF (always_tx == BIN(1) and (ready == BIT(1))) THEN
			byte <= byte0;
			byte0 <= byte0 + 1;
			send_tx_byte <= BIT(1);
			RESET(cnt);
			state <= TO_UINT(24, LEN(state));
			rx_started <= BIT(0);
			ready <= BIT(0);
		ENDIF
		 */

		//DMA ---------------------------
		IF ( PORT_BASE(uart_dma_i).data_en == BIT(1) ) THEN
			send_tx_byte <= BIT(1);
			get_rx_byte <= BIT(0);
			byte <= PORT_BASE(uart_dma_i).data;
			RESET(cnt);
			ready <= BIT(0);
			PORT_BASE(uart_dma_o).rdy <= BIT(0);
		ENDIF


		// Uart Tx ---------------------------------------------------------------------
		IF ( (send_tx_byte == BIT(1)) and (cnt == TO_UINT(0, LEN(cnt))) ) THEN
			SWITCH((state))
				CASE(CASE_BIN(11000)) uart_tx_o <= BIT(0); cnt <= div; state <= BIN(00000); // start bit
				CASE(CASE_BIN(01000)) uart_tx_o <= BIT(1); cnt <= div; state <= BIN(10000); // stop bit
				CASE(CASE_BIN(10000)) uart_tx_o <= BIT(1); state <= BIN(11000); send_tx_byte <= BIT(0); get_rx_byte <= BIT(1); ready <= BIT(1);PORT_BASE(uart_dma_o).rdy <= BIT(1);// tx end
				DEFAULT cnt <= div; uart_tx_o <= B(byte,0); state <= state + TO_UINT(1, LEN(state)); byte <= SHIFT_RIGHT(byte, 1);//data bit
			ENDCASE
		ENDIF

		// Uart Rx -----------------------------------------------------------------------
		IF ( (get_rx_byte == BIT(1)) ) THEN
			rx_trans := ( uart_rx and not PORT_BASE(uart_rx_i) );
			if ( (PORT_BASE(uart_dma_i).grant == BIT(1)) or (reset_data_en == BIT(1)) ) THEN
				PORT_BASE(uart_dma_o).data_en <= BIT(0);
			ENDIF
			//PORT_BASE(uart_dma_o).data_en <= ( PORT_BASE(uart_dma_o).data_en and not PORT_BASE(uart_dma_i).grant and not reset_data_en); // reset data_en as soon as DMA granted
			uart_rx <= uart_rx_i;
			if ( rx_trans == BIT(1) ) THEN// Waiting for start bit or detecting bit edge
				cnt <= SHIFT_RIGHT(div, 1);
				rx_started <= BIT(1);
			ENDIF

			IF ( (rx_started == BIT(1)) and (cnt == TO_UINT(0, LEN(cnt))) ) THEN
				SWITCH((state))
					CASE(CASE_BIN(11000)) cnt <= div; state <= BIN(00000); // start bit
					CASE(CASE_BIN(01000)) RESET(cnt); state <= BIN(10000); // stop bit
					CASE(CASE_BIN(10000)) state <= BIN(11000); ready <= BIT(1); PORT_BASE(uart_dma_o).data <= byte; PORT_BASE(uart_dma_o).data_en <= BIT(1); rx_started <= BIT(0);// rx end
					DEFAULT cnt <= div; byte <= (SHIFT_RIGHT(byte, 1) or (uart_rx & BIN(0000000))); state <= state + TO_UINT(1, LEN(state));
				ENDCASE
			ENDIF

		ENDIF

	ENDIF
END_PROCESS

BLK_END;
