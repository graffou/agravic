#pragma once

#include <vector>

struct flop_desc
{
};

struct vcd_entry;

struct gmodule;

struct block_desc
{
    gmodule* pmodule;
    std::string name;

    block_desc()
    {
        std::cerr << "BLOCK DESC CTOR " << " " << "\n";
    }

};
struct clk_t;
template<int N> struct slv;
// Helper structure used to initialize slvs with macros
// #define SLV(name, nbits, pmodule) slv<nbits> name = sig_desc(#name, this);

// Helper structure used to initialize slvs with macros
// #define SLV(name, nbits, pmodule) slv<nbits> name = sig_desc(#name, this);
struct sig_desc
{
	//const char* name;
	std::string name;
	gmodule* pmodule;
	sig_desc(){}
	template<class T>
	sig_desc(T name_i, gmodule* pmodule_i)
	{
		name = name_i;
		pmodule = pmodule_i;
	}
};

struct gated_clk_desc: public sig_desc
{
	//const char* name;
	clk_t* parent_clk;
	slv<1>* gating_signal;
	gated_clk_desc(){}
	template<class T, class T2, class T3>
	gated_clk_desc(T name_i, gmodule* pmodule_i,  T2& clk_i,  T3& gating_signal_i)
	{
		name = name_i;
		pmodule = pmodule_i;
		parent_clk = &(clk_i.get());
		gating_signal = &(gating_signal_i.get());
		//giprintf("#VCreating gated clk %Y from %Y", name_i, parent_clk->pvcd_entry->name);
	}

};

template<class T>
sig_desc gen_sig_desc(T name_i, gmodule* pmodule_i)
{
	return sig_desc(name_i, pmodule_i);
}
template<class T, class T2, class T3> //T2 might be clk or port<clk>
gated_clk_desc gen_gated_clk_desc(T name_i, gmodule* pmodule_i, T2& clk_i, T3& gating_signal_i)
{
	return gated_clk_desc(name_i, pmodule_i, clk_i, gating_signal_i);
}
// return type of process
// null pointers when calling a non-existing one (virtual method called)
// actual processes return actual pointers thanks to process and end_process macros
class reset_t;
class clk_t;

// replaced clk and reset references with clk and reset numbers
// could not use clk_t and reset_t forward declarations
struct control_signals
{
	int clk = 0;
	int reset_n = 0;
	control_signals(){
		clk = -1;
		reset_n = -2;
	}
	template <class T1, class T2>
	control_signals(T1& x, T2& y)
	{
		clk = x; reset_n = y;
	}
};

control_signals __control_signals__;// = control_signals(no_clk, no_reset); // gives control signals of current process under evaluation (clk and reset for clk'ed processes). This is set by the PROCESS macros

struct gmodule
{

    static std::vector<gmodule*> module_list;
    static gmodule* out_of_hier;
    static unsigned long long int vcd_time;
    //control_signals __control_signals__;// = control_signals(no_clk, no_reset); // gives control signals of current process under evaluation (clk and reset for clk'ed processes). This is set by the PROCESS macros
    template <class T>
    static gmodule* return_module_ptr(const T& x) // return module ptr from its name. Create it if necessary (for legacy vcd signal declaration / conf_vcd_entry)
        {
            gmodule* found = 0;
            for (int i = 0; i < module_list.size(); i++)
            {
                if (module_list[i]->name == x)
                {
                    found = module_list[i];
                    break;
                }
            }

            if (found)
                return (found) ;
            else
            {
                gmodule* tmp = new gmodule(x);
                if (tmp == NULL)
                {
                    std::cerr << "ERROR ALLOCATING MODULE " << x;
                    exit(0);
                }
                //module_list.push_back(tmp); !!! No, done in conf_vcd_entry(gmodule*, T2)
                return tmp;
            }

        }

    std::vector<vcd_entry*> vcd_list;

    string name;// = "NO_HIER";
    gmodule* parent = NULL;
    int hier_level = 0;

    gmodule()
        {
            std::cerr << "OUT OF HIER\n";
            parent = NULL;
            hier_level = 0;
            name = "OUT OF HIER";
        }

    gmodule(const char name_i[])
        {
            std::cerr << "((((((((((((((((GMODULE CTOR_out " << name_i << "\n";
            name = name_i;
            std::cerr << "Const module 1 " << name << "\n";
            parent = out_of_hier;
            hier_level = 1;
            module_list.push_back(this);


            std::cerr << ")))))))))))))))))))GMODULE CTOR_out " << name << "\n";
        }

    gmodule(const std::string& name_i):gmodule(name_i.c_str())
        {
        }

//    gmodule(const char name_i[], const gmodule& parent_i)
    gmodule(const char name_i[], gmodule* parent_i)
        {
            name = name_i;

            parent = parent_i;
            //std::cerr << "Const module 2: " << name << " parent ptr " << parent << " out of hier ptr " << gmodule::out_of_hier << "\n";
            hier_level = (parent_i->hier_level) + 1;
            module_list.push_back(this);

        }

    gmodule(const string& name_i, gmodule* parent_i):gmodule(name_i.c_str(), parent_i)
        {
        }

    gmodule(const block_desc& x) : gmodule(x.name, x.pmodule) // for gmodule titi = gen_blk_desc("blk_name", this)
        {
             std::cerr << "Const module from blk desc " << x.name  << "\n";
        }

    string get_name()
        {
            return name;
        }
    string get_hierarchy()
        {
            string name_o;
            gmodule* hier = parent;
            while (hier != out_of_hier)
            {
                name_o = hier->name + ":" + name_o;
                hier = hier->parent;
            }
            return name_o;
        }

    string get_full_name() const
        {
    		//std::cerr << ">GFN";
    		if (this == out_of_hier)
    			return "";
            string name_o = name;
            gmodule* hier = parent;
            while (hier != out_of_hier)
            {
                name_o = hier->name + ":" + name_o;
                hier = hier->parent;
            }
    		//std::cerr << "<GFN";
            return name_o;
        }

    void display_signals_full()
        {
/*
  if (flop_list.size())
  {
  for (int i = 0; i < flop_list.size();i++)
  giprintf("#Mmodule %R flop %R ptr %R", get_full_name(), flop_list[i].name,  flop_list[i].signal_ptr);
  }
  else
  {
  giprintf("#Mmodule %R flops %R", get_full_name(), '0');
  }
*/
        }

    void display_signals()
        {
            /*
              if (flop_list.size())
              {
              for (int i = 0; i < flop_list.size();i++)
              giprintf("#K%s:% ", get_full_name(), flop_list[i].name);
              }
              else
              {
              giprintf("#Mmodule %R flops %R", get_full_name(), '0');
              }
            */
        }


    static void list_modules()
    {
    	std::cerr << "Module list\n";
    	for (int i = 0; i < module_list.size(); i++)
    	{
    		std::cerr << module_list[i]->get_full_name() << "\n";
    	}
    }

/*

gmodule(const char name_i[]);

gmodule(const string& name_i);

gmodule(const char name_i[], const gmodule& parent_i);

gmodule(const string& name_i, const gmodule& parent_i);



string get_name();
string get_hierarchy();

string get_full_name() const;
void display_signals();
void display_signals_full();
*/
    uint64_t forever_process_flags = 0; // Flags whether processes are 'forever': can't execute them twice at initialization
    // define four processes that return 0 / so that they are not registered since not declared as non-virtual in the block
    virtual control_signals process0()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }

    virtual control_signals process1()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }

    virtual control_signals process2()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }

    virtual control_signals process3()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }

    virtual control_signals process4()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }

    virtual control_signals process5()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }
    virtual control_signals process6()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }
    virtual control_signals process7()
    {
    	control_signals res;
    	//res.clk = NULL;
    	//res.reset_n = NULL;
    	return res;
    }
};

std::vector<gmodule*> gmodule::module_list;
gmodule* gmodule::out_of_hier = new gmodule;
unsigned long long int gmodule::vcd_time;

