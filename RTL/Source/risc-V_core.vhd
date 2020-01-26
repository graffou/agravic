-- vhdl file of block risc-V_core generated by Agravic
library ieee;use ieee.std_logic_1164.all;use IEEE.NUMERIC_STD.ALL;
library work; use work.slv_utils.all;
library work; use work.structures.all;
library work; use work.risc_V_constants.all;



entity risc_V_core is port( clk_core : IN std_logic; reset_n : IN std_logic; boot_mode_i : IN std_logic; trap_o : OUT std_logic; dbg_o : OUT unsigned ((33 -1) downto 0); load_port_i : IN blk2mem_t; core2instmem_o : OUT blk2mem_t; instmem2core_i : IN mem2blk_t; core2datamem_o : OUT blk2mem_t; dma_request_i : IN blk2mem_t; dma_grant_o : OUT std_logic; datamem2core_i : IN mem2blk_t ); end risc_V_core; architecture rtl of risc_V_core is component dummy_zkw_pouet is port(clk : in std_logic);end component;
component register_file is port( clk : IN std_logic; reset_n : IN std_logic; addr_rs1_i : IN unsigned ((5 -1) downto 0); addr_rs2_i : IN unsigned ((5 -1) downto 0); addr_rd_i : IN unsigned ((5 -1) downto 0); wb_en_i : IN std_logic; wb_i : IN unsigned ((32 -1) downto 0); rs1_o : OUT unsigned ((32 -1) downto 0); rs2_o : OUT unsigned ((32 -1) downto 0) ); end component;
component dbg_mem is port( clk : IN std_logic; reset_n : IN std_logic; addr_i : IN unsigned ((5 -1) downto 0); data_i : IN unsigned ((64 -1) downto 0); wen_i : IN std_logic; data_o : OUT unsigned ((64 -1) downto 0) ); end component;
type reg_file_t is array(0 to (32 -1)) of unsigned ((32 -1) downto 0);
signal regs : reg_file_t;
signal PC : unsigned ((32 -1) downto 0);
signal PCp : unsigned ((32 -1) downto 0);


signal loading : std_logic;
signal boot_mode : std_logic;
signal boot_modep : std_logic;
signal load_mem0 : std_logic;
signal load_mem : std_logic;
signal rtrap : std_logic;
signal alt_op : std_logic;
signal cpu_wait_early : std_logic;
signal cpu_wait : std_logic;
signal mask_data_en : std_logic;
signal cpu_wait_on_write : std_logic;
signal flush : std_logic;

signal rimmediate : unsigned ((32 -1) downto 0);
signal rimmediate_type : unsigned ((3 -1) downto 0);
signal use_immediate : std_logic;
signal raw_opcode : unsigned ((7 -1) downto 0);
signal ropcode : unsigned ((7 -1) downto 0);
signal rropcode : unsigned ((7 -1) downto 0);
signal rfunct3 : unsigned ((3 -1) downto 0);
signal rop1 : unsigned ((32 -1) downto 0);
signal rop2 : unsigned ((32 -1) downto 0);
signal rop1_rf : unsigned ((32 -1) downto 0);
signal rop2_rf : unsigned ((32 -1) downto 0);
signal rs1_rd : unsigned ((32 -1) downto 0);
signal rs2_rd : unsigned ((32 -1) downto 0);
signal rrd_wrp : unsigned ((32 -1) downto 0);
signal rrd_wr : unsigned ((32 -1) downto 0);
signal rrd_wr_en : std_logic;
signal rrd_wr_enp : std_logic;

signal rfunct7 : unsigned ((7 -1) downto 0);
signal rrd : unsigned ((5 -1) downto 0);
signal rrrd : unsigned ((5 -1) downto 0);
signal rrrdp : unsigned ((5 -1) downto 0);
signal rrs1 : unsigned ((5 -1) downto 0);
signal rrs2 : unsigned ((5 -1) downto 0);
signal addr_rs1 : unsigned ((5 -1) downto 0);
signal addr_rs2 : unsigned ((5 -1) downto 0);
signal rrd_val : unsigned ((32 -1) downto 0);
signal rtaken : unsigned ((1 -1) downto 0);

signal halt : std_logic;
signal inst_cs_n : std_logic;
signal exec : std_logic;
signal pipe : unsigned ((4 -1) downto 0);
signal rwb : unsigned ((5 -1) downto 0);
signal funct3wb : unsigned ((3 -1) downto 0);
signal rshiftwb : unsigned ((2 -1) downto 0);


signal csri : std_logic;
signal priv : unsigned ((2 -1) downto 0);
signal mstatus : unsigned ((32 -1) downto 0);
signal mepc : unsigned ((32 -1) downto 0);
signal mscratch : unsigned ((32 -1) downto 0);
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
signal inst_addr : unsigned ((15 - 2 -1) downto 0);
signal inst_data : unsigned ((32 -1) downto 0);



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
signal rjalr : unsigned ((5 -1) downto 0);
signal rload_from_instmem : std_logic;
signal rdbg : std_logic;
signal rdbg2 : std_logic;
signal rcan_grant : std_logic;
signal code_loaded : std_logic;
signal code_loaded0 : std_logic;
signal cpu_stuck_cnt : unsigned ((5 -1) downto 0);
signal cond1 : std_logic;
signal cond2 : std_logic;
signal cond3 : std_logic;
signal cond4 : std_logic;
signal cond5 : std_logic;
signal dbg_pipe : unsigned ((32 -1) downto 0);


signal dbg_addr : unsigned ((5 -1) downto 0);
signal dbg_data_w : unsigned ((64 -1) downto 0);
signal dbg_data_r : unsigned ((64 -1) downto 0);
signal dbg_wen : std_logic;
signal dbg_cnt : unsigned ((7 -1) downto 0);
signal dbg_cnt2 : unsigned ((5 -1) downto 0);
signal debug_write : std_logic;
signal dbg_stop : unsigned ((8 -1) downto 0);



begin

u0_register_file : register_file port map( clk => clk_core, reset_n => reset_n, addr_rs1_i => addr_rs1, addr_rs2_i => addr_rs2, addr_rd_i => rrrd, wb_en_i => rrd_wr_en, wb_i => rrd_wr, rs1_o => rs1_rd, rs2_o => rs2_rd);
u0_dbg_mem : dbg_mem port map( clk => clk_core, reset_n => reset_n, addr_i => dbg_addr, wen_i => dbg_wen, data_i => dbg_data_w, data_o => dbg_data_r);
process0 : process(clk_core,reset_n)

variable instr : unsigned ((32 -1) downto 0);
variable opcode : unsigned ((7 -1) downto 0);
variable opcode_is_load : std_logic;
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
variable csr_addr : unsigned ((12 -1) downto 0);
variable csr_wb : unsigned ((32 -1) downto 0);
variable next_PC : unsigned ((32 -1) downto 0);
variable load_from_instmem : std_logic;

variable core_mem_access : std_logic;
variable stop_PC : BOOLEAN;
variable load_data_ok : BOOLEAN;

begin




 IF ( reset_n = '0' ) then

  boot_mode <= '0';
  pipe <= TO_UNSIGNED(0,pipe'length);
  PC <= TO_UNSIGNED(0,PC'length);
  PCp <= TO_UNSIGNED(0,PCp'length);
  cpu_wait <= '0';
  cpu_wait_early <= '0';
  inst_cs_n <= '1' ;
  flush <= '0';
  alt_op <= '0';
  halt <= '0';
  use_immediate <= '0';
  csri <= '0';
  loading <= '0';
  mcounter <= TO_UNSIGNED(0,mcounter'length);
  mstatus <= TO_UNSIGNED(0,mstatus'length);
  mcause <= TO_UNSIGNED(0,mcause'length);
  medeleg <= TO_UNSIGNED(0,medeleg'length);
  mideleg <= TO_UNSIGNED(0,mideleg'length);
  mtvec <= TO_UNSIGNED(0,mtvec'length);
  mepc <= TO_UNSIGNED(0,mepc'length);
  mip <= TO_UNSIGNED(0,mip'length);
  mie <= TO_UNSIGNED(0,mie'length);
  mscratch <= TO_UNSIGNED(0,mscratch'length);
  rinstr <= TO_UNSIGNED(0,rinstr'length);
  rrinstr <= TO_UNSIGNED(0,rrinstr'length);
  ropcode <= TO_UNSIGNED(0,ropcode'length);

  rfunct3 <= TO_UNSIGNED(0,rfunct3'length);
  rrd <= TO_UNSIGNED(0,rrd'length);
  rrrd <= TO_UNSIGNED(0,rrrd'length);
  rrrdp <= TO_UNSIGNED(0,rrrdp'length);
  cpu_stuck_cnt <= TO_UNSIGNED(0,cpu_stuck_cnt'length);
  rwb <= TO_UNSIGNED(0,rwb'length);
  funct3wb <= TO_UNSIGNED(0,funct3wb'length);
  rshiftwb <= TO_UNSIGNED(0,rshiftwb'length);
  mask_data_en <= '0';
  rtrap <= '0';
  priv <= "11";
  load_mem <= '0';
  load_mem0 <= '0';
  cpu_wait_on_write <= '0';
  rrd_wr_en <= '0';
  dma_grant_o <= '0';

  mask_data_en <= '0';
  blk2mem_t0.cs_n <= '1' ;
  blk2mem_t0.wr_n <= '1' ;
  code_loaded <= '0';
  code_loaded0 <= '0';
  boot_mode <= '0';
  boot_modep <= '0';
  debug_write <= '1' ;
  dbg_stop <= TO_UNSIGNED(0,dbg_stop'length);

  elsif ( clk_core'event and (clk_core = '1' ) ) then

   mcounter <= mcounter + 1;
   flush <= '0';
   instr := instmem2core_i.data;
   next_PC := PC;
   opcode_is_load := '0';
   load_from_instmem := '0';
   trap := '0';
   taken := "0";
   wbe := "0000";
   wrd := "00000";
   opcode := TO_UNSIGNED(0,opcode'length);
   rrd_wr_en <= '0';
   dma_grant_o <= '0';
   rcan_grant <= '0';
   boot_mode <= boot_mode_i;
   boot_modep <= boot_mode;


   IF ( (pipe(0) = '1' ) and (cpu_wait = '0') and (flush = '0') ) then
     rinstr <= instr;
     opcode := (instr(6 downto 0));
     rd := (instr(11 downto 7));
     rs1 := (instr(19 downto 15));
     rs2 := (instr(24 downto 20));
     funct3 := (instr(14 downto 12));
     funct7 := (instr(31 downto 25));


     IF (opcode = OPI) then
      ropcode <= OP;
      IF (funct3 = "101") then
       alt_op <= instr(30);
      else
       alt_op <= '0';
      end if;
     else
      IF (code_loaded = '1' ) then
       ropcode <= opcode;
      else
       ropcode <= TO_UNSIGNED(0,ropcode'length);
      end if;
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
      when CASE_LOAD => immediate_type := I_type; opcode_is_load := '1' ;
      when CASE_STORE => immediate_type := S_type;
      when CASE_LUI => immediate_type := U_type;
      when CASE_AUIPC => immediate_type := U_type;
      when CASE_JAL => immediate_type := J_type;
      when CASE_JALR => immediate_type := I_type;
      when CASE_BRANCH => immediate_type := B_type;
      when others => immediate_type := no_type;
     end case;
     rimmediate_type <= immediate_type;


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
     elsif ( (immediate_type = J_type) or (immediate_type = B_type) or (opcode = AUIPC) or
       ( (opcode = SYS) and ((funct7(6 downto 1)) = "000000" ) ) ) then
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




   load_data_ok := ( ( (datamem2core_i.data_en = '1' ) and (load_mem = '0') and (mask_data_en = '0'))
         or ( (instmem2core_i.data_en = '1' ) and (load_mem = '1' )) );

   stop_PC := ( ( (ropcode = LOAD) and cpu_wait = '0' ) or
        ( (cpu_wait = '1' ) and not load_data_ok ) or
      (halt = '1' ) );


   IF (not stop_PC) then
    rdbg <= '1' ;
    pipe <= ( (pipe(pipe'high-1 downto 0)) & '1' );
    PCp <= PC;
    next_PC := PC + TO_UNSIGNED(4,PC'length);
    inst_cs_n <= '0';
   else
   rdbg <= '0';
    inst_cs_n <= '0';
   end if;

   blk2mem_t0.wr_n <= '1' ;



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
    csr_val := TO_UNSIGNED(0,32);
    radd_res <= add_res;
    rsub_res <= sub_res;
    rrs1_lt_rs2_u <= rs1_lt_rs2_u;
    rrs1_lt_rs2_s <= rs1_lt_rs2_s;
    rrs1_eq_rs2 <= rs1_eq_rs2;

    rwb <= "00000";
    rd_val := "10101010010101011010101001010101";
    rjalr <= "00000";
    load_mem0 <= '0';

    case ropcode is
     when CASE_SYS =>
      case (rimmediate(11 downto 0)) is
       when CASE_AMSTATUS => csr_val := mstatus;

       when CASE_AMEDELEG => csr_val := medeleg;
       when CASE_AMIDELEG => csr_val := mideleg;
       when CASE_AMIE => csr_val := mie;
       when CASE_AMTVEC => csr_val := mtvec;

       when CASE_AMIP => csr_val := mip;
       when CASE_AMSCRATCH => csr_val := mscratch;
       when CASE_AMEPC => csr_val := mepc;
       when CASE_AMCAUSE => csr_val := mcause;
       when others => csr_val := TO_UNSIGNED(0,csr_val'length);
      end case;
      case rfunct3 is
       when CASE_CSRRW => csr_val := op1;
       when CASE_CSRRS => csr_val := csr_val or op1;
       when CASE_CSRRC => csr_val := csr_val and not op1;
       when CASE_CSRRWI => csr_val := op1;
       when CASE_CSRRSI => csr_val := csr_val or op1;
       when CASE_CSRRCI => csr_val := csr_val and not op1;
       when CASE_ECALL =>
       IF (use_immediate = '0') then

        mepc <= rimmediate; cause := ("0000000000000000000000000000" & (not rrs2(0)) & "0" & priv); trap := '1' ; -- gprintf("#VECALL trap addr", to_hex(TO_INTEGER(mepc)));

        elsif ( ( rfunct7 = "0011000" ) and ( rrs2 = "00010" ) ) then
        next_PC := mepc; flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length); ropcode <= TO_UNSIGNED(0,ropcode'length);
       end if;
       when others => csr_val := TO_UNSIGNED(0,csr_val'length);
      end case;
      case (rimmediate(11 downto 0)) is
       when CASE_AMSTATUS => mstatus <= csr_val;
       when CASE_AMEDELEG => medeleg <= csr_val;
       when CASE_AMIDELEG => mideleg <= csr_val;
       when CASE_AMIE => mie <= csr_val;
       when CASE_AMTVEC => mtvec <= csr_val;

       when CASE_AMIP => mip <= csr_val;
       when CASE_AMSCRATCH => mscratch <= csr_val;
       when CASE_AMEPC => mepc <= csr_val;
       when CASE_AMCAUSE => mcause <= csr_val;
       when others => csr_val := csr_val ;
      end case;
      rd_val := csr_val;
     when CASE_MEM =>
     when CASE_OP =>
      case rfunct3 is
       when CASE_ADDx => IF (alt_op = '0') then rd_val := RESIZE(add_res, 32); else rd_val := RESIZE(sub_res, 32); end if;
       when CASE_SLLx => rd_val := sll_res;
       when CASE_SRLx => IF (alt_op = '0') then rd_val := srl_res_u; else rd_val := srl_res_s; end if;
       when CASE_SLTx => rd_val := RESIZE(rs1_lt_rs2_s, 32);
       when CASE_SLTU => rd_val := RESIZE(rs1_lt_rs2_u, 32);
       when CASE_ORx => rd_val := or_res;
       when CASE_XORx => rd_val := xor_res;
       when CASE_ANDx => rd_val := and_res;
       when others => trap := '1' ; cause := ILLINSTR; if (not boot_mode = '1' ) then -- gprintf("#VILLINSTR OP");
                  end if;
      end case;
     when CASE_LOAD =>
      IF (not (rrd = "00000")) then
       cpu_wait <= '1' ;
       rwb <= rrd;
       funct3wb <= rfunct3;
       rshiftwb <= nshift;
       mask_data_en <= '1' ;

       if ((add_res(31 downto 28)) = "0000") then

         inst_addr <= (add_res(blk2mem_t0.addr'length+1 downto 2));

         inst_cs_n <= '0';
         load_from_instmem := '1' ;
         load_mem0 <= '1' ;







        else
         load_mem0 <= '0';
         blk2mem_t0.cs_n <= '0';


        end if;

        blk2mem_t0.addr <= (add_res(blk2mem_t0.addr'length+1 downto 2));
        blk2mem_t0.wr_n <= '1' ;
        blk2mem_t0.be <= "1111";
      end if;
     when CASE_STORE =>
      wbe := ( rfunct3(1) & rfunct3(1) & (rfunct3(0) or rfunct3(1)) & '1' );
      wbe := SHIFT_LEFT(wbe, TO_INTEGER(nshift));
      blk2mem_t0.addr <= (add_res(blk2mem_t0.addr'length+1 downto 2));
      blk2mem_t0.cs_n <= '0';
      blk2mem_t0.wr_n <= '0';
      blk2mem_t0.be <= wbe;
      blk2mem_t0.data <= SHIFT_LEFT(regs(TO_INTEGER(rrs2)), TO_INTEGER(nshift & "000"));

     when CASE_LUI => rd_val := rimmediate;
     when CASE_AUIPC => rd_val := rimmediate;
     when CASE_JAL => next_PC := rimmediate; rd_val := PCp; flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length);ropcode <= TO_UNSIGNED(0,ropcode'length);
     when CASE_JALR => next_PC := RESIZE( ( (add_res(31 downto 1)) & "0" ), PC'length);
      rd_val := PCp; flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length); rjalr <= rrd; ropcode <= TO_UNSIGNED(0,ropcode'length);
     when CASE_BRANCH =>
      taken := ( ( ( ( (rfunct3(1 downto 1)) and (rs1_lt_rs2_u) ) or ( (not (rfunct3(1 downto 1))) and (rs1_lt_rs2_s) ) ) and (rfunct3(2 downto 2)) ) or
         ( rs1_eq_rs2 and not (rfunct3(2 downto 2)) ) ) xor (rfunct3(0 downto 0));

      IF (taken = "1") then
       next_PC := rimmediate; flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length); ropcode <= TO_UNSIGNED(0,ropcode'length);
      end if;
     when others => trap := '1' ; cause := ILLINSTR; if (not boot_mode = '1' ) then -- gprintf("ILLINSTR trap opcode");
                end if;

    end case;
    rtaken <= taken;
    rrd_val <= rd_val;

    IF ( (not (ropcode = LOAD) and not (ropcode = STORE) ) or ( load_from_instmem = '1' ) ) then

     blk2mem_t0 <= dma_request_i;
     dma_grant_o <= not dma_request_i.cs_n;
     rcan_grant <= '1' ;
    else
     rcan_grant <= '0';
    end if;

    wrd := rrd;
   else

    blk2mem_t0.cs_n <= '1' ;
    exec <= '0';
    rd_val := "10101010010101011010101001010101";
    blk2mem_t0 <= dma_request_i;
    dma_grant_o <= not dma_request_i.cs_n;
    rcan_grant <= '1' ;
   end if;

   load_mem <= load_mem0;


   IF (cpu_wait_on_write = '1' ) then
    cpu_wait <= '0';
    cpu_wait_on_write <= '0';
   end if;


   IF ( not(rwb = TO_UNSIGNED(0,rwb'length)) ) then
    mask_data_en <= '0';
   end if;


   IF ( not(rwb = TO_UNSIGNED(0,rwb'length)) and ( mask_data_en = '0' ) and ( ( (datamem2core_i.data_en = '1' ) and (load_mem = '0')) or ( (instmem2core_i.data_en = '1' ) and (load_mem = '1' ) ) ) ) then
    cpu_wait <= '0';
    load_mem <= '0';
    load_mem0 <= '0';
    IF (load_mem = '0') then
     ld_data := datamem2core_i.data;
    else
     ld_data := instmem2core_i.data;
    end if;
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
   IF ( wrd = "00000" ) then
    rrd_wr_en <= '0';
    rrd_wr <= TO_UNSIGNED(0,32);
   else
    rrd_wr_en <= '1' ;
    rrd_wr <= rd_val;
   end if;
   rrd_wr_enp <= rrd_wr_en;
   rrd_wrp <= rrd_wr;
   rrrd <= wrd;
   rrrdp <= rrrd;


   regs(0) <= TO_UNSIGNED(0,32);


   IF (load_port_i.wr_n = '0' ) then

    loading <= '1' ;
    inst_cs_n <= '0';
    next_PC := RESIZE( ( load_port_i.addr & "00" ), PC'length );
    PCp <= load_port_i.data;
   elsif ( loading = '1' ) then
     code_loaded <= '1' ;
    -- gprintf("#RCODE LOADED, starting Giorno core");
    next_PC := TO_UNSIGNED(0,PC'length);
    inst_cs_n <= '0';
    loading <= '0';
    flush <= '1' ;
    pipe <= TO_UNSIGNED(0,pipe'length);
    cpu_wait <= '0';
   end if;


   IF ( (boot_mode = '1' ) and (boot_modep = '0') ) then
    code_loaded <= '0';
    code_loaded0 <= '0';
   end if;

   IF (boot_modep = '1' ) then
    next_PC := TO_UNSIGNED(0,PC'length);

    inst_cs_n <= dma_request_i.cs_n;
    code_loaded0 <= ( code_loaded0 or not dma_request_i.cs_n );

    IF (boot_mode = '0') then
      -- gprintf("#RCODE LOADED, starting Giorno core");
      next_PC := TO_UNSIGNED(0,PC'length);
      inst_cs_n <= '0';
      loading <= '0';
      flush <= '1' ;
      pipe <= TO_UNSIGNED(0,pipe'length);
      cpu_wait <= '0';
      code_loaded <= code_loaded0;
      ropcode <= TO_UNSIGNED(0,ropcode'length);

      PCp <= TO_UNSIGNED(0,PCp'length);
      alt_op <= '0';
      halt <= '0';
      use_immediate <= '0';
      csri <= '0';
      rfunct3 <= TO_UNSIGNED(0,rfunct3'length);
      rrd <= TO_UNSIGNED(0,rrd'length);

      rwb <= TO_UNSIGNED(0,rwb'length);
      funct3wb <= TO_UNSIGNED(0,funct3wb'length);
      rshiftwb <= TO_UNSIGNED(0,rshiftwb'length);
      mask_data_en <= '0';
      rtrap <= '0';
      priv <= "11";
      load_mem <= '0';
      load_mem0 <= '0';
      cpu_wait_on_write <= '0';
      rrd_wr_en <= '0';
      dma_grant_o <= '0';


    else
     inst_addr <= dma_request_i.addr;
     PCp <= dma_request_i.data;
    end if;

   end if;

   rtrap <= (trap and code_loaded);

   IF ((not (loading = '1' ) and not (boot_mode = '1' )) and (trap = '1' )) then
    trap_addr_base := mtvec and "11111111111111111111111111111100";
    IF ((mtvec(1 downto 0)) = "01" ) then
     trap_addr_offset := (cause(31 downto 2)) & "00";
    else
     trap_addr_offset := TO_UNSIGNED(0,trap_addr_offset'length);
    end if;
    next_PC := trap_addr_base + trap_addr_offset;
    flush <= '1' ; pipe <= TO_UNSIGNED(0,pipe'length); ropcode <= TO_UNSIGNED(0,ropcode'length);

    mcause <= cause;
   end if;

   PC <= next_PC;
   rload_from_instmem <= load_from_instmem;

   IF ( (load_from_instmem = '0') and not (boot_mode = '1' ) ) then
    inst_addr <= (next_PC(blk2mem_t0.addr'length+1 downto 2));
   end if;

   if ( (rtrap = '1' ) ) then
    debug_write <= '0';
   elsif ( boot_modep = '1' ) then
    debug_write <= '1' ;
   end if;

   dbg_stop <= ((dbg_stop(6 downto 0)) & (not debug_write) );

   IF (dbg_stop(7) = '0') then
    dbg_data_w <= (rinstr & rdbg & cpu_wait & flush & exec & rtrap & datamem2core_i.data_en & rwb & rrd & (PC(15 downto 0)));
    dbg_addr <= dbg_addr + 1;
    dbg_wen <= '0';
    dbg_cnt <= "0000000";
    dbg_cnt2 <= "00000";
    dbg_o <= ('0' & (dbg_data_r(63 downto 32)));

   elsif (dbg_cnt2 < "11111") then
    dbg_wen <= '1' ;
    IF (dbg_cnt = "0000000") then

        dbg_cnt <= TO_UNSIGNED(71,7);
    else
     dbg_cnt <= dbg_cnt - 1;
    end if;
    IF (dbg_cnt = TO_UNSIGNED(71,7)) then
     dbg_o <= ('1' & (dbg_data_r(31 downto 0)));
    end if;
    IF (dbg_cnt = TO_UNSIGNED(35,7)) then
     dbg_o <= ('1' & (dbg_data_r(63 downto 32)));
     dbg_cnt2 <= dbg_cnt2 + 1;
     dbg_addr <= dbg_addr + 1;

    end if;
   else
    dbg_o <= ('0' & (dbg_data_r(63 downto 32)));
   end if;

 end if;
 end process;



  core2instmem_o.addr <= inst_addr;
  core2instmem_o.data <= PCp;
  core2instmem_o.cs_n <= inst_cs_n;
  core2instmem_o.wr_n <= not (loading or boot_mode);
  core2instmem_o.be <= "1111";
  core2datamem_o <= blk2mem_t0;
  addr_rs1 <= (instmem2core_i.data(19 downto 15));
  addr_rs2 <= (instmem2core_i.data(24 downto 20));
  trap_o <= rtrap;
end rtl;
