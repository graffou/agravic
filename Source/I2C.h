#include "../Include_libs/slv.h"

START_OF_FILE(I2C)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(altera)
USE_PACKAGE(slv_utils)


ENTITY(I2C,
DECL_PORTS(
		PORT(clk_mcu, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(i2c_irq_o, BIT_TYPE, OUT),
		PORT(i2c_scl_io, TRISTATE(1), INOUT),
		PORT(i2c_sda_io, TRISTATE(1), INOUT)
		)
		, GEN(generic_int, INTEGER)
		, GEN(generic_int2, INTEGER, 0)
);



SIG(i2c_state, UINT(4)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
SIG(next_state, UINT(4)); // 00xxx start bit, 01xxx send bit xxx 10xxx send stop bit
CONST(i2c_idle, UINT(LEN(i2c_state))) := TO_UINT(0, LEN(i2c_state));
CONST(i2c_wait, UINT(LEN(i2c_state))) := TO_UINT(1, LEN(i2c_state));
CONST(i2c_wait_start, UINT(LEN(i2c_state))) := TO_UINT(2, LEN(i2c_state));
CONST(i2c_start, UINT(LEN(i2c_state))) := TO_UINT(4, LEN(i2c_state));
CONST(i2c_check_scl, UINT(LEN(i2c_state))) := TO_UINT(5, LEN(i2c_state));
CONST(i2c_send, UINT(LEN(i2c_state))) := TO_UINT(6, LEN(i2c_state));
CONST(i2c_read, UINT(LEN(i2c_state))) := TO_UINT(7, LEN(i2c_state));
CONST(i2c_slave_ack, UINT(LEN(i2c_state))) := TO_UINT(12, LEN(i2c_state));
CONST(i2c_stop, UINT(LEN(i2c_state))) := TO_UINT(15, LEN(i2c_state));
//CONST(is_master, BOOLEAN) := true;



// Conf registers
SIG(div, UINT(8));
SIG(nbytes, UINT(3)); // 0 -> undefined (DMA) 1-> 1 byte tsfr, ... 3 -> 4-byte tsfr
SIG(start_i2c, BIT_TYPE);
SIG(reset_i2c, BIT_TYPE);
SIG(is_master, BIT_TYPE); //
SIG(start_tsfr, BIT_TYPE); //
SIG(end_tsfr, BIT_TYPE); //
SIG(wait_tsfr, BIT_TYPE); // slave waiting for start sequence

SIG(sda, BIT_TYPE);
SIG(scl, BIT_TYPE);
SIG(sda_oe, BIT_TYPE);
SIG(scl_oe, BIT_TYPE);

SIG(master_clk, UINT(1));
SIG(cnt, UINT(8));
SIG(tsfr_cnt, UINT(4)); // +ACK
SIG(byte, UINT(8)); //
SIG(timeout, UINT(8)); //

SIG(i2c_sdap, BIT_TYPE);
SIG(i2c_sda, BIT_TYPE);
SIG(i2c_clkp, BIT_TYPE);
SIG(i2c_clk, BIT_TYPE);
SIG(i2c_rx, BIT_TYPE);
SIG(i2c_ack, BIT_TYPE);
SIG(no_ack, BIT_TYPE);
SIG(check_scl_en, BIT_TYPE);
SIG(check_scl, BIT_TYPE);
SIG(tsfr, BIT_TYPE); // when transfer in progress
SIG(start_irq, BIT_TYPE);
SIG(byte_irq, BIT_TYPE);
SIG(stop_irq, BIT_TYPE);
SIG(start_irq_en, BIT_TYPE);
SIG(byte_irq_en, BIT_TYPE);
SIG(stop_irq_en, BIT_TYPE);
SIG(sda_exception_value, BIT_TYPE); // sda value for exceptional conditions (start or stop)
SIG(force_sda_exception_value, BIT_TYPE); // sda value for exceptional conditions (start or stop)
SIG(clock_stretching, BIT_TYPE);
SIG(clock_stretching_en, BIT_TYPE);


SIG(busy, BIT_TYPE);

//dbg
SIG(rx_trans_dbg, BIT_TYPE);


//SIG(cnt_test2, UINT(8));
CONST(reg_base_addr, UINT(LEN(PORT_BASE(core2mem_i).addr))) := TO_UINT(generic_int, LEN(PORT_BASE(core2mem_i).addr));
SIG(base_addr_test, UINT(LEN(PORT_BASE(core2mem_i).addr)));
CONST(reg_addr_lsbs, INTEGER) := ( generic_int / 268435456);
SIG(addr_lsbs_test, UINT(4));

BEGIN


// Is a declaration in C++, instantiation in VHDL(after begin)
//GATED_CLK(clk_g ,clk_peri, gate_cell);
//#define LOOPBACK_TEST

PROCESS(3, clk_mcu, reset_n)

VAR(val, UINT(32));
VAR(next_cnt, UINT(LEN(cnt)));
VAR(strobe_rx, BIT_TYPE);
VAR(clk_posedge, BIT_TYPE);
VAR(clk_negedge, BIT_TYPE);
VAR(sda_posedge, BIT_TYPE);
VAR(sda_negedge, BIT_TYPE);
VAR(rx_bit, BIT_TYPE);
VAR(rx_clk, BIT_TYPE);


BEGIN
	IF ( reset_n == BIT(0) ) THEN
		div <= TO_UINT((1), LEN(div)); // assume 60MHz defualt
		//ready <= BIT(1);
		start_i2c <= BIT(0);
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		reset_i2c <= BIT(0);
		busy <= BIT(0);
		i2c_ack <= BIT(0);
		master_clk <= BIN(0);
		check_scl <= BIT(0);
		check_scl_en <= BIT(0);
		tsfr <= BIT(0);
		i2c_irq_o <= BIT(0);
		i2c_clk <= BIT(0);
		i2c_clkp <= BIT(0);
		i2c_sda <= BIT(0);
		i2c_sdap <= BIT(0);
		start_irq <= BIT(0);
		byte_irq <= BIT(0);
		stop_irq <= BIT(0);
		start_irq_en <= BIT(0);
		byte_irq_en <= BIT(0);
		stop_irq_en <= BIT(0);
		sda_exception_value <= BIT(0);
		force_sda_exception_value <= BIT(0);
		sda <= BIT(0);
		sda_oe <= BIT(0);
		clock_stretching <= BIT(0);
		clock_stretching_en <= BIT(0);
	ELSEIF ( EVENT(clk_mcu) and (clk_mcu == BIT(1)) ) THEN

		start_i2c <= BIT(0);
		reset_i2c <= BIT(0);

		// Registers R/W
		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		//busy <= (busy and not ready);
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, HI(PORT_BASE(core2mem_i).addr), REG_NBITS) == RANGE( reg_base_addr, HI(reg_base_addr), REG_NBITS) ) ) THEN
			// Write registers
			IF (PORT_BASE(core2mem_i).wr_n == BIT(0) ) THEN
				// just configure
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					val := PORT_BASE(core2mem_i).data;
					byte <= RANGE(val, 31, 24); // data if tx mode
					div <= RANGE(val, 23, 16);
					start_tsfr <= B(val, 1);
					end_tsfr <= B(val, 2);
					wait_tsfr <= B(val, 3);
					check_scl_en <= B(val, 5);// for clock stretching: check that scl is actually high
					clock_stretching_en <= B(val, 6);// for clock stretching: pull scl to gnd before sending ACK
					i2c_rx <= B(val, 4);
					i2c_ack <= BIT(0);
					no_ack <= BIT(0);
					busy <= BIT(1);
					start_irq_en <= B(val, 8);
					byte_irq_en <= B(val, 9);
					stop_irq_en <= B(val, 10);

					IF (B(val, 0) == BIT(1)) THEN // master -----
						IF (B(val, 7) == BIT(1)) THEN // master force end
							i2c_state <= i2c_stop;
						ELSEIF (B(val, 1) == BIT(1)) THEN // master starts
							i2c_state <= i2c_start;
						ELSEIF (B(val, 4) == BIT(1)) THEN //reads data
							next_state <= i2c_read;
							i2c_state <= i2c_check_scl;	// checks slave released the scl line (stretched clocking)
						ELSE					// ends data
							next_state <= i2c_send;
							i2c_state <= i2c_check_scl;	// checks slave released the scl line (stretched clocking)
						ENDIF
					ELSE // slave ----
						IF (B(val, 3) == BIT(1)) THEN // slave waits for start seq.
							i2c_state <= i2c_wait_start;
						ELSEIF (BIT2BOOL(clock_stretching)) THEN
							i2c_state <= i2c_slave_ack;
						ELSEIF (B(val, 4) == BIT(1)) THEN // reads data
							i2c_state <= i2c_read;
						ELSE					// sends data
							i2c_state <= i2c_send;
						ENDIF
					ENDIF

					is_master <= B(val, 0);
					tsfr_cnt <= TO_UINT(7, LEN(tsfr_cnt));
					force_sda_exception_value <= BIT(0);
					PORT_BASE(i2c_scl_io) <= "Z"; // in case of slave holding the bus (clock stretching)
				ENDIF

			ELSE
				// Read registers
				//IF (PORT_BASE(core2mem_i).addr == BIN(1111111000000)) THEN
				IF (RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0) == TO_UINT(0, REG_NBITS)) THEN
					PORT_BASE(mem2core_o).data_en <= BIT(1);
					PORT_BASE(mem2core_o).data <= ( byte & TO_UINT(0, 13) & stop_irq & byte_irq & start_irq & BIN(00000)& tsfr & no_ack & busy );
					start_irq <= BIT(0); byte_irq <= BIT(0); stop_irq <= BIT(0); // clear IRQ flags
				ENDIF

			ENDIF
		ENDIF

	// Tx / rx part----------------------------------------------------------------------



		// Synchronize
		IF (scl_oe == BIT(0)) THEN
			i2c_clk <= B(PORT_BASE(i2c_scl_io), 0);
			i2c_clkp <= i2c_clk;
			clk_negedge := ( (i2c_clkp) and (i2c_clk xor i2c_clkp) );
			clk_posedge := ( (i2c_clk) and (i2c_clk xor i2c_clkp) );
		ELSE
			i2c_clk <= BIT(1);
			i2c_clkp <= BIT(1);
			clk_negedge := BIT(0);
			clk_posedge := BIT(0);
		ENDIF
		IF (sda_oe == BIT(0)) THEN
			i2c_sda <= BIN2BIT(PORT_BASE(i2c_sda_io));
			i2c_sdap <= i2c_sda;
			sda_negedge := ( ( i2c_sdap) and  (i2c_sda xor i2c_sdap) );
			sda_posedge := ( ( i2c_sda)  and  (i2c_sda xor i2c_sdap) );
		ELSE
			i2c_sda <= BIT(1);
			i2c_sdap <= BIT(1);
			sda_negedge := BIT(0);
			sda_posedge := BIT(0);
		ENDIF

		rx_clk := B(PORT_BASE(i2c_scl_io), 0);


		// i2c FSM ------------------------------------
		IF (i2c_state == i2c_idle) THEN
			busy <= BIT(0);
		ELSEIF (i2c_state == i2c_wait) THEN // state used for delays
			next_cnt := cnt - TO_UINT(1, LEN(cnt));
			IF (not BIT2BOOL(is_master and check_scl)) THEN
				cnt <= next_cnt;
			ELSEIF (rx_clk == BIT(1))  THEN // scl ok (end of slv clk stretching) -> resume cycle
				check_scl <= BIT(0);
			ENDIF
			// in slave mode, exit on clk toggle
			IF ( ( (cnt == TO_UINT(1, LEN(cnt))) and BIT2BOOL(is_master) ) or ( BIT2BOOL(clk_negedge) and not BIT2BOOL(is_master)) ) THEN
				IF (tsfr_cnt < TO_UINT(2, LEN(tsfr_cnt))) THEN // ACK
					i2c_rx  <= BIT(0); // no more rx

					IF (i2c_ack == BIT(1)) THEN // 2nd time here, end of tsfr
						i2c_ack <= BIT(0);
						byte_irq <= BIT(1);
						i2c_irq_o <= byte_irq_en;
						IF (is_master == BIT(1)) THEN
							IF (end_tsfr == BIT(1)) THEN //
								i2c_state <= i2c_stop;
							ELSE
								i2c_state <= i2c_idle;
							ENDIF
						ELSE // slave
							i2c_state <= i2c_idle;
							clock_stretching <= clock_stretching_en;
							IF (BIT2BOOL(clock_stretching_en)) THEN
								i2c_scl_io <= BIN(0); // force scl to gnd until uc sends another command
							ENDIF
						ENDIF
					ELSE
						i2c_ack <= BIT(1);
						IF (is_master == BIT(1)) THEN
							sda_oe <= BIT(0); //release bus for slv ACK
							cnt <= div;
						ELSE
							sda <= BIT(0); // send ACK
							sda_oe <= BIT(1); // send ACK
						ENDIF
					ENDIF

				ELSE
					i2c_state <= next_state;
				ENDIF

//				IF (BOOL2BIT(is_master)) THEN
//					scl <= BIT(0); // before sda change
//					scl_oe <= BIT(1); // before sda change
//				ENDIF
			ENDIF

			strobe_rx := ( (is_master and BOOL2BIT(next_cnt == EXT(RANGE(div, HI(div), 1), LEN(cnt)))) or ( (not is_master) and clk_posedge) );

			IF ( BIT2BOOL(is_master and strobe_rx) ) THEN // create master i2c clk
				// ???master_clk <= BIT(1);
				check_scl <= check_scl_en;
				i2c_scl_io <= "Z";
				IF (BIT2BOOL(force_sda_exception_value)) THEN
					sda <= sda_exception_value;
				ENDIF
			ENDIF

			IF ( (i2c_rx == BIT(1)) and (strobe_rx == BIT(1)) and not (force_sda_exception_value == BIT(1)) ) THEN // rx + not master start/stop: receiver shift register
				tsfr_cnt <= tsfr_cnt - 1;
				rx_bit := BIN2BIT(i2c_sda_io);
				byte <= ( RANGE( byte, 6, 0) & rx_bit );
			ENDIF

			IF ( (is_master == BIT(1)) and (i2c_ack == BIT(1)) and (strobe_rx == BIT(1)) ) THEN //check ACK
				rx_bit := BIN2BIT(i2c_sda_io); // Z to pullup 1
				no_ack <= (rx_bit);
				i2c_ack <= BIT(0);
			ENDIF


		ELSEIF (i2c_state == i2c_start) THEN // csn low, then wait csn_clk_ncycles, then send
			sda_oe <= BIT(0); // sda 1
			sda <= BIT(1); // sda 1
			i2c_scl_io <= "Z";    // scl 1
			i2c_state <= i2c_wait;
			IF (i2c_rx == BIT(1)) THEN
				next_state <= i2c_read;
			ELSE
				next_state <= i2c_send;
			ENDIF
			force_sda_exception_value <= BIT(1);
			sda_exception_value <= BIT(0);
			cnt <= div;
			tsfr_cnt <= TO_UINT(9, LEN(tsfr_cnt)); // 8 bits + ack + stop

		ELSEIF (i2c_state == i2c_wait_start) THEN //

			IF (i2c_clk == BIT(1) and sda_negedge == BIT(1)) THEN
				IF (i2c_rx == BIT(1)) THEN
					next_state <= i2c_read;
				ELSE
					next_state <= i2c_send;
				ENDIF
				start_irq <= BIT(1);
				i2c_irq_o <= start_irq_en;
				// tsfr_cnt is the number of bits to transfer (can be renewed in case of dma tsfr)
				tsfr_cnt <= TO_UINT(9, LEN(tsfr_cnt)); // 8 bits + ack + stop
			ENDIF
			busy <= BIT(1);

		ELSEIF (i2c_state == i2c_send) THEN
			sda <= B(byte,7);
			byte <= SHIFT_LEFT(byte, 1);
			i2c_state <= i2c_wait;
			next_state <= i2c_send;
			tsfr_cnt <= tsfr_cnt - 1;
			IF (BIT2BOOL(is_master)) THEN
				i2c_scl_io <= BIN(0);
				master_clk <= BIN(0);
				check_scl <= BIT(0);
			ENDIF
			cnt <= div;

		ELSEIF (i2c_state == i2c_read) THEN
			i2c_state <= i2c_wait;
			next_state <= i2c_read;
			IF (BIT2BOOL(is_master)) THEN
				i2c_scl_io <= BIN(0);
				check_scl <= BIT(0);
				master_clk <= BIN(0);
			ENDIF
			cnt <= div;

		ELSEIF (i2c_state == i2c_check_scl) THEN // Wait for slave releasing scl line
			rx_clk := B(PORT_BASE(i2c_scl_io), 0); // converts Z to pullup 1
			IF (rx_clk == BIT(1)) THEN // 1, so is OK
				i2c_state <= next_state; // either read or send
				i2c_scl_io <= BIN(0); // before data change
			ENDIF

		ELSEIF (i2c_state == i2c_stop) THEN // should not end here (means fifo empty while receiving)
			i2c_state <= i2c_wait;
			next_state <= i2c_idle;
			i2c_scl_io <= "Z";
			force_sda_exception_value <= BIT(1);
			sda_exception_value <= BIT(1);
			sda <= BIT(0);
			cnt <= div;
			tsfr <= BIT(0);


		ELSEIF (i2c_state == i2c_slave_ack) THEN
			i2c_state <= i2c_wait;
			next_state <= i2c_idle;
			sda <= BIT(0);
			cnt <= div;
		ENDIF

		IF ((tsfr == BIT(1)) and (is_master == BIT(0)) and (sda_posedge == BIT(1)) and (rx_clk == BIT(1))) THEN // slave detects I2C stop
			tsfr <= BIT(0);
			stop_irq <= BIT(1);
			i2c_irq_o <= stop_irq_en;
			i2c_state <= i2c_idle;
		ENDIF

		IF (BIT2BOOL(sda_oe and not sda)) THEN
			i2c_sda_io <= BIN(0);
		ELSE
			i2c_sda_io <= "Z";
		ENDIF

	ENDIF
END_PROCESS


BLK_END;
