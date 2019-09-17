-- vhdl file of block mem generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
library work; use work.structures.all;
entity mem is port( clk_mem : IN std_logic; reset_n : IN std_logic; core2mem_i : IN blk2mem_t; mem2core_o : OUT mem2blk_t ); end mem; architecture rtl of mem is component dummy_zkw_pouet is port(clk : in std_logic);end component;
type mem_t is array(0 to (6144 -1)) of unsigned ((8 -1) downto 0);
signal mem0 : mem_t;
signal mem1 : mem_t;
signal mem2 : mem_t;
signal mem3 : mem_t;
signal mem2blk_t0 : mem2blk_t;
begin
process0 : process(clk_mem,reset_n)
variable baddr : unsigned ((13 -1) downto 0);
variable rdata : unsigned ((32 -1) downto 0);
begin
 IF ( clk_mem'event and (clk_mem = '1' ) ) then
  baddr := core2mem_i.addr;
  rdata := mem3(TO_INTEGER(baddr)) &
     mem2(TO_INTEGER(baddr)) &
     mem1(TO_INTEGER(baddr)) &
     mem0(TO_INTEGER(baddr));
  mem2core_o.data <= rdata;
  IF (core2mem_i.cs_n = '0') then
   mem2core_o.data_en <= '1' ;
   IF (core2mem_i.wr_n = '0') then
    -- gprintf("#MMem write % @ % ", to_hex(core2mem_i.data), to_hex(core2mem_i.addr), core2mem_i.be);
    IF ( core2mem_i.be(3) = '1' ) then
     mem3(TO_INTEGER(baddr)) <= (core2mem_i.data(31 downto 24));
    end if;
    IF ( core2mem_i.be(2) = '1' ) then
     mem2(TO_INTEGER(baddr)) <= (core2mem_i.data(23 downto 16));
    end if;
    IF ( core2mem_i.be(1) = '1' ) then
     mem1(TO_INTEGER(baddr)) <= (core2mem_i.data(15 downto 8));
    end if;
    IF ( core2mem_i.be(0) = '1' ) then
     mem0(TO_INTEGER(baddr)) <= (core2mem_i.data(7 downto 0));
    end if;
    mem2core_o.data_en <= '0';
   end if;
  else
   mem2core_o.data_en <= '0';
  end if;
 end if;
end process;
end rtl;
