#include "slv.h"
START_OF_FILE(mem)
INCLUDES
USE_PACKAGE(structures)


ENTITY(mem,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT)
		)
);

TYPE(mem_t, ARRAY_TYPE(UINT(8),6144));

SIG(mem0, mem_t);// internal;
SIG(mem1, mem_t);// internal;
SIG(mem2, mem_t);// internal;
SIG(mem3, mem_t);// internal;

BEGIN





PROCESS(0, clk, reset_n)
VAR(baddr, UINT(13));
VAR(rdata, UINT(32));
BEGIN
	IF ( reset_n == BIT(0) ) THEN
		PORT_BASE(mem2core_o).data <= TO_UINT(0, LEN(PORT_BASE(mem2core_o).data));
		PORT_BASE(mem2core_o).data_en <= BIT(0);
	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		IF (PORT_BASE(core2mem_i).cs_n == BIT(0)) THEN
			baddr := PORT_BASE(core2mem_i).addr;
			IF (PORT_BASE(core2mem_i).wr_n == BIT(1)) THEN
				rdata := 	mem3(TO_INTEGER(baddr)) &
							mem2(TO_INTEGER(baddr)) &
							mem1(TO_INTEGER(baddr)) &
							mem0(TO_INTEGER(baddr));
				PORT_BASE(mem2core_o).data <= rdata;
				PORT_BASE(mem2core_o).data_en <= BIT(1);
			ELSE
				//rmem(TO_INTEGER(PORT_BASE(core2mem_i).addr)) <= PORT_BASE(core2mem_i).data;
				IF ( B(PORT_BASE(core2mem_i).be, 3) == BIT(1) ) THEN
					mem3(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 31, 24);
				ENDIF
				IF ( B(PORT_BASE(core2mem_i).be, 2) == BIT(1) ) THEN
					mem2(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 23, 16);
				ENDIF
				IF ( B(PORT_BASE(core2mem_i).be, 1) == BIT(1) ) THEN
					mem1(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 15, 8);
				ENDIF
				IF ( B(PORT_BASE(core2mem_i).be, 0) == BIT(1) ) THEN
					mem0(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 7, 0);
				ENDIF

				PORT_BASE(mem2core_o).data_en <= BIT(0);
			ENDIF
		ELSE
			PORT_BASE(mem2core_o).data_en <= BIT(0);
		ENDIF
	ENDIF
END_PROCESS
//}

BLK_END;
