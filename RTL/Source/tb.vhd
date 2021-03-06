-- vhdl file of block tb generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
library work; use work.structures.all;
entity tb_t is \ begin end tb_t; architecture rtl of tb_t is component dummy_zkw_pouet is port(clk : in std_logic);end component;




component sUART is generic (generic_int: integer); port ( clk_peri : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; irq_o : OUT std_logic; uart_dma_i : IN d2p_8_t; uart_dma_o : OUT p2d_8_t; uart_tx_o : OUT std_logic; uart_rx_i : IN std_logic ); end component;
component top is port ( clk_top : IN std_logic; reset_n : IN std_logic; boot_mode_i : IN std_logic; spi_clk_io : INOUT unsigned ((1 -1) downto 0); spi_csn_io : INOUT unsigned ((1 -1) downto 0); spi_tx_o : OUT std_logic; spi_rx_i : IN std_logic; uart_tx_o : OUT std_logic; uart_rx_i : IN std_logic; pclk_o : OUT std_logic; red_o : OUT std_logic; green_o : OUT std_logic; blue_o : OUT std_logic; vga_hsync_o : OUT std_logic; vga_vsync_o : OUT std_logic; vga_red_o : OUT unsigned ((4 -1) downto 0); vga_green_o : OUT unsigned ((4 -1) downto 0); vga_blue_o : OUT unsigned ((4 -1) downto 0) ); end component;
component SPI is generic (generic_int: integer); port ( clk_120 : IN std_logic; clk_mcu : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; trig_i : IN std_logic; spi_dma_i : IN d2p_8_t; spi_dma_o : OUT p2d_8_t; spi_csn_io : INOUT unsigned ((1 -1) downto 0); spi_clk_io : INOUT unsigned ((1 -1) downto 0); spi_tx_o : OUT std_logic; spi_rx_i : IN std_logic ); end component;
signal clk : std_logic;
signal clk_100 : std_logic;
signal reset_n : std_logic;
signal cmd : blk2mem_t;
signal gpios : unsigned ((32 -1) downto 0);
signal uart_data : unsigned ((32 -1) downto 0);
signal uart_cnt : unsigned ((32 -1) downto 0);

signal blue : std_logic;
signal red : std_logic;
signal green : std_logic;
signal pclk : std_logic;



signal uart2core : mem2blk_t;
signal spi2core : mem2blk_t;
signal core2datamem : blk2mem_t;

signal sdram_addr : unsigned ((12 -1) downto 0);
signal sdram_ba : unsigned ((2 -1) downto 0);
signal sdram_cke : std_logic;
signal sdram_cs : std_logic;
signal sdram_we : std_logic;
signal sdram_ras : std_logic;
signal sdram_cas : std_logic;
signal sdram_dQm : unsigned ((2 -1) downto 0);
signal sdram_dQ : unsigned ((16 -1) downto 0);


signal uart_irq : std_logic;
signal uart_rx : std_logic;
signal uart_tx : std_logic;
signal uart_dbg : std_logic;

signal spi_tx : std_logic;
signal boot_mode : std_logic;
signal spi_rx : std_logic;
signal spi_csn : unsigned ((1 -1) downto 0);
signal spi_clk : unsigned ((1 -1) downto 0);
signal spi2pluto : unsigned ((16 -1) downto 0);
signal spi2pluto_en : std_logic;
signal pluto2spi : unsigned ((16 -1) downto 0);
signal pluto2spi_en : std_logic;

signal init_done : std_logic;


signal cnt_send_byte : unsigned ((5 -1) downto 0);
signal cnt_rnd : unsigned ((16 -1) downto 0);
signal cnt_byte : unsigned ((2 -1) downto 0);
signal code_loaded : std_logic;
signal code_loadedp : std_logic;
signal UART_byte : unsigned ((8 -1) downto 0);
signal uart2dma : p2d_8_t;
signal dma2uart : d2p_8_t;
signal spi2dma : p2d_8_t;
signal dma2spi : d2p_8_t;
signal spi_trig : std_logic;
begin




dut : top port map( clk_top => clk, reset_n => reset_n, boot_mode_i => boot_mode, spi_clk_io => spi_clk, spi_csn_io => spi_csn, spi_tx_o => spi_tx, spi_rx_i => spi_rx, uart_tx_o => uart_tx, uart_rx_i => uart_rx, blue_o => blue, red_o => red, green_o => green, pclk_o => pclk);
signal halt_pipe : unsigned ((32 -1) downto 0);


u0_sUART : sUART generic map(generic_int => 16#UART_TB_REGS#) port map( clk_peri => dut.clk_mcu, reset_n => reset_n, core2mem_i => core2datamem, mem2core_o => uart2core, uart_dma_o => uart2dma, uart_dma_i => dma2uart, uart_tx_o => uart_rx, irq_o => uart_irq, uart_rx_i => uart_tx) ;
u0_spi : SPI generic map(generic_int => 16#SPI_TB_REGS#) port map( clk_120 => dut.clk_120, clk_mcu => dut.clk_mcu, reset_n => reset_n, core2mem_i => core2datamem, mem2core_o => spi2core, spi_dma_i => dma2spi, spi_dma_o => spi2dma, trig_i => spi_trig, spi_clk_io => spi_clk, spi_csn_io => spi_csn, spi_tx_o => spi_rx, spi_rx_i => spi_tx) ;
FOREVER_PROCESS(0)

FOREVER_BEGIN

 clk <= "0";

 FOREVER_WAIT(2083)

 clk <= "1";


 FOREVER_WAIT_AND_LOOP(2084)


FOREVER_END


FOREVER_PROCESS(2)

FOREVER_BEGIN
 clk_100 <= "0";
 FOREVER_WAIT(5000)
 clk_100 <= "1";
 FOREVER_WAIT_AND_LOOP(5000)
FOREVER_END


FOREVER_PROCESS(1)

FOREVER_BEGIN
 reset_n <= "0";







 FOREVER_WAIT(55000)
 reset_n <= "1";

 uint32_t addr = 0;
 uint32_t data;
 while (not code_file.eof())
 {
  code_file.read ((char*)&data, sizeof(data));
  dut.u0_mem.set(addr, data);





  giprintf("#MLoading data mem % %", to_hex(addr), to_hex(data));
  addr++;
 }
 for (int i = 0; i < 6144; i++)
  dut.u0_mem.get(i);
 FOREVER_WAIT(55000)
 boot_mode <= '0';

FOREVER_END


begin

 dut.tb2core.data = uart2core.data or spi2core.data;
 dut.tb2core.data_en = (uart2core.data_en or spi2core.data_en);
 core2datamem.addr <= dut.core2datamem.addr;
 core2datamem.data <= dut.core2datamem.data;
 core2datamem.cs_n <= dut.core2datamem.cs_n;
 core2datamem.wr_n <= dut.core2datamem.wr_n;
end rtl;
