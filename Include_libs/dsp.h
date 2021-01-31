#pragma once

#ifndef VHDL

template<int N>
struct imag : Signed<N>
{
	imag(Signed<N> x) : Signed<N>(x){}
};

struct pure_imag
{
	template <int N>
	imag<N> operator*(const Signed<N>& x)
	{
		imag<N> res(x);
		return res;
	}
};

pure_imag ii;



#define DEF_RECORD1(ports)	struct cpx_int_base {\
		EVAL(DECL_FIELDS(get_##ports))

#define DEF_RECORD2(ports)	\
		struct cpx_int : cpx_int_base<N>, vcd_entry{/*\
		~type() = default; \
*/		vcd_entry* pvcd_entry = static_cast<vcd_entry*>(this);\
		using cpx_int_base<N>::re; using cpx_int_base<N>::im; \
		cpx_int() {}\
		~cpx_int(){giprintf("#RDestroying cpx_int<%d> record %", N, pvcd_entry->name); }\
		cpx_int(const sig_desc& x) : vcd_entry(x){\
		giprintf("#VNew cpx_int record : %", x.name);\
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
		void operator <= (const cpx_int<N>& x){\
		EVAL(ASSIGN_FIELDS(get_##ports)) }\
		void operator = (const cpx_int<N>& x){\
		EVAL(EQ_FIELDS(get_##ports)) }\
		void operator <= (const cpx_int_base<N>& x){\
		EVAL(ASSIGN_FIELDS(get_##ports)) }\
		void operator = (const cpx_int_base<N>& x){\
		EVAL(EQ_FIELDS(get_##ports)) } \
		/* NOT SURE static const int length = -2048;*/\
		void copy_children(const cpx_int<N>& x_i){}\
		//}

//define base type (only re+im members)
template<int N>
DEF_RECORD1(
		DECL_FIELDS(
				FIELD(re, Signed<N>),
				FIELD(im, Signed<N>)
				)
)
	cpx_int_base(Signed<N> re_i, Signed<N> im_i)
	{
		re = re_i;
		im = im_i;
	}
	cpx_int_base()
	{
		re = 0;
		im = 0;
	}
};
//define complete type
/*template<int N>
DEF_RECORD2(
		DECL_FIELDS(
				FIELD(re, Signed<N>),
				FIELD(im, Signed<N>)
				)
)*/

template<int N>
struct cpx_int : cpx_int_base<N>//, vcd_entry
{
	vcd_entry* pvcd_entry;// = static_cast<vcd_entry*>(this);
using cpx_int_base<N>::re;
using cpx_int_base<N>::im;
cpx_int() {}
~cpx_int(){/*gkprintf("#RDestroying cpx_int<%d> record %", N, pvcd_entry->name); */}
//cpx_int(const sig_desc& x) : vcd_entry(x){ gkprintf("#VNew CPX_INT record : %", x.name); vcd_entry::nbits = -2048; x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); std::string rec_name = x.name; {std::string __zozo__ = rec_name + '.' + "re"; re.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, int(-2048)+re.length);} {std::string __zozo__ = rec_name + '.' + "im"; im.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, int(-2048)+im.length);} }
cpx_int(const sig_desc& x) {conf_vcd_entry(x);}//: vcd_entry(x){ gkprintf("#VNew CPX_INT record : %", x.name); vcd_entry::nbits = -2048; x.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this)); std::string rec_name = x.name; {std::string __zozo__ = rec_name + '.' + "re"; re.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, int(-2048)+re.length);} {std::string __zozo__ = rec_name + '.' + "im"; im.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, int(-2048)+im.length);} }
void conf_vcd_entry(const sig_desc& x){
	gkprintf("#VNew CPX_INT VCD configuration: %", x.name);
	pvcd_entry = create_vcd_entry(x.name, x.pmodule, -2048);
	pvcd_entry->driver = pvcd_entry;
	pvcd_entry->nbits = -2048;
	x.pmodule->vcd_list.push_back(pvcd_entry); std::string rec_name = x.name;
{std::string __zozo__ = rec_name + '.' + "re"; re.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, int(-2048) + re.length);} {std::string __zozo__ = rec_name + '.' + "im"; im.pvcd_entry = create_vcd_entry(__zozo__, x.pmodule, int(-2048) + im.length);} }
void activate() { re.pvcd_entry->binary = pvcd_entry->binary; re.pvcd_entry->activate(); im.pvcd_entry->binary = pvcd_entry->binary; im.pvcd_entry->activate(); }
void operator <= (const cpx_int<N>& x){ re <= x.re; im <= x.im; }
void operator = (const cpx_int<N>& x){ re = x.re; im = x.im; }
void operator <= (const cpx_int_base<N>& x){ re <= x.re; im <= x.im; }
void operator = (const cpx_int_base<N>& x){ re = x.re; im = x.im; }
void copy_children(const cpx_int<N>& x_i){}


// type declaration left open by macro: define cpx_int specific methods here
cpx_int(Signed<N> re_i, Signed<N> im_i)
{
	re = re_i;
	im = im_i;
}

bool operator==(const cpx_int_base<N>& x)
{
	return((re==x.re) and (im==x.im));
}

cpx_int<N> operator+(const cpx_int_base<N>& x)
{
	return(cpx_int<N>(re+x.re, im+x.im));
}

cpx_int<N> operator-(const cpx_int_base<N>& x)
{
	return(cpx_int<N>(re-x.re, im-x.im));
}

cpx_int<N> operator+(const Signed<N>& x)
{
	return(cpx_int<N>(re+x, im));
}

cpx_int<N> operator-(const Signed<N>& x)
{
	return(cpx_int<N>(re-x, im));
}

cpx_int<N> operator+(const imag<N>& x)
{
	return(cpx_int<N>(re, im+Signed(x)));
}

cpx_int<N> operator-(const imag<N>& x)
{
	return(cpx_int<N>(re, im-Signed(x)));
}

template<int M>
cpx_int<N+M+1> operator*(const cpx_int_base<M>& x)
{
	return(cpx_int<N+M+1>(SSXT(re*x.re,N+M+1) - SSXT(im*x.im,N+M+1), SSXT(re*x.im,N+M+1) + SSXT(im*x.re,N+M+1)));
}

template<int M>
cpx_int<N+M> operator*(const Signed<M>& x)
{
	return(cpx_int<N+M>(re*x , im*x));
}

template<int M>
cpx_int<N+M> operator*(const imag<M>& x)
{
	return(cpx_int<N+M>(SSXT((-Signed<M>(x))*im,N+M) , SSXT(x.re*Signed<M>(x),N+M)));
}


//regular saturation
template <int P>
cpx_int<P> sat()
{
	return cpx_int<P>(re.template sat<P>(), im.template sat<P>());
}

//regular saturation: symmetric
template <int P>
cpx_int<P> sym_sat()
{
	return cpx_int<P>(re.template sym_sat<P>(), im.template sym_sat<P>());
}


template<int P>
cpx_int<N-P> trunc_std()
{
	return cpx_int<N-P>(re.template trunc_std<P>(), im.template trunc_std<P>());
}

template<int P, int M>
cpx_int<N-P> trunc_std(const slv<M>& nshift)
{
	return cpx_int<N-P>(re.template trunc_std<P>(nshift), im.template trunc_std<P>(nshift));
}

template<int P>
cpx_int<N-P> sround()
{
	return cpx_int<N-P>(re.template sround<P>(), im.template sround<P>());
}

template<int P, int M>
cpx_int<N-P> sround(const slv<M>& nshift)
{
	return cpx_int<N-P>(re.template sround<P>(nshift), im.template sround<P>(nshift));
}

template<int P>
cpx_int<N-P> sround_sat()
{
	return cpx_int<N-P>(re.template sround_sat<P>(), im.template sround_sat<P>());
}

template<int P, int M>
cpx_int<N-P> sround_sat(const slv<M>& nshift)
{
	return cpx_int<N-P>(re.template sround_sat<P>(nshift), im.template sround_sat<P>(nshift));
}

// Very simplified absolute value ( max(re,im) + 3*min(re,im)/8 )
slv<N> sabs()
{
	slv<N-1> sabs_re = re.sabs();
	slv<N-1> sabs_im = im.sabs();
	slv<N> max_sabs = EXT( (sabs_re > sabs_im) ? sabs_re : sabs_im, N);
	slv<N> min_sabs = EXT( (sabs_re > sabs_im) ? sabs_im : sabs_re, N);
	return max_sabs + min_sabs.srl(2) + min_sabs.srl(3); // could be better if *3 before truncating, anyway, this is approximation
}

// simplified basic operations ( -, conj(), * i, *(-i) )
// Using -x = ~x approximation (sneg)
// These are really useful and convenient, because requires little HW and return a cpx_int the same size
// the noise coming from approximations is generally unnoticeable
// Moreover, the approximation might restore pos/neg symmetry after a rough truncature (no rounding)

cpx_int<N> sconj()
{
	return cpx_int<N>(re, im.sneg());
}

cpx_int<N> sneg()
{
	return cpx_int<N>(re.sneg(), im.sneg());
}

cpx_int<N> sby_i()
{
	return cpx_int<N>(im.sneg(), re);
}

cpx_int<N> sby_minus_i()
{
	return cpx_int<N>(im, re.sneg());
}

// because of VHDL implementation usgin 2*N SLVs:
const static int length = 2*N;
const static int high = 2*N-1;
static const int size = 1;
};

template<int N>
cpx_int<N> Signed<N>::operator+(const imag<N>& x_i)
{
	return cpx_int<N>(*this, static_cast<Signed<N>>(x_i));
}

template<int N>
cpx_int<N> Signed<N>::operator-(const imag<N>& x_i)
{
	return cpx_int<N>(*this, -static_cast<Signed<N>>(x_i));
}

template<int N>
Signed<N> re(const cpx_int<N>& x)
{
	return x.re;
}
template<int N>
Signed<N> im(const cpx_int<N>& x)
{
	return x.im;
}
template<int N>
slv<2*N> to_slv(const cpx_int<N>& x)
{
	return ( (slv<2*N>(re) << N) | (slv<2*N>(im)) );
}

template<int N>
cpx_int_base<N> to_cpx_int(const slv<2*N>& x)
{
	return cpx_int_base<N>(Signed<N>(x>>N), Signed<N>(x));
}

template<int M, int N>
cpx_int<M/2> csxt(const cpx_int<N>& x)
{
	return cpx_int<M/2>(SSXT(x.re, M/2), SSXT(x.im, M/2));
}

#define CSXT(x, N) csxt<N>(x)

#define TO_CPX_INT(x, y, M) cpx_int_base<M/2>(Signed<M/2>(x), Signed<M/2>(y))


#else

#include "../Include_libs/slv.h"

START_OF_FILE(dsp)

INCLUDES

PACKAGE(dsp)

#endif
