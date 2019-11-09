#pragma once
// Risc-V constants. Put this in a package later on
// Immediate_formats
CONST(no_type, UINT(3)) = BIN(000);
CONST(R_type, UINT(3)) = BIN(001);
CONST(I_type, UINT(3)) = BIN(010);
CONST(S_type, UINT(3)) = BIN(011);
CONST(B_type, UINT(3)) = BIN(100);
CONST(U_type, UINT(3)) = BIN(101);
CONST(J_type, UINT(3)) = BIN(110);

// Instruction set codes
// Major opcodes ---------------------
CONST(LUI, UINT(7))    = BIN(0110111); // U-immediate
CONST(AUIPC, UINT(7))  = BIN(0010111);// U-immediate
CONST(JAL, UINT(7))    = BIN(1101111);
CONST(JALR, UINT(7))   = BIN(1100111);
CONST(BRANCH, UINT(7)) = BIN(1100011);
CONST(LOAD, UINT(7))   = BIN(0000011);
CONST(STORE, UINT(7))  = BIN(0100011);
CONST(OPI, UINT(7))    = BIN(0010011); // I-immediate
CONST(OP, UINT(7))     = BIN(0110011);

// Minor opcodes ----------------
// Branches
CONST(BEQ, UINT(3))  = BIN(000);
CONST(BNE, UINT(3))  = BIN(001);
CONST(BLT, UINT(3))  = BIN(100);
CONST(BGE, UINT(3))  = BIN(101);
CONST(BLTU, UINT(3)) = BIN(110);
CONST(BGEU, UINT(3)) = BIN(111);

// Load/store
CONST(LB, UINT(3))  = BIN(000);
CONST(LH, UINT(3))  = BIN(001);
CONST(LW, UINT(3))  = BIN(010);
CONST(LBU, UINT(3)) = BIN(100);
CONST(LHU, UINT(3)) = BIN(101);
CONST(SB, UINT(3))  = BIN(000);
CONST(SH, UINT(3))  = BIN(001);
CONST(SW, UINT(3))  = BIN(010);

// Operations w/ immediate
CONST(ADDI, UINT(3))  = BIN(000);
CONST(SLTI, UINT(3))  = BIN(010);
CONST(SLTIU, UINT(3)) = BIN(011);
CONST(XORI, UINT(3))  = BIN(100); // immediate is 5 lsbs of I-immediate
CONST(ORI, UINT(3))   = BIN(110);  // immediate is 5 lsbs of I-immediate
CONST(ANDI, UINT(3))  = BIN(111); // immediate is 5 lsbs of I-immediate
CONST(SLLI, UINT(3)) = BIN(001);
CONST(SRLI, UINT(3)) = BIN(101);
CONST(SLAI, UINT(3)) = BIN(101); // same as above ? Yes, check funct7(5)

// Register-register operations
CONST(ADDx, UINT(3))  = BIN(000);
CONST(SUBx, UINT(3))  = BIN(000); // same as above ? Yes, check funct7(5)
CONST(SLLx, UINT(3))  = BIN(001);
CONST(SLTx, UINT(3))  = BIN(010);
CONST(SLTU, UINT(3))  = BIN(011);
CONST(XORx, UINT(3))  = BIN(100);
CONST(SRLx, UINT(3))  = BIN(101);
CONST(SRAx, UINT(3))  = BIN(101); // same as above ? Yes, check funct7(5)
CONST(ORx, UINT(3))   = BIN(110);
CONST(ANDx, UINT(3))  = BIN(111);

