//#include "macros.h"
//#include "slv.h"
START_OF_FILE(risc-V_core)
INCLUDES
USE_PACKAGE(structures)

ENTITY(risc_V_core,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		// core to instruction memory
		PORT(core2instmem_o, blk2mem_t, OUT),
		PORT(instmem2core_i, mem2blk_t, IN),
		// core to data memory
		PORT(core2datamem_o, blk2mem_t, OUT),
		PORT(datamem2core_i, mem2blk_t, IN),
		// GPIOs
		PORT(gpios_o, UINT(32), OUT)
		)
);
BEGIN

//CONSTANT SIG(internal, SLV_TYPE(5)) := BIN(01000);// internal;
TYPE(reg_file_t, ARRAY_TYPE(UINT(32), 32));
SIG(registers, reg_file_t);
SIG(PC, UINT(32));
SIG(pipe, UINT(4));

//#define := =

PROCESS(0, clk, reset_n)
BEGIN
	VAR(instr, UINT(32));
	IF ( reset_n == BIT(0) ) THEN
	// reset statements

	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
	// rising edge
	instr := PORT_BASE(instmem2core_i).data;
	pipe <= pipe;//CAT( RANGE(pipe, HI(pipe)-1, 0), data_inst_en_i);

	//
	if ( PORT_BASE(instmem2core_i).data_en == BIT(1) ) THEN
	ENDIF
	ELSEIF ( EVENT(clk) and (clk == BIT(0)) ) THEN
	// falling edge

	ENDIF
END_PROCESS;
//}

BLK_END;
