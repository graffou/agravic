#pragma once

//#define REPORT_DELTAS_AS_PS


int current_forever_process = 0;

//#define SCHEDULER_DEBUG
#ifdef SCHEDULER_DEBUG
#  define gprintfz gkprintf	
#else
#  define gprintfz(...) 		 		 
#endif


//struct clk_type;

uint64_t max_delta_cycles = 0;

template<class T>
void create_event(const uint64_t& t, T evt_type);

struct simtime_t
{
	static int cnt;
	int n;
	uint64_t time_ps = -1;
	int next_idx = 0;
	
	simtime_t(){
	next_idx = 0;
	}
	
	simtime_t(const uint64_t& t)
	{
		time_ps = t;
		n = cnt;
		gprintfz("#VNew event  %Y %Y",t, cnt++);
		//if (cnt == 11163) exit(0);
		next_idx = 0;
	}
	~simtime_t()
	{
		gprintfz("Destroying time % n= %R\n", time_ps, n);
	}
	clk_t* pclk = NULL;	
	//std::function<void()>* pcreator_process;
	int forever_process_id = -1;
};


template <int N>
struct event_pool_t
{
    // event pool: event 0 is 'no event'
    simtime_t pool[N];
    int first_free_event = 1;
    int in_use = 0;
    //int first_event = 0;
    
    void init()
    {
        int first_free_event = 1;
        // init linked list of free events
        for (int i = 1; i < N-1; i++)
        {
            pool[i].next_idx = i+1;
            pool[i].forever_process_id = -1;
        }
        pool[N-1].next_idx = 1;
        pool[0].next_idx = 0; // is actually next_event_idx
        in_use = 0;
    }
    
    event_pool_t()
    {
        init();
    }
    
    int get_free_event(uint64_t t)
    {
        // init first_free_event with t, set first_free_event with next free event
        // then return old first_free_event
        pool[first_free_event].time_ps = t;
        pool[first_free_event].n = simtime_t::cnt++;
        int res = first_free_event;
        first_free_event = pool[first_free_event].next_idx;
        in_use++;
        gprintfz("#VGetting free idx % for t % in use: %Y", res, t, in_use);
        return res;
    }
    
    void recycle_event(int n)
    {
        // Put recycled event at the front of free events list
        gprintfz("#VRecycling: %Y %Y", n, pool[n].time_ps);
        pool[n].next_idx = first_free_event;        
        pool[n].forever_process_id = -1;
        first_free_event = n;
        in_use--;
    }
    
    simtime_t& operator()(int n)
    {
        return pool[n];
    }    
    
};

event_pool_t<64> event_pool;
int& next_event_idx = event_pool.pool[0].next_idx;

// for forever_process_id
template<class T>
inline void set_event_type( int n, T evt)
{
    event_pool(n).forever_process_id = evt;
    gprintfz("#VFOREVER event--------------------------- %Y",event_pool(n).time_ps>>3);
}

template<>
inline void set_event_type( int n, clk_t* pclk)
{
    event_pool(n).pclk = pclk;
    gprintfz("#UCLK event--------------------------- %B %B %Y:%Y", pclk->pvcd_entry->name, int(pclk->delta),event_pool(n).time_ps>>3,event_pool(n).time_ps&7);
}

int simtime_t::cnt = 0;
//simtime_t* next_event;


 
struct scheduler_t
{
	bool end_sim = 0;
	std::vector<std::function<void()>> forever_processes;
	void init()
	{
		// configure and activate vcd file
		vcd_file.set_timebase_ps(1);// 1ps is simulation time granularity
	    vcd_file.activate();
	    
		gprintfz("#VScheduler init --------------------------------------------");
		//look for forever processes
		for (int i = 0; i < gmodule::module_list.size(); i++)
		{
			event_pool.init();
			gprintfz("#UModule %", gmodule::module_list[i]->name);
			control_signals x;
			x = gmodule::module_list[i]->process0();
			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintf("#MProcess 0 answered");
				forever_processes.push_back((std::bind(&gmodule::process0, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 1;
			}
			x = gmodule::module_list[i]->process1();
			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintf("#MProcess 1 answered");
				forever_processes.push_back((std::bind(&gmodule::process1, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 2;
			}
			x = gmodule::module_list[i]->process2();
			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintf("#MProcess 2 answered");
				forever_processes.push_back((std::bind(&gmodule::process2, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 4;

			}
			x = gmodule::module_list[i]->process3();
			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintf("#MProcess 3 answered");
				forever_processes.push_back((std::bind(&gmodule::process3, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 8;

			}

			x = gmodule::module_list[i]->process4();

			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintf("#MProcess 4 answered");exit(0);
				forever_processes.push_back((std::bind(&gmodule::process4, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 16;
			}

			x = gmodule::module_list[i]->process5();
			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintfz("#MProcess 5 answered");
				forever_processes.push_back((std::bind(&gmodule::process5, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 32;

			}
			x = gmodule::module_list[i]->process6();
			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintfz("#MProcess 6 answered");
				forever_processes.push_back((std::bind(&gmodule::process6, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 64;

			}
			x = gmodule::module_list[i]->process7();
			if ( (x.reset_n == -1) and (x.clk == -1) ) // no clk, no reset: forever
			{
				gprintfz("#MProcess 7 answered");
				forever_processes.push_back((std::bind(&gmodule::process7, gmodule::module_list[i])));
				gmodule::module_list[i]->forever_process_flags |= 128;

			}

		}
		
		//vcd_file.init(); // erase what has been written when calling processe to find forever processes
		gprintfz("#TPARSE MODULES ---------------------------------------N=%", tree::trees.size());
		// find clocktrees to register processes
		for (int i = 0; i < tree::trees.size(); i++)
		{
			event_pool.init();
			if (tree::trees[i]->is_clk())
			{
				gprintfz("#VParse modules for clk %Y", tree::trees[i]->pvcd_entry->name);
				tree::trees[i]->parse_modules();
			}
		}
		
		gprintf("#TEVENT POOL INIT ---------------------------------------");
		// parse_modules has created many events at time 0: erase all
		event_pool.init();
		
		// Post event at time 0 for all forever processes (to start them)
		for (int i = 0; i < forever_processes.size(); i++)
		{
			gprintfz("#RFE create event init");
			create_event(cur_time, i);

		}	

//exit(0);
		gprintfz("#T*************************** END SCH INIT ************************");	
	}
	
	
		void run(uint64_t time)
		{
			gprintfz("#VRUN---------------------------------------------------");
			while (next_event_idx)
			{
				gprintfz("#U next % FE id % ****************** % %", event_pool(next_event_idx).forever_process_id , event_pool(next_event_idx).forever_process_id, next_event_idx, event_pool(next_event_idx).n, event_pool(next_event_idx).time_ps);
				if ( event_pool(next_event_idx).time_ps > cur_time) // don't check deltas
				{
#ifdef REPORT_DELTAS_AS_PS				
					if ( (event_pool(next_event_idx).time_ps) > cur_time)
					    vcd_file.set_vcd_time( (event_pool(next_event_idx).time_ps >> 8) + (event_pool(next_event_idx).time_ps &255) ); // report deltas as ps
					gprintfz("#B writing vcd time % R because %R > %R raw ps %", (event_pool(next_event_idx).time_ps >> 8) + (event_pool(next_event_idx).time_ps &255), event_pool(next_event_idx).time_ps , cur_time, event_pool(next_event_idx).time_ps >> 8);
#else
					if ( (event_pool(next_event_idx).time_ps & 0xfffffffffffffff8) > cur_time)
					    vcd_file.set_vcd_time( (event_pool(next_event_idx).time_ps >> 8) ); // do not report deltas
#endif					    
					cur_time = event_pool(next_event_idx).time_ps;

					gprintfz("#REnd of current events, time is % vcd_time written %M", cur_time, vcd_file.vcd_time);
				}
				gprintfz("#M***** Process event %R time %R vcd time %Y ******************", event_pool(next_event_idx).n, event_pool(next_event_idx).time_ps, vcd_file.vcd_time);
				
				if (event_pool(next_event_idx).forever_process_id >= 0)
				{
					gprintfz("#GCalling forever process");
					int id = event_pool(next_event_idx).forever_process_id;
					int idx = event_pool(next_event_idx).next_idx; 
					event_pool.recycle_event(next_event_idx);// destroy current event before creating new one: makes it faster
					next_event_idx = idx;
					CALL_FOREVER(id);//next_event->forever_process_id);
				}
				else
				{
					gprintfz("#Rclk event from SCH");		
                    if (not (event_pool(next_event_idx).pclk)->exec_delta()) // is true, all delta clk cycles are complete
				     {
				        clk_t* pclk = event_pool(next_event_idx).pclk;//next_event->pclk;
						int idx = event_pool(next_event_idx).next_idx; 
					    event_pool.recycle_event(next_event_idx);// destroy current event before creating new one: makes it faster
					    next_event_idx = idx;						
						gprintfz("#Rclk event from SCH");		
		                create_event(cur_time+1, pclk);
					 }
                     else
                     {
                        // delta executed, go to next event
                        int idx = event_pool(next_event_idx).next_idx; 
					    event_pool.recycle_event(next_event_idx);// destroy current event before creating new one: makes it faster
					    next_event_idx = idx;
                     }


				    

				}
				
/*				if (next_event->next_event)
					gprintfz("#bswitching to event % time %", next_event->next_event->n, next_event->next_event->time_ps);
				else
					gprintfz("#R event % time % has no child", next_event->n, next_event->time_ps);
				next_event = next_event->next_event;
*/				
                if (not next_event_idx)
                    gprintf("#R********************* No more events time % *************************", cur_time>>3);

			    if (cur_time > time) {gprintf("#VReached time %Y", time);break;}// end of simulation

			    if (end_sim) {gprintf("#VForced end of simulation %Y", time);break;}
			}
			gprintfz("#VEND OF SCH. RUN---------------------------------------------------");

		}
	
};

scheduler_t scheduler;


void disp(int x)
{
	if (x)
	{
		if (event_pool(x).next_idx)
		{
			gprintfz("#b idx % % points to idx % n % time", x, event_pool(x).n, event_pool(x).next_idx, event_pool(event_pool(x).next_idx).n, event_pool(event_pool(x).next_idx).time_ps);
		}
		else
		{
			gprintfz("#b idx % % points to NULL", x, event_pool(x).n);
		}
	}
	else
	{
		gprintfz("#bNULL");
	}
}

//std::allocator<simtime_t> simtime_alloc;

//std::shared_ptr<simtime_t> dummy_simtime = std::allocate_shared< simtime_t>(simtime_alloc, 0);


#define pointed_event(a) (a->next_event)
#define evt_time(a) (event_pool(a).time_ps)
#define nevt(a) (a->n)
template <class T>
//void create_event(const uint64_t& t, int forever_process_id = -1, clk_t* pclk=NULL)
void create_event(const uint64_t& t, T evt_type)
{
	gprintfz("#R<<");
	int new_event_idx = event_pool.get_free_event(t);
	//new_event->forever_process_id = forever_process_id;
	set_event_type(new_event_idx, evt_type);
	//simtime_t** ptr = &next_event;//dummy_simtime;
	int idx = next_event_idx; // points to next_event_idx
    int old_idx = 0;
	if (next_event_idx)
	{
		//pointed_event(ptr) = next_event;
		//pointed_event(new_event) = next_event;
		//event_pool(new_event_idx).next_idx = *idx;

  		while( evt_time(idx) < evt_time(new_event_idx) ) // NOOOOOOO!!!! ->if time_ps are ==, put new event at front and save time
		{
			//gprintf("#Bevt %R time %R new time %R", *idx, evt_time(*idx), evt_time(new_event_idx));
			//gprintfz("% more recent than % (% < %)\n", nevt((*ptr)) , nevt(new_event), (evt_time((*ptr))), evt_time(new_event) ); 
			old_idx = idx;
			idx = event_pool(idx).next_idx;
			//event_pool(new_event_idx).next_idx = *idx;
		    //event_pool(idx).next_idx = idx;			
			//gprintf("#Bevt %R", *idx);
			if ( (idx) == 0 )
			{
				gprintfz("#bNo more child" );
				break;
			}


		}
        //gprintf("#Gstop with *idx = % new_event n % cnt %", idx, event_pool(new_event_idx).n, simtime_t::cnt);

		//(*ptr) = new_event;
		event_pool(new_event_idx).next_idx = idx;
		event_pool(old_idx).next_idx = new_event_idx;
		//disp(new_event_idx);
		//*old_idx = new_event_idx;
		
		int z = next_event_idx;
		int n=0;
		while (z and (n < event_pool.in_use))
		{
		    disp(z);
		    z = event_pool(z).next_idx;
		    n++;
		}
		gprintfz("#ROUT next % new_evt_idx % *idx % old %", next_event_idx, new_event_idx, idx, old_idx);
		
	}
	else
	{
		gprintfz("#bto next_event % n=% cnt %",new_event_idx,event_pool(new_event_idx).n , simtime_t::cnt);

		next_event_idx = (new_event_idx);
		//std::cerr << "!!";		

		//std::cerr << "## " << event_pool(next_event_idx) << " ##";
		event_pool(next_event_idx).next_idx = 0;
		//std::cerr << "??";
		
		int z = next_event_idx;
		int n=0;

		while (z and (n <event_pool.in_use))
		{
		    disp(z);
		    z = event_pool(z).next_idx;
		    n++;
		}
        
	}
	if (0)//simtime_t::cnt > 100) exit(0);
		gprintfz("#R>>");

}




