// Giorno Risc-V core september, 2019
// ----------------------------------
// This is a simple, 3-stage pipeline implementation of RISC-V rv32i instruction set
// Written using the Agravic simulation and rtl generation platform

#include "slv.h"
START_OF_FILE(risc-V_core)
INCLUDES
USE_PACKAGE(structures)
USE_PACKAGE(risc_V_constants)

ENTITY(risc_V_core,
DECL_PORTS(
		PORT(clk_core, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		// core to instruction memory
		PORT(load_port_i, blk2mem_t, IN),
		PORT(core2instmem_o, blk2mem_t, OUT),
		PORT(instmem2core_i, mem2blk_t, IN),
		// core to data memory
		PORT(core2datamem_o, blk2mem_t, OUT),
		PORT(datamem2core_i, mem2blk_t, IN)
		// GPIOs
		//PORT(gpios_o, UINT(32), OUT)
		)
);

// Internal registers
TYPE(reg_file_t, ARRAY_TYPE(UINT(32), 32));
SIG(regs, reg_file_t);
SIG(PC, UINT(32));
SIG(PCp, UINT(32));

SIG(loading, BIT_TYPE); // 1 when loading code
SIG(rtrap, BIT_TYPE); // 1 when loading code
SIG(alt_op, BIT_TYPE); // 1 when alternate op is required (funct7 = 0100000)
SIG(cpu_wait, BIT_TYPE); // 1 halts pipeline
SIG(flush, BIT_TYPE); // 1 flushes instruction in decode / execute stages
//SIG(jmp_op, BIT_TYPE); // 1 when instruction is a jump or a branch, so that instruction is fetched in second half of the cycle
SIG(rimmediate, UINT(32));
SIG(rimmediate_type, UINT(3));
SIG(use_immediate, BIT_TYPE);
SIG(raw_opcode, UINT(7));
SIG(ropcode, UINT(7));
SIG(rropcode, UINT(7));
SIG(rfunct3, UINT(3));
SIG(rop1, UINT(32));
SIG(rop2, UINT(32));
SIG(rfunct7, UINT(7));
SIG(rrd, UINT(5));
SIG(rrs1, UINT(5));
SIG(rrs2, UINT(5));
SIG(rrd_val, UINT(32)); // debug
SIG(rtaken, UINT(1)); // debug

SIG(exec, BIT_TYPE); // 1 when executing instruction
SIG(pipe, UINT(4));
SIG(rwb, UINT(5)); //writeback register
SIG(funct3wb, UINT(3));//writeback method
SIG(rshiftwb, UINT(2)); //load value shift (in bytes)

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
SIG(blk2mem_t0, blk2mem_t);
SIG(mem2blk_t0, mem2blk_t);
SIG(rinstr, UINT(32)); // debug
SIG(rrinstr, UINT(32)); // debug

SIG(radd_res, UINT(33)); // debug
SIG(rsub_res, UINT(33));// debug
SIG(rsll_res, UINT(32));// debug
SIG(rsrl_res_u, UINT(32));// debug
SIG(rsrl_res_s, UINT(32));// debug
SIG(rrs1_lt_rs2_u, UINT(1));// debug
SIG(rrs1_lt_rs2_s, UINT(1));// debug
SIG(rrs1_eq_rs2, UINT(1));// debug
SIG(rand_res, UINT(32));// debug
SIG(ror_res, UINT(32));// debug
SIG(rxor_res, UINT(32));// debug
SIG(rjalr, UINT(5));// debug

BEGIN


PROCESS(0, clk_core, reset_n)
//std::cerr << '<';
VAR(instr, UINT(32));
VAR(opcode, UINT(7));
VAR(rd, UINT(5));
VAR(wrd, UINT(5));
VAR(rs1, UINT(5));
VAR(rs2, UINT(5));
VAR(funct3, UINT(3));
VAR(funct7, UINT(7));
VAR(immediate, UINT(32));
VAR(immediate_type, UINT(3));
VAR(op1, UINT(32));
VAR(op2, UINT(32));
VAR(add_res, UINT(33)); // keep carry
VAR(sub_res, UINT(33)); // keep carry
VAR(sll_res, UINT(32));
VAR(srl_res_u, UINT(32));
VAR(srl_res_s, UINT(32));
VAR(rs1_lt_rs2_u, UINT(1));
VAR(rs1_lt_rs2_s, UINT(1));
VAR(rs1_eq_rs2, UINT(1));
VAR(and_res, UINT(32));
VAR(or_res, UINT(32));
VAR(xor_res, UINT(32));
VAR(ld_data, UINT(32));
VAR(nshift, UINT(2));
VAR(wbe, UINT(4)); // write byte enable
VAR(taken, UINT(1)); // condition for branch taken
VAR(rd_val, UINT(32));
VAR(trap, BIT_TYPE); // going to trap
VAR(cause, UINT(32)); // trap cause
VAR(trap_addr_base, UINT(32)); // base trap addr
VAR(trap_addr_offset, UINT(32)); // trap offset
VAR(csr_val, UINT(32)); // CSR read value
VAR(csr_addr, UINT(12)); // CSR addr value
VAR(csr_wb, UINT(32)); // CSR writeback value


BEGIN

	IF ( reset_n == BIT(0) ) THEN
	// reset statements
		RESET(pipe);
		RESET(PC);
		RESET(PCp);
		cpu_wait <= BIT(0);
		flush <= BIT(0);
		alt_op <= BIT(0);
		use_immediate <= BIT(0);
		csri <= BIT(0);
		loading <= BIT(0);
		RESET(mcounter);
		RESET(mstatus);
		RESET(mcause);
		RESET(medeleg);
		RESET(mideleg);
		RESET(mtvec);
		RESET(mepc);
		RESET(mip);
		RESET(mie);
		RESET(mscratch);
		RESET(rinstr);
		RESET(rrinstr);
		RESET(ropcode);
		RESET(rfunct3);
		rtrap <= BIT(0);
		priv <= BIN(11); // Machine mode is default

		ELSEIF ( EVENT(clk_core) and (clk_core == BIT(1)) ) THEN
			// rising edge
			flush <= BIT(0);
			instr := PORT_BASE(instmem2core_i).data;


			// Instruction decoding -------------------------------------------------------------------
			IF ( (B(pipe,0) == BIT(1)) and (cpu_wait == BIT(0)) and (flush == BIT(0)) ) THEN 			//IF ( (PORT_BASE(instmem2core_i).data_en == BIT(1)) and (cpu_wait == BIT(0)) ) THEN
					rinstr <= instr;
					opcode := RANGE(instr, 6, 0);
					//gprintf("#MOPCODE %",opcode);
					rd := RANGE(instr, 11, 7);
					rs1 := RANGE(instr, 19, 15);
					rs2 := RANGE(instr, 24, 20);
					funct3 := RANGE(instr, 14, 12);
					funct7 := RANGE(instr, 31, 25);

					// Anyway, difference between OP and OPI is held in immediate choice, made one cycle later
					IF (opcode == OPI) THEN
						ropcode <= OP;
						alt_op <= BIT(0);
					ELSE
						ropcode <= opcode;
						alt_op <= B(instr, 30);
					ENDIF

					raw_opcode <= opcode;
					rfunct3 <= funct3;
					rfunct7 <= funct7;
					rrs1 <= rs1;
					rrs2 <= rs2;

					//Decode immediate value
					SWITCH(opcode) // Determine type of immediate arg.
						CASE(CASE_SYS) immediate_type := I_type;
						CASE(CASE_MEM) immediate_type := I_type;
						CASE(CASE_OPI) immediate_type := I_type;
						CASE(CASE_LOAD) immediate_type := I_type;
						CASE(CASE_STORE) immediate_type := S_type;
						CASE(CASE_LUI) immediate_type := U_type;
						CASE(CASE_AUIPC) immediate_type := U_type;
						CASE(CASE_JAL) immediate_type := J_type;
						CASE(CASE_JALR) immediate_type := I_type;
						CASE(CASE_BRANCH) immediate_type := B_type;
						DEFAULT immediate_type := no_type;
					ENDCASE
					rimmediate_type <= immediate_type;

					// Decode immediate argument. !! USE SLV_RANGE instead of RANGE (returns slv and not unsigned) to generate slices, otherwise a & b does not compile in vhdl
					SWITCH(immediate_type)
						CASE(CASE_I_type) immediate := SXT( RANGE(instr, 31, 20), LEN(immediate) );
						CASE(CASE_S_type) immediate := SXT( SLV_RANGE(instr, 31, 25) & SLV_RANGE(instr, 11, 7), LEN(immediate));
						CASE(CASE_B_type) immediate := SXT( SLV_RANGE(instr, 31, 31) & SLV_RANGE(instr, 7, 7) & SLV_RANGE(instr, 30,25) & SLV_RANGE(instr,11,8) & BIT(0), LEN(immediate));
						CASE(CASE_U_type) immediate := SXT( SLV_RANGE(instr, 31, 20) & SLV_RANGE(instr,19,12) & BIN(000000000000), LEN(immediate));
						CASE(CASE_J_type) immediate := SXT( SLV_RANGE(instr, 31, 31) & SLV_RANGE(instr,19,12) & SLV_RANGE(instr, 20,20) & SLV_RANGE(instr,30,21) & BIN(0), LEN(immediate));
						DEFAULT immediate := TO_UINT(0, LEN(immediate));
					ENDCASE

					// Set destination register when available / no one when load/store
					IF ( (immediate_type == B_type) or (immediate_type == S_type) ) THEN
						rrd <= BIN(00000);
					ELSE
						rrd <= rd;
					ENDIF

					IF (immediate_type == no_type) THEN
						use_immediate <= BIT(0);
					ELSEIF ( (immediate_type == J_type) or (immediate_type == B_type) or (opcode == AUIPC) ) THEN // precompute address offset here
						rimmediate <= immediate + PCp;
						use_immediate <= BIT(0); // immediate not for use for calculations
					ELSE
						use_immediate <= BIT(1);
						rimmediate <= immediate;
					ENDIF

					// So that op1 is decoded as rs1 and not regs(rs1)
					IF ( (opcode == SYS) and (B(funct3, 2) == BIT(1)) ) THEN
						csri <= BIT(1);
					ELSE
						csri <= BIT(0);
					ENDIF
			ENDIF

			// Basic code memory model: assume every read request is satisfied
			IF ( ( (cpu_wait == BIT(0) ) and not (ropcode == LOAD) ) or (PORT_BASE(datamem2core_i).data_en == BIT(1))) THEN
				pipe <= ( RANGE(pipe, HI(pipe)-1, 0) & BIT(1) );
				PCp <= PC;
				PC <= PC + TO_UINT(4, LEN(PC));
			ENDIF

			// Execute instruction ----------------------------------------------------------------------
			IF ( (B(pipe, 1) == BIT(1)) and (cpu_wait == BIT(0)) and (flush == BIT(0)) ) THEN
				exec <= BIT(1);
					rrinstr <= rinstr;
				IF (csri == BIT(1)) THEN// CSR instruction with immediate arg.
					op1 := RESIZE(rrs1, LEN(op1));
				ELSE
					op1 := regs(TO_INTEGER(rrs1));
				ENDIF
				IF ( use_immediate == BIT(1) ) THEN
						op2 := rimmediate;
				ELSE
					op2 := regs(TO_INTEGER(rrs2));
				ENDIF
				rop1 <= op1;
				rop2 <= op2;

				// Arithmetic operations and results
				add_res := EXT(op1, LEN(add_res)) + EXT(op2, LEN(add_res));
				sub_res := EXT(op1, LEN(add_res)) - EXT(op2, LEN(add_res));
				rs1_lt_rs2_u := RESIZE(RANGE(sub_res, 32,32),1);
				rs1_lt_rs2_s := RESIZE(RANGE(sub_res, 32, 32) xor RANGE(op1, 31,31) xor RANGE(op2, 31,31),1);

				IF (sub_res == TO_UINT(0, LEN(sub_res))) THEN
						rs1_eq_rs2 := BIN(1);
				ELSE
						rs1_eq_rs2 := BIN(0);
				ENDIF

				sll_res := SHIFT_LEFT(op1, TO_INTEGER(RESIZE(op2, 5)));
				srl_res_u := SHIFT_RIGHT(op1, TO_INTEGER(RESIZE(op2, 5)));
				srl_res_s := UNSIGNED(SHIFT_RIGHT(SIGNED(op1), TO_INTEGER(RESIZE(op2, 5))));
				and_res := op1 and op2;
				or_res := op1 or op2;
				xor_res := op1 xor op2;
				nshift := RESIZE(add_res, LEN(nshift)); // shift data for LH/LB/SH/SB
				trap := BIT(0);
				taken := BIN(0);

				radd_res <= add_res;
				rsub_res <= sub_res;
				rrs1_lt_rs2_u <= rs1_lt_rs2_u;
				rrs1_lt_rs2_s <= rs1_lt_rs2_s;

				rwb <= BIN(00000);
				rd_val := BIN(10101010010101011010101001010101); // default
				rjalr <= BIN(00000);

				SWITCH(ropcode) // Execute opcode
					CASE(CASE_SYS) // CSR R/W
						SWITCH(RANGE(rimmediate,11,0))
							CASE(CASE_AMSTATUS) csr_val := mstatus;
							//CASE(AMISA) csr_val := misa;
							CASE(CASE_AMEDELEG) csr_val := medeleg;
							CASE(CASE_AMIDELEG) csr_val := mideleg;
							CASE(CASE_AMIE) csr_val := mie;
							CASE(CASE_AMTVEC) csr_val := mtvec;
							//CASE(AMCOUNTEREN) csr_val := mtcounteren;
							CASE(CASE_AMIP) csr_val := mip;
							CASE(CASE_AMSCRATCH) csr_val := mscratch;
							CASE(CASE_AMEPC) csr_val := mepc;
							CASE(CASE_AMCAUSE) csr_val := mcause;
							DEFAULT csr_val := TO_UINT(0, LEN(csr_val));
						ENDCASE
						SWITCH(rfunct3) // same for immediate and register: choice of op1 is made previously
							CASE(CASE_CSRRW) csr_val := op1;
							CASE(CASE_CSRRS) csr_val := csr_val or op1;
							CASE(CASE_CSRRC) csr_val := csr_val and not op1;
							CASE(CASE_CSRRWI) csr_val := op1;
							CASE(CASE_CSRRSI) csr_val := csr_val or op1;
							CASE(CASE_CSRRCI) csr_val := csr_val and not op1;
							CASE(CASE_ECALL) PC <= mepc; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe)); // TODO: Change mepc to appropriate register when not in machine mode
							DEFAULT csr_val := TO_UINT(0, LEN(csr_val));
						ENDCASE
						SWITCH(RANGE(rimmediate,11,0))
							CASE(CASE_AMSTATUS) mstatus <= csr_val;
							CASE(CASE_AMEDELEG) medeleg <= csr_val;
							CASE(CASE_AMIDELEG) mideleg <= csr_val;
							CASE(CASE_AMIE) mie <= csr_val;
							CASE(CASE_AMTVEC) mtvec <= csr_val;
							//CASE(AMCOUNTEREN) csr_val := mtcounteren;
							CASE(CASE_AMIP) mip <= csr_val;
							CASE(CASE_AMSCRATCH) mscratch <= csr_val;
							CASE(CASE_AMEPC) mepc <= csr_val;
							CASE(CASE_AMCAUSE) mcause <= csr_val;
							DEFAULT csr_val := csr_val ;// TODO trap := BIT(1); cause := ILLINSTR;
						ENDCASE
						rd_val := csr_val;
					CASE(CASE_MEM)
					CASE(CASE_OP)
						SWITCH(rfunct3)
							CASE(CASE_ADDx) IF (alt_op == BIT(0)) THEN rd_val := RESIZE(add_res, 32); ELSE rd_val := RESIZE(sub_res, 32); ENDIF
							CASE(CASE_SLLx) rd_val := sll_res;
							CASE(CASE_SRLx) IF (alt_op == BIT(0)) THEN rd_val := srl_res_u; ELSE rd_val := srl_res_s; ENDIF
							CASE(CASE_SLTx) IF (alt_op == BIT(1)) THEN rd_val := RESIZE(rs1_lt_rs2_s, 32); ELSE rd_val := RESIZE(rs1_lt_rs2_u, 32); ENDIF
							CASE(CASE_ORx)  rd_val := or_res;
							CASE(CASE_XORx) rd_val := xor_res;
							CASE(CASE_ANDx) rd_val := and_res;
							DEFAULT    trap := BIT(1); cause := ILLINSTR;
						ENDCASE
					CASE(CASE_LOAD)
						cpu_wait <= BIT(1); // stall the pipeline
						rwb <= rrd; // Store writeback register and associated method
						funct3wb <= rfunct3;
						rshiftwb <= nshift;
						PORT_BASE(core2datamem_o).addr <= RANGE(add_res, LEN(blk2mem_t0.addr)+1, 2);//RESIZE(add_res, LEN(PORT_BASE(core2datamem_o).addr));
						PORT_BASE(core2datamem_o).cs_n <= BIT(0);
						PORT_BASE(core2datamem_o).wr_n <= BIT(1);
						PORT_BASE(core2datamem_o).be <= BIN(1111);
					CASE(CASE_STORE)
						//cpu_wait <= BIT(1); // stall the pipeline ?
						PORT_BASE(core2datamem_o).addr <= RANGE(add_res, LEN(blk2mem_t0.addr)+1, 2);//RESIZE(add_res, LEN(PORT_BASE(core2datamem_o).addr));
						PORT_BASE(core2datamem_o).cs_n <= BIT(0);
						PORT_BASE(core2datamem_o).wr_n <= BIT(0);
						wbe := ( B(rfunct3, 1) & B(rfunct3, 1) & (B(rfunct3, 0) or B(rfunct3, 1)) & BIT(1) ); // use minor opcode bits for wr. byte enable
						wbe := SHIFT_LEFT(wbe, TO_INTEGER(nshift));
						PORT_BASE(core2datamem_o).be <= wbe;
						PORT_BASE(core2datamem_o).data <= SHIFT_LEFT(regs(TO_INTEGER(rrs2)), TO_INTEGER(nshift & BIN(000)));
					CASE(CASE_LUI) rd_val := rimmediate;
					CASE(CASE_AUIPC) rd_val := rimmediate; // Pc added at decoding stage
					CASE(CASE_JAL) PC <= rimmediate; rd_val := PCp; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe));
					CASE(CASE_JALR) PC <= RESIZE( ( RANGE(add_res, 31,1) & BIN(0) ), LEN(PC)); // It seems that lsb could be used for stg else: clear lsb
						rd_val := PCp; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe)); rjalr <= rrd;
					CASE(CASE_BRANCH)
						taken := ( RANGE(rfunct3, 1,1) and (rs1_lt_rs2_u xor RANGE(rfunct3, 0, 0)) ) or // LT / GTE U
								 ( (RANGE(rfunct3, 1,1) xor RANGE(rfunct3,2,2) ) and (rs1_lt_rs2_s xor RANGE(rfunct3, 0,0)) ) or // LT / GTE S
								 ( not (RANGE(rfunct3, 1,1) and not RANGE(rfunct3,2,2) ) and (rs1_eq_rs2 xor RANGE(rfunct3, 0,0)) ); // BNE BEQ
						IF (taken == BIN(1)) THEN
							PC <= rimmediate; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe));
						ENDIF
					DEFAULT trap := BIT(1); cause := ILLINSTR;

				ENDCASE
				rtaken <= taken;
				rrd_val <= rd_val;

				IF ( not (ropcode == LOAD) and not (ropcode == STORE) ) THEN
					PORT_BASE(core2datamem_o).cs_n <= BIT(1);
				ENDIF
				wrd := rrd;
			ELSE
				PORT_BASE(core2datamem_o).cs_n <= BIT(1);
				exec <= BIT(0);
				rd_val := BIN(10101010010101011010101001010101);
			ENDIF

			// Register writeback (load) -----------------------------------------------------------------
			IF ( not(rwb == TO_UINT(0, LEN(rwb))) and (PORT_BASE(datamem2core_i).data_en == BIT(1)) ) THEN
				cpu_wait <= BIT(0);
				ld_data := PORT_BASE(datamem2core_i).data;
				ld_data := SHIFT_RIGHT(ld_data, TO_INTEGER( (rshiftwb & BIN(000))));
				SWITCH(funct3wb)
					CASE(CASE_LW) rd_val := ld_data;
					CASE(CASE_LH) rd_val := SXT(RANGE(ld_data, 15, 0), LEN(rd_val));
					CASE(CASE_LHU) rd_val := EXT(RANGE(ld_data, 15, 0), LEN(rd_val));
					CASE(CASE_LB) rd_val := SXT(RANGE(ld_data, 7, 0), LEN(rd_val));
					CASE(CASE_LBU) rd_val := EXT(RANGE(ld_data, 7, 0), LEN(rd_val));
					DEFAULT rd_val := TO_UINT(0, LEN(rd_val));
				ENDCASE
				wrd := rwb;
			ENDIF

			regs(TO_INTEGER(wrd)) <= rd_val;
			regs(0) <= TO_UINT(0, 32); // ensure that x0 is 0

			// Loading code from load_port
			IF (PORT_BASE(load_port_i).wr_n == BIT(0) ) THEN // loading code
				gprintf("#RLOADING CODE");
				loading <= BIT(1);
				PC <= RESIZE( PORT_BASE(load_port_i).addr, LEN(PC) );
				PCp <= PORT_BASE(load_port_i).data; // reuse these regs
			ELSEIF ( loading == BIT(1) ) THEN
				gprintf("#RCODE LOADED, starting Giorno core");
				PC <= TO_UINT(0, LEN(PC));
				loading <= BIT(0);
				flush <= BIT(1);
				pipe <= TO_UINT(0, LEN(pipe));
			ENDIF

			rtrap <= trap;
			// TRAP - try to optimize that later
			IF ((not (loading == BIT(1))) and (trap == BIT(1))) THEN
				trap_addr_base := mtvec and BIN(11111111111111111111111111111100);
				IF (RANGE(mtvec, 1, 0) == BIN(01) ) THEN
					trap_addr_offset := RANGE(cause, 31, 2) & BIN(00);
				ELSE
					trap_addr_offset := TO_UINT(0, LEN(trap_addr_offset));
				ENDIF
				PC <= trap_addr_base + trap_addr_offset;
				mcause <= cause;
			ENDIF

	ENDIF
	END_PROCESS

	// Combinational logic
	COMB_PROCESS(1, clk_core)
		PORT_BASE(core2instmem_o).addr <= RANGE(PC, LEN(blk2mem_t0.addr)+1, 2);//RESIZE(PC, LEN(PORT_BASE(core2instmem_o).addr));
		PORT_BASE(core2instmem_o).data <= PCp;
		PORT_BASE(core2instmem_o).cs_n <= BIT(0);
		PORT_BASE(core2instmem_o).wr_n <= not loading;
		PORT_BASE(core2instmem_o).be <= BIN(1111);
	END_COMB_PROCESS

BLK_END;
