#include "../Include_libs/slv.h"

START_OF_FILE(structures)

INCLUDES

PACKAGE(structures)

RECORD(blk2mem_t,
		DECL_FIELDS(
				FIELD(addr, UINT(data_addr_span - 2)),
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
RECORD(blk2mem8_t,
		DECL_FIELDS(
				FIELD(addr, UINT(13)),
				FIELD(data, UINT(8)),
				FIELD(cs_n, BIT_TYPE),
				FIELD(wr_n, BIT_TYPE)
				)
);

RECORD(mem2blk8_t,
		DECL_FIELDS(
				FIELD(data, UINT(8)),
				FIELD(data_en, BIT_TYPE)
				)
);
// dma <=> periph, slave (peripheral side)
RECORD(p2d_16_t,
		DECL_FIELDS(
				FIELD(data, UINT(16)),
				FIELD(rdy, BIT_TYPE),
				FIELD(data_en, BIT_TYPE)
				)
);

// dma <=> periph, master (dma side)
RECORD(d2p_16_t,
		DECL_FIELDS(
				FIELD(data, UINT(16)),
				FIELD(grant, BIT_TYPE),
				FIELD(data_en, BIT_TYPE)
				)
);

// dma <=> periph, slave (peripheral side)
RECORD(p2d_8_t,
		DECL_FIELDS(
				FIELD(data, UINT(8)),
				FIELD(rdy, BIT_TYPE),
				FIELD(data_en, BIT_TYPE)
				)
);

// dma <=> periph, master (dma side)
RECORD(d2p_8_t,
		DECL_FIELDS(
				FIELD(data, UINT(8)),
				FIELD(grant, BIT_TYPE),
				FIELD(init, BIT_TYPE),
				FIELD(data_en, BIT_TYPE)
				)
);


// dma <=> periph, slave (peripheral side)
RECORD(p2d_t,
		DECL_FIELDS(
				FIELD(data, UINT(32)),
				FIELD(rdy, BIT_TYPE),
				FIELD(data_en, BIT_TYPE)
				)
);

// dma <=> periph, master (dma side)
RECORD(d2p_t,
		DECL_FIELDS(
				FIELD(data, UINT(32)),
				FIELD(grant, BIT_TYPE),
				FIELD(data_en, BIT_TYPE)
				)
);


RECORD(dma_channel_t,
		DECL_FIELDS(
				FIELD(addr, UINT(data_addr_span)),
				FIELD(tsfr_sz, UINT(DMA_TSFR_SZ)),
				FIELD(wait_time, UINT(DMA_WAIT_TIME_SZ)),
				FIELD(priority, UINT(DMA_PRIORITY_SZ)),
				FIELD(tmp_data, UINT(32)),
				FIELD(tmp_we, UINT(4)),
				FIELD(time, UINT(24)),
				FIELD(timeout, UINT(DMA_TIME_SZ)),
				FIELD(auto_inc, UINT(4)),
				FIELD(periph_mask, UINT(2)),
				FIELD(data_mask, UINT(2)),
				FIELD(ntransferred, UINT(2)), // keep count of data already transferred
				FIELD(source, UINT(3)),
				FIELD(sink, UINT(3)),
				FIELD(state, UINT(DMA_STATE_SZ))
				)
);

// limit sdram accesses to full 16-bit
RECORD(sdram_req_t,
		DECL_FIELDS(
				FIELD(addr, UINT(22)),
				FIELD(rnw, BIT_TYPE),
				FIELD(sz, UINT(8)), // !!!!!! This is actual size minus 1 !!!!!!
				FIELD(en, BIT_TYPE)
				)
);
END_PACKAGE(structures)
