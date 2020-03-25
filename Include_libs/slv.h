#pragma once
#include "macros.h"
#include <iostream>
//#include "mbprt.h"
//#include "macros.h"
#include <vector>
//#include "vcd.h"
#include <typeinfo>
template<int N>
struct slv;

template<int N>
struct Signed;

struct clk_t;

std::ofstream advisor;
bool sim_abort = 0;

struct flop
{
	uint64_t _d;
	virtual void clock(){giprintf("#RVirtual clock");};
    uint64_t nclk; // idx of clock driving signal
};

// Must use this function and forward declare it to call it in slv class, but first argument is of clock type which derives from slv
template<class T1, class T2>
void register_flop(T1 x, T2 y);
template<class T2>
void register_changed_flop(T2 y);


template<int N>
struct base_slv
{
	 uint64_t n = 0;
	 base_slv(){}
	 base_slv(const base_slv<N>& x)
	{
		n = x.n;
	}
	 base_slv(uint64_t x)
	{
		n = x;
	}
	 /*
	constexpr operator int() const
		{
			return n;
		}
		*/
	const static int length = N;
	const static int high = N-1;
	static const int size = 1;
	constexpr static int _length() {return N;}

};

template <int P, int Q, class T>
struct range_t
{
	T& val;
	range_t( T& x_i) : val(x_i)
	{
		//ptr = &x_i;
	}
	operator slv<P-Q+1>()
	{
		return slv<P-Q+1>( (val.n) >> Q );
	}

	bool operator==(const slv<P-Q+1>& x_i)
	{
		return (slv<P-Q+1>(*this) == x_i);
	}

};


template <int N>
inline uint64_t adjust(uint64_t n)
{
	_Pragma ("GCC diagnostic push")
	_Pragma ("GCC diagnostic ignored \"-Wshift-count-overflow\"") // avoids warning when N = 64; situation is normal
	const uint64_t mask = (1ull << N) - 1; // Mask to keep N bits
	_Pragma ("GCC diagnostic pop")
	uint64_t nn = n & mask;
	//gprintf("adj %", nn);
	return nn;
}


// Forward declarations
template <class T> int conv_integer(const T& x_i);
template<int N > int conv_integer(const slv<N>& x_i);

int tree_val(int n);

// SLV unsigned -------------------------------------------------------------------------------------------------------------
// was originally intended to mimic std_logic_vector, but finally is equivalent to signed(N-1 downto 0) of numeric_std package
template<int N>
struct slv : public base_slv<N>, flop// inherits of flop / useless for combinational logic or variables, but much simpler (and little overhead)
{

    using base_slv<N>::n;	//uint64_t n; // Stored value
    uint8_t init = 0; // Is set at first <= assignment. 1 -> flip-flop, 2-> combinational

_Pragma ("GCC diagnostic push")
_Pragma ("GCC diagnostic ignored \"-Wshift-count-overflow\"") // avoids warning when N = 64; situation is normal
	static const uint64_t mask = (N == 64) ? -1 : (1ull << N) - 1; // Mask to keep N bits
_Pragma ("GCC diagnostic pop")

	vcd_entry* pvcd_entry = &dummy_vcd_entry;

	slv(){
		pvcd_entry = &dummy_vcd_entry;// is a global variable which has 'non probing' attribute
		//giprintf("#BBare slv of size %R ptr %R", N, this);
	}

	slv(base_slv<N> x_i)
	{
		n = x_i.n;
	}

	// init with signal name and module ptr (for a signal that is vcd probable)
	slv(sig_desc x_i)
	{
		pvcd_entry = create_vcd_entry(x_i.name, x_i.pmodule, N);
		giprintf("#BNew slv name % slv ptr % vcd ptr % module ptr %R",pvcd_entry->name, this, pvcd_entry, pvcd_entry->pmodule);
	}

	// Construct with int. Should not be used directly (set private ?)
	slv(uint64_t x_i)
	{
		//std::cerr << "??" << x_i << " " << n << "!!\n";
/*
		if ((N!=64) and (x_i & (~mask)))
		{

			if (pvcd_entry && (pvcd_entry != &dummy_vcd_entry))
			{
				//gprintf("#R% pvcd % of size % being assigned %", "xx", pvcd_entry, N, x_i);

				gzprintf("#R% of size % being assigned %", pvcd_entry->name, N, x_i);
			}
			else
				gzprintf("#R% pvcd % of size % being assigned % (%s)", "unknown", pvcd_entry, N, x_i, pvcd_entry);

			bit_adjust();
			sim_abort = 1;//exit(0);
		}
*/
		n = x_i;
		bit_adjust();
		//std::cerr << "!!" << n << "!!\n";
	}

	void conf_vcd_entry(sig_desc x_i)
	{
		pvcd_entry = create_vcd_entry(x_i.name, x_i.pmodule, N);
		giprintf("#BNew slv name % slv ptr % vcd ptr % module ptr %R",pvcd_entry->name, this, pvcd_entry, pvcd_entry->pmodule);
	}

	uint64_t conv_int() const
	{
		return n;
	}
	// Masks unnecessary bits (keep N bits)
	inline void bit_adjust()
	{
		n &= mask;
	}



	constexpr const int get_const() const
	{
		return n;
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
		//gprintf("% + % = % ", n, x_i.n, (n + x_i.n) & mask);
		//slv<N> toto((n + x_i.n) & mask) ;
		//gprintf("Yo N = % ", N);
		//return toto;
		return slv<N>( (n + x_i.n) & mask );
	}

	slv<N> operator-(const slv<N>& x_i)
	{
		return slv<N>( (n - x_i.n) & mask );
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
		return slv<N+P>( adjust<N+P>( n * x_i.conv_int() ) );
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
		//giprintf("#CU<= %R %R %M ptr %M", pvcd_entry->pmodule->name, pvcd_entry->name, int(init), this);
		//gzprintf("#GAssign slv %", pvcd_entry->name);
		if (sim_abort) exit(0);

		if (init == 1) // flip flop
		{
			//giprintf("#gFF % out % in", pvcd_entry->name, n, x_i);
			_d = x_i.n; // assign flip-flop input
			register_changed_flop(this);
			// vcd dump occurs when calling clk()
		}
		else if (init == 2) // combinational
		{
			//giprintf("#R%", "start");
			//giprintf("#R%", n);
			//giprintf("#R%", x_i.n);
			//giprintf("#R%", pvcd_entry->ID[0]);
			//giprintf("#comb vcd n % in % name % ID", n, x_i.n, pvcd_entry->name, pvcd_entry->ID);
			if ( (pvcd_entry->ID[0] != '#') and (x_i.n != n) )
			{
				n = x_i.n; // assign signal
				vcd_file.vcd_dump_ull((n), &pvcd_entry->ID[0], pvcd_entry->nbits);
			}
			else
				n = x_i.n; // assign signal


		}
		else // do not know yet
		{
			// In platform GS, combinational processes have a clock (pre or post clock, only post for the moment), but only synchronous ones have a reset!
			giprintf("#RRegistering, reset id = % clk ID = %", __control_signals__.reset_n, __control_signals__.clk);
			giprintf("#bflop % in %", pvcd_entry->name, pvcd_entry->pmodule->name);
			if (__control_signals__.reset_n != -1) // not combinational
			{
#ifndef NO_ADVISOR
				if (tree_val(__control_signals__.reset_n)) // reset is 1 at first access -> flop is not resetted
				{
					gprintf(advisor, "NOT reset signal % in module % \n", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
				}
#endif

				init = 1;
				giprintf("#rTo clk");
				_d = x_i.n; // reset value
				n = x_i.n; // reset value
				if (pvcd_entry->ID[0] != '#')
				{
					giprintf("#Rwriting vcd value %", _d);
					vcd_file.vcd_dump_ull((_d), &pvcd_entry->ID[0], pvcd_entry->nbits);
				}
				else
				{
					giprintf("#Rnot probed yet");
				}
				register_flop(__control_signals__.clk, static_cast<flop*>(this));
			}
			else
			{
				giprintf("#rTo clk comb");
				init = 2;
				n = x_i.n;
				if (pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_ull((n), &pvcd_entry->ID[0], pvcd_entry->nbits);
			}
		}
		//giprintf("#GU<= %R %R %M", pvcd_entry->pmodule->name, pvcd_entry->name, int(init));

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
	slv<N+P> operator&(const slv<P>& x_i) const
	{
		//std::cerr << n << " " << "x " << x_i.n << " " <<((n<<P)|(x_i.n));
		return slv<N+P>( (n<<P) | x_i.n );
	}
	// extract bit range toto.range<8,6>() <=> toto(8 downto 6)
#if 1
	template <int P, int Q>
	slv<P-Q+1> range() const
	{
		//gprintf("#bP % Q % P-Q+1 %", P, Q, P-Q+1);
		return slv<P-Q+1>(adjust<P-Q+1>(n>>Q));
	}

	// Version which returns constant size vector from variable position (to use in loops e.g.)
	template <int P>
	slv<P> range(int Q)
	{
		return slv<P>( adjust<P>(n>>Q) );
	}
#else
	template <int P, int Q>
	range_t<P,Q, slv<N>> range()
	{
		return range_t<P,Q,slv<N>>(*this);
	}
#endif



	// extract bit from bit vector
	slv<1> get_bit(int i)
	{
		return slv<1>( (n >> i) & 1);
	}

	// !!!!!!!!!!!!!!!!!!!!!!!! USE THIS ON VARIABLES or COMB LOGIC ONLY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//template <class T>
	inline void set_bit(unsigned int pos, slv<1> val) //const T& val)
	{
		uint64_t bitmask = (1 << (pos));
		// if (bitmask == 0) {giprintf("#RERROR, setting bit % of slv<%d> named %", pos, N, pvcd_entry->name);exit(0);} //  check index
		//giprintf("#B setting bit % with % bitmask %R n = %M adj mask %R", pos, conv_integer(val), bitmask, n, (bitmask and mask));
		if (conv_integer(val) == 1) // set to 1
		{
			n |= (bitmask & mask); // Avoid setting bit which does not belong to bitvector
		}
		else //set to 0
		{
			n &= (~bitmask);
		}
		//giprintf("#G setting bit % with % bitmask %R n = %M", pos, conv_integer(val), bitmask, n);

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
			return slv<N>( (n << x_i) & mask);
		}
	slv<N> sla(int x_i)
		{
			return slv<N>( ( (n << x_i) | (n >> (32 - x_i)) ) & mask );
		}
	slv<N> srl(int x_i)
		{
			//gprintf("srl %", x_i);
			return slv<N>( (n >> x_i) & mask );
		}
	slv<N> sra(int x_i)
		{
			return slv<N>( ( (n >> x_i) | (n << (32 - x_i)) ) & mask );
		}
	//bitwise not
	slv<N> operator!()
	{
		//slv<N> res(~n);
		return slv<N>( (~n) & mask );
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
#ifndef NO_VCD
		if ( (pvcd_entry->ID[0] != '#') and (_d != n) )
		{
			vcd_file.vcd_dump_ull((_d), &pvcd_entry->ID[0], pvcd_entry->nbits);
		}		//giprintf("#R% in % out %", pvcd_entry->name, _d, n);
#endif
		n = _d;
	}

	// replaces ext() and sxt()
	template <int m>
	slv<m> resize()
	{
		return slv<m>(adjust<m>(n));
	}

	slv<N>& get()
	{
		return *this;
	}
/*
	constexpr operator int() const
		{
			return n;
		}
*/
};

template<int N>
struct const_slv
{
	 uint64_t n;
	constexpr const_slv(const slv<N> x)
	{
		n = x.n;
	}
	constexpr operator int() const
		{
			return n;
		}
	const static int length = N;
	const static int high = N-1;
	static const int size = 1;
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
		//gprintf("#VSdone");
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
		//gprintf("Signed(%s)", x_i.n);
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

	void clock()
	{
		if ( (slv<N>::pvcd_entry->ID[0] != '#') and (slv<N>::_d != slv<N>::n) )
		{
			vcd_file.vcd_dump_ll(_d_conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			slv<N>::n = slv<N>::_d;
		}
	}

	template <int m>
	Signed<m> resize()
	{
		return Signed<m>(conv_int());
	}

	Signed<N> srl(int x_i)
		{
			return Signed<N>(adjust<N>(conv_int() >> x_i));
		}
	//This method is a copy of the one in slv
	//This is utterly ugly, but I can't see any efficient way of sharing the code, except letting it in slv and use a function pointer for vcd_dump_ll / vcd_dump_ull
	// or add a member that flags a signed slv ?
	//Maybe I'll fix that in the future

	inline void operator<=(const Signed<N>& x_i)
	{
		//gzprintf("#GAssign slv %", slv<N>::pvcd_entry->name);
		if (sim_abort) exit(0);
		//giprintf("#C<=");
		if (slv<N>::init == 1) // flip flop
		{
			slv<N>::_d = x_i.n; // assign flip-flop input
			register_changed_flop(this);
			// vcd dump occurs when calling clk()
		}
		else if (slv<N>::init == 2) // combinational
		{
			//giprintf("#comb vcd n % in % name % ID", n, x_i.n, pvcd_entry->name, pvcd_entry->ID);
			if ( (slv<N>::pvcd_entry->ID[0] != '#') and (x_i.n != slv<N>::n) )
			{
				slv<N>::n = x_i.n; // assign signal
				vcd_file.vcd_dump_ll(conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			}
			else
				slv<N>::n = x_i.n; // assign signal

		}
		else // do not know yet
		{
			// In platform GS, combinational processes have a clock (pre or post clock, only post for the moment), but only synchronous ones have a reset!
			giprintf("#RRegistering, reset id = % clk ID = %", __control_signals__.reset_n, __control_signals__.clk);
			giprintf("#bflop % in %", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
			if (__control_signals__.reset_n != -1) // not combinational
			{
#ifndef NO_ADVISOR
				if (tree_val(__control_signals__.reset_n)) // reset is 1 at first access -> flop is not resetted
				{
					gprintf(advisor, "NOT reset signal % in module % \n", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
				}
#endif

				slv<N>::init = 1;
				giprintf("#rTo clk");
				slv<N>::_d = x_i.n; // reset value
				slv<N>::n = x_i.n; // reset value

				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_ll(_d_conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
				register_flop(__control_signals__.clk, static_cast<flop*>(this));
			}
			else
			{
				giprintf("#rTo clk comb");
				slv<N>::init = 2;
				slv<N>::n = x_i.n;
				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_ll(conv_int(), &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			}
		}
	}
	friend std::ostream& operator<< (std::ostream& os, const Signed<N>& x)
	{
	    return os << x.conv_int();
	}

};


// tristate version of slv -----------------------------------------------
// required for e.g. SDRAM output, IS VERY INCOMPLETE, WORK IN PROGRESS
template<int N>
struct tristate:slv<N>
{

	uint64_t z_flags;
	uint64_t z_flags_d;
	tristate()
	{}

	tristate(sig_desc x_i) : slv<N>(x_i)
		{
			z_flags = 0xffffffffffffffff;
		}

	tristate(int64_t x_i) : slv<N>(x_i)
		{
			z_flags = 0xffffffffffffffff;
		}



	void clock()
	{
		if ( (slv<N>::pvcd_entry->ID[0] != '#') and ( (slv<N>::_d != slv<N>::n) or (z_flags_d != z_flags) ) )
		{
			//giprintf("#VDUMP ZZZ % % % % %", slv<N>::pvcd_entry->name, slv<N>::_d, slv<N>::n, z_flags_d, z_flags);
			vcd_file.vcd_dump_tristate(slv<N>::_d, z_flags_d, &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			slv<N>::n = slv<N>::_d;
			z_flags = z_flags_d;
		}
		//else
			//giprintf("#VNODUMP ZZZ %y % % % %", slv<N>::pvcd_entry->name, slv<N>::_d, slv<N>::n, z_flags_d, z_flags);

	}

	// !!!!!!!!!!!!!!!!!!!!!!!! USE THIS ON VARIABLES or COMB LOGIC ONLY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//template <class T>
	inline void set_bit(unsigned int pos, char val) //const T& val)
	{
		uint64_t bitmask = (1 << (pos));
		// if (bitmask == 0) {giprintf("#RERROR, setting bit % of slv<%d> named %", pos, N, pvcd_entry->name);exit(0);} //  check index
		//giprintf("#B setting bit % with % bitmask %R n = %M adj mask %R", pos, conv_integer(val), bitmask, n, (bitmask and mask));
		if (val == 'Z')
			z_flags |= (bitmask & slv<N>::mask); // Avoid setting bit which does not belong to bitvector
		else if ((val) == '1') // set to 1
		{
			slv<N>::n |= (bitmask & slv<N>::mask); // Avoid setting bit which does not belong to bitvector
			z_flags &= (~bitmask);
		}
		else //set to 0
		{
			slv<N>::n &= (~bitmask);
			z_flags &= (~bitmask);
		}
		//giprintf("#G setting bit % with % bitmask %R n = %M", pos, conv_integer(val), bitmask, n);

	}

	//template <class T>
	inline void set_bit_d(unsigned int pos, char val) //const T& val)
	{
		uint64_t bitmask = (1 << (pos));
		// if (bitmask == 0) {giprintf("#RERROR, setting bit % of slv<%d> named %", pos, N, pvcd_entry->name);exit(0);} //  check index
		//giprintf("#B setting bit % with % bitmask %R n = %M adj mask %R", pos, conv_integer(val), bitmask, n, (bitmask and mask));
		if (val == 'Z')
		{
			z_flags_d |= (bitmask & slv<N>::mask); // Avoid setting bit which does not belong to bitvector
		}
		else if ((val) == '1') // set to 1
		{
			slv<N>::_d |= (bitmask & slv<N>::mask); // Avoid setting bit which does not belong to bitvector
			z_flags_d &= (~bitmask);
		}
		else //set to 0
		{
			slv<N>::_d &= (~bitmask);
			z_flags_d &= (~bitmask);
		}
		//giprintf("#G setting bit % with % bitmask %R n = %M", pos, conv_integer(val), bitmask, n);

	}

	bool bit_d_eq(unsigned int i, char val)
	{
		return ( (val == 'Z') ? ( (z_flags_d >> i) & 1) : ( (slv<N>::_d >> i & 1) == (val - 48) ) );
	}

	bool bit_eq(unsigned int i, char val)
	{
		return ( (val == 'Z') ? ( (z_flags >> i) & 1) : ( (slv<N>::n >> i & 1) == (val - 48) ) );
	}

	bool eq(const char x_i[N])
	{
		int i = 0;
		while (i < N)
		{
			if (not bit_eq(i, x_i[i]))
				return false;
		}
		return true;
	}

	// for toto <= "11zz00";
	inline void set_d(const char x_i[N])
	{
		for (int i = 0; i < N; i++)
			set_bit_d(i, x_i[i]);
		//giprintf("#RZZZ % %", slv<N>::n, z_flags_d);
	}

	inline void set_q(const char x_i[N])
	{
		for (int i = 0; i < N; i++)
			set_bit(i, x_i[i]);
	}

	inline void operator<=(const slv<N>& x_i)
	{
		//gzprintf("#GAssign slv %", slv<N>::pvcd_entry->name);
		if (sim_abort) exit(0);
		//giprintf("#C<=");
		if (slv<N>::init == 1) // flip flop
		{
			slv<N>::_d = x_i.n; // assign flip-flop input
			register_changed_flop(this);
			z_flags_d = 0;
			//giprintf("#UAssigning % %", slv<N>::_d, z_flags_d);
			// vcd dump occurs when calling clk()
		}
		else if (slv<N>::init == 2) // combinational
		{
			//giprintf("#comb vcd n % in % name % ID", n, x_i.n, pvcd_entry->name, pvcd_entry->ID);
			if ( (slv<N>::pvcd_entry->ID[0] != '#') and ( (x_i.n != slv<N>::n) or (z_flags != 0) ) )
			{
				slv<N>::n = x_i.n; // assign signal
				vcd_file.vcd_dump_ll(slv<N>::n, &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			}
			else
				slv<N>::n = x_i.n; // assign signal

		}
		else // do not know yet
		{
			// In platform GS, combinational processes have a clock (pre or post clock, only post for the moment), but only synchronous ones have a reset!
			giprintf("#RRegistering, reset id = % clk ID = %", __control_signals__.reset_n, __control_signals__.clk);
			giprintf("#bflop % in %", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
			if (__control_signals__.reset_n != -1) // not combinational
			{
#ifndef NO_ADVISOR
				if (tree_val(__control_signals__.reset_n)) // reset is 1 at first access -> flop is not resetted
				{
					gprintf(advisor, "NOT reset signal % in module % \n", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
				}
#endif
				slv<N>::init = 1;
				giprintf("#rTo clk");
				slv<N>::_d = x_i.n; // reset value
				slv<N>::n = x_i.n; // reset value

				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_tristate(slv<N>::_d, z_flags, &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
				register_flop(__control_signals__.clk, static_cast<flop*>(this));
			}
			else
			{
				giprintf("#rTo clk comb");
				slv<N>::init = 2;
				slv<N>::n = x_i.n;
				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_tristate(slv<N>::n, z_flags, &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			}
		}
	}

	// for toto <= "11ZZ11";
	inline void operator<=(const char x_i[N])
	{
		//giprintf("#C<=");
		if (slv<N>::init == 1) // flip flop
		{
			set_d(x_i);
			register_changed_flop(this);
			// vcd dump occurs when calling clk()
		}
		else if (slv<N>::init == 2) // combinational
		{
			//giprintf("#comb vcd n % in % name % ID", n, x_i.n, pvcd_entry->name, pvcd_entry->ID);
			if ( (slv<N>::pvcd_entry->ID[0] != '#') and (not eq(x_i)) )
			{
				set_q(x_i); // assign signal
				vcd_file.vcd_dump_ll(slv<N>::n, &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			}
			else
				set_q(x_i); // assign signal

		}
		else // do not know yet
		{

			// In platform GS, combinational processes have a clock (pre or post clock, only post for the moment), but only synchronous ones have a reset!
			giprintf("#RRegistering, reset id = % clk ID = %", __control_signals__.reset_n, __control_signals__.clk);
			giprintf("#bflop % in %", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
			if (__control_signals__.reset_n != -1) // not combinational
			{
#ifndef NO_ADVISOR
				if (tree_val(__control_signals__.reset_n)) // reset is 1 at first access -> flop is not resetted
				{
					gprintf(advisor, "NOT reset signal % in module % \n", slv<N>::pvcd_entry->name, slv<N>::pvcd_entry->pmodule->name);
				}
#endif
				slv<N>::init = 1;
				giprintf("#rTo clk");
				set_q(x_i);
				set_d(x_i);

				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_tristate(slv<N>::_d, z_flags, &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
				register_flop(__control_signals__.clk, static_cast<flop*>(this));
			}
			else
			{
				giprintf("#rTo clk comb");
				slv<N>::init = 2;
				set_q(x_i);
				if (slv<N>::pvcd_entry->ID[0] != '#')
					vcd_file.vcd_dump_tristate(slv<N>::n, z_flags, &slv<N>::pvcd_entry->ID[0], slv<N>::pvcd_entry->nbits);
			}
		}
	}

	friend std::ostream& operator<< (std::ostream& os, const tristate<N>& x)
	{
		for (int i = N-1; i >= 0; i--)
			os << ( (x.z_flags >> i) & 1) ? 'Z' : char( 48 + ((slv<N>::n >> i) & 1));
	    return os;
	}

};
// \tristate-----------------------------------

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

	virtual bool is_clk()
	{
		giprintf("NOT CLOCK\n");
		return 0;
	}

	tree()
	{
		n = trees.size();
		giprintf("#K tree % ",n);
		trees.push_back(this);
		//n_trees++;
	}

	tree(const sig_desc& x_i) : slv<1>(x_i)
	{
		slv<1>::init = 2; // Make sure is combinational
		n = trees.size();
		giprintf("#K tree % name %",n, x_i.name);
		trees.push_back(this);
		//n_trees++;
	}

	bool event()
	{
		//std::cerr << "!";
		return evt;
	}

	// for not
	slv<1> operator!()
	{
		return slv<1>::operator!();
	}

	void operator=(const slv<1>& x_i)
	{
		//giprintf("@B % val %R en % in % ", pvcd_entry->name, static_cast<slv<1>>(*this), *p_en, x_i);
		if (*p_en == slv<1>(1))
		{
			if ( not (x_i == static_cast<slv<1>>(*this)) )
				evt = 1;//slv<1>(1);
			else
				evt = 0;//slv<1>(0);
			slv<1>::operator=(x_i);
		}
		else
		{
			evt = 0;//slv<1>(0);
			slv<1>::operator=(0);
		}
	}

	void operator<=(const slv<1>& x_i)
	{
		//giprintf("@G % val %R en % in % ", pvcd_entry->name, static_cast<slv<1>>(*this), *p_en, x_i);
		if (*p_en == slv<1>(1))
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
		//giprintf("#R ->evt % new val %", evt, static_cast<slv<1>>(*this));

	}

	// So that the get method can be called for a tree signal or a port<tree>
	tree& get()
	{
		//giprintf("#Vtree get %", pvcd_entry->name);
		return *this;
	}

	// so that tree list (trees) can call clk_t register_flop method
	virtual void register_flop(flop* x_i)
	{
		giprintf("#RVirtual register flop");
		//flop_list.push_back(x_i);
	}
	virtual void register_changed_flop(flop* x_i)
	{
		giprintf("#RVirtual register changed flop");
		//flop_list.push_back(x_i);
	}
	void init_changed_flops_list()
	{
		giprintf("#RVirtual init changed flops");
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
	virtual void copy_children( tree& x_i)
	{
		giprintf("#UIn %Y copying children, count %Y", pvcd_entry->name, children.size());
		for (int i=0; i < children.size(); i++)
		{
			x_i.children.push_back(children[i]);
			giprintf("#UIn %Y copying child %Y to driving signal %Y", pvcd_entry->name, children[i]->pvcd_entry->name, x_i.pvcd_entry->name);
		}
	}
	virtual void parse_modules(){}
};


// To retrieve tree signal from ID (reset process registers flip flops)
std::vector<tree*> tree::trees;

//void create_event(const uint64_t& t, int forever_process_id);
uint64_t cur_time = 0;
template<class T>
void create_event(const uint64_t& t, T evt_type);

struct clk_t : public tree
{
	//static std::vector<clk_t> clocks; //
	uint64_t nchanged_flops = 0;
	std::vector<flop*> flop_list;
	std::vector<flop*> changed_flop_list;
	std::vector<std::function<control_signals()>> processes;
	std::vector<std::function<control_signals()>> comb_processes; // post clock combinational logic, required by combinational assignments of output ports

	uint64_t half_period = 1; // default is same as vcd clock

	uint8_t delta = 0;

	bool is_clk()
	{
		giprintf("IS CLOCK\n");
		return 1;
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
		p_en = x_i.gating_signal;
		x_i.parent_clk->children.push_back(this);
		giprintf("#BAdding child %R to clk %R", this->pvcd_entry->name, (x_i.parent_clk)->pvcd_entry->name);
	}

	template<class T9>
	void gate_clock( clk_t& parent_clk, T9& gating_signal)
	{
		(parent_clk.get()).children.push_back(this);
		p_en = &gating_signal.get();
	}

	clk_t& get()
	{
		//giprintf("#Vclk get %", pvcd_entry->name);
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
		giprintf("#Centry %", flop_list.size());
		flop_list.push_back(x_i);
		changed_flop_list.push_back(x_i);
		changed_flop_list.push_back(x_i); // final vector will 2* actual flop count (in case of duplicate <= inside a process)
		giprintf("#C<<<<<<");
	}

	void init_changed_flops_list()
	{
		nchanged_flops = 0;
		//giprintf("#Uinit changed flops list %", pvcd_entry->name);
	}

	void register_changed_flop(flop* x_i)
	{
		//giprintf("#Centry %", flop_list.size());
		if (nchanged_flops >= changed_flop_list.size())
		{
			giprintf("#RChanged flop % of % in %", nchanged_flops, changed_flop_list.size(), pvcd_entry->name);
			//exit(0);
		}
		else
			changed_flop_list[nchanged_flops] = (x_i);
		nchanged_flops++;
		//changed_flop_list.push_back(x_i);
		//giprintf("#C<<<<<<");
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
		giprintf("#CParse modules");
		for (int i = 0; i < gmodule::module_list.size(); i++)
		{
			giprintf("#CParsing module %R", gmodule::module_list[i]->name);

			control_signals x;
			// This is ugly but I don't see any better solution
			// Parse all processes in modules
			// If they have this clock in their sensitivity list, register them to this clk
			// If they don't have reset, consider them as combinational
			if (not (gmodule::module_list[i]->forever_process_flags & 1) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process0();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();

			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process0, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process0, gmodule::module_list[i])));
				}

			}
			}

			if (not (gmodule::module_list[i]->forever_process_flags & 2) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process1();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();
			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process1, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process1, gmodule::module_list[i])));
				}

			}
			}

			if (not (gmodule::module_list[i]->forever_process_flags & 4) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process2();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();
			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process2, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process2, gmodule::module_list[i])));
				}

			}
			}

			if (not (gmodule::module_list[i]->forever_process_flags & 8) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process3();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();
			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process3, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process3, gmodule::module_list[i])));
				}
			}
			}


			if (not (gmodule::module_list[i]->forever_process_flags & 16) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process4();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();
			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process4, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process4, gmodule::module_list[i])));
				}
			}
			}

			if (not (gmodule::module_list[i]->forever_process_flags & 32) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process5();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();
			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process5, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process5, gmodule::module_list[i])));
				}
			}
			}

			if (not (gmodule::module_list[i]->forever_process_flags & 64) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process6();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();
			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process6, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process6, gmodule::module_list[i])));
				}
			}
			}

			if (not (gmodule::module_list[i]->forever_process_flags & 128) ){
			giprintf("#mexec");
			x = gmodule::module_list[i]->process7();
			if (x.clk >= 0) static_cast<clk_t*>(tree::trees[x.clk])->init_changed_flops_list();
			giprintf("#mRead clk is %R and trial clk is %R", x.clk, n);
			if (x.clk == n) // process has this clk in sensitivity list
			{
				if (x.reset_n == -1) // comb
				{
					comb_processes.push_back((std::bind(&gmodule::process7, gmodule::module_list[i])));
				}
				else
				{
					processes.push_back((std::bind(&gmodule::process7, gmodule::module_list[i])));
				}
			}
			}


			giprintf("#CEnd module");

		}


#ifdef GATED_CLOCKS
		for (int i = 0; i < children.size(); i++)
		{
			children[i]->parse_modules();

		}
		giprintf("#BEnd Parse modules");

#endif
	}


	inline void exec_processes()
	{
		//giprintf("#UProcesses: %", processes.size());
		init_changed_flops_list();
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
		//if (nchanged_flops)
		//giprintf("#VCLOCK flops: %Y changed %Y in %Y", flop_list.size(), nchanged_flops, pvcd_entry->name);
#if 0
		for (int i = 0; i < flop_list.size(); i++)
		{
			//giprintf("#cflop %", i);
			flop_list[i]->clock();
		}
#else
		for (int i = 0; i < nchanged_flops; i++)
		{
			//giprintf("#cflop %", i);
			changed_flop_list[i]->clock();
		}
#endif
	}

	// will execute stg depending on clk delta cycle value
	bool exec_delta()
	{
		if (delta == 0)
			exec_processes();
		else if (delta == 1)
			exec_clock();
		else
			exec_comb_processes();
		delta++;	
		return (delta == 3); // Flags the end of delta cycles
	}

	void operator<=(const slv<1>& x_i)
	{
		//giprintf("#m***********clock <= val %R", x_i);
		//giprintf("#mclock tree <= val %R", x_i);
#if 0		
		operator=(x_i);
#else
		tree::operator<=(x_i);
		delta = 0;
		//giprintf("#Rclk event from <= % @ curtime % -> %", pvcd_entry->name, cur_time, cur_time+1);
		//create_event(cur_time+1, this);
		if (this->event()) create_event(cur_time+1, this);
#endif		
		//giprintf("#mEND clock <= val %R", x_i);
	}

	void operator=(const slv<1>& x_i)
	{
		//giprintf("#mclock = val %R", x_i);
		//vcd_file.set_vcd_time(vcd_file.get_vcd_time() + half_period);
		//tree::operator=(x_i);
		tree::operator<=(x_i);

#ifdef GATED_CLOCKS
		for (int i = 0; i < children.size(); i++)
		{
			//giprintf("#b en %", *(children[i]->p_en) );
			//static_cast<clk_t*>(children[i])->init_changed_flops_list();
			children[i]->tree::operator<=(x_i);
			//static_cast<slv<1>>(*children[i])<=(x_i);
			//giprintf("#Mval %", (*children[i]));
		}
#endif
		if (tree::event())
		{
			//giprintf("#ball processes");
#ifdef GATED_CLOCKS
			//giprintf("#mchidren:%", children.size());
			for (int i = 0; i < children.size(); i++)
			{
				if (children[i]->event())
					{
					//giprintf("#BExec processes for clk %R", children[i]->pvcd_entry->name);
						children[i]->exec_processes();
					}

			}
#endif

			exec_processes();

#ifdef GATED_CLOCKS
			for (int i = 0; i < children.size(); i++)
			{
				if (children[i]->event()) children[i]->exec_clock();
			}
#endif
			exec_clock();
#ifdef GATED_CLOCKS
			for (int i = 0; i < children.size(); i++)
			{
				if (children[i]->event()) children[i]->exec_comb_processes();
			}
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
	giprintf("#VFunction register clock");// to %",tree::trees[x]->is_clk());
	tree::trees[x]->is_clk();
	tree::trees[x]->register_clk(y);
}
*/
template<class T1, class T2>
void register_flop(T1 x, T2 y)
{
	//giprintf("#VFunction register flop");// to %",tree::trees[x]->is_clk());
	//tree::trees[x]->is_clk();
	tree::trees[x]->register_flop(y);
	y->nclk = x;

}

template<class T2>
void register_changed_flop(T2 y)
{
	//giprintf("#VFunction register flop");// to %",tree::trees[x]->is_clk());
	//tree::trees[x]->is_clk();
	tree::trees[y->nclk]->register_changed_flop(y);
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
//Signed<N> conv_signed( slv<N> x_i)
{
	//gprintf("UNSss % %", N, x_i.n );
	//Signed<N> res = adjust<N>(x_i.n);
	//gprintf("CS done % ", res.n);
	//return res;
	return Signed<N>(adjust<N>(x_i.n));
}


template<int N>
slv<N> conv_unsigned(const slv<N>& x_i)
{
	//gprintf("UNSzzz % %", x_i.n, N);
	//gprintf("UNSzz % % %", x_i.n, N, adjust<N>(x_i.n));
	//gprintf("Yo");
	return slv<N>( adjust<N>(x_i.n) );
}

// Conv to int, general case  !!!!!!!!!! Called conv_integer for disambiguating when is called from within slv<> (set_bit method)
template <class T>
int conv_integer(const T& x_i)
{

	return int(x_i);
}

// Conv to int, slv<N> case
template<int N >
 int conv_integer(const slv<N>& x_i)
{
	uint64_t nn = x_i.conv_int();
	//gprintf("#m N % x_i % nn %", N, x_i, nn);
	//Signed<32> r = Signed<32>(x_i.n);
	return int(nn);
}

// For case statements
template<int N>
const constexpr inline int const_conv_int(const slv<N> x_i)
{
	//uint64_t nn = x_i.conv_int();
	//Signed<32> r = Signed<32>(x_i.n);
	return int(x_i.n);
}

// get value of a tree using its idx
int tree_val(int n)
{
	return conv_integer(static_cast<slv<1>>(*tree::trees[n]));
}

#include "array.h"

// Include all Agravic macros
#ifndef VHDL
#include "C_macros.h"
#else
#include "VHDL_macros.h"
#endif

// Include here all constants (slv sizes)
#include "../Source/constants.hpp"

