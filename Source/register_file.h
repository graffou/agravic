#include "slv.h"
// Dual port memory implementation of register file
START_OF_FILE(register_file)
INCLUDES

ENTITY(register_file,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(addr_rs1_i, UINT(5), IN),
		PORT(addr_rs2_i, UINT(5), IN),
		PORT(addr_rd_i, UINT(5), IN),
		PORT(wb_en_i, BIT_TYPE, IN),
		PORT(wb_i, UINT(32), IN),
		PORT(rs1_o, UINT(32), OUT),
		PORT(rs2_o, UINT(32), OUT)
		)
);

TYPE(register_file_t, ARRAY_TYPE(UINT(32),32));

SIG(mem, register_file_t);

BEGIN


PROCESS(0, clk, reset_n)
BEGIN
	IF ( reset_n == BIT(0) ) THEN
		//rs1_o <= TO_UINT(0, LEN(rs1_o));
		//rs2_o <= TO_UINT(0, LEN(rs2_o));
		//mem <= OTHERS(TO_UINT(0, LEN(rs1_o)));
	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
	IF ( wb_en_i == BIT(1) ) THEN
			mem(TO_INTEGER(PORT_BASE(addr_rd_i))) <= wb_i;
	ENDIF
		rs1_o <= mem(TO_INTEGER(PORT_BASE(addr_rs1_i))); //
		rs2_o <= mem(TO_INTEGER(PORT_BASE(addr_rs2_i))); //


	ENDIF
END_PROCESS


BLK_END;
