#pragma once
#include <vector>
#include <functional>
#include "vcd.h"

// Functions used to avoid writing to input ports
template <bool B> void abort(const string& x, const string& y){}
// This one is inlined to avoid "already defined during compilation of ..." error
template <> inline void abort<true>(const string& x, const string& y){giprintf("#RFATAL ERROR: Writing in input port %M in module %M, EXITING", x, y);exit(0);}//= delete;

template <class T, bool in>
    struct port : std::reference_wrapper<T>
{
    gmodule* pmodule;
    T dummy;// = new T; // so that reference wrapper can be constructed (needs stg to refer to)
    bool initialized = 0; // Flags that node is connected to a valid signal
    port<T, in>* parent_node = NULL; // keep trace of parent node to propagate the actual reference signal when found

    port() : std::reference_wrapper<T>(dummy)
        {
            pmodule = NULL;
        }


    // constructor links port to its parent module
    port(const sig_desc& x) : std::reference_wrapper<T>(dummy), dummy(x)
        {
    		giprintf("#BIn port CTOR from sigdesc %", x.name);

            int port_hint = in ? -1 : -2; // use nbits to code port dir (number of bits of a port will not be used anyway)

            pmodule = x.pmodule
            giprintf("#MNew port name % ptr %", dummy.pvcd_entry->name, this);
        }

    ~port()
        {
            //std::cerr << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
        }

/*     operator T() */
/*         { */
/*             return static_cast<std::reference_wrapper<T>>(*this); */
/*         } */

    void propagate( T& x )
        {
            if (parent_node != NULL)
            {
                giprintf("#RPropagate... in % to %", pmodule->name, parent_node->dummy.pvcd_entry->name);
               parent_node->operator()(x);
               parent_node->dummy.pvcd_entry->driver = x.pvcd_entry;
               //giprintf("#R!!Propagate... parent ref ptr % parent node n % %", &(parent_node->get()), parent_node->get().n, parent_node->pmodule->name);
            }
            else giprintf("#REnd propagate");
        }

        void operator()( port<T, in>& x) // is bound to another reference (port)
        {
           std::reference_wrapper<T>::operator=(x); // copy ref.
           giprintf("#MBinding %s to %R", dummy.pvcd_entry->name, x.dummy.pvcd_entry->name);

           port<T, in>* node = &x;
           if ((node->parent_node) == NULL) giprintf("#GFree parent in");// :%s ", (node)->dummy.pvcd_entry->name);
           while ( (node->parent_node) != NULL)
           {
        	   giprintf("#RDelegating to: ", (node)->dummy.pvcd_entry->name);
        	   node = (node)->parent_node;
           }

           node->parent_node = this;
           giprintf("#CSetting parent of %R as %R", (node)->dummy.pvcd_entry->name, dummy.pvcd_entry->name);

        }

    void operator()( T& x) // is bound to a signal
        {
     		dummy.copy_children(x);
    		giprintf("#V //name:: % in % ptr %Y", dummy.pvcd_entry->name, pmodule->name, this);
            std::reference_wrapper<T> tmp(x); // create reference to that signal
            std::reference_wrapper<T>::operator=(tmp); // copy reference
            dummy.pvcd_entry->driver = x.pvcd_entry->driver; // Used to trace back the path to driver signal (and activate it / use its identifiers)
            giprintf("#BBinding %s:% to drv %R", dummy.pvcd_entry->pmodule->name, dummy.pvcd_entry->name, dummy.pvcd_entry->driver->name);
            propagate(x);
        }

    //template <class T2>
    void operator=(const T& x)
        {
            abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
            std::reference_wrapper<T>::get() = x;
        }

    template<class T2>
    void operator<=(const T2& x)
        {
#ifdef CHECK_PORTS_MAPPING
            abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
#endif
            std::reference_wrapper<T>::get() <= x;
        }

    T& base_type()
	{
   	 return std::reference_wrapper<T>::get();
	}

    operator T(){
    	return std::reference_wrapper<T>::get();
    }

   	static const int high = T::high;
    	static const int length = T::length;
    	static const int size = T::size;


};

// Define binary operators, Catapult style

#define BIN_OP_DEF(BIN_OP) \
    template<class T1, class T2> auto operator BIN_OP(const port<T1, true>& x, const T2& y) {return (x.get()) BIN_OP y;} \
    template<class T1, class T2> auto operator BIN_OP(const T2& y, const port<T1, true>& x) {return y BIN_OP T1(x);} \
    template<class T1, class T2> auto operator BIN_OP(const port<T2, true>& y, const port<T1, true>& x) {return y BIN_OP (x.get());} \

BIN_OP_DEF(*)
BIN_OP_DEF(/)
BIN_OP_DEF(+)
BIN_OP_DEF(-)
BIN_OP_DEF(&)
BIN_OP_DEF(|)
BIN_OP_DEF(^)
BIN_OP_DEF(==)
BIN_OP_DEF(!=)
BIN_OP_DEF(<)
BIN_OP_DEF(>)
BIN_OP_DEF(>=)
//Not <= of course


// Structure for recursive port mapping (passes block properties to next recursion)
struct gen_blk_map_t
{
	string name;
	gmodule* pmodule;

};

// Creates a top-level block without ports (testbench)
template<class type_out, class T0>
type_out* create_block(const T0& name_i)
{
	giprintf("#VCTOR % \n", name_i);
	type_out* pblk = new type_out(name_i, gmodule::out_of_hier);

	pblk->name = name_i;
	pblk->parent = gmodule::out_of_hier;
	giprintf("#VCreating testbench");

	return pblk;
}

// Creates a new block with its port mapping (vhdl-style block instantiation)
template<class type_out, class T0, class... Args>
type_out* create_block(const T0& name_i, gmodule* pmodule, Args... args)
{
	giprintf("#VCTOR % \n", name_i);
	type_out* pblk = new type_out(name_i, pmodule);
	giprintf("#Vvcd list sz % \n", pblk->vcd_list.size());

	pblk->name = name_i;
	pblk->parent = pmodule;
	giprintf("#VMap ports\n");

	map_ports(pblk, args...);
	giprintf("#UCTOR % \n", name_i);

	return pblk;
}

// recursive functions for vhdl-style port mapping
template<class type_out,  class T1, class T2, class... Args>
void map_ports(type_out* inst, T1 x, T2 y, Args... args)
{
	//giprintf("MP1...");

	//giprintf("MP1 addr % \n", &inst);
	giprintf("MP1 x %\n", (x));
	(inst->*x)(*y);
	giprintf("/MP1\n");
	//giprintf("#r% ext val %y ref val %", name_i, y, x.get());
	//std::cerr << "BIND get " << y << "\n";
	//std::cerr << "BIND get " << toto<1>::in_stat.get() << "\n";
	map_ports(inst, args...);
}


template<class type_out, class T1, class T2>
void map_ports(type_out* inst, T1 x, T2 y)
{
	giprintf("MP2\n");
	//toto<1>::out_stat.get() = 1;
	(inst->*x)(*y);
	//giprintf("#r% ext val %y ref val %", y, inst-x.get());
	//giprintf("#r% ext val %y ref val %", name_i, y, x.get());
	//std::cerr << "BIND get " << y << "\n";
	//std::cerr << "BIND get " << toto<1>::out_stat.get() << "\n";
}




template<class T0, class T1, class T2, class... Args>
gen_blk_map_t gen_blk_map(const T0& name_i, gmodule* pmodule, T1& x,  T2& y, Args&... args)
{
	x=(y);
	giprintf("#r% ext val %y ref val %", name_i, y, x.get());
	//std::cerr << "BIND get " << y << "\n";
	//std::cerr << "BIND get " << toto<1>::in_stat.get() << "\n";
	return gen_blk_map(name_i, pmodule, args...);
}


template<class T0, class T1, class T2>
gen_blk_map_t gen_blk_map(const T0& name_i, gmodule* pmodule,  T1& x,  T2& y)
{
	//toto<1>::out_stat.get() = 1;
	x=(y);
	giprintf("#r% ext val %y ref val %", name_i, y, x.get());
	//std::cerr << "BIND get " << y << "\n";
	//std::cerr << "BIND get " << toto<1>::out_stat.get() << "\n";
	gen_blk_map_t res;
	res.name = name_i;
	res.pmodule = pmodule;
	return res;
}



