-- vhdl file of block structures generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
package structures is
type blk2mem_t is record addr : unsigned ((15 - 2 -1) downto 0); data : unsigned ((32 -1) downto 0); be : unsigned ((4 -1) downto 0); cs_n : std_logic; wr_n : std_logic; end record;
type mem2blk_t is record data : unsigned ((32 -1) downto 0); data_en : std_logic; end record;
type blk2mem8_t is record addr : unsigned ((13 -1) downto 0); data : unsigned ((8 -1) downto 0); cs_n : std_logic; wr_n : std_logic; end record;
type mem2blk8_t is record data : unsigned ((8 -1) downto 0); data_en : std_logic; end record;
type p2d_16_t is record data : unsigned ((16 -1) downto 0); rdy : std_logic; data_en : std_logic; end record;
type d2p_16_t is record data : unsigned ((16 -1) downto 0); grant : std_logic; data_en : std_logic; end record;
type p2d_8_t is record data : unsigned ((8 -1) downto 0); rdy : std_logic; data_en : std_logic; end record;
type d2p_8_t is record data : unsigned ((8 -1) downto 0); grant : std_logic; init : std_logic; data_en : std_logic; end record;
type p2d_t is record data : unsigned ((32 -1) downto 0); rdy : std_logic; data_en : std_logic; end record;
type d2p_t is record data : unsigned ((32 -1) downto 0); grant : std_logic; data_en : std_logic; end record;
type dma_channel_t is record addr : unsigned ((15 -1) downto 0); tsfr_sz : unsigned ((24 -1) downto 0); wait_time : unsigned ((10 -1) downto 0); priority : unsigned ((4 -1) downto 0); tmp_data : unsigned ((32 -1) downto 0); tmp_we : unsigned ((4 -1) downto 0); time : unsigned ((24 -1) downto 0); timeout : unsigned ((24 -1) downto 0); auto_inc : unsigned ((4 -1) downto 0); periph_mask : unsigned ((2 -1) downto 0); data_mask : unsigned ((2 -1) downto 0); ntransferred : unsigned ((2 -1) downto 0); source : unsigned ((3 -1) downto 0); sink : unsigned ((3 -1) downto 0); state : unsigned ((4 -1) downto 0); end record;
type sdram_req_t is record addr : unsigned ((22 -1) downto 0); rnw : std_logic; sz : unsigned ((8 -1) downto 0); en : std_logic; end record;
end structures;
