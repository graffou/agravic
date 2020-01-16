#include "slv.h"

START_OF_FILE(spram6144x8)
INCLUDES
USE_PACKAGE(structures)

ENTITY(spram6144x8,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, BIT_TYPE, IN),
		PORT(addr_i, UINT(data_addr_span - 2), IN),
		PORT(data_i, UINT(8), IN),
		PORT(data_o, UINT(8), OUT),
		PORT(wen_i, BIT_TYPE, IN)
		)
);

TYPE(mem_t, ARRAY_TYPE(UINT(8),6144));

SIG(mem, mem_t);// internal;

BEGIN




// adapted from read old data during write ram inferring (quartus coding guidelines)
PROCESS(0, clk, reset_n)
BEGIN
	//IF ( reset_n == BIT(0) ) THEN
	//	data_o <= TO_UINT(0, LEN(data_o));
	IF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		IF (wen_i == BIT(1)) THEN
			mem(TO_INTEGER(PORT_BASE(addr_i))) <= data_i;
//gprintf("#Gwrite %r %r", to_hex(TO_INTEGER(PORT_BASE(data_i))), to_hex(TO_INTEGER(PORT_BASE(addr_i))));
		ENDIF
		data_o <= mem(TO_INTEGER(PORT_BASE(addr_i)));
	ENDIF
END_PROCESS
//}

BLK_END;
