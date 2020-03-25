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
    //vcd_entry* pvcd_entry;
    //string name;
    //T* pdummy = new T; // so that reference wrapper can be constructed (needs stg to refer to)
    T dummy;// = new T; // so that reference wrapper can be constructed (needs stg to refer to)
    bool initialized = 0; // Flags that node is connected to a valid signal
    port<T, in>* parent_node = NULL; // keep trace of parent node to propagate the actual reference signal when found

    port() : std::reference_wrapper<T>(dummy)
        {
            pmodule = NULL;
            //name = "";
        }


    // constructor links port to its parent module
    port(const sig_desc& x) : std::reference_wrapper<T>(dummy), dummy(x)
        {
    		giprintf("#BIn port CTOR from sigdesc %", x.name);
            //string port_name = x.name;
            //string port_dir = in ? "<" : ">";
            //port_name.insert(0, port_dir);
    		//!!!!! better use port macro to insert > or <
        	//giprintf("#U naming %Y ptr %Y dummy ptr %Y pvcd %Y", x.name, this, &dummy, dummy.pvcd_entry);

            int port_hint = in ? -1 : -2; // use nbits to code port dir (number of bits of a port will not be used anyway)
            //dummy.pvcd_entry->name = x.name;//port_name.c_str();
            //dummy.conf_vcd_entry(x.pmodule, port_name.c_str());
            //dummy.pvcd_entry->nbits = port_hint;
            //dummy(x.pmodule, port_name.c_str(), port_hint, 0);
/*             if (in) */
/*                 T(x.pmodule, (x.name.insert(0, '<').c_str(), -1, 0); */
/*             else */
/*                 T(x.pmodule, (x.name.insert(0, '>').c_str(), -2, 0); */

            pmodule = x.pmodule;
            //giprintf("#U naming %Y ptr %Y dummy ptr %Y pvcd %Y", x.name, this, &dummy, dummy.pvcd_entry);
            //*(dummy.pvcd_entry) = gen_sig_desc(x.name, pmodule);
            //dummy.pvcd_entry = create_vcd_entry(x.name, pmodule, port_hint);
            //name = x.name;
            //giprintf("#bcreating % of % ptr = %r ref %", name, pmodule->name, dummy, &std::reference_wrapper<T>::get());
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

    //void operator()(const std::reference_wrapper<T>& x) // is bound to another reference (port)
        void operator()( port<T, in>& x) // is bound to another reference (port)
        {
        	//std::cerr << "?" << x.get() << "?";
            std::reference_wrapper<T>::operator=(x); // copy ref.
        	//std::cerr << "?";
            //giprintf("#bbinding (ref) % of % ptr = %r org = %r", name, pmodule->name, &std::reference_wrapper<T>::get(), &x.get());
        	//std::cerr << "?";
           //x.parent_node = this;
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
    		//giprintf("#V % name % mod % ", name, pmodule->name);
    		//giprintf("#V name:: % in % signal %", dummy.pvcd_entry->name, pmodule->name, x.name);
    		dummy.copy_children(x);
    		giprintf("#V //name:: % in % ptr %Y", dummy.pvcd_entry->name, pmodule->name, this);
            std::reference_wrapper<T> tmp(x); // create reference to that signal
            std::reference_wrapper<T>::operator=(tmp); // copy reference
            dummy.pvcd_entry->driver = x.pvcd_entry->driver; // Used to trace back the path to driver signal (and activate it / use its identifiers)

            //giprintf("#R % ", tmp);
            //if (pdummy != NULL)
            //    delete pdummy; //delete dummy
            //giprintf("#bbinding (sig) % of % ptr = %r org = %r pdummy %g", name, pmodule->name, &std::reference_wrapper<T>::get(), &x, dummy);
    		giprintf("#BBinding %s:% to drv %R", dummy.pvcd_entry->pmodule->name, dummy.pvcd_entry->name, dummy.pvcd_entry->driver->name);
            propagate(x);
        }

    //template <class T2>
    void operator=(const T& x)
        {
            //std::cerr << x;
            //if (pmodule)
            abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
            //std::cerr << "get " << std::reference_wrapper<T>::get();

            std::reference_wrapper<T>::get() = x;
            //std::cerr << '/';
        }

    template<class T2>
    void operator<=(const T2& x)
        {
            //std::cerr << x;
            //if (pmodule)
#ifdef CHECK_PORTS_MAPPING
            abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
#endif
            //std::cerr << "get " << std::reference_wrapper<T>::get();
            //std::cerr << '.';
            std::reference_wrapper<T>::get() <= x;
            //std::cerr << '/';
        }

    T& base_type()
	{
   	 return std::reference_wrapper<T>::get();
	}

    operator T(){
    	return std::reference_wrapper<T>::get();
    }

   	static const int high = -1;
    	static const int length = -1;
    	static const int size = -1;


};

// For slv class
    template < template<int> class T, int N,  bool in>
        struct port<T<N>, in> : std::reference_wrapper<T<N>>
    {
        gmodule* pmodule;
        //vcd_entry* pvcd_entry;
        //string name;
        //T<N>* pdummy = new T<N>; // so that reference wrapper can be constructed (needs stg to refer to)
        T<N> dummy;// = new T<N>; // so that reference wrapper can be constructed (needs stg to refer to)
        bool initialized = 0; // Flags that node is connected to a valid signal
        port<T<N>, in>* parent_node = NULL; // keep trace of parent node to propagate the actual reference signal when found

        port() : std::reference_wrapper<T<N>>(dummy)
            {
                pmodule = NULL;
                //name = "";
            }


        // constructor links port to its parent module
        port(const sig_desc& x) : std::reference_wrapper<T<N>>(dummy), dummy(x)
            {
                //string port_name = x.name;
                //string port_dir = in ? "<" : ">";
                //port_name.insert(0, port_dir);
        		//!!!!! better use port macro to insert > or <
            giprintf("#U naming %Y ptr %Y dummy ptr %Y pvcd %Y", x.name, this, &dummy, dummy.pvcd_entry);

                int port_hint = in ? -1 : -2; // use nbits to code port dir (number of bits of a port will not be used anyway)
                //dummy.pvcd_entry->name = x.name;//port_name.c_str();
                //dummy.conf_vcd_entry(x.pmodule, port_name.c_str());
                //dummy.pvcd_entry->nbits = port_hint;
                //dummy(x.pmodule, port_name.c_str(), port_hint, 0);
    /*             if (in) */
    /*                 T<N>(x.pmodule, (x.name.insert(0, '<').c_str(), -1, 0); */
    /*             else */
    /*                 T<N>(x.pmodule, (x.name.insert(0, '>').c_str(), -2, 0); */

                pmodule = x.pmodule;
                //dummy.pvcd_entry = create_vcd_entry(x.name, pmodule, port_hint);

                giprintf("#U naming %Y ptr %Y dummy ptr %Y pvcd %Y", x.name, this, &dummy, dummy.pvcd_entry);
                //*(dummy.pvcd_entry) = gen_sig_desc(x.name, pmodule);
                //name = x.name;
                //giprintf("#bcreating % of % ptr = %r ref %", name, pmodule->name, dummy, &std::reference_wrapper<T<N>>::get());
                giprintf("#MNew port name % ", dummy.pvcd_entry->name);
            }

        ~port()
            {
                //std::cerr << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
            }

         operator T<N>()
             {
                 return std::reference_wrapper<T<N>>::get();//static_cast<std::reference_wrapper<T<N>>>(*this);
             }

         T<N>& base_type()
		{
        	 return std::reference_wrapper<T<N>>::get();
		}

        void propagate( T<N>& x )
            {
                if (parent_node != NULL)
                {
                    giprintf("#RPropagate... %s:% to %s:%", dummy.pvcd_entry->pmodule->name, dummy.pvcd_entry->name, parent_node->dummy.pvcd_entry->pmodule->name, parent_node->dummy.pvcd_entry->name);
                    parent_node->operator()(x);
                    parent_node->dummy.pvcd_entry->driver = x.pvcd_entry;
                    giprintf("#R!!Propagate... parent ref ptr % ", &(parent_node->get()));
                }
            }

        //void operator()(const std::reference_wrapper<T<N>>& x) // is bound to another reference (port)
            void operator()( port<T<N>, in>& x) // is bound to another reference (port)
            {
            	//std::cerr << "?" << x.get() << "?";
                std::reference_wrapper<T<N>>::operator=(x); // copy ref.
            	//std::cerr << "?";
                //if (pdummy != NULL)
                //    delete pdummy; // delete dummy
                //giprintf("#bbinding (ref) % of % ptr = %r org = %r", name, pmodule->name, &std::reference_wrapper<T<N>>::get(), &x.get());
            	//std::cerr << "?";
               //x.parent_node = this;

               port<T<N>, in>* node = &x;
               if ((node->parent_node) == NULL) giprintf("#GFree parent in");// :%s ", (node)->dummy.pvcd_entry->name);
               while ( (node->parent_node) != NULL)
               {
            	   giprintf("#RDelegating to: ", (node)->dummy.pvcd_entry->name);
            	   node = (node)->parent_node;
               }

               node->parent_node = this;
               giprintf("#CSetting parent of %R as %R", (node)->dummy.pvcd_entry->name, dummy.pvcd_entry->name);

            }

        void operator()( T<N>& x) // is bound to a signal
            {
        		//giprintf("#BBinding %s:% to %s:%", dummy.pvcd_entry->pmodule->name, dummy.pvcd_entry->name);
        		//giprintf("#V % name % mod % ", name, pmodule->name);
        		//giprintf("#V name % ", dummy.pvcd_entry->name);
                std::reference_wrapper<T<N>> tmp(x); // create reference to that signal
                std::reference_wrapper<T<N>>::operator=(tmp); // copy reference
                dummy.pvcd_entry->driver = x.pvcd_entry->driver; // Used to trace back the path to driver signal (and activate it / use its identifiers)
                //giprintf("1 % module % \n", dummy.pvcd_entry->name, pmodule->name);
                //giprintf("2 %\n", x.pvcd_entry);
                //giprintf("2 %\n", x.pvcd_entry->name);
                //dummy.pvcd_entry->driver = x.pvcd_entry->driver; // Used to trace back the path to dirver signal (and activate it / use its identifiers)
        		giprintf("#BBinding %s:% to drv %R", dummy.pvcd_entry->pmodule->name, dummy.pvcd_entry->name, dummy.pvcd_entry->driver->name);
                //giprintf("#R % ", tmp);
                //if (pdummy != NULL)
                //    delete pdummy; //delete dummy
                //giprintf("#bbinding (sig) % of % ptr = %r org = %r pdummy %g", name, pmodule->name, &std::reference_wrapper<T<N>>::get(), &x, dummy);
                propagate(x);
            }

        //template <class T2>
        void operator=(const T<N>& x)
            {
                //std::cerr << x;
                //if (pmodule)
                abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
                //std::cerr << "get " << std::reference_wrapper<T>::get();

                std::reference_wrapper<T<N>>::get() = x;
                //std::cerr << '/';
            }

        template <class T2>
        void operator<=(const T2& x)
        //void operator<=(const T<N>& x)
            {
                //std::cerr << x;
                //if (pmodule)
#ifdef CHECK_PORTS_MAPPING
                abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
#endif
                //std::cerr << "get " << std::reference_wrapper<T>::get();
                //std::cerr << "!!";
                std::reference_wrapper<T<N>>::get() <= x;
                //std::cerr << "!!";
               //std::cerr << '/';
            }


       	static const int high = N-1;
        static constexpr  int length = N;
        static const int size = 1;

    };


#if 0
    template <int N>
    class slv;

    template <int N, bool in>
        struct port<slv<N>,in> : std::reference_wrapper<slv<N>>
    {
        gmodule* pmodule;
        //vcd_entry* pvcd_entry;
        //string name;
        //T* pdummy = new T; // so that reference wrapper can be constructed (needs stg to refer to)
        slv<N> dummy;// = new T; // so that reference wrapper can be constructed (needs stg to refer to)
        bool initialized = 0; // Flags that node is connected to a valid signal
        port<slv<N>, in>* parent_node = NULL; // keep trace of parent node to propagate the actual reference signal when found

        port() : std::reference_wrapper<slv<N>>(dummy)
            {
                pmodule = NULL;
                //name = "";
            }


        // constructor links port to its parent module
        port(const sig_desc& x) : std::reference_wrapper<slv<N>>(dummy), dummy(x)
            {
                //string port_name = x.name;
                //string port_dir = in ? "<" : ">";
                //port_name.insert(0, port_dir);
        		//!!!!! better use port macro to insert > or <

                int port_hint = in ? -1 : -2; // use nbits to code port dir (number of bits of a port will not be used anyway)
                //dummy.pvcd_entry->name = x.name;//port_name.c_str();
                //dummy.conf_vcd_entry(x.pmodule, port_name.c_str());
                dummy.pvcd_entry->nbits = port_hint;
                //dummy(x.pmodule, port_name.c_str(), port_hint, 0);
    /*             if (in) */
    /*                 T(x.pmodule, (x.name.insert(0, '<').c_str(), -1, 0); */
    /*             else */
    /*                 T(x.pmodule, (x.name.insert(0, '>').c_str(), -2, 0); */

                pmodule = x.pmodule;
                *(dummy.pvcd_entry) = gen_sig_desc(x.name, pmodule);
                //name = x.name;
                //giprintf("#bcreating % of % ptr = %r ref %", name, pmodule->name, dummy, &std::reference_wrapper<T>::get());
                giprintf("#MNew port name % ", dummy.pvcd_entry->name);
            }

        ~port()
            {
                //std::cerr << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
            }

    /*     operator T() */
    /*         { */
    /*             return static_cast<std::reference_wrapper<T>>(*this); */
    /*         } */

        void propagate( slv<N>& x )
            {
                if (parent_node != NULL)
                {
                    parent_node->operator()(x);
                    giprintf("#RPropagate...");
                }
            }

        //void operator()(const std::reference_wrapper<T>& x) // is bound to another reference (port)
            void operator()( port<slv<N>, in>& x) // is bound to another reference (port)
            {
            	std::cerr << "?" << x.get() << "?";
                std::reference_wrapper<slv<N>>::operator=(x); // copy ref.
            	std::cerr << "?";
                //if (pdummy != NULL)
                //    delete pdummy; // delete dummy
                //giprintf("#bbinding (ref) % of % ptr = %r org = %r", name, pmodule->name, &std::reference_wrapper<T>::get(), &x.get());
            	std::cerr << "?";
               x.parent_node = this;
            }

        void operator()( slv<N>& x) // is bound to a signal
            {
        		//giprintf("#V % name % mod % ", name, pmodule->name);
        		giprintf("#V name % ", dummy.pvcd_entry->name);
                std::reference_wrapper<slv<N>> tmp(x); // create reference to that signal
                std::reference_wrapper<slv<N>>::operator=(tmp); // copy reference
                dummy.driver = &x; // Used to trace back the path to dirver signal (and activate it / use its identifiers)

                //giprintf("#R % ", tmp);
                //if (pdummy != NULL)
                //    delete pdummy; //delete dummy
                //giprintf("#bbinding (sig) % of % ptr = %r org = %r pdummy %g", name, pmodule->name, &std::reference_wrapper<T>::get(), &x, dummy);
                propagate(x);
            }

        //template <class T2>
        void operator=(const slv<N>& x)
            {
                //std::cerr << x;
                //if (pmodule)
                abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
                //std::cerr << "get " << std::reference_wrapper<T>::get();

                std::reference_wrapper<slv<N>>::get() = x;
                //std::cerr << '/';
            }

        void operator<=(const slv<N>& x)
            {
                //std::cerr << x;
                //if (pmodule)
                abort<in>(dummy.pvcd_entry->name, pmodule->get_full_name());
                //std::cerr << "get " << std::reference_wrapper<T>::get();

                std::reference_wrapper<slv<N>>::get() <= x;
                //std::cerr << '/';
            }

        operator slv<N>(){
        	return std::reference_wrapper<slv<N>>::get();
        }

    	static const int high = N-1;
    	static const int length = N;
    	static const int size = 1;

    };
#endif

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



