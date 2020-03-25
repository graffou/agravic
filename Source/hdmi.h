// HDMI output - december 2019
#include "../Include_libs/slv.h"

START_OF_FILE(hdmi)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(slv_utils)


ENTITY(hdmi,
DECL_PORTS(
		PORT(clk_hdmi, CLK_TYPE, IN),
		PORT(clk_core, CLK_TYPE, IN),
		PORT(clk_pix, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN),
		PORT(trap_i, BIT_TYPE, IN), //
		PORT(dbg_i, UINT(33), IN), //
		PORT(core2mem_i, blk2mem_t, IN), // reg access
		PORT(mem2core_o, mem2blk_t, OUT), // reg access
		PORT(pclk_o, BIT_TYPE, OUT),
		PORT(red_o, BIT_TYPE, OUT),
		PORT(green_o, BIT_TYPE, OUT),
		PORT(blue_o, BIT_TYPE, OUT)
		)
		, INTEGER generic_int

);


COMPONENT(ddio,
DECL_PORTS(
		PORT(clk_hdmi, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(pclk_i, UINT(2), IN),
		PORT(red_i, UINT(2), IN),
		PORT(green_i, UINT(2), IN),
		PORT(blue_i, UINT(2), IN),

		PORT(pclk_o, BIT_TYPE, OUT),
		PORT(red_o, BIT_TYPE, OUT),
		PORT(green_o, BIT_TYPE, OUT),
		PORT(blue_o, BIT_TYPE, OUT)
		)
);

COMPONENT(spram_4800x8,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, BIT_TYPE, IN),
		PORT(addr_i, UINT(13), IN),
		PORT(data_i, UINT(8), IN),
		PORT(data_o, UINT(8), OUT),
		PORT(wen_i, BIT_TYPE, IN)
		)
);

COMPONENT(spram_font,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, BIT_TYPE, IN),
		PORT(addr_i, UINT(12), IN),
		PORT(data_i, UINT(8), IN),
		PORT(wen_i, BIT_TYPE, IN),
		PORT(data_o, UINT(8), OUT)
		)
);

SIG(rdc, UINT(10)); // 3for debug
SIG(rdc_tmds_encoded, UINT(4)); // 3for debug
SIG(ruse_xor, UINT(1));
SIG(rsum_ones, UINT(4));
SIG(rdata8, UINT(8));
SIG(rdata_raw, UINT(8));
SIG(rdata10, UINT(10));
SIG(rdataxor, UINT(10));
SIG(tmds_encoded, UINT(10));
SIG(rdbg, BIT_TYPE);// debug
SIG(rgb_data, UINT(24));
SIG(rgb_data2, UINT(24));
SIG(data_tmds, UINT(30));
SIG(hcnt, UINT(16));
SIG(vcnt, UINT(16));
SIG(rgb, UINT(3)); // 3-state TMDS encoding (RGB)
SIG(pipe, UINT(7)); // serializer pipe
SIG(r14, UINT(14)); //
SIG(g12, UINT(12)); //
SIG(b10, UINT(10)); //
SIG(sr_pclk, UINT(10)); //
SIG(blue, UINT(2)); //
SIG(red, UINT(2)); //
SIG(green, UINT(2)); //
SIG(pclk, UINT(2)); //

// to create nice color effects
SIG(rcnt, UINT(16));
SIG(gcnt, UINT(16));
SIG(bcnt, UINT(16));



//SIG(dc, UINT(15)); // 3 5-bit current dc level values (RGB, dc computed on each link)
SIG(dc, UINT(30)); // 3 5-bit current dc level values (RGB, dc computed on each link)
SIG(ctrl_data, UINT(1)); // 1 when tmds encoding of ctrl data (vsync etc.)
SIG(pix_toggle, UINT(1)); // 1 when tmds encoding of ctrl data (vsync etc.)
SIG(pix_togglep, UINT(1)); // 1 when tmds encoding of ctrl data (vsync etc.)
SIG(rvsync, UINT(1));
SIG(rvsyncp, UINT(1));
SIG(rhsync, UINT(1));
SIG(rde, UINT(1));
SIG(rin_frame, UINT(1));

CONST(hres, UINT(16)) := TO_UINT(640, 16);
CONST(hss, UINT(16)) := TO_UINT(656, 16);
CONST(hse, UINT(16)) := TO_UINT(752, 16);
CONST(htot, UINT(16)) := TO_UINT(762, 16);
CONST(vres, UINT(16)) := TO_UINT(480, 16);
CONST(vss, UINT(16)) := TO_UINT(489, 16); // biased values (-1) : the vsync actually begins the line before
CONST(vse, UINT(16)) := TO_UINT(491, 16);
CONST(vtot, UINT(16)) := TO_UINT(525, 16);

CONST(CTRL00, UINT(8)) := BIN(11111101);
CONST(CTRL01, UINT(8)) := BIN(00000011);
CONST(CTRL10, UINT(8)) := BIN(11111100);
CONST(CTRL11, UINT(8)) := BIN(00000010);

// For text buffer
SIG(bg_color, UINT(24));// := TO_UINT(0, 24);
CONST(fg_color, UINT(24)) := BIN(111111111111111111111111);

SIG(buf_start_line, UINT(8));
SIG(buf_end_line, UINT(8));
SIG(buf_addr, UINT(13));
SIG(buf_wen, BIT_TYPE);
SIG(buf_rd, BIT_TYPE); // for buf read
SIG(buf_rdp, BIT_TYPE);
SIG(buf_rdpp, BIT_TYPE);
SIG(test, BIT_TYPE);
SIG(line_div12, UINT(4));



SIG(buf_line, UINT(8));
SIG(buf_char, UINT(8));
SIG(buf_cur_line, UINT(8));
SIG(buf_cur_col, UINT(8));
SIG(rbuf_char, UINT(8));
SIG(rbuf_char_rd, UINT(8)); // for core read op.
SIG(wbuf_char2, UINT(8));
SIG(wbuf_char, UINT(8));

SIG(char_line, UINT(8));
SIG(buf_col, UINT(8));
SIG(char_idx, UINT(8));
SIG(char_pix, UINT(3));
SIG(buf_blank, UINT(1));
SIG(line_blank, UINT(1));
SIG(inc_start_line, UINT(1));
SIG(init_cnt, UINT(5));
SIG(dbg_init_cnt, UINT(5));

SIG(buf_state, UINT(4));
SIG(buf_read_pipe, UINT(4));
SIG(bg_cnt, UINT(8));
SIG(blink_cnt, UINT(9));
SIG(blink, BIT_TYPE);
SIG(boot_mode, BIT_TYPE);
SIG(trap, BIT_TYPE);
SIG(PC, UINT(32));
SIG(PCp, UINT(32));
SIG(PC_write, UINT(5));
SIG(stop, UINT(13));
SIG(console_mode, UINT(1));
SIG(cls, BIT_TYPE);
SIG(cls_ack, BIT_TYPE);
SIG(clsp, BIT_TYPE);

// clk domain clk_core
SIG(wbuf_pos, UINT(16));
SIG(wbuf_pos_en, BIT_TYPE);
SIG(wbuf_char_clk_core, UINT(8));
SIG(wbuf_char_clk_core_keep, UINT(1));
SIG(font_addr, UINT(12));
SIG(font_wen, BIT_TYPE);
SIG(font_data, UINT(8));
SIG(font_wdata, UINT(8));
SIG(font_waddr, UINT(12));
SIG(font_we, BIT_TYPE);
SIG(font_we_release, BIT_TYPE);
SIG(rbuf_ok, BIT_TYPE);
SIG(rbuf_wait, BIT_TYPE);
SIG(reset_mem2core, BIT_TYPE);

CONST(buf_idle, UINT(4)) := TO_UINT(0, 4);
// init message: "GIORNO CORE\n"
TYPE( init_msg_t, BASE_ARRAY_TYPE(UINT(8), 14) );
#ifdef VHDL
CONST(init_msg, init_msg_t ) :=
#else
		init_msg_t init_msg :=
#endif
LIST(
		HEX(47), HEX(49),HEX(4F),HEX(52),HEX(4E),HEX(4F),HEX(20),HEX(43),HEX(4F),HEX(52),HEX(45),HEX(0A), HEX(3E), HEX(0A) );//,
		//HEX(41),HEX(4F),HEX(4F),HEX(54),HEX(0A));
BEGIN

/*
BLK_INST(u0_ddio, ddio,
		MAPPING(
				PM(clk_hdmi, clk_hdmi),
				PM(reset_n, reset_n),
				PM(blue_i, blue),
				PM(green_i, green),
				PM(red_i, red),
				PM(pclk_i, pclk),
				PM(blue_o, blue_o),
				PM(green_o, green_o),
				PM(red_o, red_o),
				PM(pclk_o, pclk_o)
				)
		);
*/
BLK_INST(u0_char_buf, spram_4800x8,
		MAPPING(
				PM(clk, clk_pix),
				PM(reset_n, reset_n),
				PM(addr_i, buf_addr),
				PM(data_i, wbuf_char2),
				PM(data_o, buf_char),
				PM(wen_i, buf_wen)
				)
		);

BLK_INST(u0_font, spram_font,
		MAPPING(
				PM(clk, clk_pix),
				PM(reset_n, reset_n),
				PM(addr_i, font_addr),
				PM(data_i, font_wdata),
				PM(wen_i, font_wen),
				PM(data_o, font_data)
				)
		);
BLK_INST(u0_ddio, ddio,
		MAPPING(
				PM(clk_hdmi, clk_hdmi),
				PM(reset_n, reset_n),
				PM(blue_i, blue),
				PM(green_i, green),
				PM(red_i, red),
				PM(pclk_i, pclk),
				PM(blue_o, blue_o),
				PM(green_o, green_o),
				PM(red_o, red_o),
				PM(pclk_o, pclk_o)
				)
		);
PROCESS(1, clk_pix, reset_n)
VAR(vsync, UINT(1));
VAR(hsync, UINT(1));
VAR(de, UINT(1));
VAR(ctrl_word3, UINT(3));
VAR(ctrl_word, UINT(8));
VAR(red, UINT(8));
VAR(green, UINT(8));
VAR(blue, UINT(8));
VAR(buf_line_p1, UINT(8));
VAR(buf_cur_line_p1, UINT(8));
VAR(buf_cur_col_p1, UINT(8));
VAR(tmp, UINT(8));
VAR(base_hex, UINT(8));
VAR(nibble, UINT(4));

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		RESET(hcnt);
		//RESET(vcnt);
		vcnt <= TO_UINT(470, 16);
		RESET(tmds_encoded);
		RESET(rgb_data);
		RESET(ctrl_data);
		pix_toggle <= BIN(0);
		buf_wen <= BIT(1);
		test <= BIT(1);
		inc_start_line <= BIN(0);
		init_cnt <= TO_UINT(28, LEN(init_cnt));
		RESET(wbuf_char);
		RESET(wbuf_char2);
		RESET(buf_blank);
		RESET(line_blank);
		RESET(buf_cur_line);
		RESET(buf_cur_col);
		RESET(buf_line);
		RESET(buf_col);
		RESET(buf_start_line);
		RESET(buf_end_line);
		RESET(rin_frame);
		RESET(buf_read_pipe);
		RESET(char_line);
		RESET(ctrl_data);
		RESET(rgb_data);
		RESET(wbuf_char);
		RESET(wbuf_char2);
		RESET(buf_addr);
		RESET(font_addr);
		RESET(line_div12);
		RESET(bg_color);
		RESET(rvsync);
		RESET(rvsyncp);
		trap <= BIT(0);
		stop <= BIN(0000000000000);
		font_wen <= BIT(0);
		font_we_release <= BIT(0);
		clsp <= BIT(0);
		cls_ack <= BIT(0);
		buf_rd <= BIT(0);
		buf_rdp <= BIT(0);
		buf_rdpp <= BIT(0);

	ELSEIF ( EVENT(clk_pix) and (clk_pix == BIT(1)) ) THEN

		buf_read_pipe <= SHIFT_LEFT( buf_read_pipe, 1 ); //( RANGE(read_pipe, 2, 0) & BIT(0) );
		clsp <= cls;
		pix_toggle <= not pix_toggle;
		IF ( hcnt < htot) THEN
			hcnt <= hcnt + 1;

		ELSE
			hcnt <= TO_UINT(0, LEN(hcnt));
			IF ( vcnt < vtot) THEN
				vcnt <= vcnt + 1;
			ELSE
				vcnt <= TO_UINT(0, LEN(vcnt));
			ENDIF
		ENDIF

		if ( (hcnt == hss) and (vcnt == vss) ) THEN
			rcnt <= rcnt + TO_UINT(55, 16);
			gcnt <= gcnt + TO_UINT(73, 16);
			bcnt <= bcnt + TO_UINT(91, 16);
		ENDIF

		hsync := BOOL2BIN( ( (hcnt >= hss) and (hcnt < hse) ) );
		vsync := BOOL2BIN( ( (vcnt >= vss) and (vcnt < vse) ) );
		de 	  := BOOL2BIN( ( (hcnt < hres) and (vcnt < vres) ) );
		rhsync <= hsync;
		rde <= de;
		rvsync <= ( (rvsync or (hsync and vsync) ) and not (hsync and not vsync) );
		rvsyncp <= rvsync;
		rin_frame <= BOOL2BIN( ( (vcnt < vres) ) );
		IF (rde == BIN(1)) THEN

			// Retrieve rgb data from char 8-bit line
			IF ( (B(char_line, 7) xor blink)  == BIT(1)) THEN
				rgb_data <= fg_color;//BIN(010000000100000001000000);//fg_color;
			ELSE
				rgb_data <= bg_color;//BIN(000001000100000001000000);//fg_color;
			ENDIF
			// shift char line data
			char_line <= SHIFT_LEFT(char_line, 1);
		ELSE // Not pixel data, send control bits instead (hsync, vsync...)
			ctrl_word3:= ( (not rhsync) & rhsync & (not rvsync) );
			ctrl_word := SXT( (ctrl_word3), 8);
			rgb_data <= (ctrl_word & ctrl_word & ctrl_word);//( CTRL00 & CTRL00 & ctrl_word);
		ENDIF

		ctrl_data <= (not rde); // used to modifiy TMDS encoding for ctrl data (violates XOR/XNOR rule)


		buf_wen <= BIT(1); // default, no write
		buf_rd <= BIT(0);
		buf_rdp <= buf_rd;
		buf_rdpp <= buf_rdp;
		buf_cur_line_p1 := buf_cur_line + 1; // counters combinational ops.
		buf_cur_col_p1 := buf_cur_col + 1;
		IF (buf_cur_line == TO_UINT(39, LEN(buf_line))) THEN // wrap buffer
			buf_cur_line_p1	:= TO_UINT(0, LEN(buf_line));
		ENDIF

		IF (RANGE(hcnt, 2, 0) == BIN(100)) THEN // init buffer read pipe (syncs read pipe and pixel clock)
			buf_read_pipe <= BIN(0001);
			buf_addr <= (EXT( (buf_line * TO_UINT(80, 7)), LEN(buf_addr)) + EXT(buf_col, LEN(buf_addr)) );
		ELSEIF ( not (wbuf_char == BIN(00000000) ) ) THEN // something to write to buffer: can do it now
			wbuf_char <= BIN(00000000);
			wbuf_char2 <= wbuf_char; // memory input
			IF ( not (RANGE(wbuf_char, 7, 1) == BIN(0000000) ) ) THEN // wbuf_char = 00000001 is for buffer read !!!!!!!!!
				buf_wen <= BIT(0);
			ENDIF
			buf_rd <= BIT(1); // Buffer read pipe, also when writing (reads old data)

			test <= BIT(0);
			buf_addr <= (EXT( (buf_cur_line * TO_UINT(80, 7)), LEN(buf_addr)) + EXT(buf_cur_col, LEN(buf_addr)) );
			// end line
			IF ( (buf_cur_col == TO_UINT(79, LEN(buf_cur_col)) ) or (wbuf_char == BIN(00001010)) ) THEN
				IF (buf_cur_line_p1 == buf_start_line) THEN
					inc_start_line <= BIN(1);
				ENDIF
				buf_cur_line <= buf_cur_line_p1;
				buf_end_line <= buf_cur_line_p1;
				RESET(buf_cur_col);
			ELSE
				buf_cur_col <= buf_cur_col_p1;
			ENDIF

		ENDIF

		// For buffer read by core
		IF (buf_rdp == BIT(1)) THEN
			rbuf_char_rd <= buf_char;
		ENDIF



		// FILL CHAR BUFFER

		IF ( not (wbuf_char_clk_core == BIN(00000000)) ) THEN
			wbuf_char <= wbuf_char_clk_core;

		ENDIF
		IF (wbuf_pos_en == BIT(1)) THEN // set cursor pos
			RESET(buf_start_line); // this resets console mode
			buf_end_line <= TO_UINT(39,LEN(buf_end_line));
			buf_cur_col <= RANGE(wbuf_pos, 7, 0);
			buf_cur_line <= RANGE(wbuf_pos, 15, 8);
		ENDIF
		IF ( not (init_cnt == TO_UINT(0, LEN(init_cnt))) ) THEN
			// Write init msg 'giorno core'
			IF (B(init_cnt, 0) == BIT(1)) THEN
				wbuf_char <= init_msg( TO_INTEGER( BIN(1101) - RANGE(init_cnt, 4, 1)) );
				dbg_init_cnt <= EXT( (BIN(1101) - RANGE(init_cnt, 4, 1)), 5);
				//gprintf("#VBUF % %", TO_INTEGER( BIN(1101) - RANGE(init_cnt, 4, 1)), init_msg( TO_INTEGER( BIN(1101) - RANGE(init_cnt, 4, 1)) ));
			ENDIF
			init_cnt <= init_cnt - 1;
		ELSEIF ( (PC_write == TO_UINT(0, 5)) ) THEN
			// Start write trace info
			//PC <= PC_i;

			IF ( B(PORT_BASE(dbg_i), 32) == BIT(1)) THEN //and not (RANGE(PORT_BASE(PC_i), 15, 0) == RANGE(PCp, 15, 0))) THEN //and (B(PORT_BASE(PC_i), 29)) == BIT(0) ) and (RANGE(PORT_BASE(PC_i), 28, 16) < stop)) THEN // stall display after trap
				PC_write <= BIN(10001);
				PC <= RANGE(PORT_BASE(dbg_i), 31, 0);
				PCp <= RANGE(PORT_BASE(dbg_i), 31, 0);
			ENDIF
			//trap <= (trap or B(PORT_BASE(PC_i), 31));//trap_i; //!:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		ELSE // Write trace info
			IF (B(PC_write,0) == BIT(1)) THEN
				IF (PC_write == BIN(00001)) THEN
					wbuf_char <= BIN(00001010);
				ELSE
					nibble := RANGE(PC, 31, 28);
					IF (nibble > BIN(1001)) THEN
						base_hex := TO_UINT(55, 8);
					ELSE
						base_hex := TO_UINT(48, 8);
					ENDIF
					wbuf_char <= base_hex + EXT(nibble, 8);
					PC <= SHIFT_LEFT(PC, 4);
				ENDIF
			ENDIF
			PC_write <= PC_write - 1;
		ENDIF

		IF ( (cls == BIT(1)) and (clsp == BIT(0)) ) THEN
			RESET(buf_cur_col);
			RESET(buf_cur_line);
			RESET(buf_start_line);
			RESET(buf_end_line);
		ENDIF

		/*
		boot_mode <= boot_mode_i;
		IF ( (boot_mode == BIT(0)) and (boot_mode_i == BIT(1))) THEN
			wbuf_char <= BIN(01000010);
		ENDIF
		*/
		boot_mode <= boot_mode_i;
		IF (trap_i == BIT(1)) THEN
			bg_color <= BIN(111111110000000000000000); // switch to red
		ENDIF
		IF (boot_mode_i == BIT(1)) THEN
			trap <= BIT(0);
			bg_color <= BIN(000000000000000001111111); // switch to blue
		ELSEIF (boot_mode == BIT(1)) THEN
			stop <= stop + BIN(0000000100000);
			bg_color <= BIN(000001110000011100000111);// switch to dark grey
		ENDIF


		// buffer wrapped on prev. cycle
		IF (inc_start_line == BIN(1)) THEN
			buf_start_line <= buf_cur_line_p1; //
			inc_start_line <= BIN(0);
		ENDIF

		font_wen <= BIT(1);
		font_we_release <= BIT(0);
		IF (B(buf_read_pipe, 1) == BIT(1)) THEN // gain access for buffer read
			rbuf_char <= buf_char;
			IF (buf_char < BIN(00100000)) THEN // font chars not defined
				tmp := 	TO_UINT(10, 8);
			ELSE
				tmp := (buf_char - BIN(00100000)); // defined font chars start at 32 - apply offset
			ENDIF

			IF (buf_char == TO_UINT(10, 8)) THEN // should use tmp ?
				line_blank <= console_mode;
			ENDIF
			//gprintf("#M tmp % addr %", tmp, tmp * TO_UINT(12,4));
			font_addr <= EXT( (tmp * TO_UINT(12,4)), LEN(font_addr)) + EXT(line_div12, LEN(font_addr));

		ELSE // enable font write
			IF (font_we == BIT(1)) THEN
				font_addr <= font_waddr;
				font_wen <= BIT(0);
				font_we_release <= BIT(1);
			ENDIF

		ENDIF
		// Detect end of line and toggle blinking cursor
		IF (B(buf_read_pipe, 2) == BIT(1)) THEN
			IF	( (buf_line == buf_cur_line) and (buf_col == buf_cur_col) ) THEN
				blink <= (B(blink_cnt, 5) and B(console_mode, 0));
				line_blank <= console_mode;
			ELSE
				blink <= BIT(0);
			ENDIF
		ENDIF

		// Last pipeline stage: get 8-bit font data
		IF (B(buf_read_pipe, 3) == BIT(1)) THEN
			IF ( (line_blank == BIN(1)) or (buf_blank == BIN(1)) ) THEN
				RESET(char_line);
			ELSE
				char_line <= font_data;
			ENDIF

		ENDIF

		IF ( (rhsync == BIN(1)) and (hsync == BIN(0)) ) THEN
			line_blank <= BIN(0);
			RESET(buf_col);
			IF ( (rin_frame == BIN(1)) and (line_div12 == BIN(1011)) ) THEN
				buf_line_p1 := buf_line + 1;
				RESET(line_div12);
				// this assumes that no blanking is required when buffer has wrapped
				IF (buf_line == TO_UINT(39, LEN(buf_line))) THEN
					RESET(buf_line);
				ELSE
					buf_line <= buf_line_p1;
				ENDIF

				IF (buf_line == buf_end_line) THEN
					buf_blank <= console_mode;
				ENDIF
			ELSEIF (rin_frame == BIN(1)) THEN
				line_div12 <= line_div12 + 1;
			ENDIF
		ENDIF

		IF ( (rvsyncp == BIN(1)) and (rvsync == BIN(0)) ) THEN
			buf_line <= buf_start_line;
			RESET(line_div12);
			buf_blank <= BIN(0);
			blink_cnt <= blink_cnt + 1;
		ENDIF

		IF ( RANGE(hcnt, 2, 0) == BIN(011) ) THEN
			IF (rde == BIN(1)) THEN
				buf_col <= buf_col + 1;
			ENDIF

		ENDIF
/*
		// Clear screen
		cls_ack <= BIT(0);
		IF (cls == BIT(1)) THEN // clear screen from clk_core domain
			cls_ack <= BIT(1); //ack cmd
			clsp <= BIT(1);
			buf_cur_line <= TO_UINT(0, LEN(buf_cur_line));
			buf_cur_col <= TO_UINT(0, LEN(buf_cur_col));
		ENDIF

		IF (clsp == BIT(1)) THEN
			IF (buf_cur_col <= TO_UINT(79, LEN(buf_cur_col))) THEN
			ENDIF

		ENDIF
		*/
		//line_blank <= BIN(0);
		//buf_blank <= BIN(0);
	ENDIF

END_PROCESS


// register read
PROCESS(2, clk_core, reset_n)
VAR(byte, UINT(8));
VAR(cmd, UINT(8));
BEGIN
IF (reset_n == BIT(0)) THEN
	wbuf_char_clk_core <= BIN(00000000);
	wbuf_char_clk_core_keep <= BIN(0);
	font_we <= BIT(0);
	console_mode <= BIN(1);
	cls <= BIT(0);
	RESET(wbuf_pos);
	wbuf_pos_en <= BIT(0);
	rbuf_ok <= BIT(0);
	rbuf_wait <= BIT(0);
	PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
	PORT_BASE(mem2core_o).data_en <= BIT(0);
	reset_mem2core <= BIT(0);
	rbuf_wait <= BIT(0);
ELSEIF ( EVENT(clk_core) and (clk_core == BIT(1)) ) THEN
	wbuf_char_clk_core_keep <= BIN(0);

	//cls <= (cls and not cls_ack);
	IF (font_we_release == BIT(1)) THEN
		font_we <= BIT(0);
	ENDIF
	rbuf_ok <= (rbuf_ok or buf_rdpp); //
	PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
	PORT_BASE(mem2core_o).data_en <= BIT(0);

	IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and (PORT_BASE(core2mem_i).addr == TO_UINT(generic_int, LEN(PORT_BASE(core2mem_i).addr)))) THEN //BIN(1111111111110)) ) THEN//( PORT_BASE(core2mem_i).wr_n == BIT(0) ) ) THEN
		IF ( ( PORT_BASE(core2mem_i).wr_n == BIT(0) ) ) THEN //(PORT_BASE(core2mem_i).addr == BIN(1011111111110)) ) THEN
			byte := RANGE(PORT_BASE(core2mem_i).data, 7, 0);
			cmd := RANGE(PORT_BASE(core2mem_i).data, 31, 24);
			wbuf_pos_en <= BIT(0);
			wbuf_char_clk_core_keep <= BIN(1); // keep for 2 clk_core cycles (= 1 clk_pix cycle)

			IF ( not (byte == BIN(00000000)) and (cmd == TO_UINT(0, 8)) ) THEN
				wbuf_char_clk_core <= byte;//RANGE(PORT_BASE(core2mem_i).data, 7, 0);
				//gprintf("#MReceived %", to_hex(TO_INTEGER(PORT_BASE(core2mem_i).data)));
			ELSEIF (B(cmd, 7) == BIT(1)) THEN // printf commands
			ELSEIF (RANGE(cmd, 3, 0) == BIN(0011)) THEN // set console mode
				console_mode <= RANGE(byte, 0, 0);
			ELSEIF (RANGE(cmd, 3, 0) == BIN(0010)) THEN // erase char buffer !! NOT IMPLEMENTED
				cls <= BIT(1);
			ELSEIF (RANGE(cmd, 3, 0) == BIN(0001)) THEN // write font data
				font_wdata <= byte;
				font_waddr <= RANGE(PORT_BASE(core2mem_i).data, 19, 8);
				font_we <= BIT(1);
			ELSEIF (RANGE(cmd, 3, 0) == BIN(1111)) THEN // printat()
				wbuf_pos <= RANGE(PORT_BASE(core2mem_i).data, 23, 8);
				wbuf_char_clk_core <= byte;//RANGE(PORT_BASE(core2mem_i).data, 7, 0);
				wbuf_pos_en <= BIT(1);
				IF ( not (byte == BIN(00000000)) ) THEN // no write nor read command, just place cursor
					rbuf_ok <= BIT(0);
				ENDIF
			ENDIF
		ELSE // READ: wait for data
			rbuf_wait <= BIT(1);
		ENDIF
	ELSEIF (wbuf_char_clk_core_keep == BIN(0)) THEN
		wbuf_char_clk_core <= BIN(00000000);
		wbuf_pos_en <= BIT(0);
		cls <= BIT(0);
	ENDIF

	IF ( ( rbuf_wait == BIT(1)) and (rbuf_ok == BIT(1) ) ) THEN // data available
		PORT_BASE(mem2core_o).data <= EXT(rbuf_char_rd, 32);
		PORT_BASE(mem2core_o).data_en <= BIT(1);
		reset_mem2core <= BIT(1);
		rbuf_wait <= BIT(0);
		// don't reset rbuf_ok, this would lock the core in case of consecutive reads
	ENDIF

	IF (reset_mem2core == BIT(1)) THEN // better than 'default value' for vcd / sim time
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		reset_mem2core <= BIT(0);
	ENDIF

ENDIF
END_PROCESS

// HDMS & HDMI out
PROCESS(0, clk_hdmi, reset_n)

VAR(load_from_instmem, BIT_TYPE);
VAR(sum_ones, UINT(4)); // sum of unencoded one bits
VAR(sum_ones_enc, UINT(4)); // sum of encoded one bits
VAR(dc_tmds_encoded, UINT(4)); // dc of encoded one bits (is a signed value in the end)
VAR(tmds_encoded_tmp, UINT(10));
VAR(data_raw_sel, UINT(8)); // is either raw data or inverted data depending on ones sum
//VAR(dc_tmp, UINT(5)); // this is current dc level for current link (RGB)
VAR(dc_tmp2, UINT(2)); // this is current dc level for current link (RGB)
VAR(dc_tmp, UINT(10)); // this is current dc level for current link (RGB)
VAR(invert, UINT(1));
VAR(use_xor, UINT(1));
VAR(data_raw, UINT(8));
VAR(inversion_rule, BOOLEAN);

BEGIN

IF ( reset_n == BIT(0) ) THEN
		RESET(tmds_encoded);
		RESET(dc);
		RESET(rgb);
		pix_togglep <= BIN(0);
	ELSEIF ( EVENT(clk_hdmi) and (clk_hdmi == BIT(1)) ) THEN

		// New unencoded value detection
		pix_togglep <= pix_toggle;
		IF (not (pix_toggle == pix_togglep) ) THEN
			rgb <= BIN(001); // start encoding pipe
		ENDIF

		IF ( not (rgb == BIN(000) ) ) THEN	// TMDS encoding pipe
			// TMDS encoding
			// Select channel to encode
			IF (rgb == BIN(001)) THEN
				data_raw := RANGE(rgb_data, 23, 16);
			ELSEIF (rgb == BIN(010)) THEN
				data_raw := RANGE(rgb_data, 15, 8);
			ELSE
				data_raw := RANGE(rgb_data, 7, 0);
			ENDIF
			rdata_raw <= data_raw;
			//dc_tmp := ( RANGE(dc, 14, 10) );
			dc_tmp := ( RANGE(dc, 29, 20) );
			sum_ones := EXT(RANGE(data_raw,0,0), 4) +
						EXT(RANGE(data_raw,1,1), 4) +
						EXT(RANGE(data_raw,2,2), 4) +
						EXT(RANGE(data_raw,3,3), 4) +
						EXT(RANGE(data_raw,4,4), 4) +
						EXT(RANGE(data_raw,5,5), 4) +
						EXT(RANGE(data_raw,6,6), 4) +
						EXT(RANGE(data_raw,7,7), 4) ;
			rsum_ones <= sum_ones;
			IF ( ( (sum_ones >= BIN(0100)) or ( (sum_ones == BIN(0100)) and B(data_raw, 0) == BIT(1))) //) THEN
					xor (ctrl_data == BIN(1) ) ) THEN // Invert condition
				data_raw_sel := (not data_raw); // xnor encoding
				use_xor := BIN(0);//VAR_SET_BIT(tmds_encoded_tmp, 8, BIN(1)); //specify encoding type
			ELSE
				data_raw_sel := data_raw; // xor encoding
				use_xor := BIN(1);//VAR_SET_BIT(tmds_encoded_tmp, 8, BIN(0)); //specify encoding type
			ENDIF
			ruse_xor <= use_xor;
			VAR_SET_BIT(tmds_encoded_tmp, 0, B(data_raw, 0));
			//gprintf("#Vdata_raw_sel % enc %", to_hex(TO_INTEGER(data_raw_sel)), to_hex(TO_INTEGER(tmds_encoded_tmp)));
			FOR(idx, 1, 7)
				VAR_SET_BIT(tmds_encoded_tmp, idx, ( B(tmds_encoded_tmp, idx-1) xor B(data_raw_sel, idx)) ); //specify encoding type
				//gprintf("prev enc %R data %R res %M", TO_INTEGER( B(tmds_encoded_tmp, idx-1)), TO_INTEGER( B(data_raw_sel, idx)), TO_INTEGER( B(tmds_encoded_tmp, idx-1) xor B(data_raw_sel, idx)));
				//gprintf("#Uresult % enc %", TO_INTEGER( B(tmds_encoded_tmp, idx-1) xor B(data_raw_sel, idx)) , to_hex(TO_INTEGER(tmds_encoded_tmp)) );
			ENDLOOP
			rdataxor <= tmds_encoded_tmp;
			sum_ones_enc := EXT(RANGE(tmds_encoded_tmp,0,0), 4) +
							EXT(RANGE(tmds_encoded_tmp,1,1), 4) +
							EXT(RANGE(tmds_encoded_tmp,2,2), 4) +
							EXT(RANGE(tmds_encoded_tmp,3,3), 4) +
							EXT(RANGE(tmds_encoded_tmp,4,4), 4) +
							EXT(RANGE(tmds_encoded_tmp,5,5), 4) +
							EXT(RANGE(tmds_encoded_tmp,6,6), 4) +
							EXT(RANGE(tmds_encoded_tmp,7,7), 4);
			dc_tmds_encoded := ( (sum_ones_enc )  - BIN(0100) );

			inversion_rule := ( (ctrl_data == BIN(0)) and ( ( B(dc_tmds_encoded, HI(dc_tmds_encoded)) == B(dc_tmp, HI(dc_tmp)) ) or // current dc and encoded byte have same dc sign, or  -> invert tmds bits
					 ( ( ( dc_tmp == TO_UINT(0, LEN(dc_tmp)) ) or ( dc_tmds_encoded == TO_UINT(0, LEN(dc_tmds_encoded)) ) ) and (  use_xor == BIN(0) ) ) ) ) or
							 ( (ctrl_data == BIN(1) ) and not (B(data_raw, 0) == B(data_raw, 1)) );
			IF ( inversion_rule ) THEN // Other inversion condition
				invert := BIN(1);
				dc_tmds_encoded := TO_UNSIGNED(0, LEN(dc_tmds_encoded)) - dc_tmds_encoded;
				tmds_encoded_tmp := not tmds_encoded_tmp;
			ELSE
				invert := BIN(0);
			ENDIF
			rdc_tmds_encoded <= dc_tmds_encoded;

			VAR_SET_BIT(tmds_encoded_tmp, 8, TO_LOGIC(use_xor));
			VAR_SET_BIT(tmds_encoded_tmp, 9, TO_LOGIC(invert));

			rdata8 <= data_raw_sel;
			//rdata10 <= tmds_encoded_tmp;
			//debug <= tmds_encoded_tmp;

			//rgb_data <= ( RANGE(rgb_data, 23, 8) & BIN(00000000) );
			data_tmds <= ( RANGE(data_tmds, 19, 0) & tmds_encoded_tmp );
			//dc <= ( RANGE(dc, 9, 0) & dc_tmp );
			IF (rde == BIN(1)) THEN
					dc_tmp2 := (not (invert or use_xor)) & (not (invert xor use_xor));
				dc_tmp := dc_tmp + SXT(dc_tmds_encoded, LEN(dc_tmp)) + SXT(dc_tmp2, 10);
			ELSE
				dc_tmp := TO_UINT(0, LEN(dc_tmp));
			ENDIF
			IF (rgb == BIN(100)) THEN
				rdc <= dc_tmp;
				rdata10 <= tmds_encoded_tmp;
			ENDIF



			dc <= ( RANGE(dc, 19, 0) & dc_tmp );
			rgb <= ( RANGE(rgb, 1, 0) & BIT(0) ); // TMDS pipe
		ENDIF

		//Serialize data in bit pairs
		// red and green have larger shift register size because they come ahead of blue
		IF (rgb == BIN(001)) THEN
			r14 <= (tmds_encoded_tmp & RANGE(r14,5,2));
		ELSE
			r14 <= (BIN(00) & RANGE(r14, 13, 2));
		ENDIF

		IF (rgb == BIN(010)) THEN
			g12 <= (tmds_encoded_tmp & RANGE(g12, 3, 2));
		ELSE
			g12 <= (BIN(00) & RANGE(g12, 11, 2));
		ENDIF

		IF (rgb == BIN(100)) THEN
			b10 <= tmds_encoded_tmp;
			sr_pclk <= BIN(0000011111);
		ELSE
			b10 <= (BIN(00) & RANGE(b10, 9, 2));
			sr_pclk <= (BIN(00) & RANGE(sr_pclk, 9, 2));
		ENDIF

		blue  <= RANGE(b10, 1, 0);
		green <= RANGE(g12, 1, 0);
		red   <= RANGE(r14, 1, 0);
		pclk  <= RANGE(sr_pclk, 1, 0);

	ENDIF
	END_PROCESS

	// Combinational logic
	COMB_PROCESS(3, clk_pix)
/*		PORT_BASE(core2instmem_o).addr <= inst_addr;//RANGE(PC, LEN(blk2mem_t0.addr)+1, 2);//RESIZE(PC, LEN(PORT_BASE(core2instmem_o).addr));
		PORT_BASE(core2instmem_o).data <= PCp;
		PORT_BASE(core2instmem_o).cs_n <= inst_cs_n;//BIT(0);
		PORT_BASE(core2instmem_o).wr_n <= not loading;
		PORT_BASE(core2instmem_o).be <= BIN(1111);
		core2datamem_o <= blk2mem_t0;
		addr_rs1 <= RANGE(PORT_BASE(instmem2core_i).data, 19, 15);
		addr_rs2 <= RANGE(PORT_BASE(instmem2core_i).data, 24, 20);
		*/
	END_COMB_PROCESS

BLK_END;
