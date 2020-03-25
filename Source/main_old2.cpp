//sed -e 's/:=/=/g' Source/main.cpp | g++ -std=c++17 -g3 -O3 -IInclude -IInclude_libs -ISource -o toto -xc++ -
//For VHDL
// g++ -std=c++17 -g3 -IInclude -IInclude_libs -E -P -xc++ -DVHDL Source/main.cpp | sed -e 's/==/=/g'
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



#if 0

// block momo----------------------------------
INCLUDES

ENTITY(momo_t,
DECL_PORTS(
		PORT(clk, CLK_TYPE, IN),
		PORT(reset1_n, RST_TYPE, IN),
		PORT(min, SLV_TYPE(5), IN),
		PORT(mout, SLV_TYPE(5), OUT)
		)
);
BEGIN

PROCESS(0, clk, reset1_n)
BEGIN
	IF ( reset1_n == BIT(0) ) THEN
		//gprintf("#Gmomo rst % ", reset1_n.get().n);
		mout <= SLV(7, LEN(mout));
	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		//gprintf("#mmomo in % out %", in , out);
		//out_tmp <= in + SLV(3, LEN(out_tmp));
		mout <= min + SLV(4, LEN(mout));
		//gprintf("#mmomo in % out %", in , out);
	ENDIF
END_PROCESS;
//}

BLK_END;


// block zozo -----------------------------------------------------------
INCLUDES

	ENTITY(zozo_t,
	DECL_PORTS(
			PORT(clk, CLK_TYPE, IN),
			PORT(reset_n, RST_TYPE, IN),
			PORT(zin, SLV_TYPE(5), IN),
			PORT(zout, SLV_TYPE(5), OUT)
			)
	);

	// Put all internal signals and component declarations (not variables !)
	COMPONENT(momo_t,
	DECL_PORTS(
			PORT(clk, CLK_TYPE, IN),
			PORT(reset1_n, RST_TYPE, IN),
			PORT(min, SLV_TYPE(5), IN),
			PORT(mout, SLV_TYPE(5), OUT)
			)
	);

	SIG(internal, SLV_TYPE(5));// internal;

	// Put all components bindings and processes next
	BEGIN

	BLK_INST(my_momo, momo_t,
	MAPPING(
			PM(clk, clk),
			PM(reset1_n, reset_n),
			PM(min, internal),
			PM(mout, zout)
			)
	);

	PROCESS(0, clk, reset_n)
	BEGIN
	IF ( reset_n == BIT(0) ) THEN
		internal <= SLV(0, LEN(internal));
	ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
		internal <= zin + ( internal + SLV(2, LEN(internal)) );
	ENDIF
	END_PROCESS;

	BLK_END;
#else
#if 1
	RECORD(yeye_t,
			DECL_FIELDS(
					FIELD(bobo, UINT(5)),
					FIELD(baba, UINT(9))
					)
	);
#else
	 struct yeye_t_base{ slv<5> bobo; slv<9> baba;};
	 struct yeye_t : yeye_t_base, vcd_entry
	 { vcd_entry* pvcd_entry = static_cast<vcd_entry*>(this); using yeye_t_base::bobo; using yeye_t_base::baba;
	 yeye_t() {}
	 yeye_t(const sig_desc& x) : vcd_entry(x)
	 { vcd_entry::nbits = -2048; x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); std::string rec_name = x.name;
	 {std::string __zozo__ = rec_name + '.' + "bobo"; bobo.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, bobo.length);}
	 {std::string __zozo__ = rec_name + '.' + "baba"; baba.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, baba.length);}
	 }
	 void activate()
	 {
		 bobo.pvcd_entry->driver = &( (vcd_entry::driver)->bobo);
		 bobo.pvcd_entry->activate();
		 baba.pvcd_entry->driver = &((*(vcd_entry::driver)).baba);
		 baba.pvcd_entry->activate();
	 } void operator <= (const yeye_t& x){ bobo <= x.bobo; baba <= x.baba; } void operator = (const yeye_t& x){ bobo = x.bobo; baba = x.baba; } void operator <= (const yeye_t_base& x){ bobo <= x.bobo; baba <= x.baba; } void operator = (const yeye_t_base& x){ bobo = x.bobo; baba = x.baba; } static const int length = -2048; };

#endif
	CONSTANT BASE_RECORD_TYPE(yeye_t) yeye_t0 := LIST(0,0);

	INCLUDES

	ENTITY(momo_t,
	DECL_PORTS(
			PORT(clk, CLK_TYPE, IN),
			PORT(reset1_n, RST_TYPE, IN),
			PORT(min, UINT(5), IN),
			PORT(yeye_o, yeye_t, OUT),
			PORT(mout, UINT(5), OUT)
			)
	);
#if 1

#else
	 struct yeye_t_base{ slv<5> bobo; slv<9> baba;}; struct yeye_t : yeye_t_base, vcd_entry{ using yeye_t::bobo; using yeye_t::baba; } yeye_t(const sig_desc& x) : vcd_entry(x){ vcd_entry::nbits = -1024; x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); std::string rec_name = x.name; {std::string __zozo__ = rec_name + '.' + "bobo"; bobo.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, -16);} {std::string __zozo__ = rec_name + '.' + "baba"; baba.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, -16);} } void activate() { bobo.pvcd_entry->activate(); baba.pvcd_entry->activate(); } void operator <= (const yeye_t& x){ bobo <= x.bobo; baba <= x.baba; }};

#endif
	//struct yeye_t_base { slv<5> bobo; slv<9> baba;}; struct yeye_t : yeye_t_base ,vcd_entry { yeye_t(const sig_desc& x) : vcd_entry(x){ vcd_entry::nbits = -1024; x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); std::string rec_name = x.name; bobo.pvcd_entry = create_vcd_entry("yeye_t.bobo", x.pmodule, -16); baba.pvcd_entry = create_vcd_entry("yeye_t.baba", x.pmodule, -16); } void activate() { bobo.pvcd_entry->activate(); baba.pvcd_entry->activate(); } void operator <= (const yeye_t& x){ bobo <= x.bobo; baba <= x.baba; };

	//SIG(yuyu, yeye_t);
	TYPE(tata_t, ARRAY_TYPE(UINT(5), 4));
	CONSTANT BASE_ARRAY_TYPE(UINT(5), 4) tata_t0 := LIST(1,2,3,4);
	CONSTANT BASE_ARRAY_TYPE(UINT(5), 4) my_array_t0 = LIST(BIN(00000), BIN(00001), BIN(00010), BIN(00011));
	SIG(tata, tata_t);
	SIG(gogo, INT(5));
	SIG(toggle, UINT(1));
	GATED_CLK(clk_g, CLK_TYPE, clk);
	BEGIN

	PROCESS(0, clk, reset1_n)
	BEGIN
		VAR(yuyu, yeye_t);
		IF ( reset1_n == BIT(0) ) THEN
			mout <= TO_UINT(7, LEN(mout));
			//yuyu.bobo <= TO_UINT(0, LEN(yuyu.bobo));
			yeye_o <= yeye_t0;
			tata <= OTHERS(TO_UINT(0,5));
			gogo <= (TO_INT(0,5));
			toggle <= BIT(0);
		ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
			yuyu := yeye_t0;
			mout <= min + TO_UINT(4, LEN(mout));
			yuyu.bobo := min;
			gogo <= SIGNED(PORT_BASE(min));
			yeye_o <= yuyu;
			tata(0) <= yuyu.bobo;
			toggle <= not toggle;
		ENDIF
	END_PROCESS;
/*
	COMB_PROCESS(1, clk)
	BEGIN
		//gprintf("#m clk_g %", clk_g);
		clk_g <= (PORT_BASE(clk) and toggle);
		//gprintf("#<<<	m clk_g % ", clk_g);

	END_PROCESS;

	PROCESS(2, clk_g, reset1_n)
	BEGIN
		VAR(yuyu, yeye_t);
		IF ( reset1_n == BIT(0) ) THEN
			gogo <= (TO_INT(0,5));
		ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
			gogo <= SIGNED(PORT_BASE(min));

		ENDIF
	END_PROCESS;
*/

	BLK_END;


	INCLUDES

		ENTITY(zozo_t,
		DECL_PORTS(
				PORT(clk, CLK_TYPE, IN),
				PORT(reset_n, RST_TYPE, IN),
				PORT(zin, UINT(5), IN),
				PORT(zout, UINT(5), OUT)
				)
		);

		// Put all internal signals and component declarations (not variables !)
		COMPONENT(momo_t,
		DECL_PORTS(
				PORT(clk, CLK_TYPE, IN),
				PORT(reset1_n, RST_TYPE, IN),
				PORT(min, UINT(5), IN),
				PORT(yeye_o, yeye_t, OUT),
				PORT(mout, UINT(5), OUT)
				)
		);

		SIG(internal, UINT(5));// internal;
		SIG(my_yeye, yeye_t);
		// Put all components bindings and processes next
		BEGIN

		BLK_INST(my_momo, momo_t,
		MAPPING(
				PM(clk, clk),
				PM(reset1_n, reset_n),
				PM(min, internal),
				PM(yeye_o, my_yeye),
				PM(mout, zout)
				)
		);

		PROCESS(0, clk, reset_n)
		BEGIN
		IF ( reset_n == BIT(0) ) THEN
		gprintf("#RRESET INTERNAL");
			internal <= TO_UINT(0, LEN(internal));
		ELSEIF ( EVENT(clk) and (clk == BIT(1)) ) THEN
			internal <= zin  +( internal + TO_UINT(2, LEN(internal)) );
			//internal <= zin + SIGNED(PORT_BASE(zin)) +( internal + TO_UINT(2, LEN(internal)) );
		ENDIF
		END_PROCESS;

		BLK_END;

#endif

#include "structures.h"
#include "spram64x32.h"
#include "risc-V_core.h"

int main()
{
	slv<5> a = gen_sig_desc("a", gmodule::out_of_hier);
	slv<5> b = gen_sig_desc("b", gmodule::out_of_hier);
	//SIG(a, gmodule::out_of_hier);
	//SIG(b, gmodule::out_of_hier);

	//momo_t<0> my_momo(gen_blk_map("my_momo", gmodule::out_of_hier, momo_t<0>::in_stat, a, momo_t<0>::out_stat, b));
	//momo_t<0>* pmy_momo = new momo_t<0>(gen_blk_map("my_momo", gmodule::out_of_hier, momo_t<0>::in_stat, a, momo_t<0>::out_stat, b));
	//momo_t<0>& my_momo = *pmy_momo;


	TOP_SIG(clk, CLK_TYPE);
	TOP_SIG(reset_n, RST_TYPE);// reset_n;

	gprintf("#Mclk % reset % ptrs", &clk, &reset_n);

	gprintf("ptrs % % \n", &momo_t<0>::min, &momo_t<0>::mout);
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
	gprintf("#Ublk inst top");
	BLK_INST_TOP(
			titi, zozo_t,
			MAPPING(
					PM(clk, clk),
					PM(reset_n, reset_n),
					PM(zin, a),
					PM(zout, b)
					)
			);

	// Obligatory order !!
	init_vcd();
	clk.parse_modules();

	vcd_file.activate();

	gprintf("#VYO");
	a <= 1;
	b <= 2;
	reset_n <= BIN(0);
	clk <= BIN(1);
	clk <= BIN(0);
	clk <= BIN(1);
	reset_n <= BIN(1);
	for (int i = 0; i < 100000 ; i++)
	{
		//gprintf("#Rclk a % b %", a,b);
		clk <= not clk;
	}


	gprintf("#VEnd of simulation, time is % ps", vcd_file.vcd_time * vcd_file.timebase_ps);

	for (int i = 2; i < 256; i++)
		gprintf(" #define EVAL%d(...) EVAL1(EVAL%d(__VA_ARGS__))\n", i, i-1);

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
