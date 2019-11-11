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
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN),
		PORT(gpios_o, UINT(32), OUT)
		)
);

COMPONENT(sUART,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN)
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
		PORT(dbg_o, UINT(8), OUT),
		PORT(gpios_o, UINT(32), OUT)
		)
);

SIG(core2instmem, blk2mem_t);
SIG(instmem2core, mem2blk_t);
SIG(core2datamem, blk2mem_t);
SIG(datamem2core, mem2blk_t);
SIG(all2core, mem2blk_t);
SIG(uart2core, mem2blk_t);
SIG(dbg, UINT(8));
//SIG(sUART_tx, BIT_TYPE);
//SIG(sUART_rx, BIT_TYPE);

BEGIN

BLK_INST(u0_sUART, sUART,
MAPPING(
		PM(clk_peri, clk_top),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, uart2core),
		PM(uart_tx_o, uart_tx_o),
		PM(uart_rx_i, uart_rx_i)
		)
);

BLK_INST(u0_risc_V_core, risc_V_core,
		MAPPING(
				PM(clk_core, clk_top),
				PM(reset_n, reset_n),
				PM(load_port_i, load_port_i),
				PM(core2instmem_o,core2instmem),
				PM(instmem2core_i, instmem2core),
				PM(core2datamem_o,core2datamem),
				PM(datamem2core_i, all2core),
				)
		);

// Instruction RAM (ROM)
BLK_INST(u0_mem, mem,
		MAPPING(
				PM(clk_mem, clk_top),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2instmem),
				PM(mem2core_o, instmem2core)
				)
		);

// Data RAM
BLK_INST(u1_mem, mem,
		MAPPING(
				PM(clk_mem, clk_top),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2datamem),
				PM(mem2core_o, datamem2core)
				)
		);

// Base address is situated in sram
// gpios are output IOs
// TODO This will change
BLK_INST(u0_peripherals, peripherals,
		MAPPING(
				PM(clk_peri, clk_top),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2datamem),
				PM(dbg_o, dbg),
				PM(gpios_o, gpios_o)
				)
		);

// Combine responses of memory and peripherals (for now sUART) ----------
COMB_PROCESS(1, clk_top)

	all2core.data <= ( datamem2core.data or uart2core.data );
	all2core.data_en <= ( datamem2core.data_en or uart2core.data_en );

END_COMB_PROCESS

#ifndef VHDL
void check()
{
	gprintf("#Vcore2instmem % core2datamem % instmem2core % datamem2core %", &core2instmem.data, &core2datamem.data, &instmem2core.data, &datamem2core.data);
}
#endif
BLK_END;
