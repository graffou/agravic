#include "slv.h"
//#include "spram6144x8.h"
// have separate implementations for vhdl and c++, so that we don't get thousands of flops in the C++ design
// C++ memory are just arrays
// Write delay is obtained by delaying the write operation

START_OF_FILE(mem)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(slv_utils)



ENTITY(mem,
DECL_PORTS(
		PORT(clk_mem, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT)
		),
		INTEGER generic_int
);



COMPONENT(spram,
DECL_PORTS(
		PORT(clk_mem, CLK_TYPE, IN),
		PORT(reset_n, CLK_TYPE, IN),
		PORT(addr_i, UINT(data_addr_span-2), IN),
		PORT(data_i, UINT(32), IN),
		PORT(be_i, UINT(4), IN),
		PORT(wrn_i, BIT_TYPE, IN),
		PORT(data_o, UINT(32), OUT)

		),
		INTEGER generic_int
);


SIG(addr, UINT(LEN(PORT_BASE(core2mem_i).addr)));
CONST(max_addr, UINT(LEN(PORT_BASE(core2mem_i).addr))) = TO_UINT(generic_int, LEN(addr));
SIG(data_rd, UINT(32));
SIG(data_wr, UINT(32));
SIG(be, UINT(4));
SIG(wen, BIT_TYPE);
SIG(rd_en, UINT(1));
SIG(addr_dbg, UINT(LEN(PORT_BASE(core2mem_i).addr)));


BEGIN

	BLK_INST(ram0, spram,
			MAPPING(
			PM(clk_mem, clk_mem),
			PM(reset_n, reset_n),
			PM(addr_i, addr),
			PM(data_i, data_wr),
			PM(data_o, data_rd),
			PM(be_i, be),
			PM(wrn_i, wen)
			)
			, generic_int
		);


#ifndef VHDL
	uint32_t get(uint32_t baddr)
	{
		slv<32> val = ram0.mem0((baddr))(3) &
				ram0.mem0((baddr))(2) &
				ram0.mem0((baddr))(1) &
				ram0.mem0((baddr))(0);
		//gprintf("#GRd addr % val ", baddr, val);
		return TO_INTEGER(val);
	}
	void set(uint32_t addr, uint32_t val)
	{
		//gprintf("#RSet addr % val %", addr, val);
		ram0.mem0(addr)(3) = TO_UINT((val >> 24),8);
		ram0.mem0(addr)(2) = TO_UINT((val >> 16),8);
		ram0.mem0(addr)(1) = TO_UINT((val >> 8),8);
		ram0.mem0(addr)(0) = TO_UINT((val),8);

	}

#endif

	PROCESS(0, clk_mem, reset_n) // ----------------------------------------------------------

	BEGIN
		IF ( reset_n == BIT(0) ) THEN
			rd_en <= BIN(0);
		ELSEIF ( EVENT(clk_mem) and (clk_mem == BIT(1)) ) THEN
			IF ( (PORT_BASE(core2mem_i).cs_n == BIT(0)) and ((PORT_BASE(core2mem_i).addr) < max_addr) ) THEN
				addr_dbg <= (PORT_BASE(core2mem_i).addr) ; // valid access
				IF ((PORT_BASE(core2mem_i).wr_n == BIT(1))) THEN
					rd_en <= BIN(1);
					//gprintf("#MRead % addr %", data_rd, PORT_BASE(core2mem_i).addr);
				ELSE
					rd_en <= BIN(0);
				ENDIF
			ELSE
				addr_dbg <= SXT(TO_UINT(7,3), LEN(addr_dbg)) ;	// no access: return 1111...
				rd_en <= BIN(0);
			ENDIF
		ENDIF
	END_PROCESS

	COMB_PROCESS(1, clk_mem)
		addr <= ( ( PORT_BASE(core2mem_i).addr ) and SXT(BOOL2BIN( PORT_BASE(core2mem_i).addr < max_addr ), LEN(addr)) );
		data_wr <= PORT_BASE(core2mem_i).data;
		wen <= ( not PORT_BASE(core2mem_i).cs_n and not PORT_BASE(core2mem_i).wr_n and  BOOL2BIT(PORT_BASE(core2mem_i).addr < max_addr ) );
		be <= PORT_BASE(core2mem_i).be;
		PORT_BASE(mem2core_o).data <= ( data_rd and SXT(rd_en, 32));
		PORT_BASE(mem2core_o).data_en <= B( rd_en, 0 );

	END_COMB_PROCESS

BLK_END;

