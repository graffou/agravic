#include "slv.h"

START_OF_FILE(spram_4800x8)
INCLUDES
USE_PACKAGE(structures)

ENTITY(spram_4800x8,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(addr_i, UINT(13), IN),
		PORT(data_i, UINT(8), IN),
		PORT(wen_i, BIT_TYPE, IN),
		PORT(data_o, UINT(8), OUT)
		)
);

TYPE(mem_t, ARRAY_TYPE(UINT(8),4800));
#ifdef VHDL
SIG(mem, mem_t);// internal;

BEGIN




// adapted from read old data during write ram inferring (quartus coding guidelines)
PROCESS(0, clk, reset_n)
BEGIN
	//IF ( reset_n == BIT(0) ) THEN
	//	data_o <= TO_UINT(0, LEN(data_o));
	IF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		IF (wen_i == BIT(0)) THEN
			mem(TO_INTEGER(PORT_BASE(addr_i))) <= data_i;
//gprintf("#Gwrite %r %r", to_hex(TO_INTEGER(PORT_BASE(data_i))), to_hex(TO_INTEGER(PORT_BASE(addr_i))));
		ENDIF
		data_o <= mem(TO_INTEGER(PORT_BASE(addr_i)));
	ENDIF
END_PROCESS
//}
#else
mem_t mem0;

	SIG(delayed_write, BIT_TYPE);
	SIG(addr, UINT(13));
	SIG(data, UINT(8));
BEGIN




// adapted from read old data during write ram inferring (quartus coding guidelines)
PROCESS(0, clk, reset_n)
BEGIN
	IF ( reset_n == BIT(0) ) THEN

		RESET(addr);
		RESET(data);
		delayed_write <= BIT(0);

	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		IF (delayed_write == BIT(1)) THEN
			mem0(TO_INTEGER(addr)) := data;
		ENDIF

		IF (wen_i == BIT(0)) THEN
			delayed_write <= BIT(1);
			addr <= PORT_BASE(addr_i);
			data <= data_i;
		ELSE
			delayed_write <= BIT(0);
		ENDIF

		data_o <= mem0(TO_INTEGER(PORT_BASE(addr_i)));
	ENDIF
END_PROCESS

#endif
BLK_END;
