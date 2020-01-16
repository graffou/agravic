#include "../Include_libs/slv.h"


#define nCHANNELS 2
#define log2_nCHANNELS 1

START_OF_FILE(dma)
INCLUDES
USE_PACKAGE(structures)


ENTITY(dma,
DECL_PORTS(
		PORT(clk_dma, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN), // reg access
		PORT(mem2core_o, mem2blk_t, OUT), // reg access
		PORT(mem2dma_i, mem2blk_t, IN),

		// UART
		PORT(uart_dma_i, p2d_8_t, IN),
		PORT(uart_dma_o, d2p_8_t, OUT),

		PORT(core_grant_i, BIT_TYPE, IN),
		PORT(core_request_o, blk2mem_t, OUT)
		)
);

SIG(boot_mode, BIT_TYPE);

TYPE(dma_channels_t, ARRAY_TYPE(dma_channel_t,nCHANNELS)); // two dma channels
SIG(dma_channels, dma_channels_t);
SIG(cur_channel, UINT(log2_nCHANNELS));
SIG(wait_for_core_grant, BIT_TYPE);
SIG(rperiph_data_ens, UINT(8));
SIG(rperiph_data_rdys, UINT(8));
SIG(rmask_we, UINT(4));
SIG(rmask, UINT(32));
SIG(next_addr_mask, UINT(data_addr_span));
SIG(addr_mask, UINT(data_addr_span));

CONST(dma_idle, UINT(4)) := TO_UINT(0, DMA_STATE_SZ);
// periph -> mem
CONST(dma_wait_for_periph_data, UINT(4)) := TO_UINT(1, DMA_STATE_SZ);
CONST(dma_send_periph_grant, UINT(4)) := TO_UINT(2, DMA_STATE_SZ);
CONST(dma_write_mem, UINT(4)) := TO_UINT(3, DMA_STATE_SZ);
CONST(dma_wait_for_wr_grant, UINT(4)) := TO_UINT(4, DMA_STATE_SZ);
//CONST(dma_wait_send_to_mem, UINT(4)) := TO_UINT(2, DMA_STATE_SZ);

// mem -> periph
CONST(dma_read_mem, UINT(4)) := TO_UINT(8, DMA_STATE_SZ);
CONST(dma_wait_for_rd_grant, UINT(4)) := TO_UINT(9, DMA_STATE_SZ);
CONST(dma_wait_for_rd1, UINT(4)) := TO_UINT(10, DMA_STATE_SZ);
CONST(dma_wait_for_periph_rdy, UINT(4)) := TO_UINT(11, DMA_STATE_SZ);
CONST(dma_send_to_periph, UINT(4)) := TO_UINT(12, DMA_STATE_SZ);
CONST(dma_wait_for_ready, UINT(4)) := TO_UINT(13, DMA_STATE_SZ);
CONST(dma_pre_idle, UINT(4)) := TO_UINT(15, DMA_STATE_SZ); // to set data_en = 0 before idle

BEGIN



PROCESS(0, clk_dma, reset_n)

VAR(val, UINT(32));
VAR(priority, UINT(4));
VAR(periph_data, UINT(32));
VAR(periph_data_ens, UINT(8));
VAR(periph_data_rdys, UINT(8));
VAR(nshift2, UINT(2));
VAR(nshift, UINT(5));
VAR(mask, UINT(32));
VAR(mask_we, UINT(4));
VAR(next_addr, UINT(data_addr_span));

VAR(max_prio, UINT(DMA_WAIT_TIME_SZ));
VAR(sel_channel, UINT(log2_nCHANNELS));

BEGIN

IF ( reset_n == BIT(0) ) THEN

	FOR(idx, 0, nCHANNELS-1)
		dma_channels(idx).state <= dma_idle;
		dma_channels(idx).addr <= TO_UINT(0, data_addr_span);
		dma_channels(idx).tsfr_sz <= TO_UINT(0, DMA_TSFR_SZ);
	ENDLOOP

	wait_for_core_grant <= BIT(0);
	RESET(cur_channel);
	PORT_BASE(mem2core_o).data_en <= BIT(0);
	PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
	PORT_BASE(core_request_o).addr <= TO_UINT(0, data_addr_span-2);
	PORT_BASE(core_request_o).data <= TO_UINT(0, 32);
	PORT_BASE(core_request_o).be <= BIN(0000);
	PORT_BASE(core_request_o).cs_n <= '1';
	PORT_BASE(core_request_o).wr_n <= '1';
	PORT_BASE(uart_dma_o).data_en <= BIT(0);
	boot_mode <= BIT(0);

ELSEIF ( EVENT(clk_dma) and (clk_dma == BIT(1)) ) THEN
	boot_mode <= boot_mode_i;

	PORT_BASE(mem2core_o).data_en <= BIT(0);
	PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
	IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, 12, 4) == BIN(111111110) ) ) THEN
	// Write registers
		IF (PORT_BASE(core2mem_i).wr_n == BIT(0) ) THEN
			val := PORT_BASE(core2mem_i).data;

			// channel selection
			IF (PORT_BASE(core2mem_i).addr == BIN(1111111100000)) THEN
				cur_channel <= EXT(val, LEN(cur_channel));
			ENDIF
			IF (PORT_BASE(core2mem_i).addr == BIN(1111111100001)) THEN
				dma_channels(TO_INTEGER(cur_channel)).addr <= EXT(val, data_addr_span);//LEN(dma_channels(TO_INTEGER(cur_channel)).addr));
			ENDIF
			IF (PORT_BASE(core2mem_i).addr == BIN(1111111100010)) THEN
				dma_channels(TO_INTEGER(cur_channel)).tsfr_sz <= EXT(val, DMA_TSFR_SZ);//LEN(dma_channels(TO_INTEGER(cur_channel)).tsfr_sz));
			ENDIF
			IF (PORT_BASE(core2mem_i).addr == BIN(1111111100011)) THEN
				dma_channels(TO_INTEGER(cur_channel)).timeout <= EXT(val, DMA_TIME_SZ);//LEN(dma_channels(TO_INTEGER(cur_channel)).timeout));
			ENDIF
			IF (PORT_BASE(core2mem_i).addr == BIN(1111111100100)) THEN
				dma_channels(TO_INTEGER(cur_channel)).source <= RANGE(val, 2, 0);
				dma_channels(TO_INTEGER(cur_channel)).sink <= RANGE(val, 6, 4);
				dma_channels(TO_INTEGER(cur_channel)).auto_inc <= RANGE(val, 11, 8);
				dma_channels(TO_INTEGER(cur_channel)).periph_mask <= RANGE(val, 13, 12); // address mask for accesses (00 -> 32-bit, 10 -> 16-bit, 11 -> 8-bit)
				dma_channels(TO_INTEGER(cur_channel)).data_mask <= RANGE(val, 15, 14);
				priority := RANGE(val, 19, 16);
				dma_channels(TO_INTEGER(cur_channel)).priority <= priority;
				IF (  (RANGE(val, 3, 3) == BIN(1)) ) THEN // periph -> mem
					dma_channels(TO_INTEGER(cur_channel)).state <= dma_wait_for_periph_data;
				ELSE // mem -> periph
					dma_channels(TO_INTEGER(cur_channel)).state <= dma_read_mem;
				ENDIF
				dma_channels(TO_INTEGER(cur_channel)).wait_time <= EXT(priority, DMA_WAIT_TIME_SZ);//LEN(dma_channels(TO_INTEGER(cur_channel)).wait_time));
			ENDIF
		ELSE
			// Read registers
			IF (PORT_BASE(core2mem_i).addr == BIN(1111111101000)) THEN // read state
				PORT_BASE(mem2core_o).data_en <= BIT(1);
				PORT_BASE(mem2core_o).data <= EXT(dma_channels(TO_INTEGER(cur_channel)).state, 32);
			ENDIF
		ENDIF
	ENDIF

	// boot mode: prepare DMA for code loading
	IF ( (boot_mode_i == BIT(1)) and (boot_mode == BIT(0) ) ) THEN
		cur_channel <= TO_UINT(0, log2_nCHANNELS);
		dma_channels(0).source <= TO_UINT(0, 3);
		dma_channels(0).addr <= TO_UINT(0, data_addr_span);
		dma_channels(0).auto_inc <= TO_UINT(1, 4);
		dma_channels(0).periph_mask <= TO_UINT(3,2); // 8-bit
		dma_channels(0).data_mask <= TO_UINT(0,2); // 32-bit
		dma_channels(0).state <= dma_wait_for_periph_data;
	ENDIF
	IF ( (boot_mode == BIT(1)) and (boot_mode_i == BIT(0) ) ) THEN
		dma_channels(TO_INTEGER(cur_channel)).state <= dma_idle;
		PORT_BASE(core_request_o).cs_n <= '1';
		PORT_BASE(core_request_o).wr_n <= '1';
	ENDIF

	sel_channel := TO_UINT(0, LEN(sel_channel));
	max_prio := TO_UINT(0, LEN(max_prio));

	// Check requests from all channels, retain the ones with the highest cumulated priorities
	FOR(idx, 0, nCHANNELS-1)
		IF ( ( (dma_channels(idx).state == dma_write_mem) or (dma_channels(idx).state == dma_read_mem)) // channel request can be ussued
				and (dma_channels(idx).wait_time >= max_prio)  ) THEN // and this request has higher priority
			max_prio := dma_channels(idx).wait_time;
			sel_channel := TO_UINT(idx, log2_nCHANNELS);
		ENDIF
	ENDLOOP

	FOR(idx, 0, nCHANNELS-1)
		// DMA state machines
		IF (not (dma_channels(idx).state == dma_idle) ) THEN
			// Collect info from peripherals
			periph_data_ens  := (BIN(0000000) & PORT_BASE(uart_dma_i).data_en);//EXT(PORT_BASE(uart_dma_i).data_en, LEN(periph_data_ens));
			periph_data_rdys := (BIN(0000000) & PORT_BASE(uart_dma_i).rdy);//EXT(PORT_BASE(uart_dma_i).rdy, LEN(periph_data_rdys));
			rperiph_data_ens <= periph_data_ens; //dbg
			rperiph_data_rdys <= periph_data_rdys;
			// State wait for periph data-----------------------
			IF ( (dma_channels(idx).state == dma_wait_for_periph_data) ) THEN //and ( sel_channel == TO_UINT(sel_channel, log2_nCHANNELS) ) ) THEN
					//gprintf("BYo % %", periph_data_ens, B(periph_data_ens, TO_INTEGER(dma_channels(idx).source)));
				IF ( B(periph_data_ens, TO_INTEGER(dma_channels(idx).source)) == BIT(1) ) THEN // peripherals sends data

					// Select data from source and send grant to peripheral
					IF (dma_channels(idx).source == BIN(000)) THEN
						periph_data := EXT(PORT_BASE(uart_dma_i).data, 32);
						PORT_BASE(uart_dma_o).grant <= BIT(1);
					ENDIF
					// Fill
					nshift2 := RANGE(dma_channels(idx).addr, 1, 0);
					nshift := ( nshift2 & BIN(000) );
					// Mask already available data depending on peripheral data sz (00 -> 32-bit, 10 -> 16-bit, 11, 8-bit)
					mask := (SXT(RANGE(dma_channels(idx).periph_mask, 1, 1), 16) & SXT(RANGE(dma_channels(idx).periph_mask, 0, 0), 8) & BIN(00000000) );
					mask_we := ( (not B(mask, 31)) & (not B(mask,31)) & (not B(mask, 15)) & BIN(1) ); // F, 3 or 1
					rmask_we <= mask_we;//ROTATE_LEFT(mask, TO_INTEGER(nshift));
					rmask <= ROTATE_LEFT(mask, TO_INTEGER(nshift));
					dma_channels(idx).tmp_data <= ( ( dma_channels(idx).tmp_data and ROTATE_LEFT(mask, TO_INTEGER(nshift)) ) or SHIFT_LEFT(periph_data, TO_INTEGER(nshift)) );
					next_addr := dma_channels(idx).addr + EXT(dma_channels(idx).auto_inc, LEN(next_addr));
					dma_channels(idx).tmp_we <= ( dma_channels(idx).tmp_we or SHIFT_LEFT(mask_we, TO_INTEGER(nshift2)) ); // !!!!!!! To be reset on mem write !!!!!!!
					dma_channels(idx).tsfr_sz <= dma_channels(idx).tsfr_sz - 1;

					addr_mask <= (dma_channels(idx).addr and SXT(BIN(1) & dma_channels(idx).data_mask, LEN(next_addr)));
					next_addr_mask <= (next_addr and SXT(BIN(1) & dma_channels(idx).data_mask, LEN(next_addr)));
					// next address requires current data to be written to memory
					IF ( ( (next_addr and SXT(BIN(1) & dma_channels(idx).data_mask, LEN(next_addr))) == (dma_channels(idx).addr and SXT(BIN(1) & dma_channels(idx).data_mask, LEN(next_addr))) ) ) THEN
						dma_channels(idx).state <= dma_send_periph_grant;
					ELSE
						dma_channels(idx).state <= dma_write_mem;
					ENDIF
					dma_channels(idx).time <= dma_channels(idx).timeout;
				ELSE
					// Waiting for peripheral response: abort tranfer if timeout is exceeded
					dma_channels(idx).time <= dma_channels(idx).time - 1;
					IF ( ( dma_channels(idx).time == TO_UINT(0, DMA_TIME_SZ) ) and not ( dma_channels(idx).timeout == TO_UINT(0, DMA_TIME_SZ) ) ) THEN
						dma_channels(idx).state <= dma_idle;
					ENDIF
				ENDIF
			// Cycle after peripheral sent data: reset grant and issue mem write if required
			ELSEIF ( (dma_channels(idx).state == dma_send_periph_grant)  or (dma_channels(idx).state == dma_write_mem) ) THEN //and ( sel_channel == TO_UINT(sel_channel, log2_nCHANNELS) ) ) THEN

				// Reset grant
				IF (dma_channels(idx).source == BIN(000)) THEN
					PORT_BASE(uart_dma_o).grant <= BIT(0);
				ENDIF

				// increment address
				next_addr := dma_channels(idx).addr + EXT(dma_channels(idx).auto_inc, LEN(next_addr)); // once again
				//dma_channels(idx).addr <= next_addr;

				// Actual write to mem (channel is selected for write command issueing
				IF ( (dma_channels(idx).state == dma_write_mem) ) THEN
					IF ( sel_channel == TO_UINT(idx, log2_nCHANNELS) )  THEN // ok for mem access for this channel!
						PORT_BASE(core_request_o).addr <= RANGE(dma_channels(idx).addr, HI(next_addr), 2);
						PORT_BASE(core_request_o).data <= dma_channels(idx).tmp_data;
						PORT_BASE(core_request_o).be <= dma_channels(idx).tmp_we;
						PORT_BASE(core_request_o).cs_n <= '0';
						PORT_BASE(core_request_o).wr_n <= '0';
						dma_channels(idx).tmp_data <= TO_UINT(0, 32); // really required ?
						dma_channels(idx).tmp_we <= BIN(0000);
						dma_channels(idx).state <= dma_wait_for_wr_grant;
						dma_channels(idx).addr <= next_addr;
						dma_channels(idx).wait_time <= EXT(dma_channels(idx).priority, DMA_WAIT_TIME_SZ);
					ELSE // access not granted: increase priority
						dma_channels(idx).wait_time <= dma_channels(idx).wait_time + EXT(dma_channels(idx).priority, DMA_WAIT_TIME_SZ);
					ENDIF
				ENDIF

				// requires some more peripheral data before actual mem wriite
				IF (dma_channels(idx).state == dma_send_periph_grant) THEN
					dma_channels(idx).state <= dma_wait_for_periph_data;
					dma_channels(idx).addr <= next_addr;
				ENDIF

			ELSEIF ( (dma_channels(idx).state == dma_wait_for_wr_grant) ) THEN
				// write access transmitted by core
				IF (core_grant_i == BIT(1)) THEN
					PORT_BASE(core_request_o).cs_n <= '1';
					IF (dma_channels(idx).tsfr_sz == TO_UINT(0, DMA_TSFR_SZ)) THEN // End of transfer
						dma_channels(idx).state <= dma_idle;
					ELSE
						dma_channels(idx).state <= dma_wait_for_periph_data; // wait for next periph data
					ENDIF
				ENDIF


			// ---------- read from mem -> periph --------------------

			ELSEIF (dma_channels(idx).state == dma_read_mem) THEN
				IF ( sel_channel == TO_UINT(idx, log2_nCHANNELS) ) THEN
					PORT_BASE(core_request_o).addr <= RANGE(dma_channels(idx).addr, HI(next_addr), 2);
					PORT_BASE(core_request_o).data <= dma_channels(idx).tmp_data;
					PORT_BASE(core_request_o).be <= dma_channels(idx).tmp_we; // don't care
					PORT_BASE(core_request_o).cs_n <= '0';
					PORT_BASE(core_request_o).wr_n <= '1';
					dma_channels(idx).state <= dma_wait_for_rd_grant;
					dma_channels(idx).wait_time <= EXT(dma_channels(idx).priority, DMA_WAIT_TIME_SZ);
				ELSE
					dma_channels(idx).wait_time <= dma_channels(idx).wait_time + EXT(dma_channels(idx).priority, DMA_WAIT_TIME_SZ);
				ENDIF
				PORT_BASE(uart_dma_o).data_en <= BIT(0);

			ELSEIF ( (dma_channels(idx).state == dma_wait_for_rd_grant) ) THEN
				IF (core_grant_i == BIT(1)) THEN
					dma_channels(idx).state <= dma_wait_for_rd1;
					PORT_BASE(core_request_o).cs_n <= '1';
				ENDIF
			ELSEIF ( (dma_channels(idx).state == dma_wait_for_rd1) ) THEN
				dma_channels(idx).tmp_data <= PORT_BASE(mem2dma_i).data;
				dma_channels(idx).state <= dma_wait_for_periph_rdy;
			ELSEIF ( (dma_channels(idx).state == dma_wait_for_periph_rdy) ) THEN
				IF ( B(periph_data_rdys, TO_INTEGER(dma_channels(idx).sink)) == BIT(1) ) THEN	// peripheral ready to receive
					dma_channels(idx).tsfr_sz <= dma_channels(idx).tsfr_sz - 1;
					dma_channels(idx).state <= dma_send_to_periph;
				ENDIF
			ELSEIF ( (dma_channels(idx).state == dma_send_to_periph) ) THEN
				nshift2 := RANGE(dma_channels(idx).addr, 1, 0);
				nshift := ( nshift2 & BIN(000) );
				periph_data := SHIFT_RIGHT(dma_channels(idx).tmp_data, TO_INTEGER(nshift));
				IF (dma_channels(idx).source == BIN(000)) THEN
					PORT_BASE(uart_dma_o).data <= EXT(periph_data,8);
					PORT_BASE(uart_dma_o).data_en <= BIT(1);
				ENDIF

				next_addr := dma_channels(idx).addr + EXT(dma_channels(idx).auto_inc, LEN(next_addr));


				IF (dma_channels(idx).tsfr_sz == TO_UINT(0, DMA_TSFR_SZ)) THEN // End of transfer
					dma_channels(idx).state <= dma_pre_idle;
				ELSE
					dma_channels(idx).addr <= next_addr;

					IF ( ( (next_addr and SXT(BIN(1) & dma_channels(idx).data_mask, LEN(next_addr))) == (dma_channels(idx).addr and SXT(BIN(1) & dma_channels(idx).data_mask, LEN(next_addr))) ) ) THEN
						dma_channels(idx).state <= dma_wait_for_ready; // wait for next periph data
					ELSE
						dma_channels(idx).state <= dma_read_mem; // wait for next periph data
					ENDIF
				ENDIF
			ELSEIF ( (dma_channels(idx).state == dma_wait_for_ready) ) THEN	// let extra cycle to the peripheral to set its ready
				dma_channels(idx).state <= dma_wait_for_periph_rdy; // wait for next periph data
				PORT_BASE(uart_dma_o).data_en <= BIT(0);
			ELSE // idle !!!!! NOOOOO !!!! not coming here if idle
				IF (dma_channels(idx).source == BIN(000)) THEN
					periph_data := EXT(PORT_BASE(uart_dma_i).data, 32);
					PORT_BASE(uart_dma_o).data_en <= BIT(0);
				ENDIF

			ENDIF
		ELSE // idle

		ENDIF
	ENDLOOP

ENDIF
END_PROCESS

BLK_END;
