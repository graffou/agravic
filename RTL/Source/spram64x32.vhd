-- vhdl file of block spram64x32 generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
entity spram64x32 is port( clk : IN std_logic; reset_n : IN std_logic; addr_i : IN unsigned ((14 -1) downto 0); data_i : IN unsigned ((32 -1) downto 0); data_o : OUT unsigned ((32 -1) downto 0); cs_n_i : IN std_logic; wr_n_i : IN std_logic ); end spram64x32; architecture rtl of spram64x32 is component dummy_zkw_pouet is port(clk : in std_logic);end component;
type mem_t is array(0 to (16384 -1)) of unsigned ((32 -1) downto 0);
signal mem : mem_t;
begin
process0 : process(clk,reset_n)
begin
 IF ( reset_n = '0' ) then
  data_o <= TO_UNSIGNED(0,data_o'length);
 elsif ( clk'event and (clk = '1' ) ) then
  IF (cs_n_i = '1' ) then
   IF (wr_n_i = '1' ) then
    data_o <= mem(TO_INTEGER(addr_i));
   else
    mem(TO_INTEGER(addr_i)) <= data_i;
   end if;
  end if;
 end if;
end process;
end rtl;
