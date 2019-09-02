#include "slv.h"
START_OF_FILE(tb)
INCLUDES
USE_PACKAGE(structures)
TESTBENCH(tb_t);
// For now, just implement vhdl and C++ codes separately
// for clock generation

#ifdef VHDL
constant c_clock_period : time := 5 ns;
constant c_reset_length : time := 20 ns;
#else



#endif


SIG(clk, CLK_TYPE);
SIG(reset_n, RST_TYPE);// reset_n;

BEGIN

#ifdef VHDL
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
#else


void run()
{
	reset_n <= BIN(0);
	clk <= BIN(1);
	clk <= BIN(0);
	clk <= BIN(1);
	reset_n <= BIN(1);
	for (int i = 0; i < 100000 ; i++)
	{
		//gprintf("#Rclk a % b %", a,b);
		clk <= not clk;
	}

}

#endif // VHDL


BLK_END;
