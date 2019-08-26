#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "itoa.h"
struct vcd_entry;

struct gmodule;

//
/*
struct block_desc
{
    gmodule* pmodule = NULL;
    std::string name;

    block_desc()
    {
        std::cerr << "BLOCK DESC CTOR " << " " << "\n";
    }

    template <class T>
    block_desc(T name_i, gmodule* module_i)
     {
    	name = name_i;
    	module = module_i;
         //std::cerr << "BLOCK DESC CTOR " << " " << "\n";
     }

};
*/

static const char noname[5] = "none";
const char vcd_characters[91] = "!%&'()*+,-./:;<=>?@[]^_`{|}~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

struct vcd_entry
{
	//const char* name;
	vcd_entry* driver;
	static int vcd_cnt;
	std::string name;
	gmodule* pmodule = (gmodule*)(NULL);
	int nbits = 0;
	//bool initialized = 0;
	char ID[3] = "##"; // Non initialized vcd_entry -> no probing
	bool binary = 0;
	vcd_entry(){name = noname; driver = this;}
	vcd_entry(sig_desc x_i)
	{
		name = x_i.name;
		pmodule = x_i.pmodule;
		//pmodule->vcd_list.push_back(this);
		driver = this; // default, gets modified when ports are connected to actual signal
	}

	bool not_active()
	{
		return ID[0] == '#';
	}

	template <class C>
	int find_entry(const C& name_i, gmodule* pmodule)
	{
		for (int i = 0; i < pmodule->vcd_list.size(); i++)
		{
			if ( pmodule->vcd_list[i]->name == name_i)
			{
				return i;
			}
		}
		return -1; // not found
	}




	// for slvs only, arrays and records overload this method
	virtual void activate()
	{


		if (vcd_cnt < 90*90)
		{
			if (driver == this) // actual signal, not some port connected to a signal
			{
				gprintf("#Mvcd %s:% is self driven", pmodule->get_full_name(), name);
				if ( not_active() ) // might already be activated by activation of some ports //
				{
					ID[0] = vcd_characters[vcd_cnt / 90];
					ID[1] = vcd_characters[vcd_cnt % 90];
					vcd_cnt++;
				}
			}
			else
			{
				gprintf("#Mvcd %s:% is driven by %s:%", pmodule->get_full_name(), name, driver->pmodule->get_full_name(), driver->name);
				// activate vcd trace of driving signal
				if (driver->not_active())
				{
					gprintf("#GActivating driver");
					driver->activate();
				}
				// Copy driving signal ID
				ID[0] = driver->ID[0];
				ID[1] = driver->ID[1];
			}
		}
		else
		{
			gprintf("#RMaximum count of VCD signals reached");
			exit(0);
		}
	}

};

int vcd_entry::vcd_cnt = 0;


struct vcd_file_t : public std::ofstream
{

	char buf[4096];
	bool dummy = 0;
	static bool inline time_written = 0;
	char* ptr;
	static long long unsigned int inline vcd_time = 0;
	static long long unsigned int inline timebase_ps = 1;

	bool init_done = 0;

	vcd_file_t()
		{
			std::ofstream::open("dut.vcd");
			ptr = buf;
		}

	~vcd_file_t()
	{
		gprintf("#RFlushing VCD, bytes: %", ptr-buf);
		  write(buf, (ptr-buf));

	}

	void set_timebase_ps(unsigned long long x_i)
	{
		timebase_ps = x_i;
	}

	void vcd_dump_ull(const uint64_t& val, char* id, bool& bin)
	{

	  if (not dummy)
	    {
	      if (not time_written)
		{
		  *ptr++ = '#';
		  ptr = ulltoa(ptr, vcd_time);
		  *ptr++ = '\n';
		  time_written = 1;
		}
	      if (not bin)
		{
		  *ptr++ = 'r';
		  ptr = ulltoa(ptr, val);
		  *ptr++ = ' ';
		  *ptr++ = id[0];
		  *ptr++ = id[1];
		  *ptr++ = '\n';
		}
	      else
		{
		  *ptr++ = 'b';
		  for (int i = bin-1; i >=0; i--)
		    *ptr++ = char(((val >> i) & 1)+48);
		  *ptr++ = ' ';
		  *ptr++ = id[0];
		  *ptr++ = id[1];
		  *ptr++ = '\n';
		}

	      if ((ptr-buf)>3600)
		{
		  write(buf, (ptr-buf));
		  ptr = buf;
		}
	    }

	}


	void vcd_dump_ll(const uint64_t& val, char* id, bool& bin)
	{

	  if (not dummy)
	    {
	      if (not time_written)
		{
		  *ptr++ = '#';
		  ptr = lltoa(ptr, vcd_time);
		  *ptr++ = '\n';
		  time_written = 1;
		}
	      if (not bin)
		{
		  *ptr++ = 'r';
		  ptr = lltoa(ptr, val);
		  *ptr++ = ' ';
		  *ptr++ = id[0];
		  *ptr++ = id[1];
		  *ptr++ = '\n';
		}
	      else
		{
		  *ptr++ = 'b';
		  for (int i = bin-1; i >=0; i--)
		    *ptr++ = char(((val >> i) & 1)+48);
		  *ptr++ = ' ';
		  *ptr++ = id[0];
		  *ptr++ = id[1];
		  *ptr++ = '\n';
		}

	      if ((ptr-buf)>3600)
		{
		  write(buf, (ptr-buf));
		  ptr = buf;
		}
	    }

	}
	static void set_vcd_time(const unsigned long long int& t)
	{
	  vcd_time = t;
	  time_written = 0;
	}

	static uint64_t get_vcd_time()
	{
	  return vcd_time ;
	}

	void activate()
	{
		if (not init_done)
		{
			gprintf("Activating VCD ------------------------------------------------------------------------------------------- \n");
			gprintf(*this, "$comment\n");
			gprintf(*this, "   PLATFORM_GS VCD generator v1.0 )\n");
			gprintf(*this, "$end\n");
			gprintf(*this, "$timescale %dps $end\n", timebase_ps);

			gmodule* current = gmodule::out_of_hier;
			for (int kk = 0; kk < gmodule::module_list.size(); kk++)
			{

				gprintf("#Bmodule %", gmodule::module_list[kk]->name);
				gmodule* target = gmodule::module_list[kk];
#ifdef VCD_PRINT
				gprintf("#UNEW MODULE %Y", target->get_full_name());
#endif
				// Find hierarchy level common to current module and target module
				gmodule* common = gmodule::module_list[kk];
				while ( (current != common) )
				{
					int cur_level = current->hier_level;
					int com_level = common->hier_level;
					//gprintf("#Rcur % lvl % com % lvl % %b\n", current, cur_level, common, com_level, current->name);

					if (  ((cur_level) >= (com_level)) )
					{
						current = current->parent;
						gprintf(*this, "$upscope $end\n");
						//gprintf("#M$upscope $end()");
					}
					if ( (cur_level <= com_level) )
					{
						common = common->parent;
					}
					//gprintf("cur % lvl % com % lvl % %\n", current, cur_level, common, com_level, current->name);
				}
				// Now current is common hierarchy between previous hier. level and target module. Build scope to target module.
				// Actually, there should be a single hierarchy level built at a time, but do as it there could be several
				// The reason for that is that hierarchy level are built one at a time
				while (current != target)
				{
					std::cerr << ',';

					// Find target level hierarchy corresponding to current+1
					gmodule* reverse_path = target;
					while( reverse_path->parent != current )
					{
						reverse_path = reverse_path->parent;
					}

					gprintf(*this, "$scope module %s $end\n", reverse_path->name);
					//gprintf("#M$scope module %R $end", reverse_path->name);
					current = reverse_path;

				}

				for (int jj = 0; jj < gmodule::module_list[kk]->vcd_list.size(); jj++)
				{
					gprintf("vcd_list %\n", gmodule::module_list[kk]->vcd_list[jj]->name);
					if ( gmodule::module_list[kk]->vcd_list[jj]->ID[0] != '#')
					{
						gprintf("#VProbing %", gmodule::module_list[kk]->vcd_list[jj]->name);

						if ((gmodule::module_list[kk]->vcd_list[jj]->nbits == 1) or (gmodule::module_list[kk]->vcd_list[jj]->binary))// wire for binary signals
						{
							gmodule::module_list[kk]->vcd_list[jj]->binary = 1;
							gprintf(*this, "$var wire %s %s%s %s $end\n",  (gmodule::module_list[kk]->vcd_list[jj]->nbits & 63), gmodule::module_list[kk]->vcd_list[jj]->ID[0],
									gmodule::module_list[kk]->vcd_list[jj]->ID[1], gmodule::module_list[kk]->vcd_list[jj]->name);
						}
						else
						{
							gprintf(*this, "$var real %s %s%s %s $end\n",  (gmodule::module_list[kk]->vcd_list[jj]->nbits & 63), gmodule::module_list[kk]->vcd_list[jj]->ID[0],
									gmodule::module_list[kk]->vcd_list[jj]->ID[1], gmodule::module_list[kk]->vcd_list[jj]->name);
						}
					}
				}
			}

			// Cleanly end hierarchy
			while (current->hier_level != 0)
			{
				gprintf(*this, "$upscope $end\n");
				//gprintf("#M--->$upscope $end");
				current = current->parent;

			}

			gprintf(*this, "$enddefinitions $end\n");

		}
	}

};


void activate_vcd(const gstring& x_i)
{
	gmodule* pmodule;
	size_t n = x_i.find_last_of(':');
	gprintf("n = %\n", n);
	if (n == std::string::npos)
		n = -1;
	std::string module = x_i.substr(0, n);
	std::string probe = x_i.substr(n+1);

	gprintf("#K s % mod % pr % ", x_i, module, probe);
	for (int kk = 0; kk < gmodule::module_list.size(); kk++)
	{
		if ( module == gmodule::module_list[kk]->get_full_name())
		{
			for ( int i = 0; i < gmodule::module_list[kk]->vcd_list.size(); i++ )
			{
				std::string test = gmodule::module_list[kk]->vcd_list[i]->name;
				gprintf("#Gentry %m", test);
				if ( ( test[0] == '<' ) or ( test[0] == '>' ) ) // in or out
					test = test.substr(1);

				if ( (probe == test) or (probe.empty()) ) // and (gmodule::module_list[kk]->vcd_list[i]->nbits > -1024)) )
				{
					gmodule::module_list[kk]->vcd_list[i]->activate();
					gprintf("#mActivating %r:%b probe %g", gmodule::module_list[kk]->get_full_name(), gmodule::module_list[kk]->vcd_list[i]->name, probe);
				}

			}
		}
	}

}

vcd_file_t vcd_file;




// to initialize vcd entries, so that pvcd_entry->ID = "##" if no initialization occurs
// Then testing pvcd_entry->ID[0] is enough to conclude whether value change should be dumped
vcd_entry dummy_vcd_entry = gen_sig_desc("DUMMY", gmodule::out_of_hier);


template <class T>
vcd_entry* create_vcd_entry(T name_i, gmodule* pmodule_i, int nbits_i)
{
	gprintf(">CVE %\n", name_i);
	vcd_entry* pvcd_entry = new vcd_entry(gen_sig_desc(name_i, pmodule_i));
	pvcd_entry->nbits = nbits_i;
	if (nbits_i == 1)
		pvcd_entry->binary = 1;
	(pmodule_i->vcd_list).push_back(pvcd_entry);
	gprintf("<CVE\n");
	return pvcd_entry;
}

