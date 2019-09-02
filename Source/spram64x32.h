#if 1
#include "slv.h"
START_OF_FILE(spram64x32)
INCLUDES

ENTITY(spram64x32,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(addr_i, UINT(14), IN),
		PORT(data_i, UINT(32), IN),
		PORT(data_o, UINT(32), OUT),
		PORT(cs_n_i, BIT_TYPE, IN),
		PORT(wr_n_i, BIT_TYPE, IN)
		)
);

TYPE(mem_t, ARRAY_TYPE(UINT(32),16384));

SIG(mem, mem_t);// internal;

BEGIN





PROCESS(0, clk, reset_n)
BEGIN
	IF ( reset_n == BIT(0) ) THEN
		data_o <= TO_UINT(0, LEN(data_o));
	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		IF (cs_n_i == BIT(1)) THEN
			IF (wr_n_i == BIT(1)) THEN
				data_o <= mem(TO_INTEGER(addr_i));
			ELSE
				mem(TO_INTEGER(addr_i)) <= data_i;
			ENDIF
		ENDIF
	ENDIF
END_PROCESS
//}

BLK_END;
#endif
