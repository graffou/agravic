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

COMPONENT(sUART,
DECL_PORTS(
		PORT(clk_peri, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN)
		)
);

COMPONENT(top,
DECL_PORTS(
		PORT(clk_top, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(load_port_i, blk2mem_t, IN),
		// GPIOs
		PORT(uart_tx_o, BIT_TYPE, OUT),
		PORT(uart_rx_i, BIT_TYPE, IN),
		PORT(gpios_o, UINT(32), OUT)
		)
);


SIG(clk, CLK_TYPE);
SIG(reset_n, RST_TYPE);// reset_n;
SIG(cmd, blk2mem_t);
SIG(gpios, UINT(32));
SIG(sUART_tx, BIT_TYPE);
SIG(sUART_rx, BIT_TYPE);

SIG(core2datamem, blk2mem_t);
SIG(uart2core, mem2blk_t);


#ifndef VHDL
int ncycles = 10000;
bool success = 1;
std::ifstream code_file;
std::ifstream check_file;
std::ifstream sig_start_file;
#endif

BEGIN


BLK_INST(u0_sUART, sUART,
MAPPING(
		PM(clk_peri, clk),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, uart2core),
		PM(uart_tx_o, sUART_rx),
		PM(uart_rx_i, sUART_tx)
		)
);

BLK_INST(dut, top,
		MAPPING(
				PM(clk_top, clk),
				PM(reset_n, reset_n),
				PM(load_port_i, cmd),
				PM(uart_tx_o, sUART_tx),
				PM(uart_rx_i, sUART_rx)	,
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
	gstring name = name_i; // input is of input_parm<> type
	code_file.open(name.c_str(), std::ifstream::binary);
	gprintf("#VOpening code file %Y open %Y", name.c_str(), code_file.is_open());
	name.replace(".bin", ".sig_start");
	sig_start_file.open(name.c_str());
	name.replace("elf.sig_start", "reference_output");
	check_file.open(name.c_str());
	gprintf("#VOpening check file %Y open %Y", name.c_str(), check_file.is_open());
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
	cmd.addr <= TO_UINT(0, LEN(cmd.addr));
	cmd.be <= BIN(1111);
	clk <= not clk;
	clk <= not clk;
	uint32_t addr = 0;
	uint32_t end_addr = 0;
	while (not code_file.eof() and not(cmd.addr == BIN(1011111111111) ))
	{
		gprintf("% % \n", cmd.addr, BIN(101111111111100));
		__control_signals__.clk = -1;
		__control_signals__.reset_n = -1;

		uint32_t data;
		code_file.read ((char*)&data, sizeof(data)); //>> data;
		cmd.data <= slv<32>(data);
		//printf("%x\n", data)
		if (cmd.addr < BIN(0000000011100) ) {
			gprintf("#Maddr : %R write %R %Y", to_hex(TO_INTEGER(cmd.addr)), to_hex(TO_INTEGER(cmd.data)), code_file.tellg());
		}
		cmd.addr <= TO_UINT(addr/4, LEN(cmd.addr));//cmd.addr + TO_UINT(4, LEN(cmd.addr));
#ifdef NONREG // for risc-v compliance test, copy init data to data memory
		if (addr >= 8192)
		{
			dut.u1_mem.set(addr/4, data);
			gprintf("#MLoading data mem % %", to_hex(addr/4), to_hex(data));
			gprintf("#Cmem content % %", to_hex(addr/4), to_hex(dut.u1_mem.get(addr/4)));
		}

#endif
		addr = addr + 4;
		std::cerr << '>';
		clk <= not clk;
		clk <= not clk;
		std::cerr << '/';
	}
	cmd.cs_n <= BIT(1);
	cmd.wr_n <= BIT(1);
	clk <= not clk;
	clk <= not clk;

	gprintf("#VStarting simulation, % ", ncycles);
	for (int i = 0; i < ncycles*2 ; i++)
	{
		//std::cerr << '.';
		clk <= not clk;
		//gpios <= BIN(10101010010101011111111100000000);


		// Configure TB IPs from FW -------------------
		// Address range 0x1e00-1eFF

		if ((dut.core2datamem.addr and 0x1f00) == 0x1e00 )
		{
			core2datamem <= dut.core2datamem;
			core2datamem.addr <= core2datamem.addr + 0x100;
			gprintf("#RYoooooooooooooooooooooooooooo % ", to_hex(TO_INTEGER(dut.core2datamem.addr)), to_hex(TO_INTEGER(dut.core2datamem.data)));
			//exit(0);
		}
		else
			core2datamem.cs_n = 1;
	}
#ifdef NONREG
	for (int i= 2048; i < 2100;i++)
		gprintf("#Cmem content % %", to_hex(i), to_hex(dut.u1_mem.get(i)));
	for (int i= 0; i < 200;i++)
		gprintf("#Mmem content % %", to_hex(i), to_hex(dut.u0_mem.get(i)));
	sig_start_file >> std::hex >> addr;
	std::string yo;
	sig_start_file >> std::hex >> yo; // reads end of line:
	sig_start_file >> std::hex >> end_addr;
	gprintf("#VTesting signature from %Y to %Y", to_hex(addr), to_hex(end_addr));
	addr = addr >> 2;
	end_addr = end_addr >> 2;
	gprintf("#VTesting signature from %Y to %Y", to_hex(addr), to_hex(end_addr));
	while (not check_file.eof())
	{
		uint32_t check_val;
		check_file >> std::hex >> check_val;
		if (not check_file.eof() and (addr <= end_addr) )
		{
			if (check_val == dut.u1_mem.get(addr))
				gprintf("#GChecking % % %", addr, to_hex(check_val), to_hex(dut.u1_mem.get(addr)));
			else
			{
				success = 0;
				gprintf("#RChecking % % %", addr, to_hex(check_val), to_hex(dut.u1_mem.get(addr)));
			}
		}

		addr ++;
	}
#endif

}

#endif // VHDL


BLK_END;
