//#pragma once
// Risc-V constants. Put this in a package later on
#include "slv.h"

START_OF_FILE(risc_V_constants)
INCLUDES

PACKAGE(risc_V_constants)

// Immediate_formats
CONST(no_type, UINT(3)) := BIN(000);
CONST(R_type, UINT(3)) := BIN(001);
CONST(I_type, UINT(3)) := BIN(010);
CONST(S_type, UINT(3)) := BIN(011);
CONST(B_type, UINT(3)) := BIN(100);
CONST(U_type, UINT(3)) := BIN(101);
CONST(J_type, UINT(3)) := BIN(110);
CASE_CONST(CASE_no_type, UINT(3)) := CASE_BIN(000);
CASE_CONST(CASE_R_type, UINT(3)) := CASE_BIN(001);
CASE_CONST(CASE_I_type, UINT(3)) := CASE_BIN(010);
CASE_CONST(CASE_S_type, UINT(3)) := CASE_BIN(011);
CASE_CONST(CASE_B_type, UINT(3)) := CASE_BIN(100);
CASE_CONST(CASE_U_type, UINT(3)) := CASE_BIN(101);
CASE_CONST(CASE_J_type, UINT(3)) := CASE_BIN(110);
// Instruction set codes
// Major opcodes ---------------------
CONST(LUI, UINT(7))    := BIN(0110111); // U-immediate
CONST(AUIPC, UINT(7))  := BIN(0010111);// U-immediate
CONST(JAL, UINT(7))    := BIN(1101111);// U-immediate
CONST(JALR, UINT(7))   := BIN(1100111);// U-immediate
CONST(BRANCH, UINT(7)) := BIN(1100011);// B-immediate
CONST(LOAD, UINT(7))   := BIN(0000011);// I-immediate
CONST(STORE, UINT(7))  := BIN(0100011);// S-immediate
CONST(OPI, UINT(7))    := BIN(0010011); // I-immediate
CONST(OP, UINT(7))     := BIN(0110011); // reg-to-reg
CONST(OP_MULDIV, UINT(7))     := BIN(0110010); // reg-to-reg // !!!!!!!!!!!!!!! trick with bit 0
CONST(SYS, UINT(7))     := BIN(1110011);// I-immediate
CONST(MEM, UINT(7))     := BIN(0001111);// I-immediate
CASE_CONST(CASE_LUI, UINT(7))    := CASE_BIN(0110111); // U-immediate
CASE_CONST(CASE_AUIPC, UINT(7))  := CASE_BIN(0010111);// U-immediate
CASE_CONST(CASE_JAL, UINT(7))    := CASE_BIN(1101111);// U-immediate
CASE_CONST(CASE_JALR, UINT(7))   := CASE_BIN(1100111);// U-immediate
CASE_CONST(CASE_BRANCH, UINT(7)) := CASE_BIN(1100011);// B-immediate
CASE_CONST(CASE_LOAD, UINT(7))   := CASE_BIN(0000011);// I-immediate
CASE_CONST(CASE_STORE, UINT(7))  := CASE_BIN(0100011);// S-immediate
CASE_CONST(CASE_OPI, UINT(7))    := CASE_BIN(0010011); // I-immediate
CASE_CONST(CASE_OP, UINT(7))     := CASE_BIN(0110011); // reg-to-reg
CASE_CONST(CASE_OP_MULDIV, UINT(7))     := CASE_BIN(0110010); // reg-to-reg // !!!!!!!!!!!!!!! trick with bit 0
CASE_CONST(CASE_SYS, UINT(7))     := CASE_BIN(1110011);// I-immediate
CASE_CONST(CASE_MEM, UINT(7))     := CASE_BIN(0001111);// I-immediate

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
CASE_CONST(CASE_LB, UINT(3))  := CASE_BIN(000);
CASE_CONST(CASE_LH, UINT(3))  := CASE_BIN(001);
CASE_CONST(CASE_LW, UINT(3))  := CASE_BIN(010);
CASE_CONST(CASE_LBU, UINT(3)) := CASE_BIN(100);
CASE_CONST(CASE_LHU, UINT(3)) := CASE_BIN(101);
CASE_CONST(CASE_SB, UINT(3))  := CASE_BIN(000);
CASE_CONST(CASE_SH, UINT(3))  := CASE_BIN(001);
CASE_CONST(CASE_SW, UINT(3))  := CASE_BIN(010);

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

CONST(MUL, UINT(3))    := BIN(000);
CONST(MULH, UINT(3))   := BIN(001);
CONST(MULHSU, UINT(3)) := BIN(010);
CONST(MULHU, UINT(3))  := BIN(011);
CONST(DIV, UINT(3))    := BIN(100);
CONST(DIVU, UINT(3))   := BIN(101);
CONST(REMS, UINT(3))    := BIN(110);
CONST(REMU, UINT(3))   := BIN(111);


CASE_CONST(CASE_MUL, UINT(3))    := CASE_BIN(000);
CASE_CONST(CASE_MULH, UINT(3))   := CASE_BIN(001);
CASE_CONST(CASE_MULHSU, UINT(3)) := CASE_BIN(010);
CASE_CONST(CASE_MULHU, UINT(3))  := CASE_BIN(011);
CASE_CONST(CASE_DIV, UINT(3))    := CASE_BIN(100);
CASE_CONST(CASE_DIVU, UINT(3))   := CASE_BIN(101);
CASE_CONST(CASE_REMS, UINT(3))    := CASE_BIN(110);
CASE_CONST(CASE_REMU, UINT(3))   := CASE_BIN(111);

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


CASE_CONST(CASE_ADDx, UINT(3))  := CASE_BIN(000);
CASE_CONST(CASE_SUBx, UINT(3))  := CASE_BIN(000); // same as above ? Yes, check funct7(5)
CASE_CONST(CASE_SLLx, UINT(3))  := CASE_BIN(001);
CASE_CONST(CASE_SLTx, UINT(3))  := CASE_BIN(010);
CASE_CONST(CASE_SLTU, UINT(3))  := CASE_BIN(011);
CASE_CONST(CASE_XORx, UINT(3))  := CASE_BIN(100);
CASE_CONST(CASE_SRLx, UINT(3))  := CASE_BIN(101);
CASE_CONST(CASE_SRAx, UINT(3))  := CASE_BIN(101); // same as above ? Yes, check funct7(5)
CASE_CONST(CASE_ORx, UINT(3))   := CASE_BIN(110);
CASE_CONST(CASE_ANDx, UINT(3))  := CASE_BIN(111);
// CSR
CASE_CONST(CASE_CSRRC, UINT(3))  := CASE_BIN(011);
CASE_CONST(CASE_CSRRW, UINT(3))  := CASE_BIN(001);
CASE_CONST(CASE_CSRRS, UINT(3))  := CASE_BIN(010);
CASE_CONST(CASE_CSRRCI, UINT(3))  := CASE_BIN(111); // ? guess
CASE_CONST(CASE_CSRRWI, UINT(3))  := CASE_BIN(101);
CASE_CONST(CASE_CSRRSI, UINT(3))  := CASE_BIN(110);
CASE_CONST(CASE_ECALL, UINT(3))  := CASE_BIN(000);
// CSR
CONST(CSRRC, UINT(3))  := BIN(011);
CONST(CSRRW, UINT(3))  := BIN(001);
CONST(CSRRS, UINT(3))  := BIN(010);
CONST(CSRRCI, UINT(3))  := BIN(111); // ? guess
CONST(CSRRWI, UINT(3))  := BIN(101);
CONST(CSRRSI, UINT(3))  := BIN(110);
CONST(ECALL, UINT(3))  := BIN(000);

CONST(MISA, UINT(32))  := BIN(00000000000000000000000100000000);
CONST(MVENDORID, UINT(32))  := TO_UINT(0,32);
CONST(MARCHID, UINT(32))  := TO_UINT(0,32);
CONST(MIMPID, UINT(32))  := TO_UINT(0,32);
CONST(MHARTID, UINT(32))  := TO_UINT(0,32);

CONST(SD_HI, INTEGER) := 31;
CONST(TSR_HI, INTEGER) := 22;
CONST(TW_HI, INTEGER) := 21;
CONST(TVM_HI, INTEGER) := 20;
CONST(MXR_HI, INTEGER) := 19;
CONST(SUM_HI, INTEGER) := 18;
CONST(MPRV_HI, INTEGER) := 17;
CONST(XS_HI, INTEGER) := 16;
CONST(FS_HI, INTEGER) := 14;
CONST(MPP_HI, INTEGER) := 12;
CONST(SPP_HI, INTEGER) := 8;
CONST(MPIE_HI, INTEGER) := 7;
CONST(SPIE_HI, INTEGER) := 5;
CONST(UPIE_HI, INTEGER) := 4;
CONST(MIE_HI, INTEGER) := 3;
CONST(SIE_HI, INTEGER) :=1 ;
CONST(UIE_HI, INTEGER) := 0;

// CSR addresses
// Machine trap setup
CASE_CONST(CASE_AMSTATUS, UINT(12))  := CASE_BIN(001100000000);
CASE_CONST(CASE_AMISA, UINT(12))  := CASE_BIN(001100000001);
CASE_CONST(CASE_AMEDELEG, UINT(12))  := CASE_BIN(001100000010);
CASE_CONST(CASE_AMIDELEG, UINT(12))  := CASE_BIN(001100000011);
CASE_CONST(CASE_AMIE, UINT(12))  := CASE_BIN(001100000100);
CASE_CONST(CASE_AMTVEC, UINT(12))  := CASE_BIN(001100000101);
CASE_CONST(CASE_AMTCOUNTEREN, UINT(12))  := CASE_BIN(001100000110);

// Machine trap handling
CASE_CONST(CASE_AMSCRATCH, UINT(12))  := CASE_BIN(001101000000);
CASE_CONST(CASE_AMEPC, UINT(12))  := CASE_BIN(001101000001);
CASE_CONST(CASE_AMCAUSE, UINT(12))  := CASE_BIN(001101000010);
//
CASE_CONST(CASE_AMIP, UINT(12))  := CASE_BIN(001101000100);
//CASE_CONST(CASE_AMIE, UINT(12))  := CASE_BIN(001100000100);

// CSR addresses
// Machine trap setup
CONST(AMSTATUS, UINT(12))  := BIN(001100000000);
CONST(AMISA, UINT(12))  := BIN(001100000001);
CONST(AMEDELEG, UINT(12))  := BIN(001100000010);
CONST(AMIDELEG, UINT(12))  := BIN(001100000011);
CONST(AMIE, UINT(12))  := BIN(001100000100);
CONST(AMTVEC, UINT(12))  := BIN(001100000101);
CONST(AMTCOUNTEREN, UINT(12))  := BIN(001100000110);

// Machine trap handling
CONST(AMSCRATCH, UINT(12))  := BIN(001101000000);
CONST(AMEPC, UINT(12))  := BIN(001101000001);
CONST(AMCAUSE, UINT(12))  := BIN(001101000010);
//
CONST(AMIP, UINT(12))  := BIN(001101000100);



CONST(ILLINSTR, UINT(32))  := TO_UINT(2,32);

END_PACKAGE(risc_V_constants)

