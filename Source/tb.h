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

COMPONENT(top,
DECL_PORTS(
		PORT(clk_top, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(load_port_i, blk2mem_t, IN),
		// GPIOs
		PORT(gpios_o, UINT(32), OUT)
		)
);


SIG(clk, CLK_TYPE);
SIG(reset_n, RST_TYPE);// reset_n;
SIG(cmd, blk2mem_t);
SIG(gpios, UINT(32));

std::ifstream code_file;

BEGIN

BLK_INST(dut, top,
		MAPPING(
				PM(clk_top, clk),
				PM(reset_n, reset_n),
				PM(load_port_i, cmd),
				PM(gpios_o, gpios)
				)
			);

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

void init_clk_rst()
{
	clk <= BIT(0);
	reset_n <= BIT(0);
}

template<class T>
void init_file( T& name_i)
{
	std::string name = name_i; // input is of input_parm<> type
	code_file.open(name.c_str(), std::ifstream::binary);
	gprintf("#VOpening code file %Y open %Y", name.c_str(), code_file.is_open());

}

constexpr void run()
{
	reset_n <= BIN(0);
	clk <= BIN(1);
	clk <= BIN(0);
	clk <= BIN(1);
	reset_n <= BIN(1);
	cmd.cs_n <= BIT(0);
	cmd.wr_n <= BIT(0);
	cmd.addr <= BIT(0);
	cmd.be <= BIN(1111);
	clk <= not clk;
	clk <= not clk;
	uint32_t addr = 0;
	while (not code_file.eof() and not(cmd.addr == BIN(1011111111100) ))
	{
		__control_signals__.clk = -1;
		__control_signals__.reset_n = -1;

		uint32_t data;
		code_file.read ((char*)&data, sizeof(data)); //>> data;
		cmd.data <= slv<32>(data);
		//printf("%x\n", data)
		if (cmd.addr < BIN(0000000011100) )
		gprintf("#Maddr : %R write %R %Y", to_hex(TO_INTEGER(cmd.addr)), to_hex(TO_INTEGER(cmd.data)), code_file.tellg());
		cmd.addr <= TO_UINT(addr, LEN(cmd.addr));//cmd.addr + TO_UINT(4, LEN(cmd.addr));
		addr = addr + 4;
		clk <= not clk;
		clk <= not clk;
	}
	cmd.cs_n <= BIT(1);
	cmd.wr_n <= BIT(1);

	clk <= not clk;
	clk <= not clk;

	for (int i = 0; i < 10000 ; i++)
	{
		std::cerr << '.';
		clk <= not clk;
	}

}

#endif // VHDL


BLK_END;
