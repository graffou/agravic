#include "../Include_libs/slv.h"

START_OF_FILE(SPI_wrapper)
INCLUDES
USE_PACKAGE(structures)
//USE_PACKAGE(altera)
USE_PACKAGE(slv_utils)


ENTITY(SPI_wrapper,
DECL_PORTS(
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		//PORT(core2mem_i, blk2mem_t, IN),
		//PORT(mem2core_o, mem2blk_t, OUT),
		//PORT(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
		//PORT(spi_dma_i, d2p_8_t, IN),
		//PORT(spi_dma_o, p2d_8_t, OUT),
		PORT(data_i, UINT(16), IN),
		PORT(data_o, UINT(16), OUT),
		PORT(data_en_i, BIT_TYPE, IN),
		PORT(data_en_o, BIT_TYPE, OUT),
		PORT(spi_csn_o, BIT_TYPE, OUT),
		PORT(spi_clk_o, BIT_TYPE, OUT),
		PORT(spi_tx_o, BIT_TYPE, OUT),
		PORT(spi_rx_i, BIT_TYPE, IN)
		),
		INTEGER generic_int

);





COMPONENT(SPI_master,
DECL_PORTS(
		PORT(clk_120, CLK_TYPE, IN),
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
		PORT(spi_dma_i, d2p_8_t, IN),
		PORT(spi_dma_o, p2d_8_t, OUT),
		PORT(spi_csn_o, BIT_TYPE, OUT),
		PORT(spi_clk_o, BIT_TYPE, OUT),
		PORT(spi_tx_o, BIT_TYPE, OUT),
		PORT(spi_rx_i, BIT_TYPE, IN)
		)
		, INTEGER generic_int
);
SIG(spi2dma, p2d_8_t);
SIG(dma2spi, d2p_8_t);
SIG(core2datamem, blk2mem_t);
SIG(spi2core, mem2blk_t);
SIG(spi_trig, BIT_TYPE);
SIG(spi_init, BIT_TYPE);
SIG(cnt_test, UINT(8));
SIG(cnt_test2, UINT(8));

BEGIN

BLK_INST(u0_spi, SPI_master,
MAPPING(
		PM(clk_120, clk_mcu),
		PM(clk_mcu, clk_mcu),
		PM(reset_n, reset_n),
		PM(core2mem_i, core2datamem),
		PM(mem2core_o, spi2core),
		PM(spi_dma_i, dma2spi),
		PM(spi_dma_o, spi2dma),
		PM(trig_i, spi_trig),
		PM(spi_clk_o, spi_clk_o),
		PM(spi_csn_o, spi_csn_o),
		PM(spi_tx_o, spi_tx_o),
		PM(spi_rx_i, spi_rx_i)
		/*,
		(trig_i, BIT_TYPE, IN),    // signal that trigs transfers (should come from timer)
		(, d2p_8_t, IN),
		(spi_dma_o, p2d_8_t, OUT),
		(spi_csn_o, BIT_TYPE, OUT),
		(spi_clk_o, BIT_TYPE, OUT),
		(spi_tx_o, BIT_TYPE, OUT),
		(spi_rx_i, BIT_TYPE, IN)
		)*/
		),
		generic_int
);

PROCESS(3, clk_mcu, reset_n)

VAR(val, UINT(32));
VAR(tsfr_sz, UINT(3)); // in bytes. 0 for dma
VAR(clk_pol_tmp, BIT_TYPE);
VAR(fifo_full_tx , BOOLEAN);
VAR(fifo_empty_tx , BOOLEAN);
VAR(fifo_wptr_tx_tmp, UINT(3));

BEGIN
	IF ( reset_n == BIT(0) ) THEN
		spi_init <= BIT(1);
		core2datamem.cs_n <= BIT(1);//( (1 << 16) | (3 << 11) | (0 << 8) );
		core2datamem.wr_n <= BIT(1);//( (1 << 16) | (3 << 11) | (0 << 8) );
		core2datamem.be <= BIN(1111);//( (1 << 16) | (3 << 11) | (0 << 8) );
		RESET(cnt_test);
		RESET(cnt_test2);
		dma2spi.data_en <= BIT(0);

	ELSEIF ( EVENT(clk_mcu) and (clk_mcu == BIT(1)) ) THEN
		IF (spi_init == BIT(1)) THEN
			core2datamem.addr <= TO_UINT(generic_int, LEN(core2datamem.addr));//BIN(1111111010000);
			core2datamem.data <= HEX(00010800);//( (1 << 16) | (3 << 11) | (0 << 8) );
			core2datamem.cs_n <= BIT(0);//( (1 << 16) | (3 << 11) | (0 << 8) );
			core2datamem.wr_n <= BIT(0);//( (1 << 16) | (3 << 11) | (0 << 8) );
			cnt_test <= cnt_test + 1;
			IF (cnt_test == TO_UINT(19, 8)) THEN
				spi_init <= BIT(0);
				RESET(cnt_test);
			ENDIF

		ELSE
			core2datamem.cs_n <= BIT(1);//( (1 << 16) | (3 << 11) | (0 << 8) );
			core2datamem.wr_n <= BIT(1);//( (1 << 16) | (3 << 11) | (0 << 8) );
			IF (cnt_test == TO_UINT(19, 8)) THEN
				dma2spi.data <= cnt_test2;
				dma2spi.data_en <= BIT(1);
				cnt_test2 <= cnt_test2 + 1;
				RESET(cnt_test);
			ELSE
				dma2spi.data_en <= BIT(0);
				cnt_test <= cnt_test + 1;
			ENDIF
		ENDIF
	ENDIF
END_PROCESS

BLK_END;

