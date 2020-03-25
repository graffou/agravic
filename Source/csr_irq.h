#include "slv.h"

START_OF_FILE(crs_irq)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(slv_utils)
USE_PACKAGE(risc_V_constants)

// In the crs_irq block, the 4-bit ben field is used to code the CSR operation
// CSRRW ... CSRRCI, ECALL = 0
// The mcause value is stored as the msbs of CSR addr

ENTITY(csr_irq,
DECL_PORTS(
		PORT(clk_csr_irq, CLK_TYPE, IN), // might be the 'always on' clock in the future
		PORT(reset_n, RST_TYPE, IN),
		PORT(core2mem_i, blk2mem_t, IN),
		PORT(irq_i, irq_t, IN), //Array of external IRQs
		PORT(mem2core_o, mem2blk_t, OUT),
		PORT(csr2core_o, csr2core_t, OUT)
		),
		INTEGER generic_int
);


// generic addressisng test signals
CONST(reg_base_addr, UINT(LEN(PORT_BASE(core2mem_i).addr))) := TO_UINT(generic_int, LEN(PORT_BASE(core2mem_i).addr));
SIG(base_addr_test, UINT(LEN(PORT_BASE(core2mem_i).addr)));
CONST(reg_addr_lsbs, INTEGER) := ( generic_int / 268435456);
SIG(addr_lsbs_test, UINT(4));
SIG(do_read_dbg, BIT_TYPE);
SIG(do_write_dbg, BIT_TYPE);
SIG(csr_op_dbg, UINT(3));




// CSR and friends
SIG(csri, BIT_TYPE); // 1 when CSR*I instruction
SIG(priv, UINT(2));
SIG(mstatus, UINT(32));
SIG(mepc, UINT(32));
SIG(mscratch, UINT(32));
SIG(mtvec, UINT(32));
SIG(mcause, UINT(32));
SIG(mideleg, UINT(32));
SIG(medeleg, UINT(32));
SIG(mip, UINT(32));
SIG(mie, UINT(32));
SIG(mcounter, UINT(64));
SIG(mcounteren, UINT(32));
SIG(mtimecmp, UINT(64));
SIG(csr_read, UINT(32)); // register old csr value
SIG(wfi, BIT_TYPE);

SIG(dbg_addr_ok, BIT_TYPE);

BEGIN


PROCESS(0, clk_csr_irq, reset_n) // ----------------------------------------------------------
VAR(addr_lsbs, UINT(REG_NBITS)); // Is either CSR addr or cause (? yes)
VAR(csr_val, UINT(32)); //
VAR(op1, UINT(32)); //
VAR(csr_op, UINT(3)); //
VAR(do_write, BIT_TYPE); // condition for old CSR read
VAR(do_read, BIT_TYPE); // condition for modified CSR write
VAR(is_ECALL, BIT_TYPE); // condition for modified CSR write
VAR(wfi_tmp, BIT_TYPE); // wfi is bit 3 of be
VAR(irq_tmp, BIT_TYPE); //
VAR(it_bit, UINT(4)); //
VAR(mip_tmp, UINT(32)); //
VAR(cause, UINT(6)); //
VAR(mstatus_mie, BIT_TYPE); // Machine mode interrupt en

BEGIN

IF (reset_n == BIT(0)) THEN
	RESET(mcounter);
	//mcounteren <= TO_UINT(2, 32); // enable counter
	//RESET(mcounterinhib);
	RESET(mstatus);
	RESET(mcause);
	//RESET(medeleg);
	//RESET(mideleg);
	RESET(mtvec);
	RESET(mepc);
	RESET(mip);
	RESET(mie);
	RESET(mscratch);
	RESET(mtimecmp);
	wfi <= BIT(0);
	priv <= BIN(11);
	base_addr_test <= reg_base_addr;
	addr_lsbs_test <= TO_UINT(REG_NBITS, 4);
ELSEIF (EVENT(clk_csr_irq) and (clk_csr_irq == BIT(1))) THEN

		PORT_BASE(mem2core_o).data_en <= BIT(0);
		PORT_BASE(mem2core_o).data <= TO_UINT(0, 32);
		do_write_dbg <= BIT(0);
		do_read_dbg  <= BIT(0);
		dbg_addr_ok <= BIT(0);
		IF ( ( PORT_BASE(core2mem_i).cs_n == BIT(0) ) and ( RANGE( PORT_BASE(core2mem_i).addr, HI(PORT_BASE(core2mem_i).addr), REG_NBITS) == RANGE( reg_base_addr, HI(reg_base_addr), REG_NBITS) ) ) THEN//( RANGE( PORT_BASE(core2mem_i).addr, 12, 4) == BIN(111111110) ) ) THEN
			dbg_addr_ok <= BIT(1);
			addr_lsbs := RANGE(PORT_BASE(core2mem_i).addr, REG_NBITS-1, 0); // CSR addr
			op1 := PORT_BASE(core2mem_i).data;
			// is Immediate & is_C/S & is_C => x01 for RW, x10 for S and x11 for C
			csr_op := RANGE(PORT_BASE(core2mem_i).be, 2, 0); // recycle be field
			wfi_tmp := B(PORT_BASE(core2mem_i).be, 3);
			wfi <= wfi_tmp;
			is_ECALL := BOOL2BIT( csr_op == BIN(000) ); // no read, no write
			do_write := ( ( not B(csr_op, 1) or (not PORT_BASE(core2mem_i).wr_n)) and not is_ECALL ); // 1 If RW, otherwise 1 if wr_n = 0
			do_read := ( ( B(csr_op, 1) or (PORT_BASE(core2mem_i).wr_n)) and not is_ECALL ); // 1 If RC/S, otherwise 1 if wr_n = 1 (force read)
			do_write_dbg <= do_write;
			do_read_dbg <= do_read;
			csr_op_dbg <= csr_op;
			// Read old csr val
			SWITCH(addr_lsbs)
				CASE(CASE_AMSTATUS) csr_val := mstatus;
				CASE(CASE_AMISA) csr_val := HEX(40000000); // 32-bit ISA
				CASE(CASE_AMIE) csr_val := mie;
				CASE(CASE_AMTVEC) csr_val := mtvec;
				CASE(CASE_AMIP) csr_val := mip;
				CASE(CASE_AMTIME) csr_val := RANGE(mcounter, 31, 0);
				CASE(CASE_AMTIMEH) csr_val := RANGE(mcounter, 63, 32);
				CASE(CASE_AMEPC) csr_val := mepc;
				CASE(CASE_AMCAUSE) csr_val := mcause;
				CASE(CASE_AMSCRATCH) csr_val := mscratch;

				DEFAULT csr_val := TO_UINT(0, LEN(csr_val));
			ENDCASE
			IF ( (do_read and not wfi_tmp) == BIT(1)) THEN // actual read requested
				PORT_BASE(mem2core_o).data_en <= BIT(1);
				PORT_BASE(mem2core_o).data <= EXT(csr_val, 32);
			ENDIF
			// Apply operator to current csr_val
			SWITCH(csr_op) // same for immediate and register: choice of op1 is made previously
				CASE(CASE_CSRRW) csr_val := op1;
				CASE(CASE_CSRRS) csr_val := csr_val or op1;
				CASE(CASE_CSRRC) csr_val := csr_val and not op1;
				CASE(CASE_CSRRWI) csr_val := op1;
				CASE(CASE_CSRRSI) csr_val := csr_val or op1;
				CASE(CASE_CSRRCI) csr_val := csr_val and not op1;
				CASE(CASE_ECALL) csr_val := op1;// ECALL-EBREAK
					mepc <= csr_val;
					IF (wfi_tmp == BIT(0)) THEN
						mcause <= EXT(addr_lsbs, 32); // CSR address is useless in this case
					ENDIF
				DEFAULT csr_val := TO_UINT(0, LEN(csr_val));
			ENDCASE
			IF ( do_write == BIT(1) ) THEN
				SWITCH(addr_lsbs)
					CASE(CASE_AMSTATUS) mstatus <= csr_val;
					CASE(CASE_AMTIMECMP)  mtimecmp <= (RANGE(mtimecmp, 63, 32) & csr_val);
					CASE(CASE_AMTIMECMPH) mtimecmp <= (csr_val & RANGE(mtimecmp, 31, 0) );
					CASE(CASE_AMIE) mie <= csr_val;
					CASE(CASE_AMTVEC) mtvec <= csr_val;
					CASE(CASE_AMIP) mip <= csr_val;
					CASE(CASE_AMEPC) mepc <= csr_val;
					CASE(CASE_AMCAUSE) mcause <= csr_val;
					CASE(CASE_AMSCRATCH) mscratch <= csr_val;
					DEFAULT csr_val := TO_UINT(0, LEN(csr_val));
				ENDCASE
			ENDIF
		ENDIF
		// Process IRQs anyway, so that IRQ pulses can last a single cycle
		IF (true) THEN// Process IRQ anyway, no matter of interference with previous mip and mcause assignments . /////No CSR r/W request -> process IRQs
			// Update pending IRQs
			mip_tmp := mip;
			mstatus_mie := B(mstatus, 3);
			IF ( mstatus_mie == BIT(1)) THEN
				IF (mcounter >= mtimecmp) THEN
					it_bit := ( BIN(0100) + EXT(priv, 4) ); // current mode timer IRQ en
					IF ( B(mie, TO_INTEGER(it_bit)) == BIT(1) ) THEN
						VAR_SET_BIT(mip_tmp, TO_INTEGER(it_bit), BIT(1));
					ENDIF
				ENDIF
				// 'platform' IRQs
				FOR(idx, 0, 15)
					IF ( (PORT_BASE(irq_i)(idx) == BIT(1)) and (B(mie, 16+idx) == BIT(1)) ) THEN
						VAR_SET_BIT(mip_tmp, (16+idx), BIT(1));
					ENDIF
				ENDLOOP
				mip <= mip_tmp;

				// Process IRQs	----
				// Return pending IRQ with highest bit index as IRQ source
				cause := TO_UINT(0, LEN(cause));
				irq_tmp := BOOL2BIT(not (mip == TO_UINT(0, 32) ) );
				PORT_BASE(csr2core_o).irq <= irq_tmp;
				FOR(idx, 0, 31)
					IF ( (B(mip, idx) == BIT(1)) ) THEN // active IRQ
						cause := TO_UINT(idx, LEN(cause));
					ENDIF
				ENDLOOP
				PORT_BASE(csr2core_o).cause <= cause;
				mcause <= EXT(cause, 32);

				// WFI
				IF ( (wfi and irq_tmp) == BIT(1) ) THEN // core is waiting for IRQ
					PORT_BASE(mem2core_o).data_en <= BIT(1);	// unlocks cpu_wait
					wfi <= BIT(0);
				ENDIF
			ENDIF
		ENDIF

		// time counter - this one can't be inhibited
		mcounter <= (mcounter + 1);



ENDIF

END_PROCESS

COMB_PROCESS(7, clk_csr_irq)

PORT_BASE(csr2core_o).mepc <= mepc;
PORT_BASE(csr2core_o).mtvec <= mtvec;
PORT_BASE(csr2core_o).priv <= priv;

END_COMB_PROCESS

BLK_END;

