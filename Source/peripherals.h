#include "slv.h"
START_OF_FILE(peripherals)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(altera)
#ifndef VHDL
static std::ofstream dbg_file("dbg_file", std::ios::out);


static gstring dummy;

#define INT32_TYPE  0x80000000
#define INT16_TYPE  0x81000000
#define INT8_TYPE   0x82000000
#define UINT32_TYPE 0x83000000
#define UINT16_TYPE 0x84000000
#define UINT8_TYPE  0x85000000
#define BOOL_TYPE   0x86000000
#define FLOAT_TYPE  0x87000000
#define COLOR_TYPE  0xE0000000
#define END_PRINT   0xF0000000

#endif

ENTITY(peripherals,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(timer_IT_o, BIT_TYPE, OUT),
		PORT(dbg_o, UINT(8), OUT),
		PORT(gpios_o, UINT(32), OUT)
		)
		, INTEGER generic_int

);
#ifndef VHDL
int print_state = 0;
int base_color = 0;
uint32_t codep = 0;

//std::ofstream dbg_file("dbg_file", std::ios::out);
#endif
SIG(cnt, UINT(32));
SIG(cnt_cmp, UINT(32));
SIG(cnt_started, BIT_TYPE);
//SIG(gate_cell, BIT_TYPE);
// does nothing in C++, mandatory for VHDL generation
//DECL_GATED_CLK(clk_g);
CONST(reg_base_addr, UINT(LEN(PORT_BASE(core2mem_i).addr))) := TO_UINT(generic_int, LEN(PORT_BASE(core2mem_i).addr));
SIG(base_addr_test, UINT(LEN(PORT_BASE(core2mem_i).addr)));
CONST(reg_addr_lsbs, INTEGER) := ( generic_int / 268435456);
SIG(addr_lsbs_test, UINT(4));
SIG(base_addr_ok, BIT_TYPE);
SIG(addr_ok, BIT_TYPE);

BEGIN

// Is a declaration in C++, instantiation in VHDL(after begin)
//GATED_CLK(clk_g ,clk_peri, gate_cell);


PROCESS(0, clk_peri, reset_n)
VAR(DBG, UINT(8));
BEGIN
	IF ( reset_n == BIT(0) ) THEN
		gpios_o <= TO_UINT(0, 32);
		RESET(cnt);
		RESET(cnt_cmp);
		timer_IT_o <= BIT(0);
		cnt_started <= BIT(0);
		base_addr_test <= reg_base_addr;
		addr_lsbs_test <= TO_UINT(reg_addr_lsbs, 4);
		//gate_cell <= BIT(1);
	ELSEIF ( EVENT(clk_peri) and (clk_peri == BIT(1)) ) THEN
		IF ( not ( cnt == TO_UINT(0,32) ) ) THEN //timer increment
			cnt <= cnt - TO_UINT(1, 32);
			cnt_started <= BIT(1); // trig timer IT for timeout values != 0
		ELSEIF (cnt_started == BIT(1)) THEN // timer was intitialized and actually reached timeout
			timer_IT_o <= BIT(1);
		ENDIF
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( PORT_BASE(core2mem_i).wr_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, HI(PORT_BASE(core2mem_i).addr), REG_NBITS) == RANGE( reg_base_addr, HI(reg_base_addr), REG_NBITS) )) THEN
			// Timer ----------------
			// The mtime / mtimecmp on risc-V spec does not make sense to me
			//
//		IF (PORT_BASE(core2mem_i).addr == BIN(1111111111100)) THEN // timer write timeout
//				//gate_cell <= B(PORT_BASE(core2mem_i).data, 0);
//				cnt <= PORT_BASE(core2mem_i).data;
//				cnt_started <= BIT(0);
//				timer_IT_o <= BIT(0); // reset interrupt line
//			ENDIF

			// --------------
			//IF (PORT_BASE(core2mem_i).addr == BIN(1111111111111)) THEN
			base_addr_ok <= BIT(1);
			IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(3, REG_NBITS)) THEN
				//gate_cell <= B(PORT_BASE(core2mem_i).data, 0);
				gpios_o <= PORT_BASE(core2mem_i).data;
#ifndef VHDL
				gprintf("#Ugpios %Y", PORT_BASE(core2mem_i).data);
#endif
			ENDIF
			//IF (PORT_BASE(core2mem_i).addr == BIN(1111111111110)) THEN
			IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(2, REG_NBITS)) THEN
				addr_ok <= BIT(1);
				DBG := RANGE(PORT_BASE(core2mem_i).data, 7, 0);
				dbg_o <= DBG;//RANGE(PORT_BASE(core2mem_i).data, 7, 0);

				// This is for gprintf(...)
#ifndef VHDL
				char c = char(TO_INTEGER(DBG));
				uint32_t val = TO_INTEGER(PORT_BASE(core2mem_i).data);
				uint32_t code = ( (codep>>28) != 0x8 ) ? val & 0xffffff00 : 0; // Don't update code if already in coded value: the coded value might trig anything
				//if (code != 0) gprintf("#MCode % val %", to_hex(code), c);
				//gprintf("#BIn %R Write %R state %R", to_hex(TO_INTEGER(PORT_BASE(core2mem_i).data)), c, print_state);
				if (print_state == 0)
				{
					if ( (c == '#') or (c == '@') )
						print_state = 1; // wait for color code
					else if (c == '>')
					{
						dbg_file << (cur_time >> 8)/1000000.0 << "ms:";
						print_state = 2;
					}
					else
					{
						print_state = 2;
						dbg_file << char(TO_INTEGER(DBG));
					}
				}
				else if (print_state == 1)
				{
					base_color = color_code(c);
					dummy.set_color(dbg_file, base_color);
					print_state = 2;
				}
				else if (print_state == 2)
				{
					if ( (c == 0) and not ( (codep>>28) == 0x8 ) )
					{
						if (code == END_PRINT)
						{
							if (base_color)
							{
								dummy.set_color(dbg_file, color_code(0));
								dbg_file << "\033[0m";
								//gprintf("#UResetting color scheme");
							}
							base_color = 0;
							print_state = 0;
							dbg_file << "\n";
							dbg_file.flush();
						}
						else if ( (code == 0) and (base_color) ) // end of char*
						{
							dummy.set_color(dbg_file, base_color);
						}
					}
					else if (code == COLOR_TYPE)
					{
						//gprintf("#gSetting color %", c);
						dummy.set_color(dbg_file, color_code(c));
					}
					else if ( (codep>>28) == 0x8 )
					{
						//gprintf("#BWriting coded value %", to_hex(val));
						switch(codep)
						{
						case COLOR_TYPE : dummy.set_color(dbg_file, color_code(c)); break;
						case INT32_TYPE : dbg_file << int32_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case UINT32_TYPE : dbg_file << uint32_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case INT16_TYPE : dbg_file << int16_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case UINT16_TYPE : dbg_file << uint16_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case INT8_TYPE : dbg_file << int8_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case UINT8_TYPE : dbg_file << uint8_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case FLOAT_TYPE : dbg_file << *reinterpret_cast<float*>(&val); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case BOOL_TYPE : dbg_file << bool((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						default: dbg_file << char((val));
						}
						codep = 0;
						//dbg_file << char(TO_INTEGER(DBG));
					}
					else
					{
						if ( (val == 10) and (base_color) )
							dbg_file << "\033[0m";

						dbg_file << char((val));
					}
					codep = code;
				}
#endif
			ENDIF
		ENDIF
	ENDIF
END_PROCESS

//
//PROCESS(1, clk_peri, reset_n)
//BEGIN
//	IF ( reset_n == BIT(0) ) THEN
//		cnt <= TO_UINT(0, 32);
//	ELSEIF ( EVENT(clk_g) and (clk_g == BIT(1)) ) THEN
//		cnt <= cnt + 1;
//	ENDIF
//END_PROCESS

//}

BLK_END;
