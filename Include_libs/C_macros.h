
#define DIV(a, b) a\b


#define SIGNED(a) conv_signed(a)
#define UNSIGNED(a) conv_unsigned(a)
#define SLV(a, n) slv<n>(a)
#define BOOLEAN bool
#define SLV_TYPE(n) slv<n>
#define TO_SLV(a) a
#define TO_LOGIC(a) slv<1>(a)
#define PORT_BASE(a) a.base_type()  // Required to perform something that is not supported by the std:reference_wrapper used for port class ex: SIGNED(PORT_BASE(data_i))
#define SIGNED_TYPE(n) Signed<n>
#define UNSIGNED_TYPE(n) slv<n>
#define INT(n) Signed<n>
#define UINT(n) slv<n>
#define CPX_INT(n) cpx_int<n>
#define TRISTATE(n) tristate<n>

#define RESIZE(a,n)  a.template resize<n>()

#define TO_UNSIGNED(a,n) slv<n>(a)
#define TO_SIGNED(a,n) Signed<n>(a)
#define TO_UINT(a,n) slv<n>(a)
#define TO_BIT(a) slv<1>(a)
#define TO_INT(a,n) Signed<n>(a)
#define TO_INTEGER(a) conv_integer(a)

#define CLK_TYPE clk_t
#define RST_TYPE reset_t
#define BIT_TYPE slv<1>
#define CAT(a,b) a.cat(b)
#define STRING(a) #a
#define BIN(a) slv<sizeof(STRING(a))-1>(0b##a)
#define CASE_BIN(a) (0b##a)
#define HEX(a) slv<(sizeof(STRING(a))-1)*4>(0x##a)
#define HEX_INT(a) int( CAT_(0x, a) ) //0x##a
#define BIT(a) slv<sizeof(STRING(a))-1>(0b##a)
#define EQ(a,b) (a==b)
#define EXT(a,b) slv<b>(adjust<b>((a).n))
#define SXT(a,b) slv<b>(adjust<b>(conv_signed(a).conv_int()))
#define SSXT(a,b) Signed<b>(adjust<b>(conv_signed(a).conv_int()))
#define SIGNED(a) conv_signed(a)
#define UNSIGNED(a) conv_unsigned(a)
#define SABS(a) (a).sabs()
#define RANGE(a,b,c) (a.template range<b,c>())
#define SUBVECTOR(a,b,c) (a.template range<b>(c)) // Version of RANGE which extracts a vector of len b from pos c
#define SLV_RANGE(a,b,c) a.range<b,c>() // for further concatenation in vhdl
#define B(a,b) (a).get_bit(b)
#define VAR_SET_BIT(a, b, c) a.set_bit(b, c) //ONLY VARIABLES !!!!!!!!!!!!!!!!!!!!
#define SIG_SET_BIT(a, b, c) a.set_bit(b, c) //ONLY COMB !!!!!!!!!!!!!!!!!!!!
#define VAR_SET_RANGE(a, b, c, d) a.set_range(b, c, d) //ONLY VARIABLES !!!!!!!!!!!!!!!!!!!!
#define SIG_SET_RANGE(a, b, c, d) a.set_range(b, c, d) //ONLY COMB !!!!!!!!!!!!!!!!!!!!
#define HI(a) decltype(a)::high
#define LEN(a) decltype(a)::length
#define ARRAY_LEN(a) decltype(a)::array_length
#define IF if
#define THEN {
#define ENDIF }
#define ELSE } else {
#define ELSEIF } else if
#define SWITCH(a) switch(TO_INTEGER(a)){
#define CASE(b) break;case b: //.get_const()://const_conv_int(b) ://const_conv_int(b):
#define DEFAULT break;default:
#define ENDCASE }
#define VA =
#define ARRAY_TYPE(t, n) array<array_base<t,n>>
#define BASE_ARRAY_TYPE(t, n) array_base<t,n>
#define BASE_RECORD_TYPE(t) t##_base

#define TYPE(a, t) typedef t a
#define TOP_SIG(a, t)  t a = gen_sig_desc(#a, gmodule::out_of_hier)
//#define CONST(a, t)  const t a
#define SIG(a, t)  t a = gen_sig_desc(#a, this)
#define DECL_GATED_CLK(name)
#define GATED_CLK(name, clk_i, gate)  clk_t name = gen_gated_clk_desc(#name, this, clk_i, gate)
#define GATED_CLK_INV_POL(name, clk_i, gate)  clk_t name = gen_gated_clk_desc(#name, this, clk_i, gate, 0)
// For a clock mux, declare clock from clk1 with positive polarity, then reassign it to clk2 with negative polarity: this should push newly created clock to both clk1 and clk2 clk children trees
// Beware of the "no delay" switch behavior, which is not the case in a real implementation case
//#define CLOCK_MUX(name, clk1, clk2, sel) clk_t name = gen_gated_clk_desc(#name, this, clk2, sel); name = gen_gated_clk_desc(#name, this, clk1, sel, 0);
#define CLK_MUX(name, clk1, clk2, sel) clk_t name = gen_clk_duo(#name, this, clk1, clk2, sel);
#define GATE_CLK(clk_i, gated_clk, gating_signal) gated_clk.gate_clk(clk_i, gating_signal,1)
#define VAR(a, t)  t a //= gen_sig_desc(#a, this)
//#define VAR(a, t) static t a // More like VHDL behavior, however static vars might generate latches
#define CONST(a, t)   const t a
#define CASE_CONST(a, t)   const int a
#define MEMBER(a, t) t a
#define REC(a) struct a {
#define ENDREC };
#define LIST(...) { __VA_ARGS__ }
#define TEMPLATE_TYPE(type, ...) type< __VA_ARGS__ >
//#define EVENT(a) static_cast<tree>(a.get()).event()//CAT_(a,.event())
#define EVENT(a) (a.get()).event()//CAT_(a,.event())
#define CONSTANT const
#define OTHERS(a) __others(a)
#define RESET(a) a <= TO_UINT(0, LEN(a))
#define shift_left(a, b) a.sll(b)
#define shift_right(a, b) a.srl(b)
#define ROTATE_LEFT(a, b) a.sla(b)
#define ROTATE_RIGHT(a, b) a.sra(b)
#define SHIFT_LEFT(a, b) a.sll(b)
#define SHIFT_RIGHT(a, b) a.srl(b)
#define FOR(a,b,c) for (int a = b; a <= c; a++) {
#define ENDLOOP }
#define EQU(a, b) (a == TO_UINT(b, LEN(a)))
#define GT(a, b) (a > TO_UINT(b, LEN(a)))
#define GTE(a, b) (a >= TO_UINT(b, LEN(a)))
#define LT(a, b) (a < TO_UINT(b, LEN(a)))
#define LTE(a, b) (not (a > TO_UINT(b, LEN(a))))

#define SAT(x, n) x.template sat<n>()
#define SYM_SAT(x, n) x.template sym_sat<n>()
#define PSAT(x, n) x.template psat<n>()
#define TRUNC_STD(x, n) x.template trunc_std<n>()
#define SROUND(x, n) x.template sround<n>()
#define SROUND_SAT(x, n) x.template sround_sat<n>()
#define VAR_TRUNC_STD(x, y, n) x.template trunc_std<n>(y)
#define VAR_SROUND(x, y, n) x.template sround<n>(y)
#define VAR_SROUND_SAT(x, y, n) x.template sround_sat<n>(y)


#ifndef VHDL

#ifdef NOPRINT
#define giprintf(...)
#endif

#define START_OF_FILE(a) // c++ file of block a generated by platform GS

// template parameters for in and out ports
#define IN 1
#define OUT 0
#define INOUT 0 // in or out ports should be able to accept assignments for either side.

// port definition
//#define PORT_DEF(name, type, in) port<type, in> name = gen_sig_desc(#name, this);
#define STRINGIFY(x) #x// EVAL(x)

//#define PORT_DEF(name, type, in) port<type, in> name = gen_sig_desc(STRINGIFY(<name), this);
//#define PORT_DEF(name, type, in) port<type, in> name = gen_sig_desc(STRINGIFY(CAT_(PORT_CHAR(in), name)), this);
#define PORT_DEF(name, type, in) port<type, in> name = \
		IF_ELSE(in) (gen_sig_desc(STRINGIFY(<name), this)) (gen_sig_desc(STRINGIFY(>name), this));

#define FIELD_DEF(name, type) type name;
// get first and second args. of port mapping
#define get1_PM(a,b) a
#define get2_PM(a,b) b
#define get_MAPPING(...) __VA_ARGS__
#define get_GENERIC(...) __VA_ARGS__
#define get1_PORT(a,b,c) a
#define get2_PORT(a,b,c) b
#define get3_PORT(a,b,c) c
#define get_DECL_PORTS(...) __VA_ARGS__
#define get_DECL_FIELDS(...) __VA_ARGS__
#define get1_FIELD(a,b) a
#define get2_FIELD(a,b) b


// because we have to pass a member pointer (to a port) and a pointer to something to refer to
// pointers required because port map list is a template recurse, and member pointer passing require that all parameters are passed by value
// and port driver recursion on ports require that they are passed by reference
#define PORT_PTR(type, name) &type::name //type::## name

#define VAR_PTR(name) &name //type::## name

#define PORT_MAP_COMMA(type,b) PORT_PTR(type, EVAL1(get1_##b)), VAR_PTR(EVAL1(get2_##b)),
#define PORT_MAP(type,b) PORT_PTR(type, EVAL1(get1_##b)), VAR_PTR(EVAL1(get2_##b))

//#define PORT_MAP_COMMA(type,b) PORT_PTR(type, EVAL(get1_##b)), VAR_PTR(EVAL(get2_##b)),
//#define PORT_MAP(type,b) PORT_PTR(type, EVAL(get1_##b)), VAR_PTR(EVAL(get2_##b))


//#define PORT_MAPS(type,  ...) EVAL1(MAP_PLUS1(type, PORT_MAP2, __VA_ARGS__))
#define PORT_MAPS(type,  ...) EVAL(MAP_ALT_PLUS1(type, PORT_MAP_COMMA, PORT_MAP, __VA_ARGS__))
//#define PORT_MAPS2(type,  ...) PM**** __VA_ARGS__ **** MAP_ALT_PLUS1(type, PORT_MAP_COMMA, PORT_MAP, __VA_ARGS__) MP
// needs too many recursions #define PORT_MAPS2(type,  ...) MAP_ALT_PLUS1(type, PORT_MAP_COMMA, PORT_MAP, __VA_ARGS__)
#define PORT_MAPS2(type,  ...) EVAL(MAP_ALT_PLUS1(type, PORT_MAP_COMMA, PORT_MAP, __VA_ARGS__))
#define GEN_BIND(name, ...) gen_bind(name, )

#define BLK_CTOR(name) name(char* name_i, gmodule* parent) : gmodule::gmodule(name_i, parent)

#define APPLY_PORT(decl) EVAL1(get_##decl)


#define DECL_PORT(a) PORT_DEF(EVAL1(get1_##a),EVAL1(get2_##a),EVAL1(get3_##a))
#define DECL_PORTS(...) EVAL(MAP(DECL_PORT, __VA_ARGS__))
#define DECL_FIELD(a) FIELD_DEF(EVAL1(get1_##a),EVAL1(get2_##a))
#define DECL_FIELDS(...) EVAL(MAP(DECL_FIELD, __VA_ARGS__))

#define FIELD_ACT_VCD(name) name.pvcd_entry->binary = binary; name.pvcd_entry->activate();
#define FIELD_ASSIGN(name)  name <= x.name;
#define FIELD_EQ(name)  name = x.name;

#define RECORD_NAME1(a,b) a.b
#define RECORD_NAME2(a,b) STRINGIFY(EVAL(RECORD_NAME1(a,b)))

//#define FIELD_VCD1(name)   create_vcd_entry(STRINGIFY(name), x.pmodule, -16)
#define FIELD_VCD2(name)   STRINGIFY(name)
#define FIELD_VCD1(...)   __VA_ARGS__

#define FIELD_VCD(name)  {std::string __zozo__ = rec_name + '.' + FIELD_VCD2(name); name.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, (*this).length + name.length);}

#define FIELD_USING1(recname, name) FIELD_USING2(recname, name)
#define FIELD_USING(recname, name) using FIELD_USING2(recname, name);
#define FIELD_USING2(recname, name) recname##_base::name

//#define FIELD_USING(recname, name)  using recname##_base::##name;

#define USING_FIELDS(type, ...) EVAL(MAP_PLUS1(type, USING_FIELD, __VA_ARGS__))
#define USING_FIELD(recname, a) FIELD_USING(recname,EVAL1(get1_##a))

#define VCD_FIELDS( ...) EVAL(MAP(VCD_FIELD, __VA_ARGS__))
#define VCD_FIELD(a) FIELD_VCD(EVAL1(get1_##a))

#define VCD_ACT_FIELDS(...) EVAL(MAP(VCD_ACT_FIELD, __VA_ARGS__))
#define VCD_ACT_FIELD(a) FIELD_ACT_VCD(EVAL1(get1_##a))
#define ASSIGN_FIELDS(...) EVAL(MAP(ASSIGN_FIELD, __VA_ARGS__))
#define ASSIGN_FIELD(a) FIELD_ASSIGN(EVAL1(get1_##a))
#define EQ_FIELDS(...) EVAL(MAP(EQ_FIELD, __VA_ARGS__))
#define EQ_FIELD(a) FIELD_EQ(EVAL1(get1_##a))

#define BASE_TYPE(type) type##_base

#define get_GEN(...) __VA_ARGS__
#define DEFAULT_VAL(...) IF_ELSE(HAS_ARGS(__VA_ARGS__))( = __VA_ARGS__)()
#define GENERIC2(a) SECOND( a ) FIRST(a) DEFAULT_VAL(THIRD_ETC(a))
#define GENERIC0(a, ...)  __VA_ARGS__ //GENERIC2(EVAL1(get_##a))

#define GENERIC_DECL_SEMI(a) GENERIC2(get_##a), //GENERIC2(EVAL1(get_##a))GENERIC1(a);
#define GENERIC_DECL(a) GENERIC2(get_##a) //GENERIC1(a)
/*
#define ENTITY(type, ports, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
	( IF_ELSE(HAS_ARGS(GENERIC0(__VA_ARGS__))) (entity type is generic (EVAL(MAP2(GENERIC_DECL_SEMI, GENERIC_DECL, __VA_ARGS__))); port)(entity type is generic (generic_int: integer); port) )\
	(entity type is port ) (\
		EVAL(DECL_PORTS(get_##ports)) \
		); end type; architecture rtl of type is component dummy_zkw_pouet is port(clk : in std_logic);end component//std::cerr << "CTOR " << name << "\n";}
*/
// Block and ports declaration
//#define ENTITY(type, ports, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(template<int dummy0, __VA_ARGS>)(template<int dummy0>) struct type : gmodule {/*
#define ENTITY(type, ports, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
	(IF_ELSE(HAS_ARGS(GENERIC0(__VA_ARGS__))) (template<EVAL(MAP2(GENERIC_DECL_SEMI, GENERIC_DECL, __VA_ARGS__))>) (template<int generic_int>))\
	(template<int generic_int>) struct type : gmodule {/*
		*/ EVAL1(DECL_PORTS(get_##ports)) /*
		*/ type(const char*x, gmodule* y):gmodule(x,y) {}//std::cerr << "CTOR " << name << "\n";}
#define TESTBENCH(type, ...) template<int dummy0, int dummy1> struct type : gmodule {/*
		*/ type(const char*x, gmodule* y):gmodule(x,y) {}//std::cerr << "CTOR " << name << "\n";}

#define PACKAGE(a)
#define END_PACKAGE(a)
#define USE_PACKAGE(a)
#define INTEGER int
#if 0
#define RECORD(type, ports)	struct type : vcd_entry{\
		EVAL(DECL_FIELDS(get_##ports))\
		type(const sig_desc& x) : vcd_entry(x){\
		vcd_entry::nbits = -1024; /* marks record */\
		x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); \
		std::string rec_name = x.name; \
		EVAL(VCD_FIELDS(type,get_##ports)) }\
		void activate() {\
		EVAL(VCD_ACT_FIELDS(get_##ports)) }\
		void operator <= (const type& x){\
		EVAL(ASSIGN_FIELDS(get_##ports)) }\
}
#else
#define RECORD(type, ports)	struct type##_base{\
		EVAL(DECL_FIELDS(get_##ports))};\
		struct type : type##_base, vcd_entry{/*\
		~type() = default; \
*/		vcd_entry* pvcd_entry = static_cast<vcd_entry*>(this);\
		EVAL(USING_FIELDS(type,get_##ports)) \
		type() {}\
		~type(){giprintf("#RDestroying record %", pvcd_entry->name);}\
		type(const sig_desc& x) : vcd_entry(x){\
		giprintf("#VNew record : %", x.name);\
		vcd_entry::nbits = -2048; /* marks record */\
		x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); \
		std::string rec_name = x.name; \
		EVAL(VCD_FIELDS(get_##ports)) }\
		void conf_vcd_entry(const sig_desc& x){\
		giprintf("#VNew record VCD configuration: %", x.name);\
		pvcd_entry->name = x.name;\
		pvcd_entry->pmodule = x.pmodule;\
		pvcd_entry->driver = this;\
		pvcd_entry->nbits = -2048;\
		x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); \
		std::string rec_name = x.name; \
		EVAL(VCD_FIELDS(get_##ports)) }\
		void activate() {\
		EVAL(VCD_ACT_FIELDS(get_##ports)) }\
		void operator <= (const type& x){\
		EVAL(ASSIGN_FIELDS(get_##ports)) }\
		void operator = (const type& x){\
		EVAL(EQ_FIELDS(get_##ports)) }\
		void operator <= (const type##_base& x){\
		EVAL(ASSIGN_FIELDS(get_##ports)) }\
		void operator = (const type##_base& x){\
		EVAL(EQ_FIELDS(get_##ports)) } \
		static const int length = -2048;\
		static const int high = -2048;\
		static const int size = -2048;\
		void copy_children(const type& x_i){}\
		}
#endif
#define END_ENTITY //}
#define COMPONENT(type, ports, ...)

#define INTERNAL_BINDINGS
#define END_INTERNAL_BINDINGS

#define INTERNAL_SIGNALS
#define END_INTERNAL_SIGNALS

// end of block declaration
#define BLK_END }

//#define APPLY_MAP(type, port_map) APPLY_MAP2(EVAL(type), port_map) //PORT_MAPS(type, EVAL1(get_##port_map))
#define APPLY_MAP(type, port_map) PORT_MAPS(type, EVAL1(get_##port_map))
#define APPLY_MAP2(type, port_map) PORT_MAPS2(type, EVAL1(get_##port_map))

// Pass generic parameters - to be used later
#define APPLY_GENERIC(a) EVAL1(get_##a)

#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
// instantiation of block inside hierarchy - all except testbench
#if 1
#define BLK_INST(name, type, port_map, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
		(typedef type<__VA_ARGS__> CONCATENATE(type, __LINE__) ; \
		type<__VA_ARGS__>& name = *create_block< type<__VA_ARGS__> >(#name, this, APPLY_MAP( CONCATENATE(type, __LINE__), port_map)))\
	(type<0>& name = *create_block< type<0> >(#name, this, APPLY_MAP(type<0>, port_map)))
#else

#endif
#define BLK_INST2(count, name, type, port_map, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
		(typedef type<__VA_ARGS__> CONCATENATE(type, __LINE__) ; typedef multiple_instance_idx<count> multiple_instance_idx_typedef;\
		type<__VA_ARGS__>& name = *create_block< type<__VA_ARGS__> >(#name, this, APPLY_MAP2( CONCATENATE(type, __LINE__), port_map)))\
	(type<0>& name = *create_block< type<0> >(#name, this, APPLY_MAP2(type<0>, port_map)))


//type<0,APPLY_GENERIC(__VA_ARGS__)>& name = *create_block(#name, this, APPLY_MAP(type<0,APPLY_GENERIC(__VA_ARGS__)>, port_map)))\
		(type<__VA_ARGS__>& name = *create_block< type<__VA_ARGS__> >(#name, this, APPLY_MAP(type<__VA_ARGS__>, port_map)))\

// Instantiation of top rtl block - should be testbench
/*#define BLK_INST_TOP(name, type, port_map, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
	(type<0,APPLY_GENERIC(__VA_ARGS__)>& name = *create_block(#name, gmodule::out_of_hier, APPLY_MAP(type<0,APPLY_GENERIC(__VA_ARGS__)>, port_map)))\
	(type<0>& name = *create_block<type<0>>(#name, gmodule::out_of_hier, APPLY_MAP(type<0>, port_map)))*/
//#define BLK_INST_TOP(name, type, ...) type<0>& name = *create_block<type<0>>(#name)
#define BLK_INST_TOP(name, type, ...) type<0,0>& name = *create_block<type<0,0>>(#name)


#define FOREVER(type) template<int dummy##type> struct type : gmodule {\
		 type(const char*x, gmodule* y):gmodule(x,y) {}//std::cerr << "CTOR " << name << "\n";}
#define CALL_FOREVER(idx) current_forever_process = idx; forever_processes[idx]();

//#define FOREVER_PROCESS(n) void process##n(){static int cnt = 0;
#define FOREVER_PROCESS(number) control_signals process##number(){  __control_signals__.clk = -1; __control_signals__.reset_n = -1;static int cnt = -1;
#define FOREVER_BEGIN if(cnt == 0){
#define FOREVER_WAIT(n) cnt = __LINE__; create_event(cur_time + (n<<8), current_forever_process);}else if (cnt == __LINE__){
#define FOREVER_WAIT_AND_LOOP(n) cnt = 0; create_event(cur_time + (n<<8), current_forever_process);}else if (cnt == __LINE__){

#define FOREVER_END  cnt = 0;} else if (cnt == -1){cnt = 0;/*giprintf("#RFOREVER init");*/} return(__control_signals__); }
#define EXIT_AND_BACK_AFTER(n) create_event(cur_time + (n<<8), current_forever_process);
#define POST_HCI_EVENT(n) create_event(cur_time + (n<<8), hci_process);
// process_void for trial purposes, when no clk nor reset
#define PROCESS_VOID(number) constexpr control_signals process##number(){  __control_signals__.clk = -1; __control_signals__.reset_n = -1;
//#define COMB_PROCESS(number, signal1) constexpr control_signals process##number(){ __control_signals__.clk = signal1.get().n; __control_signals__.reset_n = -1; //giprintf("#### % % % ####", name, signal1.get().n, signal2.get().n);
#define COMB_PROCESS(number, signal1) control_signals process##number(){ __control_signals__.clk = signal1.get().n; __control_signals__.reset_n = -1; //giprintf("#### % % % ####", name, signal1.get().n, signal2.get().n);

//#define PROCESS(number, signal1, signal2) constexpr control_signals process##number(){ __control_signals__.clk = signal1.get().n; __control_signals__.reset_n = signal2.get().n; //giprintf("#### % % % ####", name, signal1.get().n, signal2.get().n);
#define PROCESS(number, signal1, signal2)  control_signals process##number(){ __control_signals__.clk = signal1.get().n; __control_signals__.reset_n = signal2.get().n; //giprintf("#### % % % ####", name, signal1.get().n, signal2.get().n);

#define END_PROCESS return(__control_signals__); }
#define END_COMB_PROCESS return(__control_signals__); }


#define MAP_LIST(blk_name, blk_type, ...) EVAL(MAP_PLUS2(blk_name, blk_type)

#define BEGIN
//#define INCLUDES #include "slv.h"
#define INCLUDES

#define REG_NBITS ( ((generic_int) /  268435456 + 16) & 0xF )

// macros for memory genericity. Use MEM_**(generic_int) and set generic_int accordingly
// ex: generic_int(4<<8 | 10) for a 4-byte, 1024-depth memory
#define MEM_ADDR_NBITS(a) ( a & 255 )
#define MEM_ADDR_NDEPTH(a) ( 1 << ( ( a) & 255 ) )
#define MEM_ADDR_NBYTES(a) ( ( (a) >> 8 ) & 0x7 )


#else

#endif
