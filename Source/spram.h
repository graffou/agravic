#include "slv.h"
START_OF_FILE(spram)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(slv_utils)


ENTITY(spram,
DECL_PORTS(
		PORT(clk_mem, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(addr_i, UINT(data_addr_span-2), IN),
		PORT(data_i, UINT(32), IN),
		PORT(be_i, UINT(4), IN),
		PORT(wrn_i, BIT_TYPE, IN),
		PORT(data_o, UINT(32), OUT)

		),
		INTEGER generic_int
);

TYPE(word_t, ARRAY_TYPE(UINT(8),4));
TYPE(mem_t, ARRAY_TYPE(word_t, generic_int));
SIG(mem0, mem_t);
//SIG(mem_len, UINT(32));
SIG(mem_bus, word_t);


	BEGIN

#ifdef VHDL
    -- Re-organize the read data from the RAM to match the output
	unpack: for i in 0 to 3 generate
	data_o(8*(i+1) - 1 downto 8*i) <= mem_bus(i);
	end generate unpack;

#endif

	PROCESS(0, clk_mem, reset_n) // ----------------------------------------------------------

	BEGIN
		IF ( EVENT(clk_mem) and (clk_mem == BIT(1)) ) THEN
			//mem_len <= TO_UINT(ARRAY_LEN(mem0), LEN(mem_len));
			IF (wrn_i == BIT(1)) THEN
					IF (B(PORT_BASE(be_i),3) == BIT(1)) THEN
						mem0(TO_INTEGER(PORT_BASE(addr_i)))(3) <= RANGE(PORT_BASE(data_i), 31, 24);
					ENDIF
					IF (B(PORT_BASE(be_i),2) == BIT(1)) THEN
						mem0(TO_INTEGER(PORT_BASE(addr_i)))(2) <= RANGE(PORT_BASE(data_i), 23, 16);
					ENDIF
					IF (B(PORT_BASE(be_i),1) == BIT(1)) THEN
						mem0(TO_INTEGER(PORT_BASE(addr_i)))(1) <= RANGE(PORT_BASE(data_i), 15, 8);
					ENDIF
					IF (B(PORT_BASE(be_i),0) == BIT(1)) THEN
						mem0(TO_INTEGER(PORT_BASE(addr_i)))(0) <= RANGE(PORT_BASE(data_i), 7, 0);
					ENDIF
					//gprintf("#UWrite % addr %", PORT_BASE(data_i), PORT_BASE(addr_i));
			ENDIF
#ifdef VHDL
			mem_bus <= mem0(TO_INTEGER(addr_i));
#else
			data_o <= ( mem0(TO_INTEGER(PORT_BASE(addr_i)))(3) & mem0(TO_INTEGER(PORT_BASE(addr_i)))(2) & mem0(TO_INTEGER(PORT_BASE(addr_i)))(1) & mem0(TO_INTEGER(PORT_BASE(addr_i)))(0));
#endif
		ENDIF

	END_PROCESS
BLK_END;
