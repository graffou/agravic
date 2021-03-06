// Giorno Risc-V core september, 2019
// ----------------------------------
// This is a simple, 3-stage pipeline implementation of RISC-V rv32i instruction set
// Written using the Agravic simulation and rtl generation platform

#include "slv.h"
#include "../FIRMWARE/Include/reg_def.h"// for VHDL generation
// To add a CPU wait cycle when doing stores
// Should not be needed, but sometimes helps for debug
//#define STORE_CPU_WAIT

START_OF_FILE(risc-V_core)
INCLUDES
USE_PACKAGE(slv_utils)
USE_PACKAGE(structures)
USE_PACKAGE(risc_V_constants)
#ifndef VHDL
std::ofstream tarmac("tarmac");
#endif

ENTITY(risc_V_core,
DECL_PORTS(
		PORT(clk_core, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(boot_mode_i, BIT_TYPE, IN), // for code loading
		PORT(trap_o, BIT_TYPE, OUT), // for code loading
		PORT(dbg_o, UINT(33), OUT), // for code loading
		// core to instruction memory
		PORT(load_port_i, blk2mem_t, IN),
		PORT(core2instmem_o, blk2mem_t, OUT),
		PORT(instmem2core_i, mem2blk_t, IN),
		// core to data memory
		PORT(core2datamem_o, blk2mem_t, OUT),
		//PORT(dma2core_i, blk2mem_t, IN), // These are DMA write requests from peripherals / are transmitted when mem bus is not used by core
		PORT(dma_request_i, blk2mem_t, IN),
		PORT(dma_grant_o, BIT_TYPE, OUT),
		PORT(datamem2core_i, mem2blk_t, IN),
		PORT(csr2core_i, csr2core_t, IN),
		// GPIOs
		//PORT(gpios_o, UINT(32), OUT)
		)
);

COMPONENT(register_file,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(addr_rs1_i, UINT(5), IN),
		PORT(addr_rs2_i, UINT(5), IN),
		PORT(addr_rd_i, UINT(5), IN),
		PORT(wb_en_i, BIT_TYPE, IN),
		PORT(wb_i, UINT(32), IN),
		PORT(rs1_o, UINT(32), OUT),
		PORT(rs2_o, UINT(32), OUT)
		)
);

COMPONENT(dbg_mem,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(addr_i, UINT(5), IN),
		PORT(data_i, UINT(64), IN),
		PORT(wen_i, BIT_TYPE, IN),
		PORT(data_o, UINT(64), OUT)
		)
);

#ifndef VHDL
gstring tabs;
//std::ofstream tarmac("tarmac");
#endif

// Internal registers
TYPE(reg_file_t, ARRAY_TYPE(UINT(32), 32));
SIG(regs, reg_file_t);
SIG(PC, UINT(32));
SIG(PCp, UINT(32));
SIG(PCpp, UINT(32)); // for tarmac


SIG(loading, BIT_TYPE); // 1 when loading code
SIG(boot_mode, BIT_TYPE); // 1 when loading code
SIG(boot_modep, BIT_TYPE); // 1 when loading code
SIG(load_mem0, BIT_TYPE); // 1 when loading from program memory, 0 when loading from data memory
SIG(load_mem, BIT_TYPE); // 1 when loading from program memory, 0 when loading from data memory
SIG(rtrap, BIT_TYPE); // 1 when loading code
SIG(alt_op, BIT_TYPE); // 1 when alternate op is required (funct7 = 0100000)
SIG(cpu_wait_early, BIT_TYPE); // 1 halts pipeline (is set just when load instr is detected
SIG(cpu_wait, BIT_TYPE); // 1 halts pipeline
SIG(mask_data_en, BIT_TYPE); // 1 to mask data_en from memory 1 cycle after load: data_en could be the result of a dma access
SIG(cpu_wait_on_write, BIT_TYPE); // 1 halts pipeline after write on successive load
SIG(flush, BIT_TYPE); // 1 flushes instruction in decode / execute stages
//SIG(jmp_op, BIT_TYPE); // 1 when instruction is a jump or a branch, so that instruction is fetched in second half of the cycle
SIG(rimmediate, UINT(32));
SIG(rimmediate_type, UINT(3));
SIG(use_immediate, BIT_TYPE);
SIG(raw_opcode, UINT(7));
SIG(ropcode, UINT(7));
SIG(rcsr_op_with_read, BIT_TYPE);
SIG(rropcode, UINT(7));
SIG(rfunct3, UINT(3));
SIG(rop1, UINT(32));
SIG(rop2, UINT(32));
SIG(rop1_rf, UINT(32));
SIG(rop2_rf, UINT(32));
SIG(rs1_rd, UINT(32));
SIG(rs2_rd, UINT(32));
SIG(rrd_wrp, UINT(32));
SIG(rrd_wr, UINT(32));
SIG(rrd_wr_en, BIT_TYPE);
SIG(rrd_wr_enp, BIT_TYPE);

SIG(rfunct7, UINT(7));
SIG(rrd, UINT(5));
SIG(rrrd, UINT(5));
SIG(rrrdp, UINT(5));
SIG(rrs1, UINT(5));
SIG(rrs2, UINT(5));
SIG(addr_rs1, UINT(5));
SIG(addr_rs2, UINT(5));
SIG(rrd_val, UINT(32)); // debug
SIG(rtaken, UINT(1)); // debug

SIG(halt, BIT_TYPE); // 1 to stop CPU
SIG(dma_rd_wait, BIT_TYPE); // other way  to stop CPU
SIG(dma_granted, BIT_TYPE); // 1 cycle =1 at dma grant (avoid double cs_n == 0)
SIG(inst_cs_n, BIT_TYPE); // 1 when executing instruction
SIG(exec, BIT_TYPE); // 1 when executing instruction
SIG(pipe, UINT(4));
SIG(rwb, UINT(5)); //writeback register
SIG(funct3wb, UINT(3));//writeback method
SIG(rshiftwb, UINT(2)); //load value shift (in bytes)

// CSR and friends
SIG(csri, BIT_TYPE); // 1 when CSR*I instruction
SIG(priv, UINT(2));
SIG(blk2mem_t0, blk2mem_t);
SIG(mem2blk_t0, mem2blk_t);
SIG(rinstr, UINT(32)); // debug
SIG(rrinstr, UINT(32)); // debug
SIG(inst_addr, UINT(data_addr_span - 2));
SIG(inst_data, UINT(32));
SIG(cur_irq, UINT(6));
SIG(start_irq, BIT_TYPE);//
SIG(reset_cur_irq, BIT_TYPE);//

//SIG(write_through1, BIT_TYPE);// debug
//SIG(write_through2, BIT_TYPE);// debug

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
SIG(rload_from_instmem, BIT_TYPE);// debug
SIG(wait_for_interrupt, BIT_TYPE);// debug

SIG(rdbg, BIT_TYPE);// debug
SIG(rdbg2, BIT_TYPE);// debug
SIG(rcan_grant, BIT_TYPE);// debug
SIG(code_loaded, BIT_TYPE);// debug
SIG(code_loaded0, BIT_TYPE);// debug
SIG(cpu_stuck_cnt, UINT(5));// debug

SIG(cond1, BIT_TYPE);// debug
SIG(cond2, BIT_TYPE);// debug
SIG(cond3, BIT_TYPE);// debug
SIG(cond4, BIT_TYPE);// debug
SIG(cond5, BIT_TYPE);// debug
SIG(dbg_pipe, UINT(32));// debug


SIG(dbg_addr, UINT(5));
SIG(dbg_data_w, UINT(64));
SIG(dbg_data_r, UINT(64));
SIG(dbg_wen, BIT_TYPE);
SIG(dbg_cnt, UINT(7));
SIG(dbg_cpu_wait, UINT(8)); // goto trap if cpu_wait stalled
SIG(dbg_ropcode, UINT(8)); // goto trap if cpu_wait stalled
SIG(dbg_cnt2, UINT(5));
SIG(debug_write, BIT_TYPE);
SIG(dbg_stop, UINT(8));
SIG(dbg_sys_op, BIT_TYPE);
SIG(dbg_can_read, BIT_TYPE);
SIG(dbg_toggle, BIT_TYPE);
SIG(dbg_csr_write, BIT_TYPE);
SIG(dbg_csr_read, BIT_TYPE);
SIG(dbg_illinstr, BIT_TYPE);

BEGIN

BLK_INST(u0_register_file, register_file,
MAPPING(
		PM(clk, clk_core),
		PM(reset_n, reset_n),
		PM(addr_rs1_i, addr_rs1),
		PM(addr_rs2_i, addr_rs2),
		PM(addr_rd_i, rrrd),
		PM(wb_en_i, rrd_wr_en),
		PM(wb_i, rrd_wr),
		PM(rs1_o, rs1_rd),
		PM(rs2_o, rs2_rd)
		)
);

BLK_INST(u0_dbg_mem, dbg_mem,
		MAPPING(
				PM(clk, clk_core),
				PM(reset_n, reset_n),
				PM(addr_i, dbg_addr),
				PM(wen_i, dbg_wen),
				PM(data_i, dbg_data_w),
				PM(data_o, dbg_data_r)
				)
		);

PROCESS(0, clk_core, reset_n)
//std::cerr << '<';
VAR(instr, UINT(32));
VAR(opcode, UINT(7));
VAR(opcode_is_load, BIT_TYPE);
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
VAR(csr_be, UINT(3)); // 3 lsbs of be in CSR mode
VAR(taken, UINT(1)); // condition for branch taken
VAR(rd_val, UINT(32));
VAR(trap, BIT_TYPE); // going to trap
VAR(cause, UINT(32)); // trap cause
VAR(trap_addr_base, UINT(32)); // base trap addr
VAR(trap_addr_offset, UINT(32)); // trap offset
VAR(csr_val, UINT(32)); // CSR read value
VAR(csr_addr, UINT(12)); // CSR addr value
VAR(csr_wb, UINT(32)); // CSR writeback value
VAR(next_PC, UINT(32));
VAR(load_from_instmem, BIT_TYPE);

VAR(core_mem_access, BIT_TYPE); // 1 when core accesses data memory (and thus dma cannot)
VAR(stop_PC, BOOLEAN); // 1 when core accesses data memory (and thus dma cannot)
VAR(load_data_ok, BOOLEAN); // 1 when core accesses data memory (and thus dma cannot)
VAR(csr_read, BIT_TYPE); // CSR inst. leads to CSR reg read -> cpu_wait state
VAR(csr_write, BIT_TYPE); // CSR inst. leads to CSR reg read -> cpu_wait state
VAR(csr_op, BIT_TYPE); // CSR operation required (CSRRW etc.)
VAR(wfi, BIT_TYPE); //
VAR(mret, BIT_TYPE); //
VAR(must_start_irq, BOOLEAN); //

BEGIN




	IF ( reset_n == BIT(0) ) THEN
	// reset statements
		RESET(pipe);
		PC <= HEX(FFFFFFFC);
		RESET(PCp);
		cpu_wait <= BIT(0);
		cpu_wait_early <= BIT(0);
		inst_cs_n <= BIT(1);
		flush <= BIT(0);
		alt_op <= BIT(0);
		halt <= BIT(0);
		use_immediate <= BIT(0);
		csri <= BIT(0);
		loading <= BIT(0);
		RESET(rinstr);
		RESET(rrinstr);
		RESET(ropcode);
		RESET(dbg_cpu_wait);

		RESET(rfunct3);
		RESET(rrd);
		RESET(rrrd);
		RESET(rrrdp);
		RESET(cpu_stuck_cnt);
		RESET(rwb);
		RESET(funct3wb);
		RESET(rshiftwb);
		mask_data_en <= BIT(0);
		rtrap <= BIT(0);
		priv <= BIN(11); // Machine mode is default
		load_mem <= BIT(0);
		load_mem0 <= BIT(0);
		cpu_wait_on_write <= BIT(0);
		rrd_wr_en <= BIT(0);
		dma_grant_o <= BIT(0);
		dma_granted <= BIT(0);
		dma_rd_wait <= BIT(0);

		mask_data_en <= BIT(0);
		wait_for_interrupt <= BIT(0);
		blk2mem_t0.cs_n <= BIT(1);
		blk2mem_t0.wr_n <= BIT(1);
#ifdef VHDL
		code_loaded <= BIT(0);
		code_loaded0 <= BIT(0);
#else
		// simulation: directly load code in TB
		code_loaded <= BIT(1);
		code_loaded0 <= BIT(1);
#endif
		boot_mode <= BIT(0);
		boot_modep <= BIT(1); // !!!!!!!!! So that core does not start too fast after reset (simu only)
		debug_write <= BIT(1);
		RESET(dbg_stop);
		rcsr_op_with_read <= BIT(0);
		cur_irq <= BIN(000000); // no IRQ - IRQs start at 100000
		start_irq <= BIT(0);
		dma_granted <= BIT(0);

		ELSEIF ( EVENT(clk_core) and (clk_core == BIT(1)) ) THEN
			// rising edge
			flush <= BIT(0);
			instr = PORT_BASE(instmem2core_i).data;
			next_PC = PC;
			opcode_is_load = BIT(0);
			load_from_instmem = BIT(0);
			trap = BIT(0);
			taken = BIN(0);
			wbe = BIN(0000);
			wrd = BIN(00000);
			opcode = TO_UINT(0, LEN(opcode));
			rrd_wr_en <= BIT(0);
			dma_grant_o <= BIT(0);
			rcan_grant <= BIT(0);
			boot_mode <= boot_mode_i;
			boot_modep <= boot_mode;
			// Instruction decoding -------------------------------------------------------------------
			IF ( (B(pipe,0) == BIT(1)) and (cpu_wait == BIT(0)) and (flush == BIT(0)) ) THEN 			//IF ( (PORT_BASE(instmem2core_i).data_en == BIT(1)) and (cpu_wait == BIT(0)) ) THEN
					rinstr <= instr;
					PCpp <= PCp;
					opcode = RANGE(instr, 6, 0);
					rd = RANGE(instr, 11, 7);
					rs1 = RANGE(instr, 19, 15);
					rs2 = RANGE(instr, 24, 20);
					funct3 = RANGE(instr, 14, 12);
					funct7 = RANGE(instr, 31, 25);

					// Condition to branch to IRQ + PG no nested IRQs so test cur_irq = 0 (resetted on mret)
					must_start_irq = ( (PORT_BASE(csr2core_i).irq == BIT(1)) and (cur_irq == TO_UINT(0, LEN(cur_irq))) ); //not (PORT_BASE(csr2core_i).cause == cur_irq) );

					// Anyway, difference between OP and OPI is held in immediate choice, made one cycle later
					IF (opcode == OPI) THEN
						dbg_ropcode <= TO_UINT(1, 8);
						ropcode <= OP;
						IF (funct3 == BIN(101)) THEN // SLL, SRA... imm is only 5-bit long, so funct7 holds information
							alt_op <= B(instr, 30);
						ELSE
							alt_op <= BIT(0);//B(instr, 30);
						ENDIF
					ELSE
						IF (code_loaded == BIT(1)) THEN
							ropcode <= opcode;
							dbg_ropcode <= TO_UINT(2, 8);
						ELSE
							RESET(ropcode);// <= opcode;
							dbg_ropcode <= TO_UINT(3, 8);
						ENDIF
						alt_op <= B(instr, 30);
					ENDIF

					rfunct3 <= funct3;
					rfunct7 <= funct7;
					rrs1 <= rs1;
					rrs2 <= rs2;

					//Decode immediate value
					SWITCH(opcode) // Determine type of immediate arg.
						CASE(CASE_SYS) immediate_type = I_type;
						CASE(CASE_MEM) immediate_type = I_type;
						CASE(CASE_OPI) immediate_type = I_type;
						CASE(CASE_LOAD) immediate_type = I_type; opcode_is_load = BIT(1);
						CASE(CASE_STORE) immediate_type = S_type;
						CASE(CASE_LUI) immediate_type = U_type;
						CASE(CASE_AUIPC) immediate_type = U_type;
						CASE(CASE_JAL) immediate_type = J_type;
						CASE(CASE_JALR) immediate_type = I_type;
						CASE(CASE_BRANCH) immediate_type = B_type;
						DEFAULT immediate_type = no_type;
					ENDCASE
					rimmediate_type <= immediate_type;

					// Decode immediate argument. !! USE SLV_RANGE instead of RANGE (returns slv and not unsigned) to generate slices, otherwise a & b does not compile in vhdl
					SWITCH(immediate_type)
						CASE(CASE_I_type) immediate = SXT( RANGE(instr, 31, 20), LEN(immediate) );
						CASE(CASE_S_type) immediate = SXT( SLV_RANGE(instr, 31, 25) & SLV_RANGE(instr, 11, 7), LEN(immediate));
						CASE(CASE_B_type) immediate = SXT( SLV_RANGE(instr, 31, 31) & SLV_RANGE(instr, 7, 7) & SLV_RANGE(instr, 30,25) & SLV_RANGE(instr,11,8) & BIT(0), LEN(immediate));
						CASE(CASE_U_type) immediate = SXT( SLV_RANGE(instr, 31, 20) & SLV_RANGE(instr,19,12) & BIN(000000000000), LEN(immediate));
						CASE(CASE_J_type) immediate = SXT( SLV_RANGE(instr, 31, 31) & SLV_RANGE(instr,19,12) & SLV_RANGE(instr, 20,20) & SLV_RANGE(instr,30,21) & BIN(0), LEN(immediate));
						DEFAULT immediate = TO_UINT(0, LEN(immediate));
					ENDCASE

					// Set destination register when available / no one when load/store
					IF ( (immediate_type == B_type) or (immediate_type == S_type) ) THEN
						rrd <= BIN(00000);
					ELSE
						rrd <= rd;
					ENDIF

					//determine whether next CSR instr requires pipeline stop
					rcsr_op_with_read <= BOOL2BIT( ( (opcode == SYS) and  not (rd == BIN(00000)) ) or // regular CSRRWSC
							( (funct7 == BIN(0001000)) and (rs2 == BIN(00101)) and (rd == BIN(00000)) ) ); //WFI
					dbg_can_read <= BOOL2BIT( ( not (rd == BIN(00000)) ) );
					dbg_sys_op <= BOOL2BIT( (opcode == SYS) );
					dbg_toggle <= (not dbg_toggle);
					raw_opcode <= opcode;

					// To start IRQ: use ECALL to update MEPC and MCAUSE + force trap (with start_irq signal)
					IF (must_start_irq) THEN
						immediate_type = J_type;
						//opcode = SYS; // not required because of j_type
						ropcode <= SYS;
						dbg_ropcode <= TO_UINT(4, 8);
						start_irq <= BIT(1);
						rfunct3 <= ECALL;
						immediate = TO_UINT(0, 32); //PG: not +4 !
						rrd <= BIN(00000);// !!!! otherwise a reg might be corrupted when entering ISR
					ENDIF
					//cur_irq <= PORT_BASE(csr2core_i).cause; // Keeps from running same IRQ forever

					IF (immediate_type == no_type) THEN
						use_immediate <= BIT(0);
					ELSEIF ( (immediate_type == J_type) or (immediate_type == B_type) or (opcode == AUIPC) or
							//( (opcode == SYS) and (RANGE(funct7, 6, 1) == BIN(000000) ) ) ) THEN // JAL /Branch/AUIPC or ECALL/EBREAK require instruction address (current one for ECALL)
							( (opcode == SYS) and (RANGE(instr, 31, 21) == BIN(00000000000) ) ) ) THEN // JAL /Branch/AUIPC or ECALL/EBREAK require instruction address (current one for ECALL)
						rimmediate <= immediate + PCp; // precompute address offset here // For ECALL, immediate = 0, so just get PCp // TODO ??? sure ? had to put +4 for IRQs!!!
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

			// first condition is to stop as soon as possible. Then cpu_wait is raised: unlock when data is available
			// this new conditions is because of data_en from dma accesses

			load_data_ok = ( ( (PORT_BASE(datamem2core_i).data_en == BIT(1)) and (load_mem == BIT(0)) and (mask_data_en == BIT(0))) // end of load from data mem
									or ( (PORT_BASE(instmem2core_i).data_en == BIT(1)) and (load_mem == BIT(1))) );

			//stop_PC = ( ( (ropcode == LOAD) and cpu_wait == BIT(0) ) or
			stop_PC = ( ( ( (ropcode == LOAD) or (rcsr_op_with_read == BIT(1)) ) and cpu_wait == BIT(0) ) or
					   ( (cpu_wait == BIT(1)) and not load_data_ok )  or
						(halt == BIT(1)) or
						(wait_for_interrupt == BIT(1)));//


			IF (not stop_PC) THEN
				rdbg <= BIT(1);
				pipe <= ( RANGE(pipe, HI(pipe)-1, 0) & BIT(1) );
				PCp <= PC;
				next_PC = PC + TO_UINT(4, LEN(PC));
				inst_cs_n <= BIT(0);
			ELSE
				rdbg <= BIT(0);
				inst_cs_n <= BIT(0); // Set 1 here is wrong when reading from instruction memory!!!! -> no cs_n means no instruction read after read
			ENDIF

			blk2mem_t0.wr_n <= BIT(1);


			// Execute instruction ----------------------------------------------------------------------
			IF ( (B(pipe, 1) == BIT(1)) and (cpu_wait == BIT(0)) and (flush == BIT(0)) ) THEN

				exec <= BIT(1);
#ifndef VHDL
				gprintf(tarmac, "%dns IT % %d\n", double(cur_time)/256000, to_hex(TO_INTEGER(PCpp)), to_hex(TO_INTEGER(rinstr)));
#else
#endif
				rrinstr <= rinstr;

				IF (csri == BIT(1)) THEN// CSR instruction with immediate arg.
					op1 = RESIZE(rrs1, LEN(op1));
				ELSE
					op1 = regs(TO_INTEGER(rrs1));
				ENDIF
				IF ( use_immediate == BIT(1) ) THEN
					op2 = rimmediate;
				ELSE
					op2 = regs(TO_INTEGER(rrs2));
				ENDIF

				rop1 <= op1;
				rop2 <= op2;

				// Arithmetic operations and results
				add_res = EXT(op1, LEN(add_res)) + EXT(op2, LEN(add_res));
				sub_res = EXT(op1, LEN(add_res)) - EXT(op2, LEN(add_res));
				rs1_lt_rs2_u = RESIZE(RANGE(sub_res, 32,32),1);
				rs1_lt_rs2_s = RESIZE(RANGE(sub_res, 32, 32) xor RANGE(op1, 31,31) xor RANGE(op2, 31,31),1);

				IF (sub_res == TO_UINT(0, LEN(sub_res))) THEN
					rs1_eq_rs2 = BIN(1);
				ELSE
					rs1_eq_rs2 = BIN(0);
				ENDIF

				sll_res = SHIFT_LEFT(op1, TO_INTEGER(RESIZE(op2, 5)));
				srl_res_u = SHIFT_RIGHT(op1, TO_INTEGER(RESIZE(op2, 5)));
				nshift = RESIZE(add_res, LEN(nshift)); // shift data for LH/LB/SH/SB
				srl_res_s = UNSIGNED(SHIFT_RIGHT(SIGNED(op1), TO_INTEGER(RESIZE(op2, 5))));
				and_res = op1 and op2;
				or_res = op1 or op2;
				xor_res = op1 xor op2;
				trap = BIT(0);
				taken = BIN(0);
				csr_val = TO_UINT(0, 32);

				radd_res <= add_res;
				rsub_res <= sub_res;
				rrs1_lt_rs2_u <= rs1_lt_rs2_u;
				rrs1_lt_rs2_s <= rs1_lt_rs2_s;
				rrs1_eq_rs2 <= rs1_eq_rs2;

				rwb <= BIN(00000);
				rd_val = BIN(10101010010101011010101001010101); // default
				rjalr <= BIN(00000);
				load_mem0 <= BIT(0);
				reset_cur_irq  <= BIT(0); 

				SWITCH(ropcode) // Execute opcode
					CASE(CASE_SYS) // CSR R/W -- is now a R/W to csr_irq block
						csr_write = BOOL2BIT(not (rrs1 == TO_UINT(0, LEN(rrd))) ); // works for rs1 being reg id or rs1 being an immediate. Must be != 0 anyway (spec)
						csr_read  = BOOL2BIT(not (rrd == TO_UINT(0, LEN(rrd))));
						dbg_csr_write <= csr_write;
						dbg_csr_read <= csr_read;
						wfi = BIT(0);
						mret = BIT(0);
						csr_be = rfunct3;
						SWITCH(rfunct3) // same for immediate and register: choice of op1 is made previously
						CASE(CASE_ECALL) // ECALL-EBREAK-WFI or MRET: convert to R/W to crs_irq, but addr is irrelevant and contains cause
						// this case is also used to start an IRQ from decode stage. Both update mepc and mcause, and trap
							IF (use_immediate == BIT(0)) THEN // ECALL-EBREAK. strange statement (if not use _imm ... <= rimm.) but this actually uses the immediate reg reuse used for branches
								blk2mem_t0.data <= rimmediate; // PC-4 to set mepc
								IF (start_irq == BIT(1)) THEN // if ECALL come from IRQ, send cause
									blk2mem_t0.addr <= ( RANGE(TO_UINT(HEX_INT(CSR_IRQ_REGS), data_addr_span - 2), data_addr_span - 3, 12) & EXT(PORT_BASE(csr2core_i).cause, 12) ); //EXT( ((not B(rrs2, 0)) & BIN(0) & priv), 12) ); // send cause as addr
								ELSE // else send ECALL params
									blk2mem_t0.addr <= ( RANGE(TO_UINT(HEX_INT(CSR_IRQ_REGS), data_addr_span - 2), data_addr_span - 3, 12) & EXT( ((not B(rrs2, 0)) & BIN(0) & priv), 12) ); // send cause as addr
								ENDIF
								trap = BIT(1); gprintf("%s", tabs); gprintf("#VECALL trap addr %Y time %Y", to_hex(TO_INTEGER(PORT_BASE(csr2core_i).mepc)), cur_time>>8);
								csr_read = BIT(0);
								start_irq <= BIT(0);
								ropcode <= TO_UINT(0, LEN(ropcode)); rcsr_op_with_read <= BIT(0);
								dbg_ropcode <= TO_UINT(5, 8);

							ELSEIF ( ( (rfunct7 == BIN(0011000) ) or (rfunct7 == BIN(0000000) ) ) and ( rrs2 == BIN(00010) ) ) THEN  // MRET and URET
								next_PC = PORT_BASE(csr2core_i).mepc; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe)); ropcode <= TO_UINT(0, LEN(ropcode)); rcsr_op_with_read <= BIT(0); //mret = BIT(1);// TODO: Change mepc to appropriate register when not in machine mode
								csr_read = BIT(0);
								csr_be = BIN(001); // fake write to not trig ECALL processing in irq block
								blk2mem_t0.addr <= ( RANGE(TO_UINT(HEX_INT(CSR_IRQ_REGS), data_addr_span - 2), data_addr_span - 3, 12) & AMRET ); // Flags end of IRQ
								reset_cur_irq <= BIT(1);
							ELSEIF ( ( rfunct7 == BIN(0001000) ) and ( rrs2 == BIN(00101) ) ) THEN  // WFI
	 							csr_read = BIT(0); // nothing to actually read
	 							blk2mem_t0.addr <= ( RANGE(TO_UINT(HEX_INT(CSR_IRQ_REGS), data_addr_span - 2), data_addr_span - 3, 12) & AMEPC_WFI );
	 							blk2mem_t0.data <= PCp; // Not sure of that
	 							wfi = BIT(1);
	 							wait_for_interrupt <= BIT(1);
#ifndef VHDL
	 							tarmac.flush();//Otherwise we can't see the WFI instruction
#endif
	 						ENDIF
#ifdef NONREG
								IF ( B(regs(3), 0) == BIN(1)) THEN  // program end
										halt <= BIT(1);
								ENDIF
#endif
						DEFAULT // not ecall: convert to R/W using CSR addr
							blk2mem_t0.addr <= ( RANGE(TO_UINT(HEX_INT(CSR_IRQ_REGS), data_addr_span - 2), data_addr_span - 3, 12) & RANGE(rimmediate,11,0) );
							blk2mem_t0.data <= op1;
							// IF CSRRS/C (funct3(1) = 1), wr_n = 1 if csr_write. If CSRRW, force wr_n = 1 to block read (as coded in crs_irq)
							blk2mem_t0.wr_n <= ( ( (B(rfunct3, 1)) and (not csr_write)) or (not B(rfunct3, 1) and csr_read) ) ;

						ENDCASE
						blk2mem_t0.be <= (wfi & csr_be); // code CSR op and wfi in byte enable bus
						blk2mem_t0.cs_n <= BIT(0);//(mret); // Write to AMRET in case of mret inst. to flag end of IRQ processing //Nothing to send to CSR block
						// If CSR read, set signals just like for a regular read
						cpu_wait <= (csr_read or wfi); // stall the pipeline
						rwb <= rrd; // Store writeback register and associated method
						funct3wb <= BIN(010); // is 32-bit read
						rshiftwb <= BIN(00);
						load_mem0 <= BIT(0); // not instruction memory
						mask_data_en <= csr_read;
					CASE(CASE_MEM) //FENCE: nothing, not required (1 hart) and test won't pass anyway (write to inst mem in test) Cf. fence_i PC = 0X110 ====> NOP
						//flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe));ropcode <= TO_UINT(0, LEN(ropcode));//
						//next_PC <= PCp;
					CASE(CASE_OP)
						SWITCH(rfunct3)
							CASE(CASE_ADDx) IF (alt_op == BIT(0)) THEN rd_val = RESIZE(add_res, 32); ELSE rd_val = RESIZE(sub_res, 32); ENDIF
							CASE(CASE_SLLx) rd_val = sll_res;
							CASE(CASE_SRLx) IF (alt_op == BIT(0)) THEN rd_val = srl_res_u; ELSE rd_val = srl_res_s; ENDIF
							CASE(CASE_SLTx) rd_val = RESIZE(rs1_lt_rs2_s, 32);
							CASE(CASE_SLTU) rd_val = RESIZE(rs1_lt_rs2_u, 32);
							CASE(CASE_ORx)  rd_val = or_res;
							CASE(CASE_XORx) rd_val = xor_res;
							CASE(CASE_ANDx) rd_val = and_res;
							DEFAULT    trap = BIT(1); cause = ILLINSTR; if (not boot_mode == BIT(1)) THEN gprintf("#VILLINSTR OP");
																		ENDIF
						ENDCASE
					CASE(CASE_LOAD)
						IF (not (rrd == BIN(00000))) THEN
							cpu_wait <= BIT(1); // stall the pipeline
							rwb <= rrd; // Store writeback register and associated method
							funct3wb <= rfunct3;
							rshiftwb <= nshift;
							mask_data_en <= BIT(1);

							if (RANGE(add_res,31, 28) == BIN(0000)) THEN // Reading from instruction memory
#ifndef NONREG // When running risc-V non reg., data is @ 0x2000 -> force it to data memory anyway
									inst_addr <= RANGE(add_res, LEN(blk2mem_t0.addr)+1, 2);
									//blk2mem_t0.cs_n <= BIT(0); /// TODO, change this!!!
									inst_cs_n <= BIT(0);
									load_from_instmem = BIT(1);
									load_mem0 <= BIT(1);
									//gprintf("#Vinstmem");
									//exit(0);
#else
									load_mem0 <= BIT(0);
									blk2mem_t0.cs_n <= BIT(0);
#endif

								ELSE
									load_mem0 <= BIT(0);
									blk2mem_t0.cs_n <= BIT(0);
									//gprintf("#Vdatamem");
									//exit(0);
								ENDIF

								blk2mem_t0.addr <= RANGE(add_res, LEN(blk2mem_t0.addr)+1, 2);//RESIZE(add_res, LEN(PORT_BASE(core2datamem_o).addr));
								blk2mem_t0.wr_n <= BIT(1);
								blk2mem_t0.be <= BIN(1111);
								gprintf(tarmac, "%dns MR% %d ", double(cur_time)/256000, 1<<(TO_INTEGER(rfunct3)&3), to_hex(TO_INTEGER(add_res)));
						ENDIF
					CASE(CASE_STORE)
						wbe = ( B(rfunct3, 1) & B(rfunct3, 1) & (B(rfunct3, 0) or B(rfunct3, 1)) & BIT(1) ); // use minor opcode bits for wr. byte enable
						wbe = SHIFT_LEFT(wbe, TO_INTEGER(nshift));
						gprintf(tarmac, "%dns MW% %d %\n", double(cur_time)/256000, 1<<(TO_INTEGER(rfunct3)&3), to_hex(TO_INTEGER(add_res)), to_hex(TO_INTEGER(regs(TO_INTEGER(rrs2)))));
						blk2mem_t0.addr <= RANGE(add_res, LEN(blk2mem_t0.addr)+1, 2);//RESIZE(add_res, LEN(PORT_BASE(core2datamem_o).addr));
						blk2mem_t0.cs_n <= BIT(0);
						blk2mem_t0.wr_n <= BIT(0);
						blk2mem_t0.be <= wbe;
						blk2mem_t0.data <= SHIFT_LEFT(regs(TO_INTEGER(rrs2)), TO_INTEGER(nshift & BIN(000)));

					CASE(CASE_LUI) rd_val = rimmediate;
					CASE(CASE_AUIPC) rd_val = rimmediate; // Pc added at decoding stage
					CASE(CASE_JAL) next_PC = rimmediate; rd_val = PCp; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe));ropcode <= TO_UINT(0, LEN(ropcode)); rcsr_op_with_read <= BIT(0);
					CASE(CASE_JALR) next_PC = RESIZE( ( RANGE(add_res, 31,1) & BIN(0) ), LEN(PC)); // It seems that lsb could be used for stg else: clear lsb
						rd_val = PCp; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe)); rjalr <= rrd; ropcode <= TO_UINT(0, LEN(ropcode)); rcsr_op_with_read <= BIT(0);
					CASE(CASE_BRANCH)
						taken = ( ( ( ( RANGE(rfunct3, 1,1) and (rs1_lt_rs2_u) ) or ( (not RANGE(rfunct3, 1,1)) and (rs1_lt_rs2_s) ) ) and RANGE(rfunct3,2,2) ) or
								 ( rs1_eq_rs2 and not RANGE(rfunct3,2,2) ) ) xor RANGE(rfunct3, 0,0);

						IF (taken == BIN(1)) THEN
							next_PC = rimmediate; flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe)); ropcode <= TO_UINT(0, LEN(ropcode)); rcsr_op_with_read <= BIT(0);
							dbg_ropcode <= TO_UINT(6, 8);
						ENDIF
					DEFAULT trap = BIT(1); cause = ILLINSTR;  if (not boot_mode == BIT(1)) THEN gprintf("ILLINSTR trap opcode"); dbg_illinstr <= BIT(1);
																ENDIF

				ENDCASE
				rtaken <= taken;
				rrd_val <= rd_val;

				IF ( (not (ropcode == LOAD) and not (ropcode == STORE) and not (ropcode == SYS) ) or ( load_from_instmem == BIT(1) ) ) THEN
					//blk2mem_t0.cs_n <= BIT(1);
					blk2mem_t0 <= dma_request_i; // mem bus is free: transmit dma requests
					dma_grant_o <= (not PORT_BASE(dma_request_i).cs_n); // And signal access is granted
					dma_granted <= (not PORT_BASE(dma_request_i).cs_n); // And signal access is granted
					IF (dma_granted == BIT(1)) THEN
					    blk2mem_t0.cs_n <= BIT(1);
					ELSE					
					    dma_rd_wait <= ((not PORT_BASE(dma_request_i).cs_n) and PORT_BASE(dma_request_i).wr_n);
					ENDIF
					rcan_grant <= BIT(1);
				ELSE
					rcan_grant <= BIT(0);
				ENDIF

				wrd = rrd;
			ELSE // exec inst
				//PORT_BASE(core2datamem_o).cs_n <= BIT(1);
				//blk2mem_t0.cs_n <= BIT(1);
				exec <= BIT(0);
				rd_val = BIN(10101010010101011010101001010101);
				blk2mem_t0 <= dma_request_i; // mem bus is free: transmit dma requests
				// allow DMA accesses when cpu is not waiting for read data -> (not cpu_wait or wait_for_interrupt)
				//cpu_wait <= (cpu_wait or ((not PORT_BASE(dma_request_i).cs_n) and (not cpu_wait or wait_for_interrupt) and PORT_BASE(dma_request_i).wr_n));
				dma_grant_o <= ((not PORT_BASE(dma_request_i).cs_n) and (not cpu_wait or wait_for_interrupt) ); // And signal access is granted
				dma_granted <= ((not PORT_BASE(dma_request_i).cs_n) and (not cpu_wait or wait_for_interrupt) ); // And signal access is granted
				blk2mem_t0.cs_n <= (PORT_BASE(dma_request_i).cs_n or (cpu_wait and not wait_for_interrupt)); // !!!!!! Override: matches the grant condition above !!! BUG !!!
				IF (dma_granted == BIT(1)) THEN
				  blk2mem_t0.cs_n <= BIT(1);
				ELSE
				    dma_rd_wait <= ((not PORT_BASE(dma_request_i).cs_n) and (not cpu_wait or wait_for_interrupt) and PORT_BASE(dma_request_i).wr_n);				
				ENDIF
				rcan_grant <= not cpu_wait;//BIT(1);
			ENDIF

			load_mem <= load_mem0;// Delay so that load_mem is not 1 just after load execution (this would trigger pipeline resume when accessing instruction memory)

			// Just one cycle wait for write, and only if load comes after
			IF (cpu_wait_on_write == BIT(1)) THEN
				cpu_wait <= BIT(0);
				cpu_wait_on_write <= BIT(0);
			ENDIF

			// reset mask fata_en resulting of dma accesses
#if 0		// does not work with successive loads
			IF ( not(rwb == TO_UINT(0, LEN(rwb))) ) THEN
				mask_data_en <= BIT(0);
			ENDIF
#else
			IF (cpu_wait == BIT(1)) THEN
				mask_data_en <= BIT(0);
			ENDIF
#endif
			IF ( (dma_rd_wait == BIT(1)) and (PORT_BASE(datamem2core_i).data_en == BIT(1)) ) THEN // DMA not from ROM
			    dma_rd_wait <= BIT(0);
			    //cpu_wait <= BIT(0);
			ENDIF

			// Register writeback (load) -----------------------------------------------------------------
			IF ( not(rwb == TO_UINT(0, LEN(rwb))) and ( mask_data_en == BIT(0) ) and ( ( (PORT_BASE(datamem2core_i).data_en == BIT(1)) and (load_mem == BIT(0))) or ( (PORT_BASE(instmem2core_i).data_en == BIT(1)) and (load_mem == BIT(1)) ) ) ) THEN
				cpu_wait <= BIT(0);
				load_mem <= BIT(0);
				load_mem0 <= BIT(0);
				IF (load_mem == BIT(0)) THEN // data is expected from data mem
					ld_data = PORT_BASE(datamem2core_i).data;
				ELSE // data is expected from inst. mem (ro data)
					ld_data = PORT_BASE(instmem2core_i).data;
				ENDIF
				ld_data = SHIFT_RIGHT(ld_data, TO_INTEGER( (rshiftwb & BIN(000))));
				SWITCH(funct3wb)
					CASE(CASE_LW) rd_val = ld_data;
					CASE(CASE_LH) rd_val = SXT(RANGE(ld_data, 15, 0), LEN(rd_val));
					CASE(CASE_LHU) rd_val = EXT(RANGE(ld_data, 15, 0), LEN(rd_val));
					CASE(CASE_LB) rd_val = SXT(RANGE(ld_data, 7, 0), LEN(rd_val));
					CASE(CASE_LBU) rd_val = EXT(RANGE(ld_data, 7, 0), LEN(rd_val));
					DEFAULT rd_val = TO_UINT(0, LEN(rd_val));
				ENDCASE
				gprintf(tarmac, " %d\n", to_hex(TO_INTEGER(rd_val)));

				wrd = rwb;
				RESET(rwb);
			ENDIF

			// wake-up CPU when receiving interrupt in WFI mode. // use start_irq instead ?
			IF ((wait_for_interrupt == BIT(1)) and (PORT_BASE(csr2core_i).wakeup == BIT(1))) THEN
				cpu_wait <= BIT(0);
				wait_for_interrupt <= BIT(0);
			ENDIF

			regs(TO_INTEGER(wrd)) <= rd_val;
			IF ( wrd == BIN(00000) ) THEN
				rrd_wr_en <= BIT(0);
				rrd_wr <= TO_UINT(0,32);
			ELSE
				rrd_wr_en <= BIT(1);
				rrd_wr <= rd_val;
			ENDIF
			rrd_wr_enp <= rrd_wr_en;
			rrd_wrp <= rrd_wr;
			rrrd <= wrd;
			rrrdp <= rrrd;

			regs(0) <= TO_UINT(0, 32); // ensure that x0 is 0

			// In boot mode, transmit dma requests to inst memory for UART code loading
			IF ( (boot_mode == BIT(1)) and (boot_modep == BIT(0)) ) THEN // reset code loaded flag
				//code_loaded <= BIT(0);
				code_loaded0 <= BIT(0);
			ENDIF

			IF (boot_modep == BIT(1)) THEN
				next_PC = HEX(FFFFFFFC);//TO_UINT(0, LEN(PC));
				inst_cs_n <= PORT_BASE(dma_request_i).cs_n;
				code_loaded0 <= ( code_loaded0 or not PORT_BASE(dma_request_i).cs_n );
				pipe <= TO_UINT(0, LEN(pipe));

				IF (boot_mode == BIT(0)) THEN
						gprintf("#RCODE LOADED, starting Giorno core");
						//next_PC = HEX(FFFFFFFC);//TO_UINT(0, LEN(PC));
						next_PC = TO_UINT(0, LEN(PC));
						inst_cs_n <= BIT(0);
						loading <= BIT(0);
						flush <= BIT(1);
						//pipe <= TO_UINT(0, LEN(pipe));
						cpu_wait <= BIT(0);
						code_loaded <= (code_loaded or code_loaded0);
						//RESET(ropcode); This causes the boot problem!!!
						// so forget "this fixed the reboot-on-load NOT REALLY"
						dbg_ropcode <= TO_UINT(7, 8);
						RESET(PCp);
						alt_op <= BIT(0);
						halt <= BIT(0);
						use_immediate <= BIT(0);
						csri <= BIT(0);
						RESET(rfunct3);
						RESET(rrd);
						// \this fixed the reboot-on-load
						RESET(rwb);
						RESET(funct3wb);
						RESET(rshiftwb);
						mask_data_en <= BIT(0);
						rtrap <= BIT(0);
						priv <= BIN(11); // Machine mode is default
						load_mem <= BIT(0);
						load_mem0 <= BIT(0);
						cpu_wait_on_write <= BIT(0);
						rrd_wr_en <= BIT(0);
						dma_grant_o <= BIT(0);


				ELSE
					inst_addr <= PORT_BASE(dma_request_i).addr;
					PCp <= PORT_BASE(dma_request_i).data;
				ENDIF
				// no be, assume 32-bit accesses
			ENDIF

			// Traps and IRQs -------------------------------------
			dbg_cpu_wait <= (RANGE(dbg_cpu_wait, 6, 0) & cpu_wait);
			//trap = (trap or BOOL2BIT(dbg_cpu_wait == BIN(11111111))); // Also catch stalled cpu
			rtrap <= (trap and code_loaded and not start_irq);
			// TRAP - try to optimize that later
			IF ((not (loading == BIT(1)) and not (boot_mode == BIT(1))) and (trap == BIT(1))) THEN
				trap_addr_base = PORT_BASE(csr2core_i).mtvec and BIN(11111111111111111111111111111100);
				IF ( (RANGE(PORT_BASE(csr2core_i).mtvec, 1, 0) == BIN(01) ) and (start_irq == BIT(1)) ) THEN // interrupt mode w/ cause branch enabled
					trap_addr_offset = EXT(PORT_BASE(csr2core_i).cause, 30) & BIN(00);
					cur_irq <= PORT_BASE(csr2core_i).cause; // Keeps from running same IRQ forever
					gprintf(tarmac, "%dns E % %d\n", double(cur_time)/256000, "INT_ENTRY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", to_hex(TO_INTEGER(PORT_BASE(csr2core_i).cause)));
				ELSE
					trap_addr_offset = TO_UINT(0, LEN(trap_addr_offset));
					gprintf(tarmac, "%dns E % %d\n", double(cur_time)/256000, "TRAP", to_hex(TO_INTEGER(PORT_BASE(csr2core_i).cause)));
				ENDIF
				next_PC = trap_addr_base + trap_addr_offset;
				flush <= BIT(1); pipe <= TO_UINT(0, LEN(pipe)); ropcode <= TO_UINT(0, LEN(ropcode));
				dbg_ropcode <= TO_UINT(8, 8);
			ENDIF
			IF ( reset_cur_irq == BIT(1) ) THEN //PORT_BASE(csr2core_i).cause == TO_UINT(0, 6) ) THEN
				cur_irq <= BIN(000000);
				reset_cur_irq <= BIT(0);
				gprintf(tarmac, "%dns E % %d\n", double(cur_time)/256000, "INT_LEAVE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<", to_hex(TO_INTEGER(cur_irq)));
			ENDIF


			PC <= next_PC;
			rload_from_instmem <= load_from_instmem;

			IF ( (load_from_instmem == BIT(0)) and not (boot_mode == BIT(1)) ) THEN
				inst_addr <= RANGE(next_PC, LEN(blk2mem_t0.addr)+1, 2);
			ENDIF

			if ( (rtrap == BIT(1)) ) THEN // or (RANGE(PC,15,0) == HEX(015c)) ) THEN
				debug_write <= BIT(0);
			ELSEIF ( boot_modep == BIT(1) ) THEN
				debug_write <= BIT(1);//boot_mode; // to trace at startup
			ENDIF

			dbg_stop <= (RANGE(dbg_stop , 6, 0) & (not debug_write) );

			IF (B(dbg_stop, 7) == BIT(0)) THEN
				dbg_data_w <= (rinstr & rdbg & cpu_wait & flush & exec & rtrap & PORT_BASE(datamem2core_i).data_en & rwb & rrd & RANGE(PC, 15, 0));
				dbg_addr <= dbg_addr + 1;
				dbg_wen <= BIT(0);
				dbg_cnt <= BIN(0000000);
				dbg_cnt2 <= BIN(00000);
				dbg_o <= (BIT(0) & RANGE(dbg_data_r, 63, 32));

			ELSEIF (dbg_cnt2 < BIN(11111)) THEN
				dbg_wen <= BIT(1);
				IF (dbg_cnt == BIN(0000000)) THEN
					//dbg_addr <= dbg_addr - 1;
				    dbg_cnt <= TO_UINT(71, 7);
				ELSE
					dbg_cnt <= dbg_cnt - 1;
				ENDIF
				IF (dbg_cnt == TO_UINT(71, 7)) THEN
					dbg_o <= (BIT(1) & RANGE(dbg_data_r, 31, 0));
				ENDIF
				IF (dbg_cnt == TO_UINT(35, 7)) THEN
					dbg_o <= (BIT(1) & RANGE(dbg_data_r, 63, 32));
					dbg_cnt2 <= dbg_cnt2 + 1;
					dbg_addr <= dbg_addr + 1;

				ENDIF
			ELSE
				dbg_o <= (BIT(0) & RANGE(dbg_data_r, 63, 32));
			ENDIF

	ENDIF
	END_PROCESS

	// Combinational logic
	COMB_PROCESS(1, clk_core)
		PORT_BASE(core2instmem_o).addr <= inst_addr;//RANGE(PC, LEN(blk2mem_t0.addr)+1, 2);//RESIZE(PC, LEN(PORT_BASE(core2instmem_o).addr));
		PORT_BASE(core2instmem_o).data <= PCp;
		PORT_BASE(core2instmem_o).cs_n <= inst_cs_n;//BIT(0);
		PORT_BASE(core2instmem_o).wr_n <= not (loading or boot_mode);
		PORT_BASE(core2instmem_o).be <= BIN(1111);
		core2datamem_o <= blk2mem_t0;
		addr_rs1 <= RANGE(PORT_BASE(instmem2core_i).data, 19, 15);
		addr_rs2 <= RANGE(PORT_BASE(instmem2core_i).data, 24, 20);
		trap_o <= rtrap;
		//PC_o <= (rtrap & cpu_wait & flush & inst_cs_n & loading & boot_mode & code_loaded & RANGE(pipe, 1, 0) & ropcode &  RANGE(PC, 15, 0));
		//PC_o <= (rtrap & cpu_wait & flush & inst_cs_n & loading & boot_mode & code_loaded & RANGE(pipe, 1, 0) & ropcode &  RANGE(PC, 15, 0));
		//PC_o <= (rtrap & cpu_wait & rdbg & blk2mem_t0.addr &  RANGE(PC, 15, 0));
		//PC_o <= (rtrap & cpu_wait & flush & exec & load_mem & rdbg & PORT_BASE(datamem2core_i).data_en & RANGE(pipe, 1, 0) & ropcode &  RANGE(PC, 15, 0));
		//PC_o <= (cond1 & cond2 & cond3 & cond4 & cond5 & rdbg & PORT_BASE(datamem2core_i).data_en & RANGE(pipe, 1, 0) & ropcode &  RANGE(PC, 15, 0));
		//PC_o <= dbg_pipe;
	END_COMB_PROCESS

BLK_END;
