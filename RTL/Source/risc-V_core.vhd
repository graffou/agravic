-- vhdl file of block risc-V_core generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
library work; use work.structures.all;
library work; use work.risc_V_constants.all;
entity risc_V_core is port( clk_core : IN std_logic; reset_n : IN std_logic; load_port_i : IN blk2mem_t; core2instmem_o : OUT blk2mem_t; instmem2core_i : IN mem2blk_t; core2datamem_o : OUT blk2mem_t; datamem2core_i : IN mem2blk_t ); end risc_V_core; architecture rtl of risc_V_core is component dummy_zkw_pouet is port(clk : in std_logic);end component;
type reg_file_t is array(0 to (32 -1)) of unsigned ((32 -1) downto 0);
signal regs : reg_file_t;
signal PC : unsigned ((32 -1) downto 0);
signal PCp : unsigned ((32 -1) downto 0);
signal loading : std_logic;
signal rtrap : std_logic;
signal alt_op : std_logic;
signal cpu_wait : std_logic;
signal cpu_wait2 : std_logic;
signal flush : std_logic;
signal rimmediate : unsigned ((32 -1) downto 0);
signal use_immediate : std_logic;
signal raw_opcode : unsigned ((7 -1) downto 0);
signal ropcode : unsigned ((7 -1) downto 0);
signal rropcode : unsigned ((7 -1) downto 0);
signal rfunct3 : unsigned ((3 -1) downto 0);
signal rop1 : unsigned ((32 -1) downto 0);
signal rop2 : unsigned ((32 -1) downto 0);
signal rfunct7 : unsigned ((7 -1) downto 0);
signal rrd : unsigned ((5 -1) downto 0);
signal rrs1 : unsigned ((5 -1) downto 0);
signal rrs2 : unsigned ((5 -1) downto 0);
signal rrd_val : unsigned ((32 -1) downto 0);
signal rtaken : unsigned ((1 -1) downto 0);
signal exec : std_logic;
signal pipe : unsigned ((4 -1) downto 0);
signal rwb : unsigned ((5 -1) downto 0);
signal funct3wb : unsigned ((3 -1) downto 0);
signal rshiftwb : unsigned ((2 -1) downto 0);
signal csri : std_logic;
signal priv : unsigned ((2 -1) downto 0);
signal mstatus : unsigned ((32 -1) downto 0);
signal mtvec : unsigned ((32 -1) downto 0);
signal mcause : unsigned ((32 -1) downto 0);
signal mideleg : unsigned ((32 -1) downto 0);
signal medeleg : unsigned ((32 -1) downto 0);
signal mip : unsigned ((32 -1) downto 0);
signal mie : unsigned ((32 -1) downto 0);
signal mcounter : unsigned ((64 -1) downto 0);
signal blk2mem_t0 : blk2mem_t;
signal mem2blk_t0 : mem2blk_t;
signal rinstr : unsigned ((32 -1) downto 0);
signal rrinstr : unsigned ((32 -1) downto 0);
signal radd_res : unsigned ((33 -1) downto 0);
signal rsub_res : unsigned ((33 -1) downto 0);
signal rsll_res : unsigned ((32 -1) downto 0);
signal rsrl_res_u : unsigned ((32 -1) downto 0);
signal rsrl_res_s : unsigned ((32 -1) downto 0);
signal rrs1_lt_rs2_u : unsigned ((1 -1) downto 0);
signal rrs1_lt_rs2_s : unsigned ((1 -1) downto 0);
signal rrs1_eq_rs2 : unsigned ((1 -1) downto 0);
signal rand_res : unsigned ((32 -1) downto 0);
signal ror_res : unsigned ((32 -1) downto 0);
signal rxor_res : unsigned ((32 -1) downto 0);
begin
process0 : process(clk_core,reset_n)
variable instr : unsigned ((32 -1) downto 0);
variable opcode : unsigned ((7 -1) downto 0);
variable rd : unsigned ((5 -1) downto 0);
variable wrd : unsigned ((5 -1) downto 0);
variable rs1 : unsigned ((5 -1) downto 0);
variable rs2 : unsigned ((5 -1) downto 0);
variable funct3 : unsigned ((3 -1) downto 0);
variable funct7 : unsigned ((7 -1) downto 0);
variable immediate : unsigned ((32 -1) downto 0);
variable immediate_type : unsigned ((3 -1) downto 0);
variable op1 : unsigned ((32 -1) downto 0);
variable op2 : unsigned ((32 -1) downto 0);
variable add_res : unsigned ((33 -1) downto 0);
variable sub_res : unsigned ((33 -1) downto 0);
variable sll_res : unsigned ((32 -1) downto 0);
variable srl_res_u : unsigned ((32 -1) downto 0);
variable srl_res_s : unsigned ((32 -1) downto 0);
variable rs1_lt_rs2_u : unsigned ((1 -1) downto 0);
variable rs1_lt_rs2_s : unsigned ((1 -1) downto 0);
variable rs1_eq_rs2 : unsigned ((1 -1) downto 0);
variable and_res : unsigned ((32 -1) downto 0);
variable or_res : unsigned ((32 -1) downto 0);
variable xor_res : unsigned ((32 -1) downto 0);
variable ld_data : unsigned ((32 -1) downto 0);
variable nshift : unsigned ((2 -1) downto 0);
variable wbe : unsigned ((4 -1) downto 0);
variable taken : unsigned ((1 -1) downto 0);
variable rd_val : unsigned ((32 -1) downto 0);
variable trap : std_logic;
variable cause : unsigned ((32 -1) downto 0);
variable trap_addr_base : unsigned ((32 -1) downto 0);
variable trap_addr_offset : unsigned ((32 -1) downto 0);
variable csr_val : unsigned ((32 -1) downto 0);
variable csr_wb : unsigned ((32 -1) downto 0);
begin
 IF ( reset_n = '0' ) then
  pipe <= TO_UNSIGNED(0,pipe'length);
  PC <= TO_UNSIGNED(0,PC'length);
  PCp <= TO_UNSIGNED(0,PCp'length);
  cpu_wait <= '0';
  cpu_wait2 <= '0';
  flush <= '0';
  alt_op <= '0';
  use_immediate <= '0';
  csri <= '0';
  loading <= '0';
  mcounter <= TO_UNSIGNED(0,mcounter'length);
  mstatus <= TO_UNSIGNED(0,mstatus'length);
  mcause <= TO_UNSIGNED(0,mcause'length);
  medeleg <= TO_UNSIGNED(0,medeleg'length);
  mideleg <= TO_UNSIGNED(0,mideleg'length);
  mtvec <= TO_UNSIGNED(0,mtvec'length);
  mip <= TO_UNSIGNED(0,mip'length);
  mie <= TO_UNSIGNED(0,mie'length);
  rinstr <= TO_UNSIGNED(0,rinstr'length);
  rrinstr <= TO_UNSIGNED(0,rrinstr'length);
  ropcode <= TO_UNSIGNED(0,ropcode'length);
  rfunct3 <= TO_UNSIGNED(0,rfunct3'length);
  rtrap <= '0';
  priv <= "11";
  elsif ( clk_core'event and (clk_core = '1' ) ) then
   flush <= '0';
   instr := instmem2core_i.data;
   IF ( ( (cpu_wait = '0' ) and (cpu_wait2 = '0' ) ) or (datamem2core_i.data_en = '1' )) then
    pipe <= ( (pipe(pipe'high-1 downto 0)) & '1' );
    PCp <= PC;
    PC <= PC + TO_UNSIGNED(4,PC'length);
   end if;
   cpu_wait2 <= cpu_wait;
   IF ( (pipe(0) = '1' ) and (cpu_wait = '0') and (flush = '0') ) then
     rinstr <= instr;
     opcode := (instr(6 downto 0));
     -- gprintf("#MOPCODE %",opcode);
     rd := (instr(11 downto 7));
     rs1 := (instr(19 downto 15));
     rs2 := (instr(24 downto 20));
     funct3 := (instr(14 downto 12));
     funct7 := (instr(31 downto 25));
     IF (opcode = OPI) then
      ropcode <= OP;
      alt_op <= '0';
     else
      ropcode <= opcode;
      alt_op <= instr(30);
     end if;
     raw_opcode <= opcode;
     rfunct3 <= funct3;
     rfunct7 <= funct7;
     rrs1 <= rs1;
     rrs2 <= rs2;
     case opcode is
      when CASE_SYS => immediate_type := I_type;
      when CASE_MEM => immediate_type := I_type;
      when CASE_OPI => immediate_type := I_type;
      when CASE_LOAD => immediate_type := I_type; cpu_wait2 <= '1' ;
      when CASE_STORE => immediate_type := S_type;
      when CASE_LUI => immediate_type := U_type;
      when CASE_AUIPC => immediate_type := U_type;
      when CASE_JAL => immediate_type := J_type;
      when CASE_JALR => immediate_type := J_type;
      when CASE_BRANCH => immediate_type := B_type;
      when others => immediate_type := no_type;
     end case;
     case immediate_type is
      when CASE_I_type => immediate := unsigned(RESIZE(signed((instr(31 downto 20))), immediate'length));
      when CASE_S_type => immediate := unsigned(RESIZE(signed(std_logic_vector(instr(31 downto 25)) & std_logic_vector(instr(11 downto 7))), immediate'length));
      when CASE_B_type => immediate := unsigned(RESIZE(signed(std_logic_vector(instr(31 downto 31)) & std_logic_vector(instr(7 downto 7)) & std_logic_vector(instr(30 downto 25)) & std_logic_vector(instr(11 downto 8)) & '0'), immediate'length));
      when CASE_U_type => immediate := unsigned(RESIZE(signed(std_logic_vector(instr(31 downto 20)) & std_logic_vector(instr(19 downto 12)) & "000000000000"), immediate'length));
      when CASE_J_type => immediate := unsigned(RESIZE(signed(std_logic_vector(instr(31 downto 31)) & std_logic_vector(instr(19 downto 12)) & std_logic_vector(instr(20 downto 20)) & std_logic_vector(instr(30 downto 21)) & "0"), immediate'length));
      when others => immediate := TO_UNSIGNED(0,immediate'length);
     end case;
     IF ( (immediate_type = B_type) or (immediate_type = S_type) ) then
      rrd <= "00000";
     else
      rrd <= rd;
     end if;
     IF (immediate_type = no_type) then
      use_immediate <= '0';
     elsif ( (immediate_type = J_type) or (immediate_type = B_type) or (opcode = AUIPC) ) then
      rimmediate <= immediate + PCp;
      use_immediate <= '0';
     else
      use_immediate <= '1' ;
      rimmediate <= immediate;
     end if;
     IF ( (opcode = SYS) and (funct3(2) = '1' ) ) then
      csri <= '1' ;
     else
      csri <= '0';
     end if;
   end if;
   IF ( (pipe(1) = '1' ) and (cpu_wait = '0') and (flush = '0') ) then
    exec <= '1' ;
     rrinstr <= rinstr;
    IF (csri = '1' ) then
     op1 := RESIZE(rrs1, op1'length);
    else
     op1 := regs(TO_INTEGER(rrs1));
    end if;
    IF ( use_immediate = '1' ) then
      op2 := rimmediate;
    else
     op2 := regs(TO_INTEGER(rrs2));
    end if;
    rop1 <= op1;
    rop2 <= op2;
    add_res := RESIZE(op1, add_res'length) + RESIZE(op2, add_res'length);
    sub_res := RESIZE(op1, add_res'length) - RESIZE(op2, add_res'length);
    rs1_lt_rs2_u := RESIZE((sub_res(32 downto 32)),1);
    rs1_lt_rs2_s := RESIZE((sub_res(32 downto 32)) xor (op1(31 downto 31)) xor (op2(31 downto 31)),1);
    IF (sub_res = TO_UNSIGNED(0,sub_res'length)) then
      rs1_eq_rs2 := "1";
    else
      rs1_eq_rs2 := "0";
    end if;
    sll_res := SHIFT_LEFT(op1, TO_INTEGER(RESIZE(op2, 5)));
    srl_res_u := SHIFT_RIGHT(op1, TO_INTEGER(RESIZE(op2, 5)));
    srl_res_s := unsigned(SHIFT_RIGHT(signed(op1), TO_INTEGER(RESIZE(op2, 5))));
    and_res := op1 and op2;
    or_res := op1 or op2;
    xor_res := op1 xor op2;
    nshift := RESIZE(add_res, nshift'length);
    trap := '0';
    taken := "0";
    radd_res <= add_res;
    rsub_res <= sub_res;
    rrs1_lt_rs2_u <= rs1_lt_rs2_u;
    rrs1_lt_rs2_s <= rs1_lt_rs2_s;
    rwb <= "00000";
    rd_val := "10101010010101011010101001010101";
    case ropcode is
     when CASE_SYS =>
      rd_val := TO_UNSIGNED(0,rd_val'length);
     when CASE_MEM =>
     when CASE_OP =>
      case rfunct3 is
       when CASE_ADDx => IF (alt_op = '0') then rd_val := RESIZE(add_res, 32); else rd_val := RESIZE(sub_res, 32); end if;
       when CASE_SLLx => rd_val := sll_res;
       when CASE_SRLx => IF (alt_op = '0') then rd_val := srl_res_u; else rd_val := srl_res_s; end if;
       when CASE_SLTx => IF (alt_op = '1' ) then rd_val := RESIZE(rs1_lt_rs2_s, 32); else rd_val := RESIZE(rs1_lt_rs2_u, 32); end if;
       when CASE_ORx => rd_val := or_res;
       when CASE_XORx => rd_val := xor_res;
       when CASE_ANDx => rd_val := and_res;
       when others => trap := '1' ; cause := ILLINSTR;
      end case;
     when CASE_LOAD =>
      cpu_wait <= '1' ;
      rwb <= rrd;
      funct3wb <= rfunct3;
      rshiftwb <= nshift;
      core2datamem_o.addr <= (add_res(blk2mem_t0.addr'length+1 downto 2));
      core2datamem_o.cs_n <= '0';
      core2datamem_o.wr_n <= '1' ;
      core2datamem_o.be <= "1111";
     when CASE_STORE =>
      core2datamem_o.addr <= (add_res(blk2mem_t0.addr'length+1 downto 2));
      core2datamem_o.cs_n <= '0';
      core2datamem_o.wr_n <= '0';
      wbe := ( rfunct3(1) & rfunct3(1) & (rfunct3(0) or rfunct3(1)) & '1' );
      wbe := SHIFT_LEFT(wbe, TO_INTEGER(nshift));
      core2datamem_o.be <= wbe;
      core2datamem_o.data <= SHIFT_LEFT(regs(TO_INTEGER(rrs2)), TO_INTEGER(nshift & "000"));
     when CASE_LUI => rd_val := rimmediate;
     when CASE_AUIPC => rd_val := rimmediate;
     when CASE_JAL => PC <= rimmediate; rd_val := PC; flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length);
     when CASE_JALR => PC <= RESIZE(add_res, PC'length); rd_val := PC; flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length);
     when CASE_BRANCH =>
      taken := ( (rfunct3(1 downto 1)) and (rs1_lt_rs2_u xor (rfunct3(0 downto 0))) ) or
         ( ((rfunct3(1 downto 1)) xor (rfunct3(2 downto 2)) ) and (rs1_lt_rs2_s xor (rfunct3(0 downto 0))) ) or
         ( not ((rfunct3(1 downto 1)) and not (rfunct3(2 downto 2)) ) and (rs1_eq_rs2 xor (rfunct3(0 downto 0))) );
      IF (taken = "1") then
       PC <= rimmediate; flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length);
      end if;
     when others => trap := '1' ; cause := ILLINSTR;
    end case;
    rtaken <= taken;
    rrd_val <= rd_val;
    IF ( not (ropcode = LOAD) and not (ropcode = STORE) ) then
     core2datamem_o.cs_n <= '1' ;
    end if;
    wrd := rrd;
   else
    core2datamem_o.cs_n <= '1' ;
    exec <= '0';
    rd_val := "10101010010101011010101001010101";
   end if;
   IF ( not(rwb = TO_UNSIGNED(0,rwb'length)) and (datamem2core_i.data_en = '1' ) ) then
    cpu_wait <= '0';
    ld_data := datamem2core_i.data;
    ld_data := SHIFT_RIGHT(ld_data, TO_INTEGER( (rshiftwb & "000")));
    case funct3wb is
     when CASE_LW => rd_val := ld_data;
     when CASE_LH => rd_val := unsigned(RESIZE(signed((ld_data(15 downto 0))), rd_val'length));
     when CASE_LHU => rd_val := RESIZE((ld_data(15 downto 0)), rd_val'length);
     when CASE_LB => rd_val := unsigned(RESIZE(signed((ld_data(7 downto 0))), rd_val'length));
     when CASE_LBU => rd_val := RESIZE((ld_data(7 downto 0)), rd_val'length);
     when others => rd_val := TO_UNSIGNED(0,rd_val'length);
    end case;
    wrd := rwb;
   end if;
   regs(TO_INTEGER(wrd)) <= rd_val;
   regs(0) <= TO_UNSIGNED(0,32);
   IF (load_port_i.wr_n = '0' ) then
    -- gprintf("#RLOAD");
    loading <= '1' ;
    PC <= RESIZE( load_port_i.addr, PC'length );
    PCp <= load_port_i.data;
   elsif ( loading = '1' ) then
    -- gprintf("#RENDLOAD");
    PC <= TO_UNSIGNED(0,PC'length);
    loading <= '0';
    flush <= '1' ;
    pipe <= TO_UNSIGNED(0,pipe'length);
   end if;
   rtrap <= trap;
   IF ((not (loading = '1' )) and (trap = '1' )) then
    trap_addr_base := mtvec and "11111111111111111111111111111100";
    IF ((mtvec(1 downto 0)) = "01" ) then
     trap_addr_offset := (cause(31 downto 2)) & "00";
    else
     trap_addr_offset := TO_UNSIGNED(0,trap_addr_offset'length);
    end if;
    PC <= trap_addr_base + trap_addr_offset;
    mcause <= cause;
   end if;
 end if;
 end process;

  core2instmem_o.addr <= (PC(blk2mem_t0.addr'length+1 downto 2));
  core2instmem_o.data <= PCp;
  core2instmem_o.cs_n <= '0';
  core2instmem_o.wr_n <= not loading;
  core2instmem_o.be <= "1111";
  -- gprintf("comb PC % % % % % %\n", PC, core2instmem_o.addr, core2instmem_o.data , core2instmem_o.cs_n, core2instmem_o.wr_n, core2instmem_o.be);

end rtl;
