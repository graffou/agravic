#include "slv.h"
START_OF_FILE(peripherals)
INCLUDES
USE_PACKAGE(structures)


ENTITY(peripherals,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(gpios_o, UINT(32), OUT)
		)
);

//SIG(gpios, UINT(32));

BEGIN





PROCESS(0, clk, reset_n)
BEGIN
	IF ( reset_n == BIT(0) ) THEN
	gpios_o <= TO_UINT(0, 32);
	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( PORT_BASE(core2mem_i).wr_n == BIT(0) ) ) THEN
			IF (PORT_BASE(core2mem_i).addr == BIN(1111111111111)) THEN
				gpios_o <= PORT_BASE(core2mem_i).data;
			ENDIF
		ENDIF
	ENDIF
END_PROCESS
//}

BLK_END;
