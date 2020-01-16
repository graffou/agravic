#include "slv.h"
//#include "spram6144x8.h"
// have separate implementations for vhdl and c++, so that we don't get thousands of flops in the C++ design
// C++ memory are just arrays
// Write delay is obtained by delaying the write operation
#ifdef VHDL
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
		)
);

#if 0

TYPE(mem_t, ARRAY_TYPE(UINT(8),6144));

SIG(mem0, mem_t);// internal;
SIG(mem1, mem_t);// internal;
SIG(mem2, mem_t);// internal;
SIG(mem3, mem_t);// internal;
//SIG(blk2mem_t0, blk2mem_t);
SIG(mem2blk_t0, mem2blk_t);

BEGIN



#ifndef VHDL
uint32_t get(uint32_t baddr)
{
	slv<32> val = mem3((baddr)) &
			mem2((baddr)) &
			mem1((baddr)) &
			mem0((baddr));
	return TO_INTEGER(val);
}
void set(uint32_t addr, uint32_t val)
{
	mem3(addr) = (val >> 24) & 255;
	mem2(addr) = (val >> 16) & 255;
	mem1(addr) = (val >> 8) & 255;
	mem0(addr) = (val) & 255;

}
#endif

PROCESS(0, clk_mem, reset_n)
VAR(baddr, UINT(13));
VAR(rdata, UINT(32));
BEGIN
//	IF ( reset_n == BIT(0) ) THEN
//		gprintf("#V addr ");//% ", &PORT_BASE(mem2core_o));//, &PORT_BASE(mem2core_o).data);
//		PORT_BASE(mem2core_o).data <= TO_UINT(0, LEN(mem2blk_t0.data));
//		PORT_BASE(mem2core_o).data_en <= BIT(0);
	IF ( EVENT(clk_mem) and (clk_mem == BIT(1)) ) THEN
		baddr = PORT_BASE(core2mem_i).addr;
		rdata = 	mem3(TO_INTEGER(baddr)) &
					mem2(TO_INTEGER(baddr)) &
					mem1(TO_INTEGER(baddr)) &
					mem0(TO_INTEGER(baddr));
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and not( RANGE(baddr, 12, 11) == BIN(11) ) ) THEN
				PORT_BASE(mem2core_o).data <= rdata;
			//baddr = PORT_BASE(core2mem_i).addr;
			//rdata = 	mem3(TO_INTEGER(baddr)) &
			//			mem2(TO_INTEGER(baddr)) &
			//			mem1(TO_INTEGER(baddr)) &
			//			mem0(TO_INTEGER(baddr));
			//PORT_BASE(mem2core_o).data <= rdata;
			PORT_BASE(mem2core_o).data_en <= BIT(1);
			IF (PORT_BASE(core2mem_i).wr_n == BIT(0)) THEN
				//gprintf("#MMem write % @ % ", to_hex(PORT_BASE(core2mem_i).data), to_hex(PORT_BASE(core2mem_i).addr), PORT_BASE(core2mem_i).be);
				//rmem(TO_INTEGER(PORT_BASE(core2mem_i).addr)) <= PORT_BASE(core2mem_i).data;
				IF ( B(PORT_BASE(core2mem_i).be, 3) == BIT(1) ) THEN
					mem3(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 31, 24);
				ENDIF
				IF ( B(PORT_BASE(core2mem_i).be, 2) == BIT(1) ) THEN
					mem2(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 23, 16);
				ENDIF
				IF ( B(PORT_BASE(core2mem_i).be, 1) == BIT(1) ) THEN
					mem1(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 15, 8);
				ENDIF
				IF ( B(PORT_BASE(core2mem_i).be, 0) == BIT(1) ) THEN
					mem0(TO_INTEGER(baddr)) <= RANGE(PORT_BASE(core2mem_i).data, 7, 0);
				ENDIF
				//PORT_BASE(mem2core_o).data <= PORT_BASE(core2mem_i).data;
				PORT_BASE(mem2core_o).data_en <= BIT(0);
			ENDIF
		ELSE
			PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);

			PORT_BASE(mem2core_o).data_en <= BIT(0);
		ENDIF
	ENDIF
END_PROCESS
#else

// Infer 4 6144x8 ram blocks
COMPONENT(spram6144x8,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, BIT_TYPE, IN),
		PORT(addr_i, UINT(13), IN),
		PORT(data_i, UINT(8), IN),
		PORT(data_o, UINT(8), OUT),
		PORT(wen_i, BIT_TYPE, IN)
		)
);

		SIG(addr, UINT(13));
		SIG(addr_mask, UINT(13));
		SIG(data_rd0, UINT(8));
		SIG(data_rd1, UINT(8));
		SIG(data_rd2, UINT(8));
		SIG(data_rd3, UINT(8));
		SIG(data_wr0, UINT(8));
		SIG(data_wr1, UINT(8));
		SIG(data_wr2, UINT(8));
		SIG(data_wr3, UINT(8));
		SIG(wen1, BIT_TYPE); // write enable for all 4 rams
		SIG(wen2, BIT_TYPE);
		SIG(wen3, BIT_TYPE);
		SIG(wen0, BIT_TYPE);
		SIG(wen, BIT_TYPE);
		SIG(rdata_en, UINT(1));
		SIG(data_en, BIT_TYPE);
		SIG(addr_ok, BIT_TYPE);

		SIG(mask, UINT(32)); // mask read output when cs_n = 1

	BEGIN

		BLK_INST(ram0, spram6144x8,
				MAPPING(
				PM(clk, clk_mem),
				PM(reset_n, reset_n),
				PM(addr_i, addr),
				PM(data_i, data_wr0),
				PM(data_o, data_rd0),
				PM(wen_i, wen0)
				)
			);
	BLK_INST(ram1, spram6144x8,
			MAPPING(
			PM(clk, clk_mem),
			PM(reset_n, reset_n),
			PM(addr_i, addr),
			PM(data_i, data_wr1),
			PM(data_o, data_rd1),
			PM(wen_i, wen1)
			)
		);
	BLK_INST(ram2, spram6144x8,
			MAPPING(
			PM(clk, clk_mem),
			PM(reset_n, reset_n),
			PM(addr_i, addr),
			PM(data_i, data_wr2),
			PM(data_o, data_rd2),
			PM(wen_i, wen2)
			)
		);
	BLK_INST(ram3, spram6144x8,
			MAPPING(
			PM(clk, clk_mem),
			PM(reset_n, reset_n),
			PM(addr_i, addr),
			PM(data_i, data_wr3),
			PM(data_o, data_rd3),
			PM(wen_i, wen3)
			)
		);

#ifndef VHDL
	uint32_t get(uint32_t baddr)
	{
		slv<32> val = ram3.mem((baddr)) &
				ram2.mem((baddr)) &
				ram1.mem((baddr)) &
				ram0.mem((baddr));
		return TO_INTEGER(val);
	}
	void set(uint32_t addr, uint32_t val)
	{
		ram3.mem(addr) = (val >> 24) & 255;
		ram2.mem(addr) = (val >> 16) & 255;
		ram1.mem(addr) = (val >> 8) & 255;
		ram0.mem(addr) = (val) & 255;

	}
#endif

	PROCESS(0, clk_mem, reset_n) // ----------------------------------------------------------

	BEGIN
		IF ( reset_n == BIT(0) ) THEN
			rdata_en <= BIN(0);
		ELSEIF ( EVENT(clk_mem) and (clk_mem == BIT(1)) ) THEN
			//gprintf("#MMEM addr %", to_hex(TO_INTEGER(PORT_BASE(core2mem_i).addr)));
			IF ( (PORT_BASE(core2mem_i).cs_n == BIT(0)) and
					(PORT_BASE(core2mem_i).wr_n == BIT(1)) and
					not (RANGE(PORT_BASE(core2mem_i).addr, 12, 11) == BIN(11)) ) THEN
				rdata_en <= BIN(1);
			ELSE
				rdata_en <= BIN(0);
			ENDIF

		ENDIF
	END_PROCESS

	COMB_PROCESS(1, clk_mem)
		addr <= ( PORT_BASE(core2mem_i).addr and SXT(BOOL2BIN(not (RANGE(PORT_BASE(core2mem_i).addr, 12, 11) == BIN(11))), 13) );
		data_wr0 <= RANGE(PORT_BASE(core2mem_i).data, 7, 0);
		data_wr1 <= RANGE(PORT_BASE(core2mem_i).data, 15, 8);
		data_wr2 <= RANGE(PORT_BASE(core2mem_i).data, 23, 16);
		data_wr3 <= RANGE(PORT_BASE(core2mem_i).data, 31, 24);
		wen <= ( not PORT_BASE(core2mem_i).cs_n and not PORT_BASE(core2mem_i).wr_n and not (B(PORT_BASE(core2mem_i).addr, 12) and B(PORT_BASE(core2mem_i).addr, 11)) );
		wen0 <= ( B(PORT_BASE(core2mem_i).be, 0) and wen );
		wen1 <= ( B(PORT_BASE(core2mem_i).be, 1) and wen );
		wen2 <= ( B(PORT_BASE(core2mem_i).be, 2) and wen );
		wen3 <= ( B(PORT_BASE(core2mem_i).be, 3) and wen );
		PORT_BASE(mem2core_o).data <= ( (data_rd3 & data_rd2 & data_rd1 & data_rd0) and SXT( (rdata_en & rdata_en), 32 ) );
		PORT_BASE(mem2core_o).data_en <= B( rdata_en, 0 );

	END_COMB_PROCESS

#endif
//}

BLK_END;
#else
		static const mem2blk_t_base mem2blk_t0 ;
		static const blk2mem_t_base blk2mem_t0 ;

		START_OF_FILE(mem)
		INCLUDES
		USE_PACKAGE(structures)


		ENTITY(mem,
		DECL_PORTS(
				PORT(clk_mem, CLK_TYPE, IN),
				PORT(reset_n, RST_TYPE, IN),
				PORT(core2mem_i, blk2mem_t, IN),
				PORT(mem2core_o, mem2blk_t, OUT)
				)
		);

		TYPE(mem_t, ARRAY_TYPE(UINT(8),6144));

		mem_t mem0; //SIG(mem0, mem_t);// internal;
		mem_t mem1; //SIG(mem1, mem_t);// internal;
		mem_t mem2; //SIG(mem2, mem_t);// internal;
		mem_t mem3; //SIG(mem3, mem_t);// internal;

		SIG(delayed_write, BIT_TYPE);
		SIG(addr, UINT((LEN(blk2mem_t0.addr))));
		SIG(data, UINT(LEN(blk2mem_t0.data)));
		SIG(be, UINT(LEN(blk2mem_t0.be)));

		BEGIN



		uint32_t get(uint32_t baddr)
		{
			slv<32> val = mem3((baddr)) &
					mem2((baddr)) &
					mem1((baddr)) &
					mem0((baddr));
			return TO_INTEGER(val);
		}
		void set(uint32_t addr, uint32_t val)
		{
			mem3(addr) = (val >> 24) & 255;
			mem2(addr) = (val >> 16) & 255;
			mem1(addr) = (val >> 8) & 255;
			mem0(addr) = (val) & 255;

		}
		PROCESS(0, clk_mem, reset_n)
		VAR(baddr, UINT(data_addr_span - 2));
		VAR(rdata, UINT(32));
		bool disp_ok;
		BEGIN
			IF ( reset_n == BIT(0) ) THEN
				gprintf("#V addr ");//% ", &PORT_BASE(mem2core_o));//, &PORT_BASE(mem2core_o).data);
				PORT_BASE(mem2core_o).data <= TO_UINT(0, LEN(mem2blk_t0.data));
				PORT_BASE(mem2core_o).data_en <= BIT(0);
				RESET(addr);
				RESET(data);
				RESET(be);
				delayed_write <= BIT(0);
			ELSEIF ( EVENT(clk_mem) and (clk_mem == BIT(1)) ) THEN

				// Put that before read process, otherwise it actually delays of two cycles!
				// And this required a wait cycle in CPU core for consecutive Write/read to the same address
				IF (delayed_write == BIT(1)) THEN
					//disp_ok = (addr > 0x16F0) && (addr < 0x1700);
					if (name == "u1_mem" and disp_ok)
						gprintf("#MMem write % @ % ", to_hex(TO_INTEGER(data)), to_hex(TO_INTEGER(addr)), be);
					//rmem(TO_INTEGER(PORT_BASE(core2mem_i).addr)) <= PORT_BASE(core2mem_i).data;

					IF ( B(be, 3) == BIT(1) ) THEN
						mem3[TO_INTEGER(addr)] = (RANGE(data, 31, 24));
					ENDIF
					IF ( B(be, 2) == BIT(1) ) THEN
						mem2(TO_INTEGER(addr)) = RANGE(data, 23, 16);
					ENDIF
					IF ( B(be, 1) == BIT(1) ) THEN
						mem1(TO_INTEGER(addr)) = RANGE(data, 15, 8);
					ENDIF
					IF ( B(be, 0) == BIT(1) ) THEN
						mem0(TO_INTEGER(addr)) = RANGE(data, 7, 0);
					ENDIF

				ENDIF

				baddr = PORT_BASE(core2mem_i).addr;

				IF ( (PORT_BASE(core2mem_i).cs_n == BIT(0)) and not ( RANGE(baddr, 12, 11) == BIN(11) ) ) THEN
					IF (PORT_BASE(core2mem_i).wr_n == BIT(1)) THEN
						rdata = 	mem3(TO_INTEGER(baddr)) &
									mem2(TO_INTEGER(baddr)) &
									mem1(TO_INTEGER(baddr)) &
									mem0(TO_INTEGER(baddr));
						PORT_BASE(mem2core_o).data <= rdata;
						PORT_BASE(mem2core_o).data_en <= BIT(1);
						delayed_write <= BIT(0);
						//disp_ok = (baddr > 0x16F0) && (baddr < 0x1700);
						if (name == "u1_mem" and disp_ok)
							gprintf("#BMem read % @ % ", to_hex(TO_INTEGER(rdata)), to_hex(TO_INTEGER(baddr)));
					ELSE


						delayed_write <= BIT(1);
						addr <= baddr;
						data <= PORT_BASE(core2mem_i).data;
						be <= PORT_BASE(core2mem_i).be;
						PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
						PORT_BASE(mem2core_o).data_en <= BIT(0);
					ENDIF
				ELSE
					delayed_write <= BIT(0);
					PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
					PORT_BASE(mem2core_o).data_en <= BIT(0);
				ENDIF



			ENDIF
		END_PROCESS
		//}

		BLK_END;


#endif
