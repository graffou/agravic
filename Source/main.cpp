//sed -e 's/:=/=/g' Source/main.cpp | g++ -std=c++17 -g3 -O3 -IInclude -IInclude_libs -ISource -o toto -xc++ -
//For VHDL
// g++ -std=c++17 -g3 -IInclude -IInclude_libs -E -P -xc++ -DVHDL Source/main.cpp | sed -e 's/==/=/g'
#include <stdio.h>
#include <iostream>
#define DEBUG
//#define NOPRINT

#include <sstream>

#include <iostream>
#include "mbprt.h"
#include "macros.h"
#include <vector>
#include <utility>
#define gprintf gkprintf

#include "gmodule.h"
#include "ports.h"
#include "slv.h"
#include "kb.h"
#include "input_parms.h"

#include "structures.h"
#include "spram8kx32.h"
#include "mem.h"
#include "peripherals.h"
#include "risc_V_constants.h"
#include "risc-V_core.h"
#include "top.h"
#include "tb.h"


int main(int argc, char* argv[])
{
	CLI_PARM(bin_file, std::string);
	bin_file.set_mandatory();
	bin_file.set_help("CPU code file in binary format");
	CLI_PARM_INIT(ncycles, int, 5000);
	ncycles.set_help("Number of clock cycles the simulation will run");

	CLI_PARSE(argc, argv);


	gprintf("#Ublk inst top");
	BLK_INST_TOP(
			tb, tb_t,
			);

	tb.ncycles = ncycles.val;

	tb.dut.check();
	gprintf("#CInit file");
	tb.init_file(bin_file);
	//exit(0);
	gprintf("#CInit vcd");
	init_vcd(); //exit(0);
	gprintf("#CInit clk");
	tb.init_clk_rst();
	tb.dut.check();
	tb.clk.parse_modules();
	gprintf("#CActivate vcd");

	vcd_file.set_timebase_ps(10000);// 10ns <=> 50MHz clk
	vcd_file.activate();
	tb.dut.check();

	gprintf("#CRunning testbench");

	tb.run();

	// Obligatory order !!
#if 0
	init_vcd();
	clk.parse_modules();

	vcd_file.activate();
#endif


	gprintf("#VEnd of simulation, time is % ps", vcd_file.vcd_time * vcd_file.timebase_ps);
#ifdef NONREG
	if (tb.success)
	{
		for (int j = 0 ; j < 10 ; j++) gprintf("#G!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! PASSED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
	else
	{
		for (int j = 0 ; j < 10 ; j++) gprintf("#R!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
#endif
	return tb.success;

//	for (int i = 2; i < 256; i++)
//		gprintf(" #define EVAL%d(...) EVAL1(EVAL%d(__VA_ARGS__))\n", i, i-1);

	//toto<1> titi = gen_bind( link(toto<1>::in_stat, a), link(toto<1>::out_stat, b));
	//toto<1> titi = gen_bind( toto<1>::out_stat, b);
	//toto<1> titi = gen_bind(toto<1>::in_stat, a);
	/*
	std::cerr << "b = " << b << "\n";
	std::cerr << "GO\n";
	//titi.process0();
	std::cerr << "b = " << b << "\n";

	gprintf("titi::out %\n", titi.out);
	gprintf("momo::out %\n", titi.my_momo.out);

	a = 4;

	titi.process0();
	std::cerr << "b = " << b << "\n";
	gprintf("titi::in %\n", titi.in);
	gprintf("momo::in %\n", titi.my_momo.in);
	gprintf("titi::out %\n", titi.out);
	gprintf("momo::out %\n", titi.my_momo.out);

	a = 4;

*/
	gmodule::list_modules();


/*


	gprintf("momo::in %\n", my_momo2.in);
	gprintf("momo::out %\n", my_momo2.out);
	my_momo2.process();
	gprintf("momo::in %\n", my_momo2.in);
	gprintf("momo::out %\n", my_momo2.out);
	a = 8;
	my_momo2.process();
	gprintf("momo::in %\n", my_momo2.in);
	gprintf("momo::out %\n", my_momo2.out);
	gprintf("a %, b %\n", a, b);
*/

	/*
	VAR(x,SLV_TYPE(3));
	VAR(y,SLV_TYPE(3));
	x := SLV(6,3);
	std::cerr << x.n <<  "\n";
	SLV(z, 12) := CAT(BIN(1111),CAT(x, BIN(00100)));
	Signed<3> r := SIGNED(x);
	slv<15> w := z * r;

	std::cerr << x.n << "    " << std::hex << z.n << "   " << w.n << "\n";
	if (EQ(z,BIN(111101000100))) THEN
		std::cerr << "OK\n";
	ELSE
		std::cerr << "KO\n" << z << "  " << BIN(111101000100) << "\n";
	ENDIF

	slv<15> q := w + SLV(1, 15);
	Signed<15> t := q + EXT(HEX(1000), LEN(t));
	gkprintf("w = %, signed(w) = % % % and % or % xor %\n", (w), t, RANGE(t, HI(t) - 6, 0), SIGNED(w), w and t, w or t, w xor t);

	TYPE(my_array_t, ARRAY_TYPE(SLV_TYPE(10), 10));
	VAR(my_array, my_array_t);

	int a = 2;
	switch(a)
	{
		break;
		case 0: std::cerr << 0;
		break;
		case 1: std::cerr << 1;
		break;
		default: std::cerr << "def";

	}

	SWITCH(a)
		CASE(0) std::cerr << 0;
		CASE(1) std::cerr << 1;
		DEFAULT std::cerr << "def";
	ENDCASE

	REC(tata)
		MEMBER(toto, SLV_TYPE(6));
		MEMBER(tete, SLV_TYPE(7));
	ENDREC

	while(1)
	{
		kb();
	}

	//LIST(SLV_TYPE(3),SLV_TYPE(6),SLV_TYPE(3))
#ifdef VHDL
	EVAL(MAP(GREET, Mum, Dad, Adam, Joe))

	EVAL(MAP2(GREET, BYE, Mum, Dad, Adam, Joe))

	EVAL(MAP2(END_SEMICOLON, END_NOTHING,
			PORT_IN(clk, BIT_TYPE),
			PORT_IN(rst, BIT_TYPE),
			PORT_OUT(val, SLV_TYPE(11)),
			))
	ENTITY(titi,PORT_IN(clk, BIT_TYPE),PORT_IN(rst, BIT_TYPE),PORT_OUT(val, SLV_TYPE(11)));
	REC(tata)
		MEMBER(toto, SLV_TYPE(6));
		MEMBER(tete, SLV_TYPE(7));
	ENDREC
	ENTITY(titi,
			PORT_IN(clk, BIT_TYPE), // truc
			PORT_IN(rst, BIT_TYPE), //bidule
			PORT_OUT(val, SLV_TYPE(11))
			);
	COMPONENT(titi,
			PORT_IN(clk, BIT_TYPE), // truc
			PORT_IN(rst, BIT_TYPE), //bidule
			PORT_OUT(val, SLV_TYPE(11)),
			PORT_OUT(mystruct, tata)
			);
	a VA b
	TYPE(toto, ARRAY_TYPE(SLV_TYPE(6), 12));
	SIG(titi, toto);
	VAR(titi, toto);
	CONST(titi, toto);
	REC(tata)
		MEMBER(toto, SLV_TYPE(6));
		MEMBER(tete, SLV_TYPE(7));
	ENDREC
#endif
*/

	/*
	PORT_DEF(toto, SLV_TYPE(4), IN)
	STATIC_PORT_DEF(toto, SLV_TYPE(4), IN)
	PORT_BIND(toto)

	PORT_MAP(toto, titi, tata)
	M(titi, tata)

	PORT_MAP2(toto, M(titi11, tata))
*/


#if 0


	BLK_INST(myblk, blk_t,
			MAPPING(
					PM(titi22, tata),
					PM(yoyo, yiyi)
					),
					GENERIC(1,2)
					)
#endif
/*
	//PORT_MAPS(toto, )
#define B(n) n is my favourite!
DEFER1(B)(321)
#define EVAL1(...) __VA_ARGS__

EVAL1(A EMPTY() (123))


#define EMPTY()
#define A(n) I like the number n

#define EVAL1(...) __VA_ARGS__

EVAL1(A EMPTY() (123))

EVAL1( PORT_MAP EMPTY() (type, M(titi, tata)))
EVAL1( DEFER1(PORT_MAP) (type, M(titi, tata)))
*/
}
