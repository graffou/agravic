#include "slv.h"
START_OF_FILE(peripherals)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(altera)
#ifndef VHDL
static std::ofstream dbg_file("dbg_file", std::ios::out);
static int print_state = 0;
static int base_color = 0;
static uint32_t codep = 0;

static gstring dummy;

#define INT32_TYPE 0x00000100
#define INT16_TYPE 0x00000200
#define INT8_TYPE 0x00000400
#define UINT32_TYPE 0x80000100
#define UINT16_TYPE 0x80000200
#define UINT8_TYPE 0x80000400
#define BOOL_TYPE 0x80000800
#define COLOR_TYPE 0xFFFFFF00
#define END_PRINT 0x88000000
#endif

ENTITY(peripherals,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(dbg_o, UINT(8), OUT),
		PORT(gpios_o, UINT(32), OUT)
		)
);

SIG(cnt, UINT(32));
SIG(gate_cell, BIT_TYPE);
// does nothing in C++, mandatory for VHDL generation
DECL_GATED_CLK(clk_g);

BEGIN

// Is a declaration in C++, instantiation in VHDL(after begin)
GATED_CLK(clk_g ,clk_peri, gate_cell);


PROCESS(0, clk_peri, reset_n)
VAR(DBG, UINT(8));
BEGIN
	IF ( reset_n == BIT(0) ) THEN
	gpios_o <= TO_UINT(0, 32);
	gate_cell <= BIT(1);
	ELSEIF ( EVENT(clk_peri) and (clk_peri == BIT(1)) ) THEN
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( PORT_BASE(core2mem_i).wr_n == BIT(0) ) ) THEN
			IF (PORT_BASE(core2mem_i).addr == BIN(1011111111111)) THEN
				gate_cell <= B(PORT_BASE(core2mem_i).data, 0);
				gpios_o <= PORT_BASE(core2mem_i).data;
#ifndef VHDL
				gprintf("#Ugpios %Y", PORT_BASE(core2mem_i).data);
#endif
			ENDIF
			IF (PORT_BASE(core2mem_i).addr == BIN(1011111111110)) THEN
				DBG := RANGE(PORT_BASE(core2mem_i).data, 7, 0);
				dbg_o <= DBG;//RANGE(PORT_BASE(core2mem_i).data, 7, 0);
#ifndef VHDL
				char c = char(TO_INTEGER(DBG));
				uint32_t val = TO_INTEGER(PORT_BASE(core2mem_i).data);
				uint32_t code = ( (codep>>24) != 0x80 ) ? val & 0xffffff00 : 0; // Don't update code if already in coded value: the coded value might trig anything
				//if (code != 0) gprintf("#MCode % val %", to_hex(code), c);
				gprintf("#BIn %R Write %R", to_hex(TO_INTEGER(PORT_BASE(core2mem_i).data)), c);
				if (print_state == 0)
				{
					if ( (c == '#') or (c == '@') )
						print_state = 1; // wait for color code
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
					if ( (c == 0) and not ( (codep>>24) == 0x80 ) )
					{
						if (code == END_PRINT)
						{
							if (base_color)
							{
								dummy.set_color(dbg_file, color_code(0));
								dbg_file << "\033[0m";
								gprintf("#UResetting color scheme");
							}
							base_color = 0;
							print_state = 0;
						}
						else if ( (code == 0) and (base_color) ) // end of char*
						{
							dummy.set_color(dbg_file, base_color);
						}
					}
					else if (code == COLOR_TYPE)
					{
						gprintf("#gSetting color %", c);
						dummy.set_color(dbg_file, color_code(c));
					}
					else if ( (codep>>24) == 0x80 )
					{
						gprintf("#BWriting coded value %", to_hex(val));
						switch(codep)
						{
						case COLOR_TYPE : dummy.set_color(dbg_file, color_code(c)); break;
						case INT32_TYPE : dbg_file << int32_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case UINT32_TYPE : dbg_file << uint32_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case INT16_TYPE : dbg_file << int16_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case UINT16_TYPE : dbg_file << uint16_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case INT8_TYPE : dbg_file << int8_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
						case UINT8_TYPE : dbg_file << uint8_t((val)); if(base_color) dummy.set_color(dbg_file, base_color);break;
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


PROCESS(1, clk_g, reset_n)
BEGIN
	IF ( reset_n == BIT(0) ) THEN
		cnt <= TO_UINT(0, 32);
	ELSEIF ( EVENT(clk_g) and (clk_g == BIT(1)) ) THEN
		cnt <= cnt + 1;
	ENDIF
END_PROCESS

//}

BLK_END;
