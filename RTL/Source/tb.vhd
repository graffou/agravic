-- vhdl file of block tb generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
library work; use work.structures.all;
entity tb_t is ); end tb_t; architecture rtl of tb_t is component dummy_zkw_pouet is port(clk : in std_logic);end component;
constant c_clock_period : time := 5 ns;
constant c_reset_length : time := 20 ns;
component top is port( clk_top : IN std_logic; reset_n : IN std_logic; load_port_i : IN blk2mem_t; gpios_o : OUT unsigned ((32 -1) downto 0) ); end component;
signal clk : std_logic;
signal reset_n : std_logic;
signal cmd : blk2mem_t;
signal gpios : unsigned ((32 -1) downto 0);
std::ifstream code_file;
begin
dut : top port map( clk_top => clk, reset_n => reset_n, load_port_i => cmd, gpios_o => gpios);
gen_clk: process
begin
   wait for (c_clock_period / 2);
   clk <= not clk;
end process gen_clk;
gen_reset_n: process
begin
   wait for c_reset_length;
   reset_n <= '1';
   wait;
end process gen_reset_n;
end rtl;