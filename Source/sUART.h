#include "slv.h"
START_OF_FILE(sUART)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(altera)

// Half-duplex simple UART

ENTITY(sUART,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
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
SIG(send_tx_byte, BIT_TYPE);
SIG(get_rx_byte, BIT_TYPE); //  Rx operation
SIG(uart_rx, BIT_TYPE); // prev value
SIG(rx_started, BIT_TYPE); //
SIG(ready, BIT_TYPE);
SIG(state, UINT(5)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit

BEGIN


// Is a declaration in C++, instantiation in VHDL(after begin)
//GATED_CLK(clk_g ,clk_peri, gate_cell);


PROCESS(0, clk_peri, reset_n) // ----------------------------------------------------------

VAR(DBG, UINT(8));
VAR(rx_trans, BIT_TYPE);
VAR(val, UINT(32));

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		cnt <= TO_UINT(0, LEN(cnt));
		div <= TO_UINT((434), LEN(div)); // assume 50Mhz clk and 115200bauds uart default
		ready <= BIT(1);
		uart_tx_o <= BIT(1);
		state <= BIN(00000);
		RESET(byte);
		uart_rx <= BIT(1);
		rx_started <= BIT(0);
		get_rx_byte  <= BIT(0);
		send_tx_byte <= BIT(0);
	ELSEIF ( EVENT(clk_peri) and (clk_peri == BIT(1)) ) THEN
		cnt <= cnt - TO_UINT(1, LEN(cnt));

		// Registers R/W
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, 12, 4) == BIN(111111111) ) ) THEN
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
				ENDIF
			ELSE
				// Read registers
				IF (PORT_BASE(core2mem_i).addr == BIN(1111111110000)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( div & TO_UINT(0, 7) & ready & byte );
				ENDIF
			ENDIF
		ENDIF

		// Uart Tx ---------------------------------------------------------------------
		IF ( (send_tx_byte == BIT(1)) and (cnt == TO_UINT(0, LEN(cnt))) ) THEN
			SWITCH((state))
				CASE(CASE_BIN(11000)) uart_tx_o <= BIT(0); cnt <= div; state <= BIN(00000); // start bit
				CASE(CASE_BIN(01000)) uart_tx_o <= BIT(1); cnt <= div; state <= BIN(10000); // stop bit
				CASE(CASE_BIN(10000)) uart_tx_o <= BIT(1); state <= BIN(11000); send_tx_byte <= BIT(0); ready <= BIT(1);// tx end
				DEFAULT cnt <= div; uart_tx_o <= B(byte,0); state <= state + TO_UINT(1, LEN(state)); byte <= SHIFT_RIGHT(byte, 1);//data bit
			ENDCASE
		ENDIF

		// Uart Rx -----------------------------------------------------------------------
		IF ( (get_rx_byte == BIT(1)) ) THEN
			rx_trans := ( uart_rx and not PORT_BASE(uart_rx_i) );
			uart_rx <= uart_rx_i;
			if ( rx_trans == BIT(1) ) THEN// Waiting for start bit or detecting bit edge
				cnt <= SHIFT_RIGHT(div, 1);
				rx_started <= BIT(1);
			ENDIF

			IF ( (rx_started == BIT(1)) and (cnt == TO_UINT(0, LEN(cnt))) ) THEN
				SWITCH((state))
					CASE(CASE_BIN(11000)) cnt <= div; state <= BIN(00000); // start bit
					CASE(CASE_BIN(01000)) cnt <= div; state <= BIN(10000); // stop bit
					CASE(CASE_BIN(10000)) state <= BIN(11000); ready <= BIT(1);// tx end
					DEFAULT cnt <= div; byte <= (SHIFT_RIGHT(byte, 1) or (uart_rx & BIN(0000000))); state <= state + TO_UINT(1, LEN(state));
				ENDCASE
			ENDIF

		ENDIF

	ENDIF
END_PROCESS

BLK_END;
