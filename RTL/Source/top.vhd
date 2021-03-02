-- vhdl file of block risc-V_core generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
library work; use work.structures.all;

entity top is port ( clk_top : IN std_logic; reset_n : IN std_logic; boot_mode_i : IN std_logic; spi_clk_io : INOUT unsigned ((1 -1) downto 0); spi_csn_io : INOUT unsigned ((1 -1) downto 0); spi_tx_o : OUT std_logic; spi_rx_i : IN std_logic; uart_tx_o : OUT std_logic; uart_rx_i : IN std_logic; pclk_o : OUT std_logic; red_o : OUT std_logic; green_o : OUT std_logic; blue_o : OUT std_logic; vga_hsync_o : OUT std_logic; vga_vsync_o : OUT std_logic; vga_red_o : OUT unsigned ((4 -1) downto 0); vga_green_o : OUT unsigned ((4 -1) downto 0); vga_blue_o : OUT unsigned ((4 -1) downto 0) ); end top; architecture rtl of top is component dummy_zkw_pouet is port(clk : in std_logic);end component;
component hdmi is generic (generic_int: integer); port ( clk_hdmi : IN std_logic; clk_core : IN std_logic; clk_pix : IN std_logic; boot_mode_i : IN std_logic; trap_i : IN std_logic; dbg_i : IN unsigned ((33 -1) downto 0); reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; pclk_o : OUT std_logic; red_o : OUT std_logic; green_o : OUT std_logic; blue_o : OUT std_logic; vga_hsync_o : OUT std_logic; vga_vsync_o : OUT std_logic; vga_red_o : OUT unsigned ((4 -1) downto 0); vga_green_o : OUT unsigned ((4 -1) downto 0); vga_blue_o : OUT unsigned ((4 -1) downto 0) ); end component;
component dma is generic (generic_int: integer); port ( clk_dma : IN std_logic; reset_n : IN std_logic; boot_mode_i : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; mem2dma_i : IN mem2blk_t; uart_dma_i : IN p2d_8_t; uart_dma_o : OUT d2p_8_t; spi_dma_i : IN p2d_8_t; spi_dma_o : OUT d2p_8_t; irq_dma_o : OUT irq_dma_t; core_grant_i : IN std_logic; core_request_o : OUT blk2mem_t ); end component;
component sUART is generic (generic_int: integer); port ( clk_peri : IN std_logic; reset_n : IN std_logic; boot_mode_i : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; uart_dma_i : IN d2p_8_t; uart_dma_o : OUT p2d_8_t; uart_rts_o : OUT std_logic; uart_cts_i : IN std_logic; uart_tx_o : OUT std_logic; uart_rx_i : IN std_logic ); end component;
component risc_V_core is port ( clk_core : IN std_logic; reset_n : IN std_logic; boot_mode_i : IN std_logic; trap_o : OUT std_logic; dbg_o : OUT unsigned ((33 -1) downto 0); load_port_i : IN blk2mem_t; core2instmem_o : OUT blk2mem_t; instmem2core_i : IN mem2blk_t; core2datamem_o : OUT blk2mem_t; dma_request_i : IN blk2mem_t; dma_grant_o : OUT std_logic; csr2core_i : IN csr2core_t; datamem2core_i : IN mem2blk_t ); end component;
component mem is generic (generic_int: integer); port ( clk_mem : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t ); end component;
component peripherals is generic (generic_int: integer); port ( clk_peri : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; dbg_o : OUT unsigned ((8 -1) downto 0); gpios_o : OUT unsigned ((32 -1) downto 0) ); end component;
component clk_gen is port ( areset : IN std_logic; inclk0 : IN std_logic; c0 : OUT std_logic; c1 : OUT std_logic; c2 : OUT std_logic; c3 : OUT std_logic; locked : OUT std_logic ); end component;
component sdram_ctrl is generic (generic_int: integer); port ( clk_sdram : IN std_logic; reset_n : IN std_logic; req0_i : IN sdram_req_t; req1_i : IN sdram_req_t; data_i : IN unsigned ((16 -1) downto 0); addr_o : OUT unsigned ((12 -1) downto 0); ba_o : OUT unsigned ((2 -1) downto 0); data_o : OUT unsigned ((16 -1) downto 0); cke_o : OUT std_logic; cs_o : OUT std_logic; we_o : OUT std_logic; cas_o : OUT std_logic; ras_o : OUT std_logic; en0_o : OUT std_logic; en1_o : OUT std_logic; dQm_o : OUT unsigned ((2 -1) downto 0); dQ_io : INOUT unsigned ((16 -1) downto 0) ); end component;
component SPI is generic (generic_int: integer); port ( clk_120 : IN std_logic; clk_mcu : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; trig_i : IN std_logic; spi_dma_i : IN d2p_8_t; spi_dma_o : OUT p2d_8_t; spi_csn_io : INOUT unsigned ((1 -1) downto 0); spi_clk_io : INOUT unsigned ((1 -1) downto 0); spi_tx_o : OUT std_logic; spi_rx_i : IN std_logic ); end component;
component SPI_master is generic (generic_int: integer); port ( clk_120 : IN std_logic; clk_mcu : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; trig_i : IN std_logic; spi_dma_i : IN d2p_8_t; spi_dma_o : OUT p2d_8_t; spi_csn_o : OUT std_logic; spi_clk_o : OUT std_logic; spi_tx_o : OUT std_logic; spi_rx_i : IN std_logic ); end component;
component SPI_slave is generic (generic_int: integer); port ( clk_120 : IN std_logic; clk_mcu : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t; spi_dma_i : IN d2p_8_t; spi_dma_o : OUT p2d_8_t; spi_csn_i : IN std_logic; spi_clk_i : IN std_logic; spi_tx_o : OUT std_logic; spi_rx_i : IN std_logic ); end component;
component csr_irq is generic (generic_int: integer); port ( clk_csr_irq : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; irq_i : IN irq_t; mem2core_o : OUT mem2blk_t; csr2core_o : OUT csr2core_t ); end component;
signal boot_mode : std_logic;

signal load_port : blk2mem_t;
signal core2instmem : blk2mem_t;
signal instmem2core : mem2blk_t;
signal core2datamem : blk2mem_t;
signal datamem2core : mem2blk_t;
signal all2core : mem2blk_t;
signal uart2core : mem2blk_t;
signal dma2core : mem2blk_t;
signal hdmi2core : mem2blk_t;
signal spi2core : mem2blk_t;
signal spi_slv2core : mem2blk_t;
signal csr_irq2core : mem2blk_t;

signal dbg : unsigned ((8 -1) downto 0);
signal gpios : unsigned ((32 -1) downto 0);
signal gate_cell : std_logic;
signal clk_mcu : std_logic;
signal clk_24 : std_logic;
signal clk_120 : std_logic;
signal clk_240 : std_logic;

signal tb2core : mem2blk_t;

signal bmkrD : unsigned ((15 -1) downto 0);
signal bmkrA : unsigned ((6 -1) downto 0);
signal uart_rx : std_logic;
signal uart_tx : std_logic;
signal uart_rts : std_logic;
signal uart_cts : std_logic;

signal uart2dma : p2d_8_t;
signal dma2uart : d2p_8_t;
signal spi2dma : p2d_8_t;
signal dma2spi : d2p_8_t;
signal dma_request : blk2mem_t;
signal dma2irq : irq_dma_t;
signal dma_grant : std_logic;
signal trap : std_logic;

signal sdram_req0 : sdram_req_t;
signal sdram_req1 : sdram_req_t;

signal sdram_en0 : std_logic;
signal sdram_en1 : std_logic;

signal hdmi2sdram_data : unsigned ((16 -1) downto 0);
signal sdram2hdmi_data : unsigned ((16 -1) downto 0);
signal dbg33 : unsigned ((33 -1) downto 0);

signal spi_clk : std_logic;
signal spi_csn : std_logic;
signal spi_mosi : std_logic;
signal spi_miso : std_logic;
signal spi_miso2 : std_logic;
signal spi_trig : std_logic;

signal csr2core : csr2core_t;
signal irq_vec : irq_t;




begin

u0_clk_gen : clk_gen port map( areset => reset_n, inclk0 => clk_top, c0 => clk_mcu, c1 => clk_24, c2 => clk_120, c3 => clk_240);
u0_csr_irq : csr_irq generic map(generic_int => 16#BFFFE000#) port map( clk_csr_irq => clk_mcu, reset_n => reset_n, core2mem_i => core2datamem, mem2core_o => csr_irq2core, irq_i => irq_vec, csr2core_o => csr2core) ;
u0_hdmi : hdmi generic map(generic_int => 16#2FFFFFFE#) port map( clk_hdmi => clk_120, clk_core => clk_mcu, clk_pix => clk_24, reset_n => reset_n, boot_mode_i => boot_mode_i, trap_i => trap, dbg_i => dbg33, core2mem_i => core2datamem, mem2core_o => hdmi2core, blue_o => blue_o, green_o => green_o, red_o => red_o, pclk_o => pclk_o, vga_hsync_o => vga_hsync_o, vga_vsync_o => vga_vsync_o, vga_blue_o => vga_blue_o, vga_green_o => vga_green_o, vga_red_o => vga_red_o) ;
u0_dma : dma generic map(generic_int => 16#4FFFFFD0#) port map( clk_dma => clk_mcu, reset_n => reset_n, boot_mode_i => boot_mode_i, core2mem_i => core2datamem, mem2core_o => dma2core, mem2dma_i => datamem2core, uart_dma_i => uart2dma, uart_dma_o => dma2uart, spi_dma_i => spi2dma, spi_dma_o => dma2spi, irq_dma_o => dma2irq, core_grant_i => dma_grant, core_request_o => dma_request) ;
u0_sUART : sUART generic map(generic_int => 16#1FFFFFF0#) port map( clk_peri => clk_mcu, reset_n => reset_n, boot_mode_i => boot_mode_i, core2mem_i => core2datamem, mem2core_o => uart2core, uart_dma_o => uart2dma, uart_dma_i => dma2uart, uart_rts_o => uart_rts, uart_cts_i => uart_cts, uart_tx_o => uart_tx_o, uart_rx_i => uart_rx_i) ;
u0_risc_V_core : risc_V_core port map( clk_core => clk_mcu, reset_n => reset_n, boot_mode_i => boot_mode_i, trap_o => trap, dbg_o => dbg33, load_port_i => load_port, core2instmem_o => core2instmem, instmem2core_i => instmem2core, core2datamem_o => core2datamem, dma_request_i => dma_request, dma_grant_o => dma_grant, csr2core_i => csr2core, datamem2core_i => all2core);
u0_mem : mem generic map(generic_int => 6144) port map( clk_mem => clk_mcu, reset_n => reset_n, core2mem_i => core2instmem, mem2core_o => instmem2core) ;
u1_mem : mem generic map(generic_int => 6144) port map( clk_mem => clk_mcu, reset_n => reset_n, core2mem_i => core2datamem, mem2core_o => datamem2core) ;
u0_peripherals : peripherals generic map(generic_int => 16#2FFFFFFE#) port map( clk_peri => clk_mcu, reset_n => reset_n, core2mem_i => core2datamem, dbg_o => dbg, gpios_o => gpios) ;
u0_spi : SPI generic map(generic_int => 16#4FFFFA00#) port map( clk_120 => clk_120, clk_mcu => clk_mcu, reset_n => reset_n, core2mem_i => core2datamem, mem2core_o => spi2core, spi_dma_i => dma2spi, spi_dma_o => spi2dma, trig_i => spi_trig, spi_clk_io => spi_clk_io, spi_csn_io => spi_csn_io, spi_tx_o => spi_tx_o, spi_rx_i => spi_rx_i) ;
 tb2core.data <= TO_UNSIGNED(0,32);
 tb2core.data_en <= '0';
 all2core.data <= ( datamem2core.data or uart2core.data or dma2core.data or tb2core.data or hdmi2core.data or csr_irq2core.data or spi2core.data);
 all2core.data_en <= ( datamem2core.data_en or uart2core.data_en or dma2core.data_en or tb2core.data_en or hdmi2core.data_en or csr_irq2core.data_en or spi2core.data_en);






 uart_cts <= '0';



 irq_vec(0) <= dma2irq(0);
 irq_vec(1) <= dma2irq(1);



 load_port.wr_n <= '1' ;
end rtl;
