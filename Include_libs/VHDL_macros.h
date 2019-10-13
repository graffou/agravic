#define CONSTANT constant
//#define CONST(a,n) constant a : std_logic_vector((n-1) downto 0)
//#define CASE_CONST(a,n) constant a : std_logic_vector((n-1) downto 0)
#define SLV(a, n) conv_std_logic_vector(a, n)
#define SLV_TYPE(n) std_logic_vector((n-1) downto 0)
#define TO_SLV(a) std_logic_vector(a)
#define CLK_TYPE std_logic
#define RST_TYPE std_logic
#define BIT_TYPE std_logic

#define SIGNED(a) signed(a)
#define UNSIGNED(a) unsigned(a)

#define SIGNED_TYPE(n) signed ((n-1) downto 0)
#define UNSIGNED_TYPE(n) unsigned ((n-1) downto 0)
#define INT(n) signed ((n-1) downto 0)
#define UINT(n) unsigned ((n-1) downto 0)
// OK for vhdl
//#define RESIZE(a,n) a.resize<n>()

// For unsigned ints only
#define EXT(x, n) RESIZE(x, n)
#define SXT(x, n) UNSIGNED(RESIZE(SIGNED(x), n))

#define TO_INT(a,n) TO_SIGNED(a,n)
#define TO_UINT(a,n) TO_UNSIGNED(a,n)

#define PORT_BASE(a) a // useless in vhdl

#define CAT(a,b) a & b
#define STRING(a) #a
#define BIN(a) #a
#define CASE_BIN(a) #a
#define HEX(a) x##a

#define BIT(a) IF_ELSE(a) ('1')('0')
#define EQ(a,b) (a=b)

// added unsigned since vhdl find an ambiguity to & operator when array types are defined as well ??????!!!!!!
#define TYPE_UNSIGNED_CONV UNSIGNED'
#define RANGE(a,b,c) (a(b downto c)) //a.range<b,c>()
#define SLV_RANGE(a,b,c) std_logic_vector(a(b downto c)) //a.range<b,c>()
#define B(a,b) a(b)
#define HI(a) a'high
#define LEN(a) a'length
#define ELSEIF elsif
#define THEN then
#define ENDIF end if;
#define ELSE  else
#define SWITCH(a) case a is
#define CASE(b) when b =>
#define DEFAULT when others =>
#define ENDCASE end case;
#define VA =
#define ARRAY_TYPE(t, n) array(0 to (n-1)) of t
#define TYPE(a, t) type a is t
#define SIG(a, t) signal a : t
#define VAR(a, t) variable a : t
#define CONST(a, t) constant a : t
#define CASE_CONST(a, t) constant a : t
#define REC(a) type a is record
#define ENDREC end record;
#define MEMBER(a, t) a : t
#define PORT_IN(a,t) a : in t
#define PORT_OUT(a,t) a : out t
#define END_SEMICOLON(a) a;
#define END_COMMA(a) a,

#define END_NOTHING(a) a
#define ENTITYz(name, ...) entity name is port( EVAL(MAP2(END_SEMICOLON, END_NOTHING, __VA_ARGS__)) ) end
//#define COMPONENT(name, ...) component name is port( EVAL(MAP2(END_SEMICOLON, END_NOTHING, __VA_ARGS__)) ) end component
//#define ENTITY(name, ...)  EVAL(MAP2(END_SEMICOLON, END_NOTHING, __VA_ARGS__))
#define LIST(...) ( __VA_ARGS__ )
#define gkprintf(...) --

#define get1_PM(a,b) a
#define get2_PM(a,b) b
#define get_MAPPING(...) __VA_ARGS__
#define get_GENERIC(...) __VA_ARGS__
#define get1_PORT(a,b,c) a
#define get2_PORT(a,b,c) b
#define get3_PORT(a,b,c) c
#define get_DECL_PORTS(...) __VA_ARGS__

#define FIELD_DEF(name, type) name : type;
#define get_DECL_FIELDS(...) __VA_ARGS__
#define get1_FIELD(a,b) a
#define get2_FIELD(a,b) b

#define STRINGIFY(x) #x// EVAL(x)
#define PORT_DEF(name, type, in) name :  \
		IF_ELSE(in) (in) (out) type
#define DECL_PORT_SEMI(a) PORT_DEF(EVAL1(get1_##a),EVAL1(get2_##a),EVAL1(get3_##a));
#define DECL_PORT(a) PORT_DEF(EVAL1(get1_##a),EVAL1(get2_##a),EVAL1(get3_##a))
#define DECL_PORTS(...) EVAL(MAP2(DECL_PORT_SEMI, DECL_PORT, __VA_ARGS__))

#define ENTITY(type, ports, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(template<int dummy0, __VA_ARGS>)(entity)  type is port(/*
		*/ EVAL(DECL_PORTS(get_##ports)) /*
		*/ ); end type; architecture rtl of type is component dummy_zkw_pouet is port(clk : in std_logic);end component//std::cerr << "CTOR " << name << "\n";}

#define TESTBENCH(type) entity  type is /*
		*/ begin end type; architecture rtl of type is component dummy_zkw_pouet is port(clk : in std_logic);end component//std::cerr << "CTOR " << name << "\n";}

#define COMPONENT(type, ports, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(template<int dummy0, __VA_ARGS>)(component)  type is port(/*
		*/ EVAL(DECL_PORTS(get_##ports)) /*
		*/ ); end component //std::cerr << "CTOR " << name << "\n";}

#define END_ENTITY //}

#define BLK_END end rtl

#define PORT_PTR(type, name) name //type::## name
#define VAR_PTR(name) name //type::## name
#define PORT_MAP_COMMA(type,b) PORT_PTR(type, EVAL1(get1_##b)) => VAR_PTR(EVAL1(get2_##b)),
#define PORT_MAP(type,b) PORT_PTR(type, EVAL1(get1_##b)) => VAR_PTR(EVAL1(get2_##b))



//#define PORT_MAPS(type,  ...) EVAL1(MAP_PLUS1(type, PORT_MAP2, __VA_ARGS__))
#define PORT_MAPS(type,  ...) EVAL(MAP_ALT_PLUS1(type, PORT_MAP_COMMA, PORT_MAP, __VA_ARGS__))
#define APPLY_MAP(type, port_map) PORT_MAPS(type, EVAL1(get_##port_map))

// Pass generic parameters - to be used later
#define APPLY_GENERIC(a) EVAL1(get_##a)

// instantiation of block inside hierarchy - all except testbench
#define BLK_INST(name, type, port_map, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
	(type<0,APPLY_GENERIC(__VA_ARGS__)>& name = *create_block(#name, this, APPLY_MAP(type<0,APPLY_GENERIC(__VA_ARGS__)>, port_map)))\
	(name : type port map( APPLY_MAP(type<0>, port_map)))


#define DECL_FIELD(a) FIELD_DEF(EVAL1(get1_##a),EVAL1(get2_##a))
#define DECL_FIELDS(...) EVAL(MAP(DECL_FIELD, __VA_ARGS__))

#define RECORD(typename, ports)	type typename is record \
		EVAL(DECL_FIELDS(get_##ports))\
		end record


#define EVENT(a) a'event

// this is fake in vhdl
#define COMB_PROCESS(a,b)
#define END_COMB_PROCESS

#define PROCESS(a,b,c) process##a : process(b,c)
#define BEGIN begin
#define END_PROCESS end process;
#define INTERNAL_SIGNALS

#define INCLUDESz library ieee;\
use ieee.std_logic_1164.all;\
use ieee.std_logic_arith.all;\
use ieee.std_logic_unsigned.all;
#define START_OF_FILE(a) -- vhdl file of block a generated by Agravic
#define INCLUDES library ieee;\
use ieee.std_logic_1164.all;\
use IEEE.NUMERIC_STD.ALL;
#define OTHERS(a) (others => a)
#define RESET(a) a <= TO_UINT(0, LEN(a))

#define PACKAGE(a) package a is
#define END_PACKAGE(a) end a;
#define USE_PACKAGE(a) library work; use work.a.all;

#define gprintf(...) -- gprintf(__VA_ARGS__)