#include "../Include_libs/slv.h"


START_OF_FILE(sdram_ctrl)
INCLUDES
USE_PACKAGE(structures)

#define ras_latency 3
#define cas_latency 3
#define rc_latency 1
#define rp_latency 1 // 21 ns ?
#define rw_latency 1 // 2ns ?
#define refresh_latency 6
#define refresh_max TO_UINT(1024, 12)



ENTITY(sdram_ctrl,
DECL_PORTS(
		PORT(clk_sdram, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(req0_i, sdram_req_t, IN),
		PORT(req1_i, sdram_req_t, IN),
		PORT(data_i, UINT(16), IN),
		PORT(addr_o, UINT(12), OUT),
		PORT(ba_o, UINT(2), OUT),
		PORT(data_o, UINT(16), OUT),
		PORT(cke_o, BIT_TYPE, OUT),
		PORT(cs_o, BIT_TYPE, OUT),
		PORT(we_o, BIT_TYPE, OUT),
		PORT(cas_o, BIT_TYPE, OUT),
		PORT(ras_o, BIT_TYPE, OUT),
		PORT(en0_o, BIT_TYPE, OUT), // either data _en or data_request to initiator
		PORT(en1_o, BIT_TYPE, OUT),
		PORT(done0_o, BIT_TYPE, OUT), // either data _en or data_request to initiator
		PORT(done1_o, BIT_TYPE, OUT),
		PORT(dQm_o, UINT(2), OUT),
		PORT(dQ_io, TRISTATE(16), INOUT)
		)
//		, INTEGER generic_int
);


SIG(last_state, UINT(4));
SIG(state, UINT(4));
SIG(next_state, UINT(4));
SIG(wait_cnt, UINT(5)); //
SIG(burst_cnt, UINT(8)); // enough for a full page read
SIG(refresh_cnt, UINT(12));
SIG(device, UINT(1)); // selected device (arbitated)
SIG(en, BIT_TYPE); // selected device (arbitated)
SIG(read_burst, BIT_TYPE); //
SIG(write_burst, BIT_TYPE); //
SIG(read_burst_active, BIT_TYPE); //
SIG(write_burst_active, BIT_TYPE); //
SIG(row, UINT(12)); // enough for a full page read
SIG(col, UINT(8)); // enough for a full page read
SIG(bank, UINT(2)); // enough for a full page read
SIG(en_pipe, UINT(4));
SIG(dev_pipe, UINT(4));
SIG(first_data, BIT_TYPE); // selected device (arbitated)
SIG(sdram_dQ, TRISTATE(16));
SIG(req_test, sdram_req_t);
SIG(dbg, BIT_TYPE); // selected device (arbitated)
SIG(page_change_required, BIT_TYPE); // selected device (arbitated)

//SIG(burst_cnt, UINT(8)); // actual burst size minus 1

CONST(sdram_idle, UINT(4)) := TO_UINT(0, DMA_STATE_SZ);
CONST(sdram_wait, UINT(4)) := TO_UINT(1, DMA_STATE_SZ);
CONST(sdram_bank_activate, UINT(4)) := TO_UINT(2, DMA_STATE_SZ);
//CONST(sdram_wait_cas, UINT(4)) := TO_UINT(4, DMA_STATE_SZ);
//CONST(sdram_ras, UINT(4)) := TO_UINT(4, DMA_STATE_SZ);
CONST(sdram_burst, UINT(4)) := TO_UINT(10, DMA_STATE_SZ);
CONST(sdram_precharge, UINT(4)) := TO_UINT(11, DMA_STATE_SZ);

CONST(sdram_init, UINT(4)) := TO_UINT(14, DMA_STATE_SZ);
CONST(sdram_refresh, UINT(4)) := TO_UINT(15, DMA_STATE_SZ);

BEGIN


PROCESS(0, clk_sdram, reset_n)
VAR(stop_tsfr, BOOLEAN);
VAR(end_of_page, BOOLEAN);
VAR(will_refresh, BOOLEAN);
VAR(req, BASE_TYPE(sdram_req_t));



BEGIN

IF ( reset_n == BIT(0) ) THEN
	state <= sdram_idle; // ???sdram_wait;
	last_state <= sdram_idle;
	next_state <= sdram_init;
	wait_cnt <= BIN(11111);
	cke_o <= BIT(1); // always on, optimize later on
	cs_o <= BIT(0);
	we_o <= BIT(0);
	ras_o <= BIT(1);
	cas_o <= BIT(1);
	PORT_BASE(dQ_io) <= "ZZZZZZZZZZZZZZZZ";
	sdram_dQ  <= "ZZZZZZZZZZZZZZZZ";
	dQm_o <= BIN(00);
	en <= BIT(0);
	RESET(en_pipe);
	RESET(dev_pipe);
	RESET(data_o);
	RESET(refresh_cnt);
	RESET(burst_cnt);
	RESET(bank);
	RESET(col);
	RESET(row);
	first_data <= BIT(0);
	en0_o <= BIT(0);
	en1_o <= BIT(0);
	done0_o <= BIT(0);
	done1_o <= BIT(0);
	device <= BIN(0);
	read_burst_active <= BIT(0);
	write_burst_active <= BIT(0);
	read_burst <= BIT(0);
	write_burst <= BIT(0);

ELSEIF ( EVENT(clk_sdram) and (clk_sdram == BIT(1)) ) THEN

	refresh_cnt <= refresh_cnt + 1;
	data_o <= dQ_io;
	last_state <= state;
	dbg <= BIT(0);

	IF ( (state == sdram_idle) and (refresh_cnt > refresh_max) ) THEN
		state <= sdram_refresh;
		refresh_cnt <= TO_UINT(0, LEN(refresh_cnt));

	ELSEIF (state == sdram_wait) THEN
		wait_cnt <= wait_cnt - 1;
		IF ( not (last_state == state) ) THEN	 //once
			cas_o <= BIT(1); // NOP
			ras_o <= BIT(1);
			we_o <= BIT(1);
			dQm_o <= BIN(00);
			PORT_BASE(dQ_io) <= "ZZZZZZZZZZZZZZZZ";
			sdram_dQ <=  "ZZZZZZZZZZZZZZZZ";
			//gprintf("#UWAIT ZZZZ");
			dbg <= BIT(1);
		ENDIF

		IF (wait_cnt == TO_UINT(0, LEN(wait_cnt))) THEN
			state <= next_state;
			IF ( (next_state == sdram_burst) and (write_burst == BIT(1)) ) THEN // Must tell initiator to send data
				IF (device == TO_UINT(0, LEN(device))) THEN
					en0_o <= BIT(1);
					done0_o <= BIT(0);
				ELSE
					en1_o <= BIT(1);
					done1_o <= BIT(0);
				ENDIF
			ENDIF
			IF (next_state == sdram_burst) THEN
				first_data <= BIT(1);
				en <= BIT(1); // en for read en pipe
			ENDIF
		ENDIF

	ELSEIF (state == sdram_refresh) THEN
		wait_cnt <= TO_UINT(refresh_latency, LEN(wait_cnt));
		state <= sdram_wait;
		next_state <= sdram_idle;
		cas_o <= BIT(0);
		ras_o <= BIT(0);
		we_o <= BIT(1);

	ELSEIF (state == sdram_bank_activate) THEN
		ba_o <= bank;
		addr_o <= row;
		ras_o <= BIT(0);
		cas_o <= BIT(1);
		we_o <= BIT(1);
		PORT_BASE(dQ_io) <= "ZZZZZZZZZZZZZZZZ";
		sdram_dQ <= "ZZZZZZZZZZZZZZZZ";
		//gprintf("#UACT ZZZZ");
		dbg <= BIT(1);

		state <= sdram_wait;
		wait_cnt <= TO_UINT(rc_latency, LEN(wait_cnt));
		next_state <= sdram_burst;
		read_burst_active <= read_burst;
		write_burst_active <= write_burst;

	ELSEIF (state == sdram_burst) THEN
		addr_o <= ( BIN(0000) & col); // no auto precharge
		ras_o <= BIT(1);// read/write command then NOP
		cas_o <= (not first_data); // read/write command then NOP
		we_o <= (read_burst_active or not first_data);// read/write command then NOP
		first_data <= BIT(0);

		IF (write_burst_active == BIT(1)) THEN
			PORT_BASE(dQ_io) <= PORT_BASE(data_i);
			sdram_dQ <= PORT_BASE(data_i);
			//gprintf("#UNOT ZZZZ");

		ENDIF

		stop_tsfr := (burst_cnt == TO_UINT(0, LEN(burst_cnt)));
		end_of_page := (col == BIN(11111111));

		IF (end_of_page) THEN // must change page
			en0_o <= BIT(0); //overriden in read mode
			en1_o <= BIT(0);

			en <= BIT(0);
			row <= row + 1;
			IF (row == BIN(111111111111)) THEN
				bank <= bank + 1;
			ENDIF
			state <= sdram_precharge;
		ENDIF
		col <= col + 1;
		burst_cnt <= burst_cnt - 1;
		// Improve this code in order to deal with interleaved accesses
		// If tsfr is close to completion, without page change predicted, and if no refresh is required soon
		// then it would be safe to go to idle state to process the next command and initiate its precharging
		will_refresh := (refresh_cnt > (refresh_max-rp_latency));
		IF (stop_tsfr) THEN
			state <= sdram_precharge;
			next_state <= sdram_idle;
			wait_cnt <= TO_UINT(rp_latency-1, LEN(wait_cnt));
			// ????? next_state <= sdram_burst; // at least one idle state between bursts (idle state, process req)
			en <= BIT(0);
			en0_o <= BIT(0); //overriden in read mode
			en1_o <= BIT(0);
			IF (device == BIN(0)) THEN
				done0_o <= BIT(1);
			ELSE
				done1_o <= BIT(1);
			ENDIF
		ELSEIF (end_of_page) THEN // case tsfr end because of page end
			wait_cnt <= TO_UINT(rp_latency, LEN(wait_cnt)); // ??? no idle state between bursts
			next_state <= sdram_bank_activate;
			page_change_required <= BIT(0);
		ENDIF


	ELSEIF (state == sdram_precharge) THEN
		state <= sdram_wait;
		//wait_cnt <= rp_latency;
		PORT_BASE(dQ_io) <= "ZZZZZZZZZZZZZZZZ";
		sdram_dQ <=  "ZZZZZZZZZZZZZZZZ";
		dbg <= BIT(1);
		//gprintf("#UPRECH ZZZZ");
		IF (write_burst_active == BIT(1)) THEN
			dQm_o <= BIN(11);
		ENDIF
		read_burst_active <= BIT(0);
		write_burst_active <= BIT(0);
		ras_o <= BIT(0);
		cas_o <= BIT(1);
		we_o <= BIT(0);
		addr_o <= BIN(010000000000);

	ELSEIF (state == sdram_init) THEN //configure burst mode
		cas_o <= BIT(0); // NOP
		ras_o <= BIT(0);
		we_o <= BIT(0);
		addr_o <= BIN(000000110111);
		ba_o <= BIN(00);
		next_state <= sdram_idle;
		state <= sdram_wait;

		req_test.en <= BIT(1);
		req_test.addr <= BIN(0000000000000000100000);
		req_test.sz <= BIN(11111111);
		req_test.rnw <= BIT(1);

	ELSEIF 	(state == sdram_idle) THEN // receive request

		IF ( PORT_BASE(req0_i).en == BIT(1) ) THEN// port 0 has priority
			req := PORT_BASE(req0_i);
			device <= BIN(0);
		ELSEIF ( PORT_BASE(req1_i).en == BIT(1) ) THEN
			req := PORT_BASE(req1_i);
			device <= BIN(1);
		ELSEIF ( req_test.en == BIT(1) ) THEN
			req := (req_test);
			//req_test.en <= BIT(0);
			req_test.rnw <= not req_test.rnw;
			req_test.sz <= BIN(10000000);
			req_test.addr <= BIN(0011111111111111111100);
			device <= not device;
		ENDIF

		IF (req.en == BIT(1)) THEN
			read_burst <= req.rnw;
			write_burst <= not req.rnw;
			col <= RANGE(req.addr, 7, 0);
			row <= RANGE(req.addr, 19, 8);
			bank <= RANGE(req.addr, 21, 20);
			burst_cnt <= req.sz;
			state <= sdram_bank_activate;
			IF ( (EXT(RANGE(req.addr, 7, 0), 9) + EXT(req.sz, 9)) > TO_UINT(255,9) ) THEN
				page_change_required <= BIT(1);
			ELSE
				page_change_required <= BIT(0);
			ENDIF
		ENDIF

	ENDIF

	dev_pipe <= ( RANGE( dev_pipe, 2, 0) & device);
	en_pipe <= ( RANGE( en_pipe, 2, 0) & en);
	IF (read_burst == BIT(1)) THEN //sure that the pipeline might be used
		IF (B(dev_pipe, cas_latency-1) == BIT(1)) THEN
			en1_o <= B(en_pipe, cas_latency -1);
		ELSE
			en0_o <= B(en_pipe, cas_latency -1);
		ENDIF
	ENDIF

ENDIF



END_PROCESS



BLK_END;







