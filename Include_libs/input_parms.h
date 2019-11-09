
// Classes for parsing CLI input parameters --------- Graffou 2018 -----------
// 1) Include this file at top level (main function)
// 2) Declare your CLI parameters using the CLI_PARM macro:
//    CLI_PARM(foo, std::string);
//    CLI_PARM(bar, integer);
//    CLI_PARM(boolarg, bool);
// 3) Pass argc and argc of your main to the CLI_PARSE macro:
//    CLI_PARSE(argc, argv);
// 4) Then call your main with these arguments:
//    my_main -foo filename -bar 6 -boolarg
//    --> note that boolean arguments do not require values !!!!!!!!!!!!!!
//        other types of arguments do not have a default value, but they might be initialized
//        using macro: CLI_PARM_INIT(bar, integer, 12);
// 5) You can check that foo.val = "filename" and bar.val = 6 boolarg = 1
//    foo and bar are not directly usable. It could be possible with an inheritance
//    mechanism, which would exclude base types like integer, double...
// 6) you can add command line help this way: foo.set_help("Very useful option");
//    This help will be displayed when the --help option
//    To add a general description of the SW, add a CLI parameter called help,
//    AFTER OTHER PARAMETERS (helps are displayed in reverse order):
//      CLI_PARM(help, bool);
//	    help.set_help("This is a very useful piece of software");
//      CLI_PARSE(argc, argv);	

// Gstring container for input parameters 
struct arg_list
{
	std::vector<gstring> args;
/*	~arg_list()
	{
		~args();
	}
*/
	arg_list(int argc, char* argv[])
	{
		for(int i = 1; i < argc; i++)
		{
			gstring toto = argv[i];
			args.push_back(toto);
		}
	}	
};

// parameter base class w/ linked list mechanism
struct param_base
{
	gstring name;
	gstring desc;
	bool is_set;
	bool is_mandatory; // So that sw aborts is parm is not set by CLI
	//param_set* p_set_value; // pointer to the parameter which value will be set by next arg
	param_base* p_prev; // linked list 
	static param_base* p_set_value;// pointer to the last created object (at construction), then the parameter which value will be set by next arg
	param_base(const char* name_i)
	{
		is_set = 0;
		is_mandatory = 0;
		name = name_i;
		//p_set_value = NULL;
		p_prev = p_set_value;
		p_set_value = this; // at this point p_set_value points to the last constructed obj. 
		//gprintf("#VAdding %Y to CLI parameter list", name);
	}
	
	void set_mandatory()
	{is_mandatory = 1;}
	
	template <class T2>
	void set_help(T2& x)
	{
		desc = x;
	}
	
	// 
	template <class T2>
	void check_name(T2& argname_i)
	{		
		//gprintf("#Rchecking %", argname_i);
		p_set_value = NULL;

		// check that arguments starts with dash char
		gstring argname = argname_i;
		if (argname[0] == '-') 
		{
			//argname.replace("-", ""); // remove dash
			argname.erase(0,1);
			//gprintf("#KAllegedly removed dash: %", argname);
		}
		else 
		{
			//gprintf("#no dash: %", argname);
			return; // no dash, not valid. exit
		}
		bool help_asked = (argname == "help");
		if (help_asked) gprintf("\n\n---------------------------- Parameters of executable ----------------------------------\n");
		
		// Find parm pointer that corresponds to this argument name
		param_base* x = this;
		//gprintf("#Ustarting from %", x->name);
		//int cnt = 0;	
		while ( (x != NULL) )
		{
			if (help_asked)
				x->display_help();
			//gprintf("#Vcompare /%Y/ and /%Y/ sz %Y %Y -> %Y", x->name, argname, (x->name).size(), argname.size(), (x->name == argname));
			if (x->name == argname) // found argument
			{
				//gprintf("#bfound %", x->name);
				p_set_value = x;
				x = x->p_prev;
				//x = NULL;
			}
			else
			{
				//gprintf("#UNo match current is % p_prev %", x->name, x->p_prev);				
				x = x->p_prev;
				//gprintf("#UBack to % x=%", x->name, x);				
			}
		}
		
		if (help_asked) { gprintf("\n\n--------------------------------------------------------------\n"); exit(0);}

		
		
	}
	
	void display_help()
	{
		//gprintf("#k%K:%", name, tab<40>(desc) ); not working anymore ???
		gprintf("#k%K: %", name, (desc) );
	}
	
	void parse_args(arg_list& x)
	{
		int idx = 0;

		while (idx < x.args.size())
		{
			gprintf("#Kidx is %R, size is %R", idx, x.args.size());
			check_name(x.args[idx]);
			if (p_set_value) // argument found in expected arguments list, p_set_value points to it
			{
				if (p_set_value->is_bool()) // boolean arguments do not require values
				{
					p_set_value->set_value("1");
					gprintf("#BActivating boolean option %R", p_set_value->name);
				}
				else
				{
					idx++;
					if (idx < x.args.size()) // sanity check
					{
						p_set_value->set_value(x.args[idx].c_str()); // set parm value
						gprintf("#MSetting parameter %R with value %R", p_set_value->name, x.args[idx]);
					}
				}
			}
			idx++;
		}
		
		// Now check all mandatory parms have been set
		param_base* p = this;
		bool failed = 0;
		while ( (p != NULL) )
		{
			if (p->is_mandatory and not p->is_set)
			{
				gprintf("#RERROR! parameter %M is mandatory but is not set!", p->name);
				failed = 1;
			}	
			p = p->p_prev;
		}		
		if (failed)
		{
			std::cerr << char(7);
			exit(0);
		}
		
		
	}
	
	virtual void set_value(const char* arg){}
	virtual bool is_bool(){return false;}
	
};

param_base* param_base::p_set_value = NULL;

template <class T> struct param : param_base
{
	T val; // The parameter set by arg
	
	param(const char* name_i) : param_base(name_i)
	{
		
	}
	bool is_bool() {return false;}
	
	void set_value(const char* arg){
		std::stringstream s;
		gprintf(s, arg);
		s >> val;
		is_set = 1;
		} 	
		
	operator T() const {return val;}
	
	char* c_str(){return val.c_str();} // might be required when operating with string/gstring
	
};
template <class T> struct param< std::vector<T> > : param_base
{
	std::vector<T> val; // The parameter set by arg
	
	param(const char* name_i) : param_base(name_i)
	{
		
	}
	bool is_bool() {return false;}
	
	void set_value(const char* arg){
		std::stringstream s;
		gprintf(s, arg);
		T tmp;
		s >> tmp;
		val.push_back(tmp);
		is_set = 1;
		} 	
	template<class T2>	
	T operator[](T2 x) {return val[x];}
};
template <> struct param<bool> : param_base
{
	bool val; // The parameter set by arg
	
	param(const char* name_i) : param_base(name_i)
	{
		val = 0;
	}
	bool is_bool() {return true;}
	
	void set_value(const char* arg){
		val = 1;
		is_set = 1;
		} 	
		
	operator bool() const {return val;}

};
#define CLI_PARM(name, stype) param<stype> name = #name
#define CLI_PARMS(name, stype) param< std::vector<stype> > name = #name
#define CLI_PARM_INIT(name, stype, val) param<stype> name = #name; name.set_value(#val)

#define CLI_PARSE(argc, argv) arg_list my_args(argc, argv); param_base::p_set_value->parse_args(my_args);
