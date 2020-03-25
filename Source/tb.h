#include "slv.h"
START_OF_FILE(tb)
INCLUDES
USE_PACKAGE(structures)
TESTBENCH(tb_t);



COMPONENT(sUART,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN)
		),
		INTEGER addr
);

COMPONENT(top,
DECL_PORTS(
		PORT(clk_top, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		//PORT(load_port_i, blk2mem_t, IN),
		// GPIOs
		/*
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN),
		*/
		PORT(bmkrD_io, UINT(15), INOUT),
		PORT(bmkrA_io, UINT(7), INOUT),
		PORT(pclk_o, BIT_TYPE, OUT),
		PORT(red_o, BIT_TYPE, OUT),
		PORT(green_o, BIT_TYPE, OUT),
		PORT(blue_o, BIT_TYPE, OUT)//,
		//PORT(gpios_o, UINT(32), OUT)
		)
);


COMPONENT(SPI_wrapper,
DECL_PORTS(
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(data_i, UINT(16), IN),
		PORT(data_o, UINT(16), OUT),
		PORT(data_en_i, BIT_TYPE, IN),
		PORT(data_en_o, BIT_TYPE, OUT),
		PORT(spi_csn_o, BIT_TYPE, OUT),
		PORT(spi_clk_o, BIT_TYPE, OUT),
		PORT(spi_tx_o, BIT_TYPE, OUT),
		PORT(spi_rx_i, BIT_TYPE, IN)
		)
		, INTEGER generic_int

);

SIG(clk, CLK_TYPE);
SIG(clk_100, CLK_TYPE);
SIG(reset_n, RST_TYPE);// reset_n;
SIG(cmd, blk2mem_t);
SIG(gpios, UINT(32));

SIG(blue, BIT_TYPE);
SIG(red, BIT_TYPE);
SIG(green, BIT_TYPE);
SIG(pclk, BIT_TYPE);
SIG(bmkrD, UINT(15));
SIG(bmkrA, UINT(7));

SIG(uart2core, mem2blk_t);
SIG(core2datamem, blk2mem_t);

SIG(sdram_addr, UINT(12));
SIG(sdram_ba, UINT(2));
SIG(sdram_cke, BIT_TYPE);
SIG(sdram_cs, BIT_TYPE);
SIG(sdram_we, BIT_TYPE);
SIG(sdram_ras, BIT_TYPE);
SIG(sdram_cas, BIT_TYPE);
SIG(sdram_dQm, UINT(2));
SIG(sdram_dQ, TRISTATE(16));


SIG(uart_rx, BIT_TYPE);
SIG(uart_tx, BIT_TYPE);
SIG(uart_dbg, BIT_TYPE);

SIG(spi_tx, BIT_TYPE);
SIG(spi_rx, BIT_TYPE);
SIG(spi_csn, BIT_TYPE);
SIG(spi_clk, BIT_TYPE);
SIG(spi2pluto, UINT(16));
SIG(spi2pluto_en,BIT_TYPE );
SIG(pluto2spi, UINT(16));
SIG(pluto2spi_en, BIT_TYPE);

SIG(init_done, BIT_TYPE);


#ifndef VHDL
int ncycles = 10000;
bool success = 1;
std::ifstream code_file;
std::ifstream check_file;
std::ifstream sig_start_file;
#define LOAD_PORT dut.load_port //(&top)->load_port


template<class T>
void init_file( T& name_i)
{
	gstring name = name_i; // input is of input_parm<> type
	code_file.open(name.c_str(), std::ifstream::binary);
	gprintf("#VOpening code file %Y open %Y", name.c_str(), code_file.is_open());
	name.replace(".bin", ".sig_start");
	sig_start_file.open(name.c_str());
	name.replace("elf.sig_start", "reference_output");
	check_file.open(name.c_str());
	gprintf("#VOpening check file %Y open %Y", name.c_str(), check_file.is_open());
}

#endif



BEGIN




BLK_INST(dut, top,
		MAPPING(
				PM(clk_top, clk),
				PM(reset_n, reset_n),
				/*
				PM(uart_tx_o, sUART_tx),
				PM(uart_rx_i, sUART_rx)	,
				*/
				PM(bmkrD_io, bmkrD),
				PM(bmkrA_io, bmkrA),
				PM(blue_o, blue),
				PM(red_o, red),
				PM(green_o, green),
				PM(pclk_o, pclk),
				PM(sdram_addr_o,sdram_addr),
				PM(sdram_ba_o,sdram_ba),
				PM(sdram_cke_o,sdram_cke),
				PM(sdram_cs_o,sdram_cs),
				PM(sdram_we_o,sdram_we),
				PM(sdram_ras_o,sdram_ras),
				PM(sdram_cas_o,sdram_cas),
				PM(sdram_dQm_o,sdram_dQm),
				PM(sdram_dQ_io,sdram_dQ)				//PM(gpios_o, gpios)
				)
			);
SIG(halt_pipe, UINT(32));

// Instantiate this block AFTER top, because dut.mcu must exist before we can map it to clk_peri !!!!!!!!!!!!!!!!!!!!!!!!!!!!
BLK_INST(u0_sUART, sUART,
MAPPING(
		PM(clk_peri, dut.clk_mcu), // get clk from inside dut
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, uart2core),
		PM(uart_tx_o, uart_tx),
		PM(uart_rx_i, uart_rx)
		),
		HEX_INT(UART_TB_REGS) //1073774528

);

BLK_INST(u0_SPI_wrapper, SPI_wrapper,
MAPPING(
		PM(clk_mcu, clk_100),
		PM(reset_n, reset_n),
		PM(spi_clk_o, spi_clk),
		PM(spi_tx_o, spi_tx),
		PM(spi_rx_i, spi_rx),
		PM(spi_clk_o, spi_clk)
		)
		, HEX_INT(SPI_MST_REGS)

);

// Clk 240MHz
FOREVER_PROCESS(0)
//gprintf("#M>01 % ", cnt);
FOREVER_BEGIN
//gprintf("#M>02");
	clk <= BIN(0);
	//gprintf("#M>03");
	FOREVER_WAIT(2083)
	//gprintf("#M>04");
	clk <= BIN(1);
	//gprintf("#M>05");

	FOREVER_WAIT_AND_LOOP(2084)
	//gprintf("#M>06");

FOREVER_END

// Clk 100MHz
FOREVER_PROCESS(2)
//gprintf("#M>2");
FOREVER_BEGIN
	clk_100 <= BIN(0);
	FOREVER_WAIT(5000)
	clk_100 <= BIN(1);
	FOREVER_WAIT_AND_LOOP(5000)
FOREVER_END

// Reset
FOREVER_PROCESS(1)
	//gprintf("#M>1");
FOREVER_BEGIN
	reset_n <= BIN(0);
#ifndef VHDL
	// Directly upload code to program memory

	SIG_SET_BIT(bmkrD,7, BIT(0));// set boot mode
	LOAD_PORT.cs_n <= BIT(1);
	LOAD_PORT.wr_n <= BIT(1);

#endif
	FOREVER_WAIT(55000)
	reset_n <= BIN(1);
	uint32_t addr = 0;
	uint32_t data;
	while (not code_file.eof())
	{
		code_file.read ((char*)&data, sizeof(data)); //>> data;
		dut.u0_mem.set(addr, data);
#ifdef NONREG
		if (addr >= 2048)
			dut.u1_mem.set(addr, data);

#endif
		giprintf("#MLoading data mem % %", to_hex(addr), to_hex(data));
		addr++;
	}
	for (int i = 0; i < 6144; i++)
		dut.u0_mem.get(i);
	FOREVER_WAIT(55000)
	SIG_SET_BIT(bmkrD,7, BIT(0));// reset boot mode

FOREVER_END

COMB_PROCESS(3, clk)
BEGIN

	SIG_SET_BIT(bmkrD,14, uart_tx);
	dut.tb2core = uart2core;
	core2datamem.addr <= dut.core2datamem.addr;//  + TO_UINT(256, LEN(dut.core2datamem.addr));
	core2datamem.data <= dut.core2datamem.data;
	core2datamem.cs_n <= dut.core2datamem.cs_n;
	core2datamem.wr_n <= dut.core2datamem.wr_n;
	//SIG_SET_BIT(bmkrD,8, spi_tx);
	//SIG_SET_BIT(bmkrD,9, spi_clk);
	spi_rx <= B(bmkrD, 10);
#ifdef NONREG
	if (B(halt_pipe, 31) == BIT(1)) // end of nonreg test
	{
		scheduler.end_sim = 1;
	}

	halt_pipe = (RANGE(halt_pipe,30,0) & dut.u0_risc_V_core.halt);
#endif

END_COMB_PROCESS


COMB_PROCESS(4, clk_100)
BEGIN

	SIG_SET_BIT(bmkrD,8, spi_tx);
	SIG_SET_BIT(bmkrD,9, spi_clk);

END_COMB_PROCESS

BLK_END;
