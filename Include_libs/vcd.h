#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "itoa.h"
struct vcd_entry;

struct gmodule;

#ifdef USE_FST
#include "fstapi.h"
static void* m_fst = fstWriterCreate("dut.fst", 1);
#endif

static const char noname[5] = "none";
const char vcd_characters[91] = "!%&'()*+,-./:;<=>?@[]^_`{|}~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
void clean_vcd();

struct vcd_entry
{
	//const char* name;
	vcd_entry* driver;
	vcd_entry* driven_by;

	static int vcd_cnt;
	std::string name;
	gmodule* pmodule = (gmodule*)(NULL);
	int nbits = 0;
	//bool initialized = 0;
	char ID[3] = "##"; // Non initialized vcd_entry -> no probing
	bool binary = 0;
#ifdef USE_FST
	int fst_handle = -1;
#endif

	~vcd_entry(){giprintf("#VDestroying vcd entry %Y %Y ptr %!y", pmodule->name, name, this);}

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

		//if ( (nbits == -2048) or (nbits == -65536) ) return; //ugly fix to cpx_int & array extra vcd probes		
		if ( ( (nbits & 2047) == 0) ) return; //ugly fix to cpx_int & array extra vcd probes

		if (vcd_cnt < 90*90)
		{
			if (driver == this) // actual signal, not some port connected to a signal
			{
				giprintf("#Mvcd %s:% is self driven bin %", pmodule->get_full_name(), name, binary);
				if ( not_active() ) // might already be activated by activation of some ports //
				{
					ID[0] = vcd_characters[vcd_cnt / 90];
					ID[1] = vcd_characters[vcd_cnt % 90];
					vcd_cnt++;
				}
			}
			else
			{
				giprintf("#Mvcd %s:% is driven by %s:% bin %", pmodule->get_full_name(), name, driver->pmodule->get_full_name(), driver->name, binary);
				// activate vcd trace of driving signal
				if (driver->not_active())
				{
					giprintf("#GActivating driver");
					driver->activate();
					driver->binary = binary; // Driver not activated: copy binary flag to driver					
				}
				else
				{
					binary = driver->binary; // Driver already activated: keep driver binary flag to keep consistency among driven signals
				}
				// Copy driving signal ID
				ID[0] = driver->ID[0];
				ID[1] = driver->ID[1];
			}
		}
		else
		{
			gkprintf("#RMaximum count of VCD signals reached");
			exit(0);
		}
	}

};

int vcd_entry::vcd_cnt = 0;


struct vcd_file_t : public std::ofstream
{
	std::ofstream probes;
	char buf[4096];
	bool dummy = 0;
	static bool inline time_written = 0;
	char* ptr;
	static long long unsigned int inline vcd_time = 0;
	static long long unsigned int inline last_vcd_time = 0;
	static long long unsigned int inline timebase_ps = 1;

	bool init_done = 0;

	vcd_file_t()
		{
			std::ofstream::open("dut.vcd");
			probes.open("probes");
			ptr = buf;
		}

	~vcd_file_t()
	{
		giprintf("#RFlushing VCD, bytes: %", ptr-buf);
		  write(buf, (ptr-buf));

	}

	void init()
	{
		ptr = buf;
	}

	void vcd_close()
	{
		write(buf, (ptr-buf));
		std::ofstream::flush();
		std::ofstream::close();

#ifdef USE_FST
		fstWriterClose(m_fst);
#endif
		gprintf("#VTrace file flushed last vcd time %Y", last_vcd_time);
	}

#ifndef USE_FST
	void set_timebase_ps(unsigned long long x_i)
	{
		timebase_ps = x_i;
	}

	void vcd_dump_ull(const uint64_t& val, vcd_entry* pvcd_entry) //char* id, const uint8_t& bin)
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
			char* id = &pvcd_entry->ID[0];
			uint8_t bin = pvcd_entry->nbits;
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

	void vcd_dump_tristate(const uint64_t& val, const uint64_t& z_flags,vcd_entry* pvcd_entry) //char* id, const uint8_t& bin)
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
			char* id = &pvcd_entry->ID[0];
			uint8_t bin = pvcd_entry->nbits;
			// always bin
		{
		  *ptr++ = 'b';
		  for (int i = bin-1; i >=0; i--)
		  {
			  if ( (z_flags >> i) & 1)
				  *ptr++ = 'Z';
			  else
				  *ptr++ = char(((val >> i) & 1)+48);
		  }
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

	void vcd_dump_ll(const uint64_t& val, vcd_entry* pvcd_entry) //char* id, const uint8_t& bin)
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
			char* id = &pvcd_entry->ID[0];
			uint8_t bin = pvcd_entry->nbits;
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

#else // FST traces
void set_timebase_ps(unsigned long long x_i)
{
timebase_ps = x_i;

}

void check(int fst_handle, int nbits)
{
	if ((fst_handle < 0) or (nbits == 0) or (nbits > 64) )
	{
		gprintf("#UFST error hdl % nbits % time %", fst_handle, nbits, vcd_time);
	}
}

void vcd_dump_ull(const uint64_t& val, vcd_entry* pvcd_entry) //int fst_handle, int nbits)
{

 if (not dummy)
   {
     if (not time_written)
     {
 		fstWriterEmitTimeChange(m_fst, vcd_time);//fst_handle, nbits, val);
  		//last_vcd_time = vcd_time;

     }
     int fst_handle = pvcd_entry->fst_handle;
     int nbits = pvcd_entry->nbits&127;
     //check(fst_handle, nbits);
     //gprintf("#MFST % % %", pvcd_entry->name, fst_handle, nbits);
     if (1) //not bin)
     {
  		fstWriterEmitValueChange64(m_fst, fst_handle, nbits, val);

     }

   }

}

void vcd_dump_tristate(const uint64_t& val, const uint64_t& z_flags, vcd_entry* pvcd_entry) //const uint64_t& z_flags,int fst_handle, const uint8_t& bin)
{

 if (not dummy)
   {
     if (not time_written)
     {
  		fstWriterEmitTimeChange(m_fst, vcd_time);//fst_handle, nbits, val);
  		//last_vcd_time = vcd_time;

     }
     int fst_handle = pvcd_entry->fst_handle;
     int nbits = pvcd_entry->nbits&127;
     //check(fst_handle, nbits);
     //gprintf("#MFST % % %", pvcd_entry->name, fst_handle, nbits);
    // always bin
		fstWriterEmitValueChange64(m_fst, fst_handle, nbits, val);

   }

}

void vcd_dump_ll(const uint64_t& val, vcd_entry* pvcd_entry) //int fst_handle, const uint8_t& bin)
{

 if (not dummy)
   {
     if (not time_written)
     {
  		fstWriterEmitTimeChange(m_fst, vcd_time);//fst_handle, nbits, val);
  		//last_vcd_time = vcd_time;
     }
     int fst_handle = pvcd_entry->fst_handle;
     int nbits = pvcd_entry->nbits&127;
     //check(fst_handle, nbits);
     //gprintf("#MFST % % %", pvcd_entry->name, fst_handle, nbits);
		fstWriterEmitValueChange64(m_fst, fst_handle, nbits, val);

   }

}
#endif


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
			gprintf(*this, "   AGRAVIC VCD generator v1.0 )\n");
			gprintf(*this, "$end\n");
			gprintf(*this, "$timescale %dps $end\n", timebase_ps);
#ifdef USE_FST
			std::string s;
			s = std::to_string(timebase_ps) + "ps";
			fstWriterSetTimescaleFromString(m_fst, s.c_str());
			fstWriterSetPackType(m_fst, FST_WR_PT_LZ4);
#endif

			gmodule* current = gmodule::out_of_hier;
			for (int kk = 0; kk < gmodule::module_list.size(); kk++)
			{

				giprintf("#Bmodule %", gmodule::module_list[kk]->name);
				gmodule* target = gmodule::module_list[kk];
#ifdef VCD_PRINT
				giprintf("#UNEW MODULE %Y", target->get_full_name());
#endif
				// Find hierarchy level common to current module and target module
				gmodule* common = gmodule::module_list[kk];
				while ( (current != common) )
				{
					int cur_level = current->hier_level;
					int com_level = common->hier_level;
					//giprintf("#Rcur % lvl % com % lvl % %b\n", current, cur_level, common, com_level, current->name);

					if (  ((cur_level) >= (com_level)) )
					{
						current = current->parent;
						gprintf(*this, "$upscope $end\n");
#ifdef USE_FST
						fstWriterSetUpscope(m_fst);
#endif
						//giprintf("#M$upscope $end()");
					}
					if ( (cur_level <= com_level) )
					{
						common = common->parent;
					}
					//giprintf("cur % lvl % com % lvl % %\n", current, cur_level, common, com_level, current->name);
				}
				// Now current is common hierarchy between previous hier. level and target module. Build scope to target module.
				// Actually, there should be a single hierarchy level built at a time, but do as it there could be several
				// The reason for that is that hierarchy level are built one at a time
				while (current != target)
				{
					//std::cerr << ',';

					// Find target level hierarchy corresponding to current+1
					gmodule* reverse_path = target;
					while( reverse_path->parent != current )
					{
						reverse_path = reverse_path->parent;
					}

					gprintf(*this, "$scope module %s $end\n", reverse_path->name);
					//giprintf("#M$scope module %R $end", reverse_path->name);
#ifdef USE_FST
					fstWriterSetScope(m_fst, FST_ST_VCD_SCOPE, reverse_path->name.c_str(), NULL);
#endif //giprintf("#M$scope module %R $end", reverse_path->name);
					current = reverse_path;

				}

				for (int jj = 0; jj < gmodule::module_list[kk]->vcd_list.size(); jj++)
				{
					gkprintf(probes, "%s:%s\n", gmodule::module_list[kk]->get_full_name(), gmodule::module_list[kk]->vcd_list[jj]->name);
					//giprintf("vcd_list %\n", gmodule::module_list[kk]->vcd_list[jj]->name);
					if ( gmodule::module_list[kk]->vcd_list[jj]->ID[0] != '#')
					{
						giprintf("#VProbing % bin % nbits %", gmodule::module_list[kk]->vcd_list[jj]->name, gmodule::module_list[kk]->vcd_list[jj]->binary, gmodule::module_list[kk]->vcd_list[jj]->nbits&127);

#ifdef USE_FST
// This won't do
// With vcd, I can activate a probe without any reference to its hierarchy level
// With FST, I need to activate a driver knowing its hierarchy before activating aliases
// OR, find driver in the hierarchy
						// get port type
						gstring signame =  gmodule::module_list[kk]->vcd_list[jj]->name.c_str();
						fstVarDir vardir = FST_VD_MIN; // no in/out
						if( (signame[0] == '<') or signame[0] == '<') // in / out, inout partially implemented as in
						{
							if (signame[0] == '<') // in
							{
								vardir = FST_VD_INPUT;
								signame = signame.substr(1);
							}
							else
							{
								vardir = FST_VD_OUTPUT;
								signame = signame.substr(1);
							}
						}

						// Modify name if signal is a record member ---------------------------
						if (signame.contains('.')) // record member
						{
							signame = "\\" + signame; // So that gtkwave keeps record name before field
						}

						int nbits = (gmodule::module_list[kk]->vcd_list[jj]->nbits & 127); // remove extra info (array, record...)
						fstVarType FST_type = (nbits == 1) ? FST_VT_VCD_WIRE : FST_VT_VCD_REG;

						// Create fst entry ------------------------------------------------------
						if (gmodule::module_list[kk]->vcd_list[jj]->driver == gmodule::module_list[kk]->vcd_list[jj]) // self driven
						{
							if (gmodule::module_list[kk]->vcd_list[jj]->fst_handle == -1) // fst has not been indirectly activated by a driven signal
							{
								// new fst entry
								gmodule::module_list[kk]->vcd_list[jj]->fst_handle = fstWriterCreateVar(m_fst, FST_type, vardir, nbits, signame.c_str(), 0);
							}
							else
							{
								// The handle was actually the handle of a driven signal: create an alias to the initial handle for the actual probe of the driver signal
								gmodule::module_list[kk]->vcd_list[jj]->fst_handle = fstWriterCreateVar(m_fst, FST_type, vardir, nbits, signame.c_str(), gmodule::module_list[kk]->vcd_list[jj]->fst_handle);
							}
						}
						else // Is not a self-driven signal (is a port)
						{
							if (gmodule::module_list[kk]->vcd_list[jj]->driver->fst_handle == -1) // driver not been indirectly activated by a driven signal, nor directly
							{
								// create new fst entry for drivent signal probe
								gmodule::module_list[kk]->vcd_list[jj]->fst_handle = fstWriterCreateVar(m_fst, FST_type, vardir, nbits, signame.c_str(), 0);
								//copy fst_handle of the driven signal to the driver's handle: any driven signal can retrieve the originally created probe (of a driven signal) and alias to it
								gmodule::module_list[kk]->vcd_list[jj]->driver->fst_handle = gmodule::module_list[kk]->vcd_list[jj]->fst_handle;
							}
							else
							{
								// driver handle exists, is either the actual driver probe or the probe of an equally driven signal. Anyway, alias to it.
								gmodule::module_list[kk]->vcd_list[jj]->fst_handle = fstWriterCreateVar(m_fst, FST_type, vardir, nbits, signame.c_str(), gmodule::module_list[kk]->vcd_list[jj]->driver->fst_handle);
							}
						}

#endif
						if ((gmodule::module_list[kk]->vcd_list[jj]->nbits == 1) or (gmodule::module_list[kk]->vcd_list[jj]->binary))// wire for binary signals
						{
							gmodule::module_list[kk]->vcd_list[jj]->binary = 1;
							gprintf(*this, "$var wire %s %s%s %s $end\n",  (gmodule::module_list[kk]->vcd_list[jj]->nbits & 127), gmodule::module_list[kk]->vcd_list[jj]->ID[0],
									gmodule::module_list[kk]->vcd_list[jj]->ID[1], gmodule::module_list[kk]->vcd_list[jj]->name);
						}
						else
						{
							gprintf(*this, "$var real %s %s%s %s $end\n",  (gmodule::module_list[kk]->vcd_list[jj]->nbits & 127), gmodule::module_list[kk]->vcd_list[jj]->ID[0],
									gmodule::module_list[kk]->vcd_list[jj]->ID[1], gmodule::module_list[kk]->vcd_list[jj]->name);
						}
					}
					//else
					//	giprintf("#UAlready Probed %", gmodule::module_list[kk]->vcd_list[jj]->name);

				}
			}

			// Cleanly end hierarchy
			while (current->hier_level != 0)
			{
				gprintf(*this, "$upscope $end\n");
				//giprintf("#M--->$upscope $end");
				current = current->parent;

			}

			gprintf(*this, "$enddefinitions $end\n");

		}
		// Now, can set 0 bits for non-bin entries
		clean_vcd();
	}

};

// activate vcd probe using a line from vcd.scn file
void activate_vcd(const gstring& x_i)
{
	gmodule* pmodule;
	bool bin = 0;
	size_t n = x_i.find_last_of(':');
	giprintf("n = %\n", n);
	if (n == std::string::npos)
		n = -1;
	std::string module = x_i.substr(0, n);
	std::string probe_mode = x_i.substr(n+1);
	std::string probe;
	std::string mode;
	n = probe_mode.find_last_of(' ');
	//giprintf("n = %\n", n);
	if (n == std::string::npos)
	{
		giprintf("No space char found in % \n", probe_mode);
		probe = probe_mode;
	}
	else
	{
		probe = probe_mode.substr(0, n);
		mode = probe_mode.substr(n+1);
		bin = (mode == "bin");
#ifdef USE_FST
		bin = 1;
#endif
	}



	giprintf("#K s % mod % pr % ", x_i, module, probe);
	for (int kk = 0; kk < gmodule::module_list.size(); kk++)
	{
		if ( module == gmodule::module_list[kk]->get_full_name())
		{
			for ( int i = 0; i < gmodule::module_list[kk]->vcd_list.size(); i++ )
			{
				std::string test = gmodule::module_list[kk]->vcd_list[i]->name;
				//giprintf("#Gentry %m", test);
				if ( ( test[0] == '<' ) or ( test[0] == '>' ) ) // in or out
					test = test.substr(1);

				if ( (probe == test) or (probe.empty()) ) // and (gmodule::module_list[kk]->vcd_list[i]->nbits > -1024)) )
				{
					//gmodule::module_list[kk]->vcd_list[i]->activate();
					if (bin)
					{
						giprintf("#mActivating %r:%b probe %g in binary mode", gmodule::module_list[kk]->get_full_name(), gmodule::module_list[kk]->vcd_list[i]->name, probe);
						gmodule::module_list[kk]->vcd_list[i]->binary = 1;
					}
					else
					{
						giprintf("#mActivating %r:%b probe %g", gmodule::module_list[kk]->get_full_name(), gmodule::module_list[kk]->vcd_list[i]->name, probe);
						//gmodule::module_list[kk]->vcd_list[i]->nbits = 0;

					}
					gmodule::module_list[kk]->vcd_list[i]->activate();

				}

			}
		}
	}

}

// set nbits = 0 for all vcd entries that are not binaries
void clean_vcd()
{
	for (int kk = 0; kk < gmodule::module_list.size(); kk++)
	{

		for ( int i = 0; i < gmodule::module_list[kk]->vcd_list.size(); i++ )
		{
			if (not gmodule::module_list[kk]->vcd_list[i]->binary)
			{
				gmodule::module_list[kk]->vcd_list[i]->nbits = 0;
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
	//giprintf(">CVE %\n", name_i);
	vcd_entry* pvcd_entry = new vcd_entry(gen_sig_desc(name_i, pmodule_i));
	pvcd_entry->nbits = nbits_i;
	if (nbits_i == 1)
		pvcd_entry->binary = 1;
	(pmodule_i->vcd_list).push_back(pvcd_entry);
	giprintf("#CCreate vcd entry %R %R ptr %R", pmodule_i->name, name_i, pvcd_entry);
	//giprintf("<CVE\n");
	return pvcd_entry;
}

