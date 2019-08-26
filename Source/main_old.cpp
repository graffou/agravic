//sed -e 's/:=/=/g' Source/main.cpp | g++ -std=c++14 -IInclude_libs -o toto -xc++ -



#include <stdio.h>
#include <iostream>
#define DEBUG
#include <sstream>

#include <iostream>
#include "mbprt.h"
#include "macros.h"
#include <vector>
#include <utility>
#define gprintf gkprintf

#include "gmodule.h"
/*std::vector<gmodule*> gmodule::module_list;
gmodule* gmodule::out_of_hier = new gmodule;
unsigned long long int gmodule::vcd_time;
*/
#include "ports.h"
#include "slv.h"
#include "kb.h"



#if 1

ENTITY(momo_t,
DECL_PORTS(
		PORT(in, SLV_TYPE(5), IN),
		PORT(out, SLV_TYPE(5), OUT)
		)
);
END_ENTITY;

INTERNAL_BINDINGS;



END_INTERNAL_BINDINGS;

INTERNAL_SIGNALS;

END_INTERNAL_SIGNALS;

//control_signals process0()
//{
PROCESS_VOID(0)
	gprintf("#mmomo in % out %", in , out);
	VAR(out_tmp, SLV_TYPE(5));
	//out_tmp = in + SLV(3, LEN(out_tmp));
	//out_tmp = in + SLV(3, (out_tmp.length));
	//gprintf("ULEN %", int(out_tmp.length));
	//out = in + SLV(3, LEN(out));
	//out = out_tmp;
	out_tmp <= in + SLV(3, LEN(out_tmp));
	out <= in + SLV(3, LEN(out));
	gprintf("#mmomo in % out %", in , out);
END_PROCESS;
//}

BLK_END;

	ENTITY(zozo_t,
	DECL_PORTS(
			PORT(in, SLV_TYPE(5), IN),
			PORT(out, SLV_TYPE(5), OUT)
			)
	);
	END_ENTITY;

	// Put all components bindings !)
	INTERNAL_BINDINGS;

	BLK_INST(my_momo, momo_t,
	MAPPING(
			PM(in, internal),
			PM(out, out)
			)
	);

	END_INTERNAL_BINDINGS;

	// Put all internal signals (not variables !)
	INTERNAL_SIGNALS;

	SIG(internal, SLV_TYPE(5));// internal;

	END_INTERNAL_SIGNALS;

//	control_signals process0()
//	{
	PROCESS_VOID(0)
		gprintf("#bzozo in % out %", in , out);
		internal = in + 2;
		my_momo.process0();
		gprintf("#bzozo in % out %", in , out);
	END_PROCESS;

//	}

	BLK_END;
#else
// put preprocessor output here for platform debugging
#endif


int main()
{
	slv<5> a = gen_sig_desc("a", gmodule::out_of_hier);
	slv<5> b = gen_sig_desc("b", gmodule::out_of_hier);
	//SIG(a, gmodule::out_of_hier);
	//SIG(b, gmodule::out_of_hier);
	a = 1;
	b = 2;
	//momo_t<0> my_momo(gen_blk_map("my_momo", gmodule::out_of_hier, momo_t<0>::in_stat, a, momo_t<0>::out_stat, b));
	//momo_t<0>* pmy_momo = new momo_t<0>(gen_blk_map("my_momo", gmodule::out_of_hier, momo_t<0>::in_stat, a, momo_t<0>::out_stat, b));
	//momo_t<0>& my_momo = *pmy_momo;



	gprintf("ptrs % % \n", &momo_t<0>::in, &momo_t<0>::out);
	//momo_t<0>& my_momo2 = *create_block<momo_t<0>>("my_momo2", gmodule::out_of_hier, &momo_t<0>::in, &a, &momo_t<0>::out, &b);

	//momo_t<0>& my_momo2 = *create_block< momo_t<0> >("my_momo", gmodule::out_of_hier, &momo_t<0>::in, &a, &momo_t<0>::out, &b);

/*
	 BLK_INST_TOP(
			my_momo2, momo_t,
			MAPPING(
					PM(in, a),
					PM(out, b)
					)
			);
*/


	//toto<1> titi = gen_bind(toto<1>::in_stat, a, toto<1>::out_stat, b);
	BLK_INST_TOP(
			titi, zozo_t,
			MAPPING(
					PM(in, a),
					PM(out, b)
					)
			);


	//toto<1> titi = gen_bind( link(toto<1>::in_stat, a), link(toto<1>::out_stat, b));
	//toto<1> titi = gen_bind( toto<1>::out_stat, b);
	//toto<1> titi = gen_bind(toto<1>::in_stat, a);
	std::cerr << "b = " << b << "\n";
	std::cerr << "GO\n";
	titi.process0();
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
