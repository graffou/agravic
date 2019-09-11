-- vhdl file of block risc_V_constants generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
package risc_V_constants is
constant no_type : unsigned ((3 -1) downto 0) := "000";
constant R_type : unsigned ((3 -1) downto 0) := "001";
constant I_type : unsigned ((3 -1) downto 0) := "010";
constant S_type : unsigned ((3 -1) downto 0) := "011";
constant B_type : unsigned ((3 -1) downto 0) := "100";
constant U_type : unsigned ((3 -1) downto 0) := "101";
constant J_type : unsigned ((3 -1) downto 0) := "110";
constant CASE_no_type : unsigned ((3 -1) downto 0) := "000";
constant CASE_R_type : unsigned ((3 -1) downto 0) := "001";
constant CASE_I_type : unsigned ((3 -1) downto 0) := "010";
constant CASE_S_type : unsigned ((3 -1) downto 0) := "011";
constant CASE_B_type : unsigned ((3 -1) downto 0) := "100";
constant CASE_U_type : unsigned ((3 -1) downto 0) := "101";
constant CASE_J_type : unsigned ((3 -1) downto 0) := "110";
constant LUI : unsigned ((7 -1) downto 0) := "0110111";
constant AUIPC : unsigned ((7 -1) downto 0) := "0010111";
constant JAL : unsigned ((7 -1) downto 0) := "1101111";
constant JALR : unsigned ((7 -1) downto 0) := "1100111";
constant BRANCH : unsigned ((7 -1) downto 0) := "1100011";
constant LOAD : unsigned ((7 -1) downto 0) := "0000011";
constant STORE : unsigned ((7 -1) downto 0) := "0100011";
constant OPI : unsigned ((7 -1) downto 0) := "0010011";
constant OP : unsigned ((7 -1) downto 0) := "0110011";
constant SYS : unsigned ((7 -1) downto 0) := "1110011";
constant MEM : unsigned ((7 -1) downto 0) := "0001111";
constant CASE_LUI : unsigned ((7 -1) downto 0) := "0110111";
constant CASE_AUIPC : unsigned ((7 -1) downto 0) := "0010111";
constant CASE_JAL : unsigned ((7 -1) downto 0) := "1101111";
constant CASE_JALR : unsigned ((7 -1) downto 0) := "1100111";
constant CASE_BRANCH : unsigned ((7 -1) downto 0) := "1100011";
constant CASE_LOAD : unsigned ((7 -1) downto 0) := "0000011";
constant CASE_STORE : unsigned ((7 -1) downto 0) := "0100011";
constant CASE_OPI : unsigned ((7 -1) downto 0) := "0010011";
constant CASE_OP : unsigned ((7 -1) downto 0) := "0110011";
constant CASE_SYS : unsigned ((7 -1) downto 0) := "1110011";
constant CASE_MEM : unsigned ((7 -1) downto 0) := "0001111";
constant BEQ : unsigned ((3 -1) downto 0) := "000";
constant BNE : unsigned ((3 -1) downto 0) := "001";
constant BLT : unsigned ((3 -1) downto 0) := "100";
constant BGE : unsigned ((3 -1) downto 0) := "101";
constant BLTU : unsigned ((3 -1) downto 0) := "110";
constant BGEU : unsigned ((3 -1) downto 0) := "111";
constant LB : unsigned ((3 -1) downto 0) := "000";
constant LH : unsigned ((3 -1) downto 0) := "001";
constant LW : unsigned ((3 -1) downto 0) := "010";
constant LBU : unsigned ((3 -1) downto 0) := "100";
constant LHU : unsigned ((3 -1) downto 0) := "101";
constant SB : unsigned ((3 -1) downto 0) := "000";
constant SH : unsigned ((3 -1) downto 0) := "001";
constant SW : unsigned ((3 -1) downto 0) := "010";
constant CASE_LB : unsigned ((3 -1) downto 0) := "000";
constant CASE_LH : unsigned ((3 -1) downto 0) := "001";
constant CASE_LW : unsigned ((3 -1) downto 0) := "010";
constant CASE_LBU : unsigned ((3 -1) downto 0) := "100";
constant CASE_LHU : unsigned ((3 -1) downto 0) := "101";
constant CASE_SB : unsigned ((3 -1) downto 0) := "000";
constant CASE_SH : unsigned ((3 -1) downto 0) := "001";
constant CASE_SW : unsigned ((3 -1) downto 0) := "010";
constant ADDI : unsigned ((3 -1) downto 0) := "000";
constant SLTI : unsigned ((3 -1) downto 0) := "010";
constant SLTIU : unsigned ((3 -1) downto 0) := "011";
constant XORI : unsigned ((3 -1) downto 0) := "100";
constant ORI : unsigned ((3 -1) downto 0) := "110";
constant ANDI : unsigned ((3 -1) downto 0) := "111";
constant SLLI : unsigned ((3 -1) downto 0) := "001";
constant SRLI : unsigned ((3 -1) downto 0) := "101";
constant SLAI : unsigned ((3 -1) downto 0) := "101";
constant ADDx : unsigned ((3 -1) downto 0) := "000";
constant SUBx : unsigned ((3 -1) downto 0) := "000";
constant SLLx : unsigned ((3 -1) downto 0) := "001";
constant SLTx : unsigned ((3 -1) downto 0) := "010";
constant SLTU : unsigned ((3 -1) downto 0) := "011";
constant XORx : unsigned ((3 -1) downto 0) := "100";
constant SRLx : unsigned ((3 -1) downto 0) := "101";
constant SRAx : unsigned ((3 -1) downto 0) := "101";
constant ORx : unsigned ((3 -1) downto 0) := "110";
constant ANDx : unsigned ((3 -1) downto 0) := "111";
constant CASE_ADDx : unsigned ((3 -1) downto 0) := "000";
constant CASE_SUBx : unsigned ((3 -1) downto 0) := "000";
constant CASE_SLLx : unsigned ((3 -1) downto 0) := "001";
constant CASE_SLTx : unsigned ((3 -1) downto 0) := "010";
constant CASE_SLTU : unsigned ((3 -1) downto 0) := "011";
constant CASE_XORx : unsigned ((3 -1) downto 0) := "100";
constant CASE_SRLx : unsigned ((3 -1) downto 0) := "101";
constant CASE_SRAx : unsigned ((3 -1) downto 0) := "101";
constant CASE_ORx : unsigned ((3 -1) downto 0) := "110";
constant CASE_ANDx : unsigned ((3 -1) downto 0) := "111";
constant CASE_CSRRC : unsigned ((3 -1) downto 0) := "011";
constant CASE_CSRRW : unsigned ((3 -1) downto 0) := "001";
constant CASE_CSRRS : unsigned ((3 -1) downto 0) := "010";
constant CASE_CSRRCI : unsigned ((3 -1) downto 0) := "111";
constant CASE_CSRRWI : unsigned ((3 -1) downto 0) := "101";
constant CASE_CSRRSI : unsigned ((3 -1) downto 0) := "110";
constant CASE_ECALL : unsigned ((3 -1) downto 0) := "000";
constant CSRRC : unsigned ((3 -1) downto 0) := "011";
constant CSRRW : unsigned ((3 -1) downto 0) := "001";
constant CSRRS : unsigned ((3 -1) downto 0) := "010";
constant CSRRCI : unsigned ((3 -1) downto 0) := "111";
constant CSRRWI : unsigned ((3 -1) downto 0) := "101";
constant CSRRSI : unsigned ((3 -1) downto 0) := "110";
constant ECALL : unsigned ((3 -1) downto 0) := "000";
constant MISA : unsigned ((32 -1) downto 0) := "00000000000000000000000100000000";
constant MVENDORID : unsigned ((32 -1) downto 0) := TO_UNSIGNED(0,32);
constant MARCHID : unsigned ((32 -1) downto 0) := TO_UNSIGNED(0,32);
constant MIMPID : unsigned ((32 -1) downto 0) := TO_UNSIGNED(0,32);
constant MHARTID : unsigned ((32 -1) downto 0) := TO_UNSIGNED(0,32);
constant SD_HI : INTEGER := 31;
constant TSR_HI : INTEGER := 22;
constant TW_HI : INTEGER := 21;
constant TVM_HI : INTEGER := 20;
constant MXR_HI : INTEGER := 19;
constant SUM_HI : INTEGER := 18;
constant MPRV_HI : INTEGER := 17;
constant XS_HI : INTEGER := 16;
constant FS_HI : INTEGER := 14;
constant MPP_HI : INTEGER := 12;
constant SPP_HI : INTEGER := 8;
constant MPIE_HI : INTEGER := 7;
constant SPIE_HI : INTEGER := 5;
constant UPIE_HI : INTEGER := 4;
constant MIE_HI : INTEGER := 3;
constant SIE_HI : INTEGER :=1 ;
constant UIE_HI : INTEGER := 0;
constant CASE_AMSTATUS : unsigned ((12 -1) downto 0) := "001100000000";
constant CASE_AMISA : unsigned ((12 -1) downto 0) := "001100000001";
constant CASE_AMEDELEG : unsigned ((12 -1) downto 0) := "001100000010";
constant CASE_AMIDELEG : unsigned ((12 -1) downto 0) := "001100000011";
constant CASE_AMIE : unsigned ((12 -1) downto 0) := "001100000100";
constant CASE_AMTVEC : unsigned ((12 -1) downto 0) := "001100000101";
constant CASE_AMTCOUNTEREN : unsigned ((12 -1) downto 0) := "001100000110";
constant CASE_AMSCRATCH : unsigned ((12 -1) downto 0) := "001101000000";
constant CASE_AMEPC : unsigned ((12 -1) downto 0) := "001101000001";
constant CASE_AMCAUSE : unsigned ((12 -1) downto 0) := "001101000010";
constant CASE_AMIP : unsigned ((12 -1) downto 0) := "001101000100";
constant AMSTATUS : unsigned ((12 -1) downto 0) := "001100000000";
constant AMISA : unsigned ((12 -1) downto 0) := "001100000001";
constant AMEDELEG : unsigned ((12 -1) downto 0) := "001100000010";
constant AMIDELEG : unsigned ((12 -1) downto 0) := "001100000011";
constant AMIE : unsigned ((12 -1) downto 0) := "001100000100";
constant AMTVEC : unsigned ((12 -1) downto 0) := "001100000101";
constant AMTCOUNTEREN : unsigned ((12 -1) downto 0) := "001100000110";
constant AMSCRATCH : unsigned ((12 -1) downto 0) := "001101000000";
constant AMEPC : unsigned ((12 -1) downto 0) := "001101000001";
constant AMCAUSE : unsigned ((12 -1) downto 0) := "001101000010";
constant AMIP : unsigned ((12 -1) downto 0) := "001101000100";
constant ILLINSTR : unsigned ((32 -1) downto 0) := TO_UNSIGNED(2,32);
end risc_V_constants;
