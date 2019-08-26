#include "slv.h"
START_OF_FILE(risc-V_core)
INCLUDES

ENTITY(risc_V_core,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(min, UINT(5), IN),
		PORT(mout, UINT(5), OUT)
		)
);
BEGIN

//CONSTANT SIG(internal, SLV_TYPE(5)) := BIN(01000);// internal;



PROCESS(0, clk, reset_n)
BEGIN
	IF ( reset_n == BIT(0) ) THEN
	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
	ENDIF
END_PROCESS;
//}

BLK_END;
