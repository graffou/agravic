#include "slv.h"

START_OF_FILE(structures)
INCLUDES

PACKAGE(structures)

RECORD(blk2mem_t,
		DECL_FIELDS(
				FIELD(addr, UINT(14)),
				FIELD(data, UINT(32)),
				FIELD(be,   UINT(4)),
				FIELD(cs_n, BIT_TYPE),
				FIELD(wr_n, BIT_TYPE)
				)
);

RECORD(mem2blk_t,
		DECL_FIELDS(
				FIELD(data, UINT(32)),
				FIELD(data_en, BIT_TYPE)
				)
);

END_PACKAGE(structures)
