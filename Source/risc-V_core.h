//#include "macros.h"
///#include "gmodule.h"
//#include "ports.h"
#include "slv.h"
//#include "slv.h"
START_OF_FILE(risc-V_core)
INCLUDES
USE_PACKAGE(structures)

ENTITY(risc_V_core,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
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
#include "risc_V_constants.h"
// Internal registers
TYPE(reg_file_t, ARRAY_TYPE(UINT(32), 32));
SIG(regs, reg_file_t);
SIG(PC, UINT(32));
SIG(PCp, UINT(32));

SIG(loading, BIT_TYPE); // 1 when loading code
SIG(alt_op, BIT_TYPE); // 1 when alternate op is required (funct7 = 0100000)
SIG(cpu_wait, BIT_TYPE); // 1 halts pipeline
SIG(flush, BIT_TYPE); // 1 flushes instruction in decode / execute stages
//SIG(jmp_op, BIT_TYPE); // 1 when instruction is a jump or a branch, so that instruction is fetched in second half of the cycle
SIG(rimmediate, UINT(32));
SIG(use_immediate, BIT_TYPE);
SIG(ropcode, UINT(7));
SIG(rfunct3, UINT(3));
//SIG(rfunct7, UINT(7));
SIG(rrd, UINT(5));
SIG(rrs1, UINT(5));
SIG(rrs2, UINT(5));

SIG(pipe, UINT(4));
SIG(rwb, UINT(5)); //writeback register
SIG(funct3wb, UINT(3));//writeback method
SIG(rshiftwb, UINT(2)); //load value shift (in bytes)


BEGIN


PROCESS(0, clk, reset_n)
VAR(instr, UINT(32));
VAR(opcode, UINT(7));
VAR(rd, UINT(5));
VAR(wrd, UINT(5));
VAR(rs1, UINT(5));
VAR(rs2, UINT(5));
VAR(funct3, UINT(3));
VAR(funct7, UINT(7));
//VAR(alt_op, UINT(1));
//VAR(use_immediate, UINT(1));
VAR(immediate, UINT(32));
VAR(immediate_type, UINT(3));
VAR(op1, UINT(32));
VAR(op2, UINT(32));
VAR(add_res, UINT(33)); // keep carry
VAR(sub_res, UINT(33));
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


BEGIN

	IF ( reset_n == BIT(0) ) THEN
	// reset statements
		RESET(pipe);
		RESET(PC);
		RESET(PCp);
		//RESET(is_immediate);
		cpu_wait <= BIT(0);
		flush <= BIT(0);
		alt_op <= BIT(0);
		use_immediate <= BIT(0);

	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
			// rising edge
			flush <= BIT(0);
			instr := PORT_BASE(instmem2core_i).data;
			IF ( cpu_wait == BIT(0) ) THEN
					pipe <= ( RANGE(pipe, HI(pipe)-1, 0) & BIT(1) );//PORT_BASE(instmem2core_i).data_en );
				PCp <= PC;
				PC <= PC + TO_UINT(4, LEN(PC));
			ENDIF

			// Instruction decoding -------------------------------------------------------------------
			// Basic model: assume every read request is satisfied
			IF ( (B(pipe,0) = BIT(1)) and (cpu_wait == BIT(0)) and (flush == BIT(0)) ) THEN 			//IF ( (PORT_BASE(instmem2core_i).data_en == BIT(1)) and (cpu_wait == BIT(0)) ) THEN
					opcode := RANGE(instr, 6, 0);
					rd := RANGE(instr, 11, 7);
					rs1 := RANGE(instr, 19, 15);
					rs2 := RANGE(instr, 24, 20);
					funct3 := RANGE(instr, 14, 12);
					funct7 := RANGE(instr, 31, 25);

					// Anyway, difference between OP and OPI is held in immediate choice, made one cycle later
					IF (opcode == OPI) THEN
						ropcode <= OP;
					ELSE
						ropcode <= opcode;
					ENDIF

					rfunct3 <= funct3;
					alt_op <= B(instr, 30);
					rrd <= rd;
					rrs1 <= rs1;
					rrs2 <= rs2;

					//Decode immediate value
					SWITCH(opcode) // Determine type of immediate arg.
						CASE(OPI) immediate_type := I_type;
						CASE(LOAD) immediate_type := I_type;
						CASE(STORE) immediate_type := S_type;
						CASE(LUI) immediate_type := U_type;
						CASE(AUIPC) immediate_type := U_type;
						CASE(JAL) immediate_type := J_type;
						CASE(JALR) immediate_type := J_type;
						CASE(BRANCH) immediate_type := B_type;
						DEFAULT immediate_type := no_type;
					ENDCASE

					// Decode immediate argument. !! USE SLV_RANGE instead of RANGE (returns slv and not unsigned) to generate slices, otherwise cat does not compile in vhdl
					SWITCH(immediate_type)
						CASE(I_type) immediate := SXT( RANGE(instr, 31, 20), LEN(immediate) );
						CASE(S_type) immediate := SXT( SLV_RANGE(instr, 31, 20) & SLV_RANGE(instr, 11, 7), LEN(immediate));
						CASE(B_type) immediate := SXT( SLV_RANGE(instr, 31, 31) & SLV_RANGE(instr, 7, 7) & SLV_RANGE(instr, 30,25) & SLV_RANGE(instr,11,8) & BIT(0), LEN(immediate));
						CASE(U_type) immediate := SXT( SLV_RANGE(instr, 31, 20) & SLV_RANGE(instr,19,12) & BIN(000000000000), LEN(immediate));
						CASE(J_type) immediate := SXT( SLV_RANGE(instr, 31, 31) & SLV_RANGE(instr,19,12) & SLV_RANGE(instr, 20,20) & SLV_RANGE(instr,30,21) & BIN(0), LEN(immediate));
						DEFAULT immediate := TO_UINT(0, LEN(immediate));
					ENDCASE

					// Set data load before instruction exec.
					IF (opcode == LOAD) THEN

					ENDIF

					IF ( (immediate_type == J_type) or (opcode == AUIPC) ) THEN // precompute address offset here
						rimmediate <= immediate + PCp;
					ELSE
						rimmediate <= immediate;
					ENDIF

					IF (immediate_type == no_type) THEN
						use_immediate <= BIT(1);
					ELSE
						use_immediate <= BIT(0);
					ENDIF


			ENDIF
	// Execute instruction ----------------------------------------------------------------------
			IF ( (B(pipe, 1) == BIT(1)) and (cpu_wait == BIT(0)) and (flush == BIT(0)) ) THEN
				op1 := regs(TO_INTEGER(rrs1));
				IF ( use_immediate == BIT(1) ) THEN
						op2 := rimmediate;
				ELSE
					op2 := regs(TO_INTEGER(rrs2));
				ENDIF

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

				SWITCH(ropcode) // Determine type of immediate arg.
					CASE(OP)
						SWITCH(rfunct3)
							CASE(ADDx) IF (alt_op == BIT(0)) THEN rd_val := RESIZE(add_res, LEN(regs(0))); ELSE rd_val := RESIZE(sub_res, LEN(regs(0))); ENDIF
							CASE(SLLx) rd_val := sll_res;
							CASE(SRLx) IF (alt_op == BIT(0)) THEN rd_val := srl_res_u; ELSE rd_val := srl_res_s; ENDIF
							CASE(SLTx) IF (alt_op == BIT(1)) THEN rd_val := RESIZE(rs1_lt_rs2_s, LEN(regs(0))); ELSE rd_val := RESIZE(rs1_lt_rs2_u, LEN(regs(0))); ENDIF
							CASE(ORx)  rd_val := or_res;
							CASE(XORx) rd_val := xor_res;
							CASE(ANDx) rd_val := and_res;
							DEFAULT    rd_val := and_res;
						ENDCASE
					CASE(LOAD)
						cpu_wait <= BIT(1); // stall the pipeline
						rwb <= rrd; // Store writeback register and associated method
						funct3wb <= rfunct3;
						rshiftwb <= nshift;
						PORT_BASE(core2datamem_o).addr <= RANGE(PC, LEN(PORT_BASE(core2datamem_o).addr)+1, 2);//RESIZE(add_res, LEN(PORT_BASE(core2datamem_o).addr));
						PORT_BASE(core2datamem_o).cs_n <= BIT(0);
						PORT_BASE(core2datamem_o).wr_n <= BIT(1);
						PORT_BASE(core2datamem_o).be <= BIN(1111);
					CASE(STORE)
						//cpu_wait <= BIT(1); // stall the pipeline ?
						PORT_BASE(core2datamem_o).addr <= RANGE(PC, LEN(PORT_BASE(core2datamem_o).addr)+1, 2);//RESIZE(add_res, LEN(PORT_BASE(core2datamem_o).addr));
						PORT_BASE(core2datamem_o).cs_n <= BIT(0);
						PORT_BASE(core2datamem_o).wr_n <= BIT(0);
						wbe := ( B(rfunct3, 1) & B(rfunct3, 1) & B(rfunct3, 0) & BIT(1) ); // use minor opcode bits for wr. byte enable
						wbe := SHIFT_LEFT(wbe, TO_INTEGER(nshift));
						PORT_BASE(core2datamem_o).be <= wbe;
						PORT_BASE(core2datamem_o).data <= SHIFT_LEFT(regs(TO_INTEGER(rrs2)), TO_INTEGER(nshift & BIN(000)));
					CASE(LUI) rd_val := rimmediate;
					CASE(AUIPC) rd_val := rimmediate; // Pc added at decoding stage
					CASE(JAL) PC <= rimmediate; rd_val := PC; cpu_wait <= BIT(1);
					CASE(JALR) PC <= RESIZE(add_res, LEN(PC)); rd_val := PC; cpu_wait <= BIT(1);
					CASE(BRANCH)
						taken := ( RANGE(rfunct3, 1,1) and (rs1_lt_rs2_u xor RANGE(rfunct3, 0, 0)) ) and // LT / GTE U
								 ( (RANGE(rfunct3, 1,1) xor RANGE(rfunct3,2,2) ) and (rs1_lt_rs2_u xor RANGE(rfunct3, 0,0)) ) and // LT / GTE S
								 ( not (RANGE(rfunct3, 1,1) and not RANGE(rfunct3,2,2) ) and (rs1_eq_rs2 xor RANGE(rfunct3, 0,0)) ); // BNE BEQ
						IF (taken == BIN(1)) THEN
							PC <= rimmediate; cpu_wait <= BIT(1);
						ENDIF
					DEFAULT rd_val := and_res;

				ENDCASE

				IF ( not (ropcode == LOAD) and not (ropcode == STORE) ) THEN
					PORT_BASE(core2datamem_o).cs_n <= BIT(1);
				ENDIF
				wrd := rrd;
			ENDIF

			// Register writeback (load) -----------------------------------------------------------------
			IF ( not(rwb == TO_UINT(0, LEN(rwb))) and (PORT_BASE(datamem2core_i).data_en == BIT(1)) ) THEN
				cpu_wait <= BIT(0);
				ld_data := PORT_BASE(datamem2core_i).data;
				ld_data := SHIFT_RIGHT(ld_data, TO_INTEGER( (rshiftwb & BIN(000))));
				SWITCH(funct3wb)
					CASE(LW) rd_val := ld_data;
					CASE(LH) rd_val := SXT(RANGE(ld_data, 15, 0), LEN(rd_val));
					CASE(LHU) rd_val := EXT(RANGE(ld_data, 15, 0), LEN(rd_val));
					CASE(LB) rd_val := SXT(RANGE(ld_data, 7, 0), LEN(rd_val));
					CASE(LBU) rd_val := EXT(RANGE(ld_data, 7, 0), LEN(rd_val));
					DEFAULT rd_val := TO_UINT(0, LEN(rd_val));
				ENDCASE
				wrd := rwb;
			ENDIF
	//ELSEIF ( EVENT(clk) and (clk == BIT(0)) ) THEN
	// falling edge
			regs(TO_INTEGER(wrd)) <= rd_val;
			regs(0) <= TO_UINT(0, LEN(regs(0))); // ensure that x0 is 0

			// Loading code from load_port
			IF (PORT_BASE(load_port_i).wr_n == BIT(0) ) THEN // loading code
				loading <= BIT(1);
				PC <= RESIZE( PORT_BASE(load_port_i).addr, LEN(PC) );
				PCp <= PORT_BASE(load_port_i).data; // reuse these regs
			ELSEIF ( loading == BIT(1) ) THEN
				PC <= TO_UINT(0, LEN(PC));
				loading <= BIT(0);
			ENDIF


	ENDIF
	END_PROCESS

	// Combinational logic
	COMB_PROCESS(1, clk)
		PORT_BASE(core2instmem_o).addr <= RANGE(PC, LEN(PORT_BASE(core2instmem_o).addr)+1, 2);//RESIZE(PC, LEN(PORT_BASE(core2instmem_o).addr));
		PORT_BASE(core2instmem_o).data <= PCp;
		PORT_BASE(core2instmem_o).cs_n <= BIT(0);
		PORT_BASE(core2instmem_o).wr_n <= not loading;
		PORT_BASE(core2instmem_o).be <= BIN(1111);

	END_COMB_PROCESS

BLK_END;
