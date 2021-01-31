#include "slv.h"
#include "../FIRMWARE/Include/reg_def.h" // required by block register addresses
//#include "slv.h"
START_OF_FILE(risc-V_core)
INCLUDES
USE_PACKAGE(structures)

ENTITY(top,
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
		PORT(vga_blue_o, UINT(4), OUT),
#ifdef TEST_SDRAM_CTRL
		PORT(sdram_addr_o, UINT(12), OUT),
		PORT(sdram_ba_o, UINT(2), OUT),
		PORT(sdram_cke_o, BIT_TYPE, OUT),
		PORT(sdram_cs_o, BIT_TYPE, OUT),
		PORT(sdram_we_o, BIT_TYPE, OUT),
		PORT(sdram_cas_o, BIT_TYPE, OUT),
		PORT(sdram_ras_o, BIT_TYPE, OUT),
		PORT(sdram_dQm_o, UINT(2), OUT),

		PORT(sdram_dQ_io, TRISTATE(16), INOUT)
#endif
		//PORT(gpios_o, UINT(32), OUT)
		//,
		//PORT(debug_o, UINT(10), OUT)
		)
		// not ok for synthesis w/o default value, INTEGER generic_int

);

COMPONENT(hdmi,
DECL_PORTS(
		PORT(clk_hdmi, CLK_TYPE, IN),
		PORT(clk_core, CLK_TYPE, IN),
		PORT(clk_pix, CLK_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN),
		PORT(trap_i, BIT_TYPE, IN), //
		PORT(dbg_i, UINT(33), IN), // for code loading
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN), // reg access
		PORT(mem2core_o, mem2blk_t, OUT), // reg access
		PORT(pclk_o, BIT_TYPE, OUT),
		PORT(red_o, BIT_TYPE, OUT),
		PORT(green_o, BIT_TYPE, OUT),
		PORT(blue_o, BIT_TYPE, OUT),
		PORT(vga_hsync_o, BIT_TYPE, OUT),
		PORT(vga_vsync_o, BIT_TYPE, OUT),
		PORT(vga_red_o, UINT(4), OUT),
		PORT(vga_green_o, UINT(4), OUT),
		PORT(vga_blue_o, UINT(4), OUT)
		)
		, INTEGER generic_int

);

COMPONENT(dma,
DECL_PORTS(
		PORT(clk_dma, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN), // reg access
		PORT(mem2core_o, mem2blk_t, OUT), // reg access
		PORT(mem2dma_i, mem2blk_t, IN),

		// UART
		PORT(uart_dma_i, p2d_8_t, IN),
		PORT(uart_dma_o, d2p_8_t, OUT),
		PORT(spi_dma_i, p2d_8_t, IN),
		PORT(spi_dma_o, d2p_8_t, OUT),
		PORT(irq_dma_o, irq_dma_t, OUT),
		PORT(core_grant_i, BIT_TYPE, IN),
		PORT(core_request_o, blk2mem_t, OUT)
		)
		, INTEGER generic_int

);

COMPONENT(sUART,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN), // have to reset the UART for DMA
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(uart_dma_i, d2p_8_t, IN),
		PORT(uart_dma_o, p2d_8_t, OUT),
		PORT(uart_rts_o, BIT_TYPE, OUT),
		PORT(uart_cts_i, BIT_TYPE, IN),
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN)
		),
		INTEGER generic_int
);

COMPONENT(risc_V_core,
DECL_PORTS(
		PORT(clk_core, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN), // for code loading
		PORT(trap_o, BIT_TYPE, OUT), // for code loading
		PORT(dbg_o, UINT(33), OUT), // for code loading
		// core to instruction memory
		PORT(load_port_i, blk2mem_t, IN),
		PORT(core2instmem_o, blk2mem_t, OUT),
		PORT(instmem2core_i, mem2blk_t, IN),
		// core to data memory
		PORT(core2datamem_o, blk2mem_t, OUT),
		//PORT(dma2core_i, blk2mem_t, IN), // These are DMA write requests from peripherals / are transmitted when mem bus is not used by core
		PORT(dma_request_i, blk2mem_t, IN),
		PORT(dma_grant_o, BIT_TYPE, OUT),
		PORT(csr2core_i, csr2core_t, IN),
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
		, INTEGER generic_int

);

COMPONENT(peripherals,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(dbg_o, UINT(8), OUT),
		PORT(gpios_o, UINT(32), OUT)
		)
		, INTEGER generic_int
);

COMPONENT( clk_gen,
	DECL_PORTS
	(
		PORT(areset, RST_TYPE, IN),// 		: IN STD_LOGIC  := '0';
		PORT(inclk0, CLK_TYPE, IN),	//	: IN STD_LOGIC  := '0';
		PORT(c0, CLK_TYPE, OUT),	//	: OUT STD_LOGIC ;
		PORT(c1, CLK_TYPE, OUT),	//	: OUT STD_LOGIC ;
		PORT(c2, CLK_TYPE, OUT),	//	: OUT STD_LOGIC ;
		PORT(c3, CLK_TYPE, OUT),	//	: OUT STD_LOGIC ;
		PORT(locked, BIT_TYPE, OUT)	//	: OUT STD_LOGIC
	)
);


COMPONENT(sdram_ctrl,
DECL_PORTS(
		PORT(clk_sdram, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(req0_i, sdram_req_t, IN),
		PORT(req1_i, sdram_req_t, IN),
		PORT(data_i, UINT(16), IN),
		PORT(addr_o, UINT(12), OUT),
		PORT(ba_o, UINT(2), OUT),
		PORT(data_o, UINT(16), OUT),
		PORT(cke_o, BIT_TYPE, OUT),
		PORT(cs_o, BIT_TYPE, OUT),
		PORT(we_o, BIT_TYPE, OUT),
		PORT(cas_o, BIT_TYPE, OUT),
		PORT(ras_o, BIT_TYPE, OUT),
		PORT(en0_o, BIT_TYPE, OUT), // either data _en or data_request to initiator
		PORT(en1_o, BIT_TYPE, OUT),
		PORT(dQm_o, UINT(2), OUT),

		PORT(dQ_io, TRISTATE(16), INOUT)

		)
		, INTEGER generic_int
);

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

COMPONENT(SPI_master,
DECL_PORTS(
		PORT(clk_120, CLK_TYPE, IN),
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
		PORT(spi_dma_i, d2p_8_t, IN),
		PORT(spi_dma_o, p2d_8_t, OUT),
		PORT(spi_csn_o, BIT_TYPE, OUT),
		PORT(spi_clk_o, BIT_TYPE, OUT),
		PORT(spi_tx_o, BIT_TYPE, OUT),
		PORT(spi_rx_i, BIT_TYPE, IN)
		)
		, INTEGER generic_int
);

COMPONENT(SPI_slave,
DECL_PORTS(
		PORT(clk_120, CLK_TYPE, IN),
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(spi_dma_i, d2p_8_t, IN),
		PORT(spi_dma_o, p2d_8_t, OUT),
		PORT(spi_csn_i, BIT_TYPE, IN),
		PORT(spi_clk_i, BIT_TYPE, IN),
		PORT(spi_tx_o, BIT_TYPE, OUT),
		PORT(spi_rx_i, BIT_TYPE, IN)
		)
		, INTEGER generic_int
);

COMPONENT(csr_irq,
DECL_PORTS(
		PORT(clk_csr_irq, CLK_TYPE, IN), // might be the 'always on' clock in the future
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(irq_i, irq_t, IN), //Array of external IRQs
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(csr2core_o, csr2core_t, OUT)
		),
		INTEGER generic_int
);



SIG(boot_mode, BIT_TYPE);

SIG(load_port, blk2mem_t);
SIG(core2instmem, blk2mem_t);
SIG(instmem2core, mem2blk_t);
SIG(core2datamem, blk2mem_t);
SIG(datamem2core, mem2blk_t);
SIG(all2core, mem2blk_t);
SIG(uart2core, mem2blk_t);
SIG(dma2core, mem2blk_t);
SIG(hdmi2core, mem2blk_t);
SIG(spi2core, mem2blk_t);
SIG(spi_slv2core, mem2blk_t);
SIG(csr_irq2core, mem2blk_t);

SIG(dbg, UINT(8));
SIG(gpios, UINT(32));
SIG(gate_cell, BIT_TYPE);
SIG(clk_mcu, CLK_TYPE);
SIG(clk_24, CLK_TYPE);
SIG(clk_120, CLK_TYPE);
SIG(clk_240, CLK_TYPE);
// use this to force a response from tb blocks
SIG(tb2core, mem2blk_t);
// SAMD21 I/F
SIG(bmkrD, UINT(15));
SIG(bmkrA, UINT(6));
SIG(uart_rx, BIT_TYPE);
SIG(uart_tx, BIT_TYPE);
SIG(uart_rts, BIT_TYPE);
SIG(uart_cts, BIT_TYPE);

SIG(uart2dma, p2d_8_t);
SIG(dma2uart, d2p_8_t);
SIG(spi2dma, p2d_8_t);
SIG(dma2spi, d2p_8_t);
SIG(dma_request, blk2mem_t);
SIG(dma2irq, irq_dma_t);
SIG(dma_grant, BIT_TYPE);
SIG(trap, BIT_TYPE);

SIG(sdram_req0, sdram_req_t);
SIG(sdram_req1, sdram_req_t);

SIG(sdram_en0, BIT_TYPE);
SIG(sdram_en1, BIT_TYPE);

SIG(hdmi2sdram_data, UINT(16));
SIG(sdram2hdmi_data, UINT(16));
SIG(dbg33, UINT(33));

SIG(spi_clk, BIT_TYPE);
SIG(spi_csn, BIT_TYPE);
SIG(spi_mosi, BIT_TYPE);
SIG(spi_miso, BIT_TYPE);
SIG(spi_miso2, BIT_TYPE);
SIG(spi_trig, BIT_TYPE);

SIG(csr2core, csr2core_t);
SIG(irq_vec, irq_t); // Array of 16 external IRQs

//SIG(sUART_tx, BIT_TYPE);
//SIG(sUART_rx, BIT_TYPE);
//CONST(ZERO, RST_TYPE) := BIT(0);
BEGIN

BLK_INST(u0_clk_gen, clk_gen,
MAPPING(
		PM(areset, reset_n), // PG is forced to zero in VHDL code
		PM(inclk0, clk_top),
		PM(c0, clk_mcu),
		PM(c1, clk_24),
		PM(c2, clk_120),
		PM(c3, clk_240),
		)
);

BLK_INST(u0_csr_irq, csr_irq,
MAPPING(
		PM(clk_csr_irq, clk_mcu),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, csr_irq2core),
		PM(irq_i, irq_vec),
		PM(csr2core_o, csr2core)
		),
		HEX_INT(CSR_IRQ_REGS)
);

BLK_INST(u0_hdmi, hdmi,
MAPPING(
		PM(clk_hdmi, clk_120),
		PM(clk_core, clk_mcu),
		PM(clk_pix, clk_24),
		PM(reset_n, reset_n),
		PM(boot_mode_i, boot_mode_i),
		PM(trap_i, trap),
		PM(dbg_i, dbg33),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, hdmi2core),
		//PM(debug, debug_o)
		PM(blue_o, blue_o),
		PM(green_o, green_o),
		PM(red_o, red_o),
		PM(pclk_o, pclk_o),
		PM(vga_hsync_o, vga_hsync_o),
		PM(vga_vsync_o, vga_vsync_o),
		PM(vga_blue_o, vga_blue_o),
		PM(vga_green_o, vga_green_o),
		PM(vga_red_o, vga_red_o)
		),
		HEX_INT(HDMI_REGS)
);


BLK_INST(u0_dma, dma,
MAPPING(
		PM(clk_dma, clk_mcu),
		PM(reset_n, reset_n),
		PM(boot_mode_i, boot_mode_i),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, dma2core),
		PM(mem2dma_i, datamem2core),
		PM(uart_dma_i, uart2dma),
		PM(uart_dma_o, dma2uart),
		PM(spi_dma_i, spi2dma),
		PM(spi_dma_o, dma2spi),
		PM(irq_dma_o, dma2irq),
		PM(core_grant_i, dma_grant),
		PM(core_request_o, dma_request)
	),
	HEX_INT(DMA_REGS)
);


BLK_INST(u0_sUART, sUART,
MAPPING(
		PM(clk_peri, clk_mcu),
		PM(reset_n, reset_n),
		PM(boot_mode_i, boot_mode_i),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, uart2core),
		PM(uart_dma_o, uart2dma),
		PM(uart_dma_i, dma2uart),
		PM(uart_rts_o, uart_rts), // actually not connected at top level
		PM(uart_cts_i, uart_cts), // forced to 0 below
		PM(uart_tx_o, uart_tx_o),
		PM(uart_rx_i, uart_rx_i)
		),
		HEX_INT(UART_REGS) //HEX_INT(40001FF0) //1073774528
);

BLK_INST(u0_risc_V_core, risc_V_core,
		MAPPING(
				PM(clk_core, clk_mcu),
				PM(reset_n, reset_n),
				PM(boot_mode_i, boot_mode_i),
				PM(trap_o, trap),
				PM(dbg_o, dbg33),
				PM(load_port_i, load_port),
				PM(core2instmem_o,core2instmem),
				PM(instmem2core_i, instmem2core),
				PM(core2datamem_o,core2datamem),
				PM(dma_request_i, dma_request),
				PM(dma_grant_o, dma_grant),
				PM(csr2core_i, csr2core),
				PM(datamem2core_i, all2core),
				)
		);

// Instruction RAM (ROM)
//BLK_INST(u0_mem, mem_delayed,
BLK_INST(u0_mem, mem,
		MAPPING(
				PM(clk_mem, clk_mcu),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2instmem),
				PM(mem2core_o, instmem2core)
				)
				, 6144 //
		);

// Data RAM
BLK_INST(u1_mem, mem,
		MAPPING(
				PM(clk_mem, clk_mcu),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2datamem),
				PM(mem2core_o, datamem2core)
				)
				, 6144
		);

// Base address is situated in sram
// gpios are output IOs
// TODO This will change
BLK_INST(u0_peripherals, peripherals,
		MAPPING(
				PM(clk_peri, clk_mcu),
				PM(reset_n, reset_n),
				PM(core2mem_i,core2datamem),
				PM(dbg_o, dbg),
				PM(gpios_o, gpios)
				),
				HEX_INT(HDMI_REGS) // peripherals does display chars in dbg_file as well
		);

#ifdef TEST_SDRAM_CTRL
BLK_INST( u0_sdram_ctrl, sdram_ctrl,
		MAPPING(
				PM(clk_sdram, clk_120),
				PM(reset_n, reset_n),
				PM(req0_i, sdram_req0),
				PM(req1_i, sdram_req1),
				PM(data_i, hdmi2sdram_data),
				PM(data_o, sdram2hdmi_data),
				PM(addr_o,sdram_addr_o),
				PM(ba_o,sdram_ba_o),
				PM(cke_o,sdram_cke_o),
				PM(cs_o,sdram_cs_o),
				PM(we_o,sdram_we_o),
				PM(ras_o,sdram_ras_o),
				PM(cas_o,sdram_cas_o),
				PM(dQm_o,sdram_dQm_o),
				PM(dQ_io,sdram_dQ_io)
				),
				HEX_INT(SDRAM_REGS)
		);
#endif

BLK_INST(u0_spi, SPI,
MAPPING(
		PM(clk_120, clk_120),
		PM(clk_mcu, clk_mcu),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, spi2core),
		PM(spi_dma_i, dma2spi),
		PM(spi_dma_o, spi2dma),
		PM(trig_i, spi_trig),
		PM(spi_clk_io, spi_clk_io),
		PM(spi_csn_io, spi_csn_io),
		PM(spi_tx_o, spi_tx_o),
		PM(spi_rx_i, spi_rx_i)
		),
		HEX_INT(SPI_REGS)
);


#ifdef SPI_MASTER
BLK_INST(u0_spi, SPI_master,
MAPPING(
		PM(clk_120, clk_240),
		PM(clk_mcu, clk_mcu),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, spi2core),
		PM(spi_dma_i, dma2spi),
		PM(spi_dma_o, spi2dma),
		PM(trig_i, spi_trig),
		PM(spi_clk_o, spi_clk),
		PM(spi_csn_o, spi_csn),
		PM(spi_tx_o, spi_mosi),
		PM(spi_rx_i, spi_miso)
//		,
//		(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
//		(, d2p_8_t, IN),
//		(spi_dma_o, p2d_8_t, OUT),
//		(spi_csn_o, BIT_TYPE, OUT),
//		(spi_clk_o, BIT_TYPE, OUT),
//		(spi_tx_o, BIT_TYPE, OUT),
//		(spi_rx_i, BIT_TYPE, IN)
//		)
		),
		HEX_INT(SPI_REGS)
);
#endif

#ifdef SPI_SLAVE
BLK_INST(u0_spi_slv, SPI_slave,
MAPPING(
		PM(clk_120, clk_240),
		PM(clk_mcu, clk_mcu),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, spi_slv2core),
		PM(spi_dma_i, dma2spi),
		PM(spi_dma_o, spi2dma),
		PM(spi_clk_i, spi_clk),
		PM(spi_csn_i, spi_csn),
		PM(spi_rx_i, spi_mosi),
		PM(spi_tx_o, spi_miso)
//		,
//		(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
//		(, d2p_8_t, IN),
//		(spi_dma_o, p2d_8_t, OUT),
//		(spi_csn_o, BIT_TYPE, OUT),
//		(spi_clk_o, BIT_TYPE, OUT),
//		(spi_tx_o, BIT_TYPE, OUT),
//		(spi_rx_i, BIT_TYPE, IN)
//		)
		),
		HEX_INT(SPI_SLV_REGS)
);
#endif



// Combine responses of memory and peripherals (for now sUART) ----------
COMB_PROCESS(1, clk_top)

#ifdef VHDL
	tb2core.data <= TO_UINT(0, 32);
	tb2core.data_en <= BIT(0);
#endif

#ifdef SPI_SLAVE
	all2core.data <= ( datamem2core.data or uart2core.data or dma2core.data or tb2core.data or hdmi2core.data or spi_slv2core.data or csr_irq2core.data);
	all2core.data_en <= ( datamem2core.data_en or uart2core.data_en or dma2core.data_en or tb2core.data_en or hdmi2core.data_en or spi_slv2core.data_en or csr_irq2core.data_en);
#elsif defined(SPI_MASTER)
	all2core.data <= ( datamem2core.data or uart2core.data or dma2core.data or tb2core.data or hdmi2core.data or spi2core.data or csr_irq2core.data);
	all2core.data_en <= ( datamem2core.data_en or uart2core.data_en or dma2core.data_en or tb2core.data_en or hdmi2core.data_en or spi2core.data_en or csr_irq2core.data_en);
#else
	all2core.data <= ( datamem2core.data or uart2core.data or dma2core.data or tb2core.data or hdmi2core.data or csr_irq2core.data or spi2core.data);
	all2core.data_en <= ( datamem2core.data_en or uart2core.data_en or dma2core.data_en or tb2core.data_en or hdmi2core.data_en or csr_irq2core.data_en or spi2core.data_en);
#endif


	// A bit tricky for inouts
	//boot_mode <= B(PORT_BASE(bmkrD_io), 7); // changed from 8 (is MISO)
	//uart_rx <= B(PORT_BASE(bmkrD_io), 14);
	uart_cts <= BIT(0);
	//SIG_SET_BIT(PORT_BASE(bmkrD_io),13, uart_tx);
	//SIG_SET_BIT(PORT_BASE(bmkrD_io),12, uart_rts);
	//SIG_SET_BIT(irq_vec, 0, dma2irq(0).dma2irq(0));
	irq_vec(0) <=  dma2irq(0);
	irq_vec(1) <=  dma2irq(1);


#ifdef VHDL
	load_port.wr_n <= BIT(1);
#endif

END_COMB_PROCESS

#ifndef VHDL
void check()
{
	gprintf("#Vcore2instmem % core2datamem % instmem2core % datamem2core %", &core2instmem.data, &core2datamem.data, &instmem2core.data, &datamem2core.data);
}
#endif
BLK_END;
