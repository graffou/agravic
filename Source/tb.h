#include "slv.h"
START_OF_FILE(tb)
INCLUDES
USE_PACKAGE(structures)
TESTBENCH(tb_t);

#ifndef NONREG
//#define UART_LOADING
#endif
COMPONENT(sUART,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(irq_o, BIT_TYPE, OUT),
		PORT(uart_dma_i, d2p_8_t, IN),
		PORT(uart_dma_o, p2d_8_t, OUT),
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN)
		),
		INTEGER addr
);

COMPONENT(top,
DECL_PORTS(
		PORT(clk_top, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN),
		PORT(spi_clk_io, TRISTATE(1), INOUT),
		PORT(spi_csn_io, TRISTATE(1), INOUT),
		PORT(spi_tx_o, BIT_TYPE, OUT), //whether these are MISO or MOSI depends on SPI mode
		PORT(spi_rx_i, BIT_TYPE, IN),
		PORT(uart_tx_o, BIT_TYPE, OUT), //whether these are MISO or MOSI depends on SPI mode
		PORT(uart_rx_i, BIT_TYPE, IN),
		PORT(pclk_o, BIT_TYPE, OUT),
		PORT(red_o, BIT_TYPE, OUT),
		PORT(green_o, BIT_TYPE, OUT),
		PORT(blue_o, BIT_TYPE, OUT),
		PORT(vga_hsync_o, BIT_TYPE, OUT),
		PORT(vga_vsync_o, BIT_TYPE, OUT),
		PORT(vga_red_o, UINT(4), OUT),
		PORT(vga_green_o, UINT(4), OUT),
		PORT(vga_blue_o, UINT(4), OUT)
		//PORT(gpios_o, UINT(32), OUT)
		//,
		//PORT(debug_o, UINT(10), OUT)
		)
		// not ok for synthesis w/o default value, INTEGER generic_int

);

/*
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
*/

COMPONENT(SPI,
DECL_PORTS(
		PORT(clk_120, CLK_TYPE, IN),
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
		PORT(spi_dma_i, d2p_8_t, IN),
		PORT(spi_dma_o, p2d_8_t, OUT),
		PORT(spi_csn_io, TRISTATE(1), INOUT),
		PORT(spi_clk_io, TRISTATE(1), INOUT),
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
SIG(uart_data, UINT(32));
SIG(uart_cnt, UINT(32));

SIG(blue, BIT_TYPE);
SIG(red, BIT_TYPE);
SIG(green, BIT_TYPE);
SIG(pclk, BIT_TYPE);
//SIG(bmkrD, UINT(15));
//SIG(bmkrA, UINT(7));

SIG(uart2core, mem2blk_t);
SIG(spi2core, mem2blk_t);
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


SIG(uart_irq, BIT_TYPE);
SIG(uart_rx, BIT_TYPE);
SIG(uart_tx, BIT_TYPE);
SIG(uart_dbg, BIT_TYPE);

SIG(spi_tx, BIT_TYPE);
SIG(boot_mode, BIT_TYPE);
SIG(spi_rx, BIT_TYPE);
SIG(spi_csn, TRISTATE(1));
SIG(spi_clk, TRISTATE(1));
SIG(spi2pluto, UINT(16));
SIG(spi2pluto_en,BIT_TYPE );
SIG(pluto2spi, UINT(16));
SIG(pluto2spi_en, BIT_TYPE);

SIG(init_done, BIT_TYPE);

// For UART boot
SIG(cnt_send_byte, UINT(5)); // delay between two program bytes loaded through UART I/F
SIG(cnt_rnd, UINT(16)); // delay between two program bytes loaded through UART I/F
SIG(cnt_byte, UINT(2)); //
SIG(code_loaded, BIT_TYPE);
SIG(code_loadedp, BIT_TYPE);
SIG(UART_byte, UINT(8));
SIG(uart2dma, p2d_8_t);
SIG(dma2uart, d2p_8_t);
SIG(spi2dma, p2d_8_t);
SIG(dma2spi, d2p_8_t);
SIG(spi_trig, BIT_TYPE);

#ifndef VHDL
int ncycles = 10000;
bool success = 1;
std::ifstream code_file;
std::ifstream check_file;
std::ifstream sig_start_file;

gstring bin_file_name;
template<class T>
void init_file( T& name_i)
{
	gstring name = name_i; // input is of input_parm<> type
	bin_file_name = name; // save to reopen
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
				PM(boot_mode_i, boot_mode),
				PM(spi_clk_io, spi_clk),
				PM(spi_csn_io, spi_csn),
				PM(spi_tx_o, spi_tx),
				PM(spi_rx_i, spi_rx),
				PM(uart_tx_o, uart_tx),
				PM(uart_rx_i, uart_rx),
				PM(blue_o, blue),
				PM(red_o, red),
				PM(green_o, green),
				PM(pclk_o, pclk)
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
		PM(uart_dma_o, uart2dma),
		PM(uart_dma_i, dma2uart),
		PM(uart_tx_o, uart_rx),
		PM(irq_o, uart_irq),
		PM(uart_rx_i, uart_tx)
		),
		HEX_INT(UART_TB_REGS) //1073774528

);

BLK_INST(u0_spi, SPI,
MAPPING(
		PM(clk_120, dut.clk_120),
		PM(clk_mcu, dut.clk_mcu),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, spi2core),
		PM(spi_dma_i, dma2spi),
		PM(spi_dma_o, spi2dma),
		PM(trig_i, spi_trig),
		PM(spi_clk_io, spi_clk),
		PM(spi_csn_io, spi_csn),
		PM(spi_tx_o, spi_rx),
		PM(spi_rx_i, spi_tx)
		),
		HEX_INT(SPI_TB_REGS)
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

	boot_mode <= BIT(0);// set boot mode


#endif
	FOREVER_WAIT(55000)
	reset_n <= BIN(1);
#ifndef UART_LOADING // direct memory upload before startup
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
	boot_mode <= BIT(0);// reset boot mode
#endif
FOREVER_END

COMB_PROCESS(3, clk)
BEGIN

	dut.tb2core.data = uart2core.data or spi2core.data;
	dut.tb2core.data_en = (uart2core.data_en or spi2core.data_en);
	core2datamem.addr <= dut.core2datamem.addr;//  + TO_UINT(256, LEN(dut.core2datamem.addr));
	core2datamem.data <= dut.core2datamem.data;
	core2datamem.cs_n <= dut.core2datamem.cs_n;
	core2datamem.wr_n <= dut.core2datamem.wr_n;

#ifdef NONREG
	if (B(halt_pipe, 31) == BIT(1)) // end of nonreg test
	{
		scheduler.end_sim = 1;
	}

	halt_pipe = (RANGE(halt_pipe,30,0) & dut.u0_risc_V_core.halt);
#endif

END_COMB_PROCESS


// Process for UART boot
#ifdef UART_LOADING
PROCESS(5, dut.clk_mcu, reset_n)
VAR(data, UINT(32));
BEGIN
IF (reset_n == BIT(0)) THEN
	code_loaded <= BIT(0);
	code_loadedp <= BIT(0);
	RESET(cnt_send_byte);
	RESET(cnt_byte);
	RESET(cnt_rnd);
	boot_mode <= BIT(1);
ELSEIF (EVENT(dut.clk_mcu) and (dut.clk_mcu == BIT(1)) ) THEN
		IF (code_loadedp == BIT(0)) THEN
			boot_mode <= BIT(1);

			IF ( (uart2dma.rdy == BIT(1)) and ( cnt_send_byte > TO_UINT(8, LEN(cnt_send_byte))) ) THEN//cnt_send_byte == TO_UINT(24, LEN(cnt_send_byte))) THEN
				IF (cnt_byte == BIN(00)) THEN
					code_loadedp <= code_loaded;
					IF (code_file.eof()) THEN
						code_loaded <= BIT(1);
					ELSE
						uint32_t data_tmp;
						code_file.read ((char*)&data_tmp, sizeof(data_tmp)); //>> data;
						data := data_tmp;
						uart_data <= data;
						gprintf("#Mtime % Read % cnt %", cur_time/256000000, to_hex(data_tmp), (TO_INTEGER(uart_cnt)));
					ENDIF
				ELSE
					data :=  SHIFT_RIGHT(uart_data, 8);
					uart_data <= data;
				ENDIF
				core2datamem.addr <= TO_UINT(HEX_INT(UART_TB_REGS), LEN(core2datamem.addr));//0x1ff0; //dut.core2datamem.addr  + TO_UINT(256, LEN(dut.core2datamem.addr));
				//core2datamem.data <= (HEX(00010000) or ( data and HEX(000000FF)) );
				core2datamem.data <= (HEX(01B10000) or ( data and HEX(000000FF)) );
				core2datamem.cs_n <= code_loaded;
				core2datamem.wr_n <= BIT(0);
				UART_byte <= RANGE(data, 7, 0);
				uart_cnt <= uart_cnt + 1;
				RESET(cnt_send_byte);
				cnt_byte <= cnt_byte + 1;
				RESET(cnt_rnd);
			ELSE
				core2datamem.cs_n <= BIT(1);
				core2datamem.wr_n <= BIT(1);
				cnt_send_byte <= cnt_send_byte + 1;
			ENDIF
		ELSE
			cnt_rnd <= cnt_rnd + 1;
			IF ( cnt_rnd == TO_UINT(333, LEN(cnt_rnd)) ) THEN // test boot signal bouncing
				boot_mode <= BIT(0);/*
			ELSEIF ( cnt_rnd == TO_UINT(1353, LEN(cnt_rnd)) ) THEN
				boot_mode <= BIT(1);
			ELSEIF ( cnt_rnd == TO_UINT(1373, LEN(cnt_rnd)) ) THEN
				boot_mode <= BIT(0);
			ELSEIF ( cnt_rnd == TO_UINT(2393, LEN(cnt_rnd)) ) THEN
					code_loaded <= BIT(0); // test code reloading
					code_loadedp <= BIT(0);
					code_file.close();
					init_file(bin_file_name);
					cnt_byte <= BIN(00);
				boot_mode <= BIT(1);
			ELSEIF ( cnt_rnd == TO_UINT(2433, LEN(cnt_rnd)) ) THEN
					boot_mode <= BIT(0);*/
			ENDIF
		ENDIF
ENDIF

END_PROCESS

#endif

#ifndef VHDL
// scn file control
FOREVER_PROCESS(7)
//gprintf("#M>01 % ", cnt);
FOREVER_BEGIN

	//init
	int next_event_time = parse_scn_file();


	FOREVER_WAIT_AND_LOOP(next_event_time)
	//gprintf("#M>06");

FOREVER_END
#endif

BLK_END;
