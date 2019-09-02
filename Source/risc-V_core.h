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
		PORT(core2instmem_o, blk2mem_t, OUT),
		PORT(instmem2core_i, mem2blk_t, IN),
		// core to data memory
		PORT(core2datamem_o, blk2mem_t, OUT),
		PORT(datamem2core_i, mem2blk_t, IN),
		// GPIOs
		PORT(gpios_o, UINT(32), OUT)
		)
);

// Risc-V constants. Put this in a package later on
// Immediate_formats
CONST(no_type, UINT(3)) := BIN(000);
CONST(R_type, UINT(3)) := BIN(001);
CONST(I_type, UINT(3)) := BIN(010);
CONST(S_type, UINT(3)) := BIN(011);
CONST(B_type, UINT(3)) := BIN(100);
CONST(U_type, UINT(3)) := BIN(101);
CONST(J_type, UINT(3)) := BIN(110);

// Instruction set codes
// Major opcodes ---------------------
CONST(LUI, UINT(7))    := BIN(0110111); // U-immediate
CONST(AUIPC, UINT(7))  := BIN(0010111);// U-immediate
CONST(JAL, UINT(7))    := BIN(1101111);
CONST(JALR, UINT(7))   := BIN(1100111);
CONST(BRANCH, UINT(7)) := BIN(1100011);
CONST(LOAD, UINT(7))   := BIN(0000011);
CONST(STORE, UINT(7))  := BIN(0100011);
CONST(OPI, UINT(7))    := BIN(0010011); // I-immediate
CONST(OP, UINT(7))     := BIN(0110011);

// Minor opcodes ----------------
// Branches
CONST(BEQ, UINT(3))  := BIN(000);
CONST(BNE, UINT(3))  := BIN(001);
CONST(BLT, UINT(3))  := BIN(100);
CONST(BGE, UINT(3))  := BIN(101);
CONST(BLTU, UINT(3)) := BIN(110);
CONST(BGEU, UINT(3)) := BIN(111);

// Load/store
CONST(LB, UINT(3))  := BIN(000);
CONST(LH, UINT(3))  := BIN(001);
CONST(LW, UINT(3))  := BIN(010);
CONST(LBU, UINT(3)) := BIN(100);
CONST(LHU, UINT(3)) := BIN(101);
CONST(SB, UINT(3))  := BIN(000);
CONST(SH, UINT(3))  := BIN(001);
CONST(SW, UINT(3))  := BIN(010);

// Operations w/ immediate
CONST(ADDI, UINT(3))  := BIN(000);
CONST(SLTI, UINT(3))  := BIN(010);
CONST(SLTIU, UINT(3)) := BIN(011);
CONST(XORI, UINT(3))  := BIN(100); // immediate is 5 lsbs of I-immediate
CONST(ORI, UINT(3))   := BIN(110);  // immediate is 5 lsbs of I-immediate
CONST(ANDI, UINT(3))  := BIN(111); // immediate is 5 lsbs of I-immediate
CONST(SLLI, UINT(3)) := BIN(001);
CONST(SRLI, UINT(3)) := BIN(101);
CONST(SLAI, UINT(3)) := BIN(101); // same as above ? Yes, check funct7(5)

// Register-register operations
CONST(ADDx, UINT(3))  := BIN(000);
CONST(SUBx, UINT(3))  := BIN(000); // same as above ? Yes, check funct7(5)
CONST(SLLx, UINT(3))  := BIN(001);
CONST(SLTx, UINT(3))  := BIN(010);
CONST(SLTU, UINT(3))  := BIN(011);
CONST(XORx, UINT(3))  := BIN(100);
CONST(SRLx, UINT(3))  := BIN(101);
CONST(SRAx, UINT(3))  := BIN(101); // same as above ? Yes, check funct7(5)
CONST(ORx, UINT(3))   := BIN(110);
CONST(ANDx, UINT(3))  := BIN(111);
// Internal registers
TYPE(reg_file_t, ARRAY_TYPE(UINT(32), 32));
SIG(regs, reg_file_t);
SIG(PC, UINT(32));
SIG(PCp, UINT(32));

SIG(alt_op, BIT_TYPE); // 1 when alternate op is required (funct7 = 0100000)
SIG(cancel, BIT_TYPE); // 1 when alternate op is required (funct7 = 0100000)
//SIG(jmp_op, BIT_TYPE); // 1 when instruction is a jump or a branch, so that instruction is fetched in second half of the cycle
SIG(rimmediate, UINT(32));
SIG(ropcode, UINT(7));
SIG(rfunct3, UINT(3));
//SIG(rfunct7, UINT(7));
SIG(rrd, UINT(5));
SIG(rrs1, UINT(5));
SIG(rrs2, UINT(5));

SIG(pipe, UINT(4));


BEGIN


PROCESS(0, clk, reset_n)
VAR(instr, UINT(32));
VAR(opcode, UINT(7));
VAR(rd, UINT(5));
VAR(rs1, UINT(5));
VAR(rs2, UINT(5));
VAR(funct3, UINT(3));
VAR(funct7, UINT(7));
//VAR(alt_op, UINT(1));
VAR(immediate, UINT(32));
VAR(immediate_type, UINT(3));
VAR(op1, UINT(32));
VAR(op2, UINT(32));
VAR(add_res, UINT(33)); // keep carry
VAR(sub_res, UINT(33));
VAR(sll_res, UINT(32));
VAR(srl_res_u, UINT(32));
VAR(srl_res_s, UINT(32));
VAR(rs2_gt_rs1_u, UINT(1));
VAR(rs2_gt_rs1_s, UINT(1));
VAR(rs2_eq_rs1, UINT(1));
VAR(and_res, UINT(32));
VAR(or_res, UINT(32));
VAR(xor_res, UINT(32));


BEGIN

	IF ( reset_n == BIT(0) ) THEN
	// reset statements
		RESET(pipe);
		RESET(PC);
		RESET(PCp);
		//RESET(is_immediate);
		cancel <= BIT(0);
		alt_op <= BIT(0);

	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
			// rising edge
			instr := PORT_BASE(instmem2core_i).data;
			pipe <= ( RANGE(pipe, HI(pipe)-1, 0) & BIT(1) );//PORT_BASE(instmem2core_i).data_en );
			PCp <= PC;
			PC <= PC + TO_UINT(4, LEN(PC));

			// Instruction decoding -------------------------------------------------------------------
			// Basic model: assume every read request is satisfied
			IF ( (B(pipe,0) = BIT(1)) and (cancel == BIT(0)) ) THEN 			//IF ( (PORT_BASE(instmem2core_i).data_en == BIT(1)) and (cancel == BIT(0)) ) THEN
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

					IF (immediate_type == J_type) THEN // precompute address offset here
						rimmediate <= immediate + PCp;
					ELSE
						rimmediate <= immediate;
					ENDIF

			ENDIF
	// Execute instruction ----------------------------------------------------------------------
			IF ( (B(pipe, 1) == BIT(1)) and (cancel == BIT(0)) ) THEN
				op1 := regs(TO_INTEGER(rrs1));
				IF ( not (ropcode == OP) ) THEN
						op2 := immediate;
				ELSE
					op2 := regs(TO_INTEGER(rrs2));
				ENDIF

				// Arithmetic operations and results
				add_res := EXT(op1, LEN(add_res)) + EXT(op2, LEN(add_res));
				sub_res := EXT(op1, LEN(add_res)) - EXT(op2, LEN(add_res));
				rs2_gt_rs1_u := RESIZE(RANGE(sub_res, 32,32),1);
				rs2_gt_rs1_s := RESIZE(RANGE(sub_res, 32, 32) xor RANGE(op1, 31,31) xor RANGE(op2, 31,31),1);

				IF (sub_res == TO_UINT(0, LEN(sub_res))) THEN
						rs2_eq_rs1 := BIN(1);
				ELSE
						rs2_eq_rs1 := BIN(0);
				ENDIF

				sll_res := SHIFT_LEFT(op1, TO_INTEGER(RESIZE(op2, 5)));
				srl_res_u := SHIFT_RIGHT(op1, TO_INTEGER(RESIZE(op2, 5)));
				srl_res_s := UNSIGNED(SHIFT_RIGHT(SIGNED(op1), TO_INTEGER(RESIZE(op2, 5))));
				and_res := op1 and op2;
				or_res := op1 or op2;
				xor_res := op1 xor op2;

				SWITCH(ropcode) // Determine type of immediate arg.
					CASE(OP)
						SWITCH(rfunct3)
							CASE(ADDx) IF (alt_op == BIT(0)) THEN regs(TO_INTEGER(rrd)) <= RESIZE(add_res, LEN(regs(0))); ELSE regs(TO_INTEGER(rrd)) <= RESIZE(sub_res, LEN(regs(0))); ENDIF
							CASE(SLLx) regs(TO_INTEGER(rrd)) <= sll_res;
							CASE(SRLx) IF (alt_op == BIT(0)) THEN regs(TO_INTEGER(rrd)) <= srl_res_u; ELSE regs(TO_INTEGER(rrd)) <= srl_res_s; ENDIF
							CASE(SLTx) IF (alt_op == BIT(1)) THEN regs(TO_INTEGER(rrd)) <= RESIZE(rs2_gt_rs1_s, LEN(regs(0))); ELSE regs(TO_INTEGER(rrd)) <= RESIZE(rs2_gt_rs1_u, LEN(regs(0))); ENDIF
							CASE(ORx)  regs(TO_INTEGER(rrd)) <= or_res;
							CASE(XORx) regs(TO_INTEGER(rrd)) <= xor_res;
							CASE(ANDx) regs(TO_INTEGER(rrd)) <= and_res;
							DEFAULT    regs(TO_INTEGER(rrd)) <= and_res;
						ENDCASE
					CASE(LOAD)
						PORT_BASE(core2datamem_o).addr <= RESIZE(add_res, LEN(PORT_BASE(core2datamem_o).addr));
						PORT_BASE(core2datamem_o).cs_n <= BIT(0);
						PORT_BASE(core2datamem_o).wr_n <= BIT(1);
						SWITCH(rfunct3)
							CASE(LB) PORT_BASE(core2datamem_o).be <= BIN(0001);
							DEFAULT PORT_BASE(core2datamem_o).be <= BIN(1111);
						ENDCASE
/*					CASE(STORE)
					CASE(LUI)
					CASE(AUIPC)
					CASE(JAL)
					CASE(JALR)
					CASE(BRANCH)*/
					DEFAULT regs(TO_INTEGER(rrd)) <= and_res;

				ENDCASE

			ENDIF
	//ELSEIF ( EVENT(clk) and (clk == BIT(0)) ) THEN
	// falling edge
			regs(0) <= TO_UINT(0, LEN(regs(0))); // ensure that x0 is 0

	ENDIF
	END_PROCESS

	// Combinational logic
	COMB_PROCESS(1, clk)
		PORT_BASE(core2instmem_o).addr <= RESIZE(PC, LEN(PORT_BASE(core2instmem_o).addr));
		PORT_BASE(core2instmem_o).cs_n <= BIT(0);
		PORT_BASE(core2instmem_o).wr_n <= BIT(1);
		PORT_BASE(core2instmem_o).be <= BIN(1111);

	END_COMB_PROCESS

BLK_END;
