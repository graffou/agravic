
#include <stdio.h>
#include <iostream>
#define DEBUG
#define GATED_CLOCKS
//#define NOPRINT

#include <sstream>

#include <iostream>
#include "mbprt.h"
#include "macros.h"
#include <vector>
#include <utility>

//#define DEEP_PLATFORM_DEBUG 1 // To get all messages from the agravic platform
#define gprintf gkprintf
#if DEEP_PLATFORM_DEBUG
#  define giprintf gkprintf
#else
#  define giprintf(...)
#endif
#define gzprintf gkprintf
//#define gzprintf(...)

#include "gmodule.h"
#include "ports.h"
#include "slv.h"
#include "../Include_libs/dsp.h"
#include "scheduler2.h"
#include "kb.h"
#include "input_parms.h"

#include "structures.h"
#include "slv_utils.h"
#include "spram6144x8.h"

// contains registers addresses and address span
#include "../FIRMWARE/Include/reg_def.h"

#include "sUART.h"
#include "SPI_master.h"
//#include "SPI_slave.h"
#include "I2C.h"
//#include "SPI_wrapper.h"
#include "SPI.h"

#include "spram.h"
#include "mem.h"
//#include "mem_delayed.h"
#include "peripherals.h"
#include "dbg_mem.h"
#include "risc_V_constants.h"
#include "register_file.h"
#include "risc-V_core.h"
//#include "risc-V_core.before_wait_instrmem.h"
#include "clk_gen.h"
#include "ddio.h"
#include "sdram_ctrl.h"
#include "csr_irq.h"


#include "spram_font.h"
#include "spram_4800x8.h"

#include "hdmi.h"
#include "dma.h"

#include "top.h"
#include "tb.h"


int main(int argc, char* argv[])
{
	CLI_PARM(bin_file, std::string);
	bin_file.set_mandatory();
	bin_file.set_help("CPU code file in binary format");
	CLI_PARM_INIT(time, int, 10); // default 10ms
	time.set_help("Simulation duration in ms");
	CLI_PARSE(argc, argv);

// instance of testbench
	BLK_INST_TOP(
			tb, tb_t,
			);

	tb.init_file(bin_file);
    init_vcd();
	// init simulator
	scheduler.init();
	
	// run simulator
	scheduler.run(time*256000000000);// convert to ps	and *256 (delta is 8lsb)

	// For risc-V compliance tests
#ifdef NONREG
	bool success = 1;
	uint32_t addr, end_addr;
	for (int i= 2048; i < 2100;i++)
		gprintf("#Cmem content % %", to_hex(i), to_hex(tb.dut.u1_mem.get(i)));
	for (int i= 0; i < 200;i++)
		gprintf("#Mmem content % %", to_hex(i), to_hex(tb.dut.u0_mem.get(i)));
	tb.sig_start_file >> std::hex >> addr;
	std::string yo;
	tb.sig_start_file >> std::hex >> yo; // reads end of line:
	tb.sig_start_file >> std::hex >> end_addr;
	gprintf("#VTesting signature from %Y to %Y", to_hex(addr), to_hex(end_addr));
	addr = addr >> 2;
	end_addr = end_addr >> 2;
	gprintf("#VTesting signature from %Y to %Y", to_hex(addr), to_hex(end_addr));
	while (not tb.check_file.eof())
	{
		uint32_t check_val;
		tb.check_file >> std::hex >> check_val;
		if (not tb.check_file.eof() and (addr <= end_addr) )
		{
			if (check_val == tb.dut.u1_mem.get(addr))
				gprintf("#GChecking % % %", addr, to_hex(check_val), to_hex(tb.dut.u1_mem.get(addr)));
			else
			{
				success = 0;
				gprintf("#RChecking % Expected % read %", addr, to_hex(check_val), to_hex(tb.dut.u1_mem.get(addr)));
			}
		}

		addr ++;
	}
	if (success)
	{
		for (int j = 0 ; j < 10 ; j++) gprintf("#G!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! PASSED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
	else
	{
		for (int j = 0 ; j < 10 ; j++) gprintf("#R!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
#endif

}

