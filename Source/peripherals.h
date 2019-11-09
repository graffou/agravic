#include "slv.h"
START_OF_FILE(peripherals)
INCLUDES
USE_PACKAGE(structures)

#ifndef VHDL
static std::ofstream dbg_file("dbg_file", std::ios::out);
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
//SIG(clk_g, CLK_TYPE);

BEGIN

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
				//gpios <= PORT_BASE(core2mem_i).data;
				gpios_o <= PORT_BASE(core2mem_i).data;
			ENDIF
			IF (PORT_BASE(core2mem_i).addr == BIN(1011111111110)) THEN
				DBG := RANGE(PORT_BASE(core2mem_i).data, 7, 0);
				dbg_o <= DBG;//RANGE(PORT_BASE(core2mem_i).data, 7, 0);
#ifndef VHDL
			//gprintf("#V dbg: %Y",char(TO_INTEGER(PORT_BASE(dbg_o))));
				dbg_file << char(TO_INTEGER(DBG));
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
