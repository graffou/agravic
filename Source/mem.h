#include "slv.h"
// have separate implementations for vhdl and c++, so that we don't get thousands of flops in the C++ design
// C++ memory are just arrays
// Write delay is obtained by delaying the write operation
#ifdef VHDL
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

SIG(mem0, mem_t);// internal;
SIG(mem1, mem_t);// internal;
SIG(mem2, mem_t);// internal;
SIG(mem3, mem_t);// internal;
//SIG(blk2mem_t0, blk2mem_t);
SIG(mem2blk_t0, mem2blk_t);

BEGIN





PROCESS(0, clk_mem, reset_n)
VAR(baddr, UINT(13));
VAR(rdata, UINT(32));
BEGIN
//	IF ( reset_n == BIT(0) ) THEN
//		gprintf("#V addr ");//% ", &PORT_BASE(mem2core_o));//, &PORT_BASE(mem2core_o).data);
//		PORT_BASE(mem2core_o).data <= TO_UINT(0, LEN(mem2blk_t0.data));
//		PORT_BASE(mem2core_o).data_en <= BIT(0);
	IF ( EVENT(clk_mem) and (clk_mem == BIT(1)) ) THEN
		IF (PORT_BASE(core2mem_i).cs_n == BIT(0)) THEN
			baddr := PORT_BASE(core2mem_i).addr;
			rdata := 	mem3(TO_INTEGER(baddr)) &
						mem2(TO_INTEGER(baddr)) &
						mem1(TO_INTEGER(baddr)) &
						mem0(TO_INTEGER(baddr));
			PORT_BASE(mem2core_o).data <= rdata;
			PORT_BASE(mem2core_o).data_en <= BIT(1);
			IF (PORT_BASE(core2mem_i).wr_n == BIT(0)) THEN
				gprintf("#MMem write % @ % ", to_hex(PORT_BASE(core2mem_i).data), to_hex(PORT_BASE(core2mem_i).addr), PORT_BASE(core2mem_i).be);
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
			PORT_BASE(mem2core_o).data_en <= BIT(0);
		ENDIF
	ENDIF
END_PROCESS
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
		VAR(baddr, UINT(13));
		VAR(rdata, UINT(32));
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
				IF (PORT_BASE(core2mem_i).cs_n == BIT(0)) THEN
					baddr := PORT_BASE(core2mem_i).addr;
					IF (PORT_BASE(core2mem_i).wr_n == BIT(1)) THEN
						rdata := 	mem3(TO_INTEGER(baddr)) &
									mem2(TO_INTEGER(baddr)) &
									mem1(TO_INTEGER(baddr)) &
									mem0(TO_INTEGER(baddr));
						PORT_BASE(mem2core_o).data <= rdata;
						PORT_BASE(mem2core_o).data_en <= BIT(1);
						delayed_write <= BIT(0);
					ELSE


						delayed_write <= BIT(1);
						addr <= baddr;
						data <= PORT_BASE(core2mem_i).data;
						be <= PORT_BASE(core2mem_i).be;

						PORT_BASE(mem2core_o).data_en <= BIT(0);
					ENDIF
				ELSE
					delayed_write <= BIT(0);
					PORT_BASE(mem2core_o).data_en <= BIT(0);
				ENDIF

				IF (delayed_write == BIT(1)) THEN

					gprintf("#MMem write % @ % ", to_hex(data), to_hex(addr), be);
				//rmem(TO_INTEGER(PORT_BASE(core2mem_i).addr)) <= PORT_BASE(core2mem_i).data;

					IF ( B(be, 3) == BIT(1) ) THEN
						mem3[TO_INTEGER(addr)] := (RANGE(data, 31, 24));
					ENDIF
					IF ( B(be, 2) == BIT(1) ) THEN
						mem2(TO_INTEGER(addr)) := RANGE(data, 23, 16);
					ENDIF
					IF ( B(be, 1) == BIT(1) ) THEN
						mem1(TO_INTEGER(addr)) := RANGE(data, 15, 8);
					ENDIF
					IF ( B(be, 0) == BIT(1) ) THEN
						mem0(TO_INTEGER(addr)) := RANGE(data, 7, 0);
					ENDIF

				ENDIF

			ENDIF
		END_PROCESS
		//}

		BLK_END;


#endif
