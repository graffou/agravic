#include "slv.h"
//#include "slv.h"
START_OF_FILE(risc-V_core)
INCLUDES
USE_PACKAGE(structures)

ENTITY(top,
DECL_PORTS(
		PORT(clk_top, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(load_port_i, blk2mem_t, IN),
		// GPIOs
		PORT(gpios_o, UINT(32), OUT)
		)
);

COMPONENT(risc_V_core,
DECL_PORTS(
		PORT(clk_core, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		// core to instruction memory
		PORT(load_port_i, blk2mem_t, IN),
		PORT(core2instmem_o, blk2mem_t, OUT),
		PORT(instmem2core_i, mem2blk_t, IN),
		// core to data memory
		PORT(core2datamem_o, blk2mem_t, OUT),
		PORT(datamem2core_i, mem2blk_t, IN)
		// GPIOs
		//PORT(gpios_o, UINT(32), OUT)
		)
);

COMPONENT(mem,
DECL_PORTS(
		PORT(clk_mem, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT)
		)
);

COMPONENT(peripherals,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(gpios_o, UINT(32), OUT)
		)
);

SIG(core2instmem, blk2mem_t);
SIG(instmem2core, mem2blk_t);
SIG(core2datamem, blk2mem_t);
SIG(datamem2core, mem2blk_t);


BEGIN

BLK_INST(u0_risc_V_core, risc_V_core,
		MAPPING(
				PM(clk_core, clk_top),
				PM(reset_n, reset_n),
				PM(load_port_i, load_port_i),
				PM(core2instmem_o,core2instmem),
				PM(instmem2core_i, instmem2core),
				PM(core2datamem_o,core2datamem),
				PM(datamem2core_i, datamem2core),
				)
		);

BLK_INST(u0_mem, mem,
		MAPPING(
				PM(clk_mem, clk_top),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2instmem),
				PM(mem2core_o, instmem2core)
				)
		);

BLK_INST(u1_mem, mem,
		MAPPING(
				PM(clk_mem, clk_top),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2datamem),
				PM(mem2core_o, datamem2core)
				)
		);

BLK_INST(u0_peripherals, peripherals,
		MAPPING(
				PM(clk_peri, clk_top),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2datamem),
				PM(gpios_o, gpios_o)
				)
		);
#ifndef VHDL
void check()
{
	gprintf("#Vcore2instmem % core2datamem % instmem2core % datamem2core %", &core2instmem.data, &core2datamem.data, &instmem2core.data, &datamem2core.data);
}
#endif
BLK_END;
