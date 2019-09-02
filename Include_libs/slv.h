#pragma once

#include <iostream>
#include "mbprt.h"
#include "macros.h"
#include <vector>
#include "vcd.h"
#include <typeinfo>
template<int N>
struct slv;

template<int N>
struct Signed;

struct flop
{
	uint64_t _d;
	virtual void clock(){gprintf("#RVirtual clock");};
};

// Must use this function and forward declare it to call it in slv class, but first argument is of clock type which derives from slv
template<class T1, class T2>
void register_flop(T1 x, T2 y);

// SLV unsigned -------------------------------------------------------------------------------------------------------------
// was originally intended to mimmic std_logic_vector, but finally is equivalent to signed(N-1 downto 0) of numeric_std package
template<int N>
struct slv : flop// inherits of flop / useless for combinational logic or variables, but much simpler (and little overhead)
{

	uint64_t n; // Stored value
	uint8_t init = 0; // Is set at first <= assignment. 1 -> flip-flop, 2-> combinational
	const uint64_t mask = (1ull << N) - 1; // Mask to keep N bits
	vcd_entry* pvcd_entry;

	slv(){
		pvcd_entry = &dummy_vcd_entry;// is a global variable which has 'non probing' attribute
	}

	// init with signal name and module ptr (for a signal that is vcd probable)
	slv(sig_desc x_i)
	{
		pvcd_entry = create_vcd_entry(x_i.name, x_i.pmodule, N);
		gprintf("#BNew slv name % slv ptr % vcd ptr % module ptr %R",pvcd_entry->name, this, pvcd_entry, pvcd_entry->pmodule);
	}

	// Construct with int. Should not be used directly (set private ?)
	slv(uint64_t x_i)
	{
		n = x_i;
		bit_adjust();
		//std::cerr << "!!" << n << "!!\n";
	}

	// Masks unnecessary bits (keep N bits)
	inline void bit_adjust()
	{
		n &= mask;
	}

	// Only assignment allowed is with same type signals
	void operator=(const slv<N>& x_i)
	{
		n = x_i.n;
	}

	// Arithmetic operators
	// types allowed are more restrictive than in VHDL to avoid errors
	slv<N> operator+(const slv<N>& x_i)
	{
		return slv<N>(n + x_i.n);
	}

	slv<N> operator-(const slv<N>& x_i)
	{
		return slv<N>(n - x_i.n);
	}

	bool operator==(const slv<N>& x_i) const
	{
		return(n == x_i.n);
	}

	bool operator>(const slv<N>& x_i)
	{
		return(n > x_i.n);
	}

	bool operator>=(const slv<N>& x_i)
	{
		return(n >= x_i.n);
	}

	bool operator<(const slv<N>& x_i)
	{
		return(n < x_i.n);
	}

	template<int P>
	slv<N+P> operator*(const Signed<P>& x_i)
	{
		return slv<N+P>( n * x_i.conv_int() );
	}

	template<int P>
	slv<N+P> operator*(const slv<P>& x_i)
	{
		return slv<N+P>( n * x_i.n );
	}


	// Signal assignment operator / depends of whether process is synchronous or not
	// TODO Should replicate that in Signed class and call vcd_dump_ull here
	inline void operator<=(const slv<N>& x_i)
	{
		if (init == 1) // flip flop
		{
			_d = x_i.n; // assign flip-flop input
			if ( (pvcd_entry->ID[0] != '#') and (x_i.n != n) )
			{
				vcd_file.vcd_dump_ull((_d), &pvcd_entry->ID[0], pvcd_entry->binary);
			}
		}
		else if (init == 2) // combinational
		{
			//gprintf("#comb vcd n % in % name % ID", n, x_i.n, pvcd_entry->name, pvcd_entry->ID);
			if ( (pvcd_entry->ID[0] != '#') and (x_i.n != n) )
			{
				n = x_i.n; // assign signal
				vcd_file.vcd_dump_ull((n), &pvcd_entry->ID[0], pvcd_entry->binary);
			}
			else
				n = x_i.n; // assign signal


		}
		else // do not know yet
		{
			// In platform GS, combinational processes have a clock (pre or post clock, only post for the moment), but only synchronous ones have a reset!
			gprintf("#RRegistering, reset id = % clk ID = %", __control_signals__.reset_n, __control_signals__.clk);
			gprintf("#bflop % in %", pvcd_entry->name, pvcd_entry->pmodule->name);
			if (__control_signals__.reset_n != -1) // not combinational
			{
				init = 1;
				gprintf("#rTo clk");
				_d = x_i.n; // reset value
				n = x_i.n; // reset value
				if (pvcd_entry->ID[0] != '#')
				{
					gprintf("#Rwriting vcd value %", _d);
					vcd_file.vcd_dump_ull((_d), &pvcd_entry->ID[0], pvcd_entry->binary);
				}
				else
				{
					gprintf("#Rnot probed yet");
				}
				register_flop(__control_signals__.clk, static_cast<flop*>(this));
			}
			else
			{
				gprintf("#rTo clk comb");
				init = 2;
				n = x_i.n;
				if (pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_ull((n), &pvcd_entry->ID[0], pvcd_entry->binary);
			}
		}
	}


	// bit vector concatenation
	template<int P>
	slv<N+P> cat(const slv<P>& x_i)
	{
		//std::cerr << n << " " << "x " << x_i.n << " " <<((n<<P)|(x_i.n));
		return slv<N+P>( (n<<P) | x_i.n );
	}
	// bit vector concatenation
	template<int P>
	slv<N+P> operator&(const slv<P>& x_i)
	{
		//std::cerr << n << " " << "x " << x_i.n << " " <<((n<<P)|(x_i.n));
		return slv<N+P>( (n<<P) | x_i.n );
	}
	// extract bit range toto.range<8,6>() <=> toto(8 downto 6)
	template <int P, int Q>
	slv<P-Q+1> range()
	{
		return slv<P-Q+1>(n>>Q);
	}

	// extract bit from bit vector
	slv<1> get_bit(int i)
	{
		return slv<1>(n >> i);
	}

	friend std::ostream& operator<< (std::ostream& os, const slv<N>& x)
	{
	        return os << x.n;
	}

	// mimmic vhdl attributes
	static const int high = N-1;
	static const int length = N;
	static const int size = 1;

	slv<N> sll(int x_i)
		{
			return slv<N>(n << x_i);
		}
	slv<N> srl(int x_i)
		{
			return slv<N>(n >> x_i);
		}
	//bitwise not
	slv<N> operator!()
	{
		slv<N> res(~n);
		return slv<N>(~n);
	}
	//bitwise and
	slv<N> operator&&(const slv<N>& x_i)
	{
		return slv<N>(n & x_i.n);
	}
	//bitwise or
	slv<N> operator||(const slv<N>& x_i)
	{
		return slv<N>(n | x_i.n);
	}
	//bitwise xor
	slv<N> operator^(const slv<N>& x_i)
	{
		return slv<N>(n ^ x_i.n);
	}

	// called after clocked process execution
	void clock()
	{
		//gprintf("#R% in % out %", pvcd_entry->name, _d, n);
		n = _d;
	}

	// replaces ext() and sxt()
	template <int m>
	slv<m> resize()
	{
		return slv<m>(n);
	}
};

// finally unused ?
/*
template< template<int> class T, int N>
constexpr int length( T<N> x )
{
	return N;
}
*/
// Signed version of slv
template<int N>
struct Signed:slv<N>
{


	Signed()
	{}

	Signed(sig_desc x_i) : slv<N>(x_i)
		{

		}

	Signed(int64_t x_i) : slv<N>(x_i)
		{

		}
	// forbidden in vhdl!
	/*
	Signed(slv<N> x_i)
	{
		slv<N>::n = x_i.n;
	}
*/
	Signed(const Signed<N>& x_i)
	{
		slv<N>::n = x_i.n;
	}

	int64_t conv_int() const
	{
		return ( ( int64_t(slv<N>::n) << (64-N)) >> (64-N) );
	}
	int64_t _d_conv_int() const
	{
		return ( ( int64_t(slv<N>::_d) << (64-N)) >> (64-N) );
	}
	// should delete ?
	operator slv<N> ()
		{
			return (slv<N>(slv<N>::n));
		}


	template <int m>
	Signed<m> resize()
	{
		return Signed<m>(conv_int());
	}

	slv<N> srl(int x_i)
		{
			return slv<N>(conv_int() >> x_i);
		}
	//This method is a copy of the one in slv
	//This is utterly ugly, but I can't see any efficient way of sharing the code, except letting it in slv and use a function pointer for vcd_dump_ll / vcd_dump_ull
	// or add a member that flags a signed slv ?
	//Maybe I'll fix that in the future
	inline void operator<=(const Signed<N>& x_i)
	{
		if (slv<N>::init == 1) // flip flop
		{
			slv<N>::_d = x_i.n; // assign flip-flop input
			if ( (slv<N>::pvcd_entry->ID[0] != '#') and (x_i.n != slv<N>::n) )
			{
				vcd_file.vcd_dump_ll(_d_conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->binary);
			}
		}
		else if (slv<N>::init == 2) // combinational
		{
			//gprintf("#comb vcd n % in % name % ID", n, x_i.n, pvcd_entry->name, pvcd_entry->ID);
			if ( (slv<N>::pvcd_entry->ID[0] != '#') and (x_i.n != slv<N>::n) )
			{
				slv<N>::n = x_i.n; // assign signal
				vcd_file.vcd_dump_ll(conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->binary);
			}
			else
				slv<N>::n = x_i.n; // assign signal

		}
		else // do not know yet
		{
			// In platform GS, combinational processes have a clock (pre or post clock, only post for the moment), but only synchronous ones have a reset!
			gprintf("#RRegistering, reset id = % clk ID = %", __control_signals__.reset_n, __control_signals__.clk);
			gprintf("#bflop % in %", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
			if (__control_signals__.reset_n != -1) // not combinational
			{
				slv<N>::init = 1;
				gprintf("#rTo clk");
				slv<N>::_d = x_i.n; // reset value
				slv<N>::n = x_i.n; // reset value

				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_ll(_d_conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->binary);
				register_flop(__control_signals__.clk, static_cast<flop*>(this));
			}
			else
			{
				gprintf("#rTo clk comb");
				slv<N>::init = 2;
				slv<N>::n = x_i.n;
				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_ll(conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->binary);
			}
		}
	}
	friend std::ostream& operator<< (std::ostream& os, const Signed<N>& x)
	{
	    return os << x.conv_int();
	}

};

// special derivation for clocktrees, reset trees...
struct tree : slv<1>
{
	bool evt = 0;
	slv<1> en = 1; // dummy tree gating
	slv<1>* p_en = &en; // default: no clock gating
	std::vector<tree*> children;
	int n = 0; // enables signal ID
	//static int n_trees; // enables signal ID

	// To retrieve tree signal from ID (reset process registers flip flops)
	static std::vector<tree*> trees;

	virtual void is_clk()
	{
		gprintf("NOT CLOCK\n");
	}

	tree()
	{
		n = trees.size();
		gprintf("#K tree % ",n);
		trees.push_back(this);
		//n_trees++;
	}

	tree(const sig_desc& x_i) : slv<1>(x_i)
	{
		slv<1>::init = 2; // Make sure is combinational
		n = trees.size();
		gprintf("#K tree % name %",n, x_i.name);
		trees.push_back(this);
		//n_trees++;
	}

	bool event()
	{
		return evt;
	}

	// for not
	slv<1> operator!()
	{
		return slv<1>::operator!();
	}

	void operator=(const slv<1>& x_i)
	{
		if (*p_en == 1)
		{
			if ( not (x_i == static_cast<slv<1>>(*this)) )
				evt = 1;
			else
				evt = 0;
			slv<1>::operator=(x_i);
		}
		else
		{
			evt = 0;
			slv<1>::operator=(0);
		}
	}

	void operator<=(const slv<1>& x_i)
	{
		if (*p_en == 1)
		{
			if ( not (x_i == static_cast<slv<1>>(*this)) )
				evt = 1;
			else
				evt = 0;
			slv<1>::operator<=(x_i);
		}
		else
		{
			evt = 0;
			slv<1>::operator<=(0);
		}
	}

	// So that the get method can be called for a tree signal or a port<tree>
	tree& get()
	{
		gprintf("#Vtree get");
		return *this;
	}

	// so that tree list (trees) can call clk_t register_flop method
	virtual void register_flop(flop* x_i)
	{
		gprintf("#RVirtual register flop");
		//flop_list.push_back(x_i);
	}
	// not good. Must be able to create a gated clock, not a gated tree
	void create_gated(tree& tree_o, slv<1>& gate_i)
	{
		tree* ptree = new tree;
		ptree->p_en = &gate_i;
		children.push_back(ptree);
	}

	virtual ~tree(){}

	virtual void reset()
	{
		std::cerr << "ERROR! calling virtual reset method!\n";
	}
	virtual void process()
	{
		std::cerr << "ERROR! calling virtual process method!\n";
	}
	virtual void clock()
	{
		std::cerr << "ERROR! calling virtual clock method!\n";
	}

	virtual void exec_clock()
	{
		std::cerr << "ERROR! calling virtual clocks method!\n";
	}
	virtual void exec_processes()
	{
		std::cerr << "ERROR! calling virtual processes method!\n";
	}
	virtual void exec_comb_processes()
	{
		std::cerr << "ERROR! calling virtual comb processes method!\n";
	}
};


// To retrieve tree signal from ID (reset process registers flip flops)
std::vector<tree*> tree::trees;



struct clk_t : public tree
{
	//static std::vector<clk_t> clocks; //
	std::vector<flop*> flop_list;
	std::vector<std::function<control_signals()>> processes;
	std::vector<std::function<control_signals()>> comb_processes; // post clock combinational logic, required by combinational assignments of output ports

	uint64_t half_period = 1; // default is same as vcd clock

	void is_clk()
	{
		gprintf("IS CLOCK\n");
	}

	clk_t()
	{

	}
	clk_t(const sig_desc& x_i) : tree(x_i)
	{
		// TODO
	}

	clk_t(const gated_clk_desc& x_i) : tree(x_i)
	{
		x_i.parent_clk->children.push_back(this);
	}
	clk_t& get()
	{
		gprintf("#Vclk get");
		return *this;
	}
	// set half period in numbers of vcd periods
	void set_half_period(uint64_t half_period_i)
	{
		half_period = half_period_i;
	}


	// Register a flip-flop (called from reset statement of synchronous processes)
	void register_flop(flop* x_i)
	{
		flop_list.push_back(x_i);
	}
	// Register a flip-flop (called from reset statement of synchronous processes)
	void register_clk(clk_t* x_i)
	{
		children.push_back(x_i);
	}
	// Must be called after the whole hierarchy has been built
	// The module list is parsed and all possible processes tested (process0 -> process3)
	// Non implemented processes return NULL for clock signal ptr
	// Others are either synchronous or combinational (comb processes return NULL for reset_n ptr)
	// All relevant processes are registered and will be executed on clock events
	void parse_modules( )
	{
		for (int i = 0; i < gmodule::module_list.size(); i++)
		{
			control_signals x;
			// This is ugly but I don't see any better solution
			// Parse all processes in modules
			// If they have this clock in their sensitivity list, register them to this clk
			// If they don't have reset, consider them as combinational
			x = gmodule::module_list[i]->process0();
			if (x.clk == n) // process has this clk in sensitivity list
				if (x.reset_n == -1) // comb
					comb_processes.push_back((std::bind(&gmodule::process0, gmodule::module_list[i])));
				else
					processes.push_back((std::bind(&gmodule::process0, gmodule::module_list[i])));

			x = gmodule::module_list[i]->process1();
			if (x.clk == n) // process has this clk in sensitivity list
				if (x.reset_n == -1) // comb
					comb_processes.push_back((std::bind(&gmodule::process1, gmodule::module_list[i])));
				else
					processes.push_back((std::bind(&gmodule::process1, gmodule::module_list[i])));
			x = gmodule::module_list[i]->process2();
			if (x.clk == n) // process has this clk in sensitivity list
				if (x.reset_n == -1) // comb
					comb_processes.push_back((std::bind(&gmodule::process2, gmodule::module_list[i])));
				else
					processes.push_back((std::bind(&gmodule::process2, gmodule::module_list[i])));
			x = gmodule::module_list[i]->process3();
			if (x.clk == n) // process has this clk in sensitivity list
				if (x.reset_n == -1) // comb
					comb_processes.push_back((std::bind(&gmodule::process3, gmodule::module_list[i])));
				else
					processes.push_back((std::bind(&gmodule::process3, gmodule::module_list[i])));
		}
	}


	inline void exec_processes()
	{
		//gprintf("#UProcesses: %", processes.size());
		for (int i = 0; i < processes.size(); i++)
		{

			processes[i]();
		}
	}
	inline void exec_comb_processes()
	{
		for (int i = 0; i < comb_processes.size(); i++)
		{

			comb_processes[i]();
		}
	}
	inline void exec_clock()
	{
		//gprintf("#VCLOCK flops: %Y", flop_list.size());
		for (int i = 0; i < flop_list.size(); i++)
		{
			flop_list[i]->clock();
		}
	}


	void operator<=(const slv<1>& x_i)
	{
		tree::operator<=(x_i);
		operator=(x_i);
	}

	void operator=(const slv<1>& x_i)
	{
		//gprintf("#mclock event val %R", x_i);
		//vcd_file.set_vcd_time(vcd_file.get_vcd_time() + half_period);
		//tree::operator=(x_i);
		if (tree::event())
		{
			//gprintf("#ball processes");
#ifdef GATED_CLOCKS
			gprintf("#mchidren:%", children.size());
			for (int i = 0; i < children.size(); i++)
				children[i]->exec_processes();
#endif
			exec_processes();
#ifdef GATED_CLOCKS
			for (int i = 0; i < children.size(); i++)
				children[i]->exec_clock();
#endif
			exec_clock();
#ifdef GATED_CLOCKS
			for (int i = 0; i < children.size(); i++)
				children[i]->exec_comb_processes();
#endif
			exec_comb_processes();

		}
		vcd_file.set_vcd_time(vcd_file.get_vcd_time() + half_period);

	}

};
/*
template<class T1, class T2> register_clk(T1 g_clk, T2)
void register_clk(T1 x, T2 y)
{
	gprintf("#VFunction register clock");// to %",tree::trees[x]->is_clk());
	tree::trees[x]->is_clk();
	tree::trees[x]->register_clk(y);
}
*/
template<class T1, class T2>
void register_flop(T1 x, T2 y)
{
	gprintf("#VFunction register flop");// to %",tree::trees[x]->is_clk());
	tree::trees[x]->is_clk();
	tree::trees[x]->register_flop(y);
}

struct reset_t : tree
{
	reset_t()
	{}
	reset_t(const sig_desc& x_i) : tree(x_i)
	{
		//TODO
	}

	void operator=(const slv<1>& x_i)
	{
		tree::operator=(x_i);
	}
};

// required for reference building even when no clk or rst
clk_t no_clock;
reset_t no_reset;



template<int N>
Signed<N> conv_signed(const slv<N>& x_i)
{
	return Signed<N>(x_i.n);
}


template<int N>
slv<N> conv_unsigned(const slv<N>& x_i)
{
	return slv<N>(x_i.n);
}

// should work
template<class T>
int conv_int(const T& x_i)
{
	Signed<32> r = x_i;
	return int(x_i.n);
}


template <class T, int N>
struct array_base
{
	T v[N];
	T& operator()(int n)
	{
		return array_base<T,N>::v[n];
	}
};




template < class T1 >
struct array {}; // does not exist

// This to mimmic vhdl statement toto <= ( others => to_signed(0, 8) );
// especially important for array use
template <class T>
struct others_type
{
	T x;
	others_type(T x_i)
	{
		x = (x_i);
	}
};

template <class T> others_type<T> __others(T x)
		{
			return others_type<T>(x);
		}

// To be called with array_base as T1
template < template<class ,int> class T1, class T, int N>
struct array< T1<T,N> > : T1<T,N>, vcd_entry
{
	//T v[N];
	vcd_entry* pvcd_entry;

	array( ) // for constants
	{
	}
	array(const T1<T,N>& x_i ) : T1<T,N>(x_i) // for constants init
	{
		/*
		for (int i = 0; i < N; i++)
		{
			array_base<T,N>::v[i] = x_i.v[i];
		}*/
		//array_base(x_i);
	}

	array(const sig_desc& x_i ) : vcd_entry(x_i)
	{
		//vcd_entry = create_vcd_entry(x_i.name, x_i.pmodule, -4);
		vcd_entry::nbits = -65536; // marks an array of vcd entries
		x_i.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this));

		for (int i = 0; i < N; i++)
		{
			std::string name = x_i.name + '(' + std::to_string(i) + ')';
			array_base<T,N>::v[i].pvcd_entry = create_vcd_entry(name, x_i.pmodule, (*this).length + array_base<T,N>::v[i].length);
		}
	}

	T1<T,N>* pbase()
		{
			return &static_cast<T1<T,N>>(*this);
		}

	template <class T2>
	void operator<=(others_type<T2> x_i)
	{
		for (int i = 0; i < N; i++)
		{
			array_base<T,N>::v[i].operator<=(x_i.x);
		}
	}

	void activate()
	{
		for (int i = 0; i < N; i++)
		{
			array_base<T,N>::v[i].pvcd_entry->activate();
		}
	}
	/*
	T& operator()(int n)
	{
		return array_base<T,N>::v[n];
	}
	*/
	static const int length = -65536;
};


#define VHDLz
#ifndef VHDL
#define SIGNED(a) conv_signed(a)
#define UNSIGNED(a) conv_unsigned(a)
#define SLV(a, n) slv<n>(a)

#define SLV_TYPE(n) slv<n>
#define TO_SLV(a) a
#define PORT_BASE(a) a.base_type()  // Required to perform something that is not supported by the std:reference_wrapper used for port class ex: SIGNED(PORT_BASE(data_i))
#define SIGNED_TYPE(n) Signed<n>
#define UNSIGNED_TYPE(n) slv<n>
#define INT(n) Signed<n>
#define UINT(n) slv<n>

#define RESIZE(a,n) a.resize<n>()

#define TO_UNSIGNED(a,n) slv<n>(a)
#define TO_SIGNED(a,n) Signed<n>(a)
#define TO_UINT(a,n) slv<n>(a)
#define TO_INT(a,n) Signed<n>(a)
#define TO_INTEGER(a) conv_int(a)

#define CLK_TYPE clk_t
#define RST_TYPE reset_t
#define BIT_TYPE slv<1>
#define CAT(a,b) a.cat(b)
#define STRING(a) #a
#define BIN(a) slv<sizeof(STRING(a))-1>(0b##a)
#define HEX(a) slv<sizeof(STRING(a))*4-1>(0x##a)
#define BIT(a) slv<sizeof(STRING(a))-1>(0b##a)
#define EQ(a,b) (a==b)
#define EXT(a,b) slv<b>(a.n)
#define SXT(a,b) slv<b>(Signed(a).conv_int())
#define SIGNED(a) conv_signed(a)
#define UNSIGNED(a) conv_unsigned(a)
#define RANGE(a,b,c) a.range<b,c>()
#define SLV_RANGE(a,b,c) a.range<b,c>() // for further concatenation in vhdl
#define B(a,b) a.get_bit(b)
#define HI(a) a.high
#define LEN(a) a.length
#define IF if
#define THEN {
#define ENDIF }
#define ELSE } else {
#define ELSEIF } else if
#define SWITCH(a) switch(a){
#define CASE(b) break;case b:
#define DEFAULT break;default:
#define ENDCASE }
#define VA =
#define ARRAY_TYPE(t, n) array<array_base<t,n>>
#define BASE_ARRAY_TYPE(t, n) array_base<t,n>
#define BASE_RECORD_TYPE(t) t##_base

#define TYPE(a, t) typedef t a
#define TOP_SIG(a, t)  t a = gen_sig_desc(#a, gmodule::out_of_hier)
#define CONST(a, t)  const t a
#define SIG(a, t)  t a = gen_sig_desc(#a, this)
#define GATED_CLK(a, t, c)  t a = gen_gated_clk_desc(#a, this, c)
#define VAR(a, t)  t a //= gen_sig_desc(#a, this)
#define CONST(a, t) const t a
#define MEMBER(a, t) t a
#define REC(a) struct a {
#define ENDREC };
#define LIST(...) { __VA_ARGS__ }
#define EVENT(a) static_cast<tree>(a).event()//CAT_(a,.event())
#define CONSTANT const
#define OTHERS(a) __others(a)
#define RESET(a) a <= TO_UINT(0, LEN(a))
#define shift_left(a, b) a.sll(b)
#define shift_right(a, b) a.srl(b)
#define SHIFT_LEFT(a, b) a.sll(b)
#define SHIFT_RIGHT(a, b) a.srl(b)

#else
#define CONSTANT constant
#define CONST(a,n) constant a : std_logic_vector((n-1) downto 0)
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
#define REC(a) type a is record
#define ENDREC end record;
#define MEMBER(a, t) a : t
#define PORT_IN(a,t) a : in t
#define PORT_OUT(a,t) a : out t
#define END_SEMICOLON(a) a;
#define END_COMMA(a) a,

#define END_NOTHING(a) a
#define ENTITYz(name, ...) entity name is port( EVAL(MAP2(END_SEMICOLON, END_NOTHING, __VA_ARGS__)) ) end
#define COMPONENT(name, ...) component name is port( EVAL(MAP2(END_SEMICOLON, END_NOTHING, __VA_ARGS__)) ) end component
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

#define TESTBENCH(type, ...) entity  type is /*
		*/ ); end type; architecture rtl of type is component dummy_zkw_pouet is port(clk : in std_logic);end component//std::cerr << "CTOR " << name << "\n";}

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




#endif



#ifndef VHDL
#define START_OF_FILE(a) // c++ file of block a generated by platform GS

// template parameters for in and out ports
#define IN 1
#define OUT 0

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



//#define PORT_MAPS(type,  ...) EVAL1(MAP_PLUS1(type, PORT_MAP2, __VA_ARGS__))
#define PORT_MAPS(type,  ...) EVAL(MAP_ALT_PLUS1(type, PORT_MAP_COMMA, PORT_MAP, __VA_ARGS__))
#define GEN_BIND(name, ...) gen_bind(name, )

#define BLK_CTOR(name) name(char* name_i, gmodule* parent) : gmodule::gmodule(name_i, parent)

#define APPLY_PORT(decl) EVAL1(get_##decl)


#define DECL_PORT(a) PORT_DEF(EVAL1(get1_##a),EVAL1(get2_##a),EVAL1(get3_##a))
#define DECL_PORTS(...) EVAL(MAP(DECL_PORT, __VA_ARGS__))
#define DECL_FIELD(a) FIELD_DEF(EVAL1(get1_##a),EVAL1(get2_##a))
#define DECL_FIELDS(...) EVAL(MAP(DECL_FIELD, __VA_ARGS__))

#define FIELD_ACT_VCD(name)  name.pvcd_entry->activate();
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

// Block and ports declaration
#define ENTITY(type, ports, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(template<int dummy0, __VA_ARGS>)(template<int dummy0>) struct type : gmodule {/*
		*/ EVAL1(DECL_PORTS(get_##ports)) /*
		*/ type(const char*x, gmodule* y):gmodule(x,y) {}//std::cerr << "CTOR " << name << "\n";}
#define TESTBENCH(type, ...) template<int dummy0> struct type : gmodule {/*
		*/ type(const char*x, gmodule* y):gmodule(x,y) {}//std::cerr << "CTOR " << name << "\n";}

#define PACKAGE(a)
#define END_PACKAGE(a)
#define USE_PACKAGE(a)
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
		struct type : type##_base, vcd_entry{\
		vcd_entry* pvcd_entry = static_cast<vcd_entry*>(this);\
		EVAL(USING_FIELDS(type,get_##ports)) \
		type() {}\
		type(const sig_desc& x) : vcd_entry(x){\
		vcd_entry::nbits = -2048; /* marks record */\
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

#define APPLY_MAP(type, port_map) PORT_MAPS(type, EVAL1(get_##port_map))

// Pass generic parameters - to be used later
#define APPLY_GENERIC(a) EVAL1(get_##a)

// instantiation of block inside hierarchy - all except testbench
#define BLK_INST(name, type, port_map, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
	(type<0,APPLY_GENERIC(__VA_ARGS__)>& name = *create_block(#name, this, APPLY_MAP(type<0,APPLY_GENERIC(__VA_ARGS__)>, port_map)))\
	(type<0>& name = *create_block< type<0> >(#name, this, APPLY_MAP(type<0>, port_map)))

// Instantiation of top rtl block - should be testbench
/*#define BLK_INST_TOP(name, type, port_map, ...) IF_ELSE(HAS_ARGS(__VA_ARGS__))\
	(type<0,APPLY_GENERIC(__VA_ARGS__)>& name = *create_block(#name, gmodule::out_of_hier, APPLY_MAP(type<0,APPLY_GENERIC(__VA_ARGS__)>, port_map)))\
	(type<0>& name = *create_block<type<0>>(#name, gmodule::out_of_hier, APPLY_MAP(type<0>, port_map)))*/
#define BLK_INST_TOP(name, type, ...) type<0>& name = *create_block<type<0>>(#name)

// process_void for trial purposes, when no clk nor reset
#define PROCESS_VOID(number) constexpr control_signals process##number(){  __control_signals__.clk = -1; __control_signals__.reset_n = -1;
#define COMB_PROCESS(number, signal1) constexpr control_signals process##number(){ __control_signals__.clk = signal1.get().n; __control_signals__.reset_n = -1; //gprintf("#### % % % ####", name, signal1.get().n, signal2.get().n);

#define PROCESS(number, signal1, signal2) constexpr control_signals process##number(){ __control_signals__.clk = signal1.get().n; __control_signals__.reset_n = signal2.get().n; //gprintf("#### % % % ####", name, signal1.get().n, signal2.get().n);

#define END_PROCESS return(__control_signals__); }
#define END_COMB_PROCESS return(__control_signals__); }


#define MAP_LIST(blk_name, blk_type, ...) EVAL(MAP_PLUS2(blk_name, blk_type)

#define BEGIN
//#define INCLUDES #include "slv.h"
#define INCLUDES
#else

#endif
