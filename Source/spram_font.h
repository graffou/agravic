#include "slv.h"

START_OF_FILE(spram_font)
INCLUDES
USE_PACKAGE(structures)

ENTITY(spram_font,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(addr_i, UINT(12), IN),
		PORT(data_i, UINT(8), IN),
		PORT(wen_i, BIT_TYPE, IN),
		PORT(data_o, UINT(8), OUT)
		)
);

// font defintion
#include "../Source/agravic_font_FreePixel_12.hpp"

//TYPE(mem_t, ARRAY_TYPE(UINT(8),2688));
//SIG(mem, mem_t);// internal;

BEGIN


// adapted from read old data during write ram inferring (quartus coding guidelines)
PROCESS(0, clk, reset_n)
BEGIN
	//IF ( reset_n == BIT(0) ) THEN
	//	data_o <= TO_UINT(0, LEN(data_o));
//gprintf("#Raddr % wen ", addr_i, wen_i);
	IF ( EVENT(clk) and (clk == BIT(1)) ) THEN

	IF (wen_i == BIT(0)) THEN
#ifdef VHDL
		mem(TO_INTEGER(PORT_BASE(addr_i))) <= data_i;
#else
		mem(TO_INTEGER(PORT_BASE(addr_i))) := data_i; // Yes, this is wrong, but it is harmless here
#endif
//gprintf("#Gwrite %r %r", to_hex(TO_INTEGER(PORT_BASE(data_i))), to_hex(TO_INTEGER(PORT_BASE(addr_i))));
	ENDIF

		data_o <= mem(TO_INTEGER(PORT_BASE(addr_i)));
	ENDIF
//	gprintf("#Gaddr %", addr_i);

END_PROCESS
//}

BLK_END;
