
template <class T, int N>
struct array_base
{
	T v[N];
	T& operator()(int n)
	{
		if (n >= N) { gkprintf("R#ERROR: accessing element % of array 0 to %", n, N-1); exit(0);}
		return array_base<T,N>::v[n];
	}
	T& operator[](int n)
	{
		if (n >= N) { gkprintf("R#ERROR: accessing element % of array 0 to %", n, N-1); exit(0);}
		return array_base<T,N>::v[n];
	}
	
	template <int M, int P>
	array_base<T, M> shift(Signed<P> n)
	{
	    array_base<T, M> res;
	    for (int i = 0; i < M; i++)
	    {
	        int idx = i - conv_integer(n);
	        if ( (idx >= 0) and (idx < N) )
	        {
	            res.v[i] = v[idx];
	        }      
	    }
	    return res;
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
struct array< T1<T,N> > : T1<T,N> //, vcd_entry
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
	T& operator()(int n)
	{
		if (n >= N) { gkprintf("#R************** ERROR: accessing element % of array 0 to % ( % ) ********************", n, N-1, pvcd_entry->name); exit(0);}
		return array_base<T,N>::v[n];
	}

	// For arrays of arrays
	void conf_vcd_entry(const sig_desc& x_i )
	{
		giprintf("#MConv vcd entry % ", x_i.name);
		pvcd_entry = create_vcd_entry(x_i.name, x_i.pmodule, N);

		pvcd_entry->nbits = -65536; // marks an array of vcd entries
		//x_i.pmodule->vcd_list.push_back(pvcd_entry);//static_cast<vcd_entry*>(this));
		for (int i = 0; i < N; i++)
		{
#if 0
			std::string name = x_i.name + '(' + std::to_string(i) + ')';
			array_base<T,N>::v[i].pvcd_entry = create_vcd_entry(name, x_i.pmodule, (*this).length + array_base<T,N>::v[i].length);
#else // trying to make arrays of records possible
			std::string name = x_i.name + '(' + std::to_string(i) + ')';
			int nbits = ( (*this).length | array_base<T,N>::v[i].length);
			array_base<T,N>::v[i].conf_vcd_entry(gen_sig_desc(name, x_i.pmodule));//, (*this).length + array_base<T,N>::v[i].length);
			array_base<T,N>::v[i].pvcd_entry->nbits = nbits; //gen_sig_desc(name, x_i.pmodule);//, (*this).length + array_base<T,N>::v[i].length);
#endif

		}
		giprintf("#BConv vcd entry % ", x_i.name);

	}

	array(const sig_desc& x_i ) //: vcd_entry(x_i)
	{
		giprintf("#Marray ctor from vcd_entry% ", x_i.name);
		conf_vcd_entry(x_i);
		//vcd_entry = create_vcd_entry(x_i.name, x_i.pmodule, -4);
		//vcd_entry::nbits = -65536; // marks an array of vcd entries
		//x_i.pmodule->vcd_list.push_back(static_cast<vcd_entry*>(this));
/*
		for (int i = 0; i < N; i++)
		{
#if 0
			std::string name = x_i.name + '(' + std::to_string(i) + ')';
			array_base<T,N>::v[i].pvcd_entry = create_vcd_entry(name, x_i.pmodule, (*this).length + array_base<T,N>::v[i].length);
#else // trying to make arrays of records possible
			std::string name = x_i.name + '(' + std::to_string(i) + ')';
			gprintf("#Gname: %", name);
			int nbits = (*this).length + array_base<T,N>::v[i].length;
			array_base<T,N>::v[i].conf_vcd_entry(gen_sig_desc(name, x_i.pmodule));//, (*this).length + array_base<T,N>::v[i].length);
			gprintf("#V1");
			array_base<T,N>::v[i].pvcd_entry->nbits = nbits; //gen_sig_desc(name, x_i.pmodule);//, (*this).length + array_base<T,N>::v[i].length);
			gprintf("#V2");
#endif

		}
*/		giprintf("#Barray ctor % ", pvcd_entry->name);

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

	void operator<=(const T1<T,N>& x_i)
	{
		for (int i = 0; i < N; i++)
		{
			array_base<T,N>::v[i].operator<=(x_i.v[i]);
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
	// This one is required for tree signals, however all non-slv ports are handled by the same SW
	void copy_children(const array< T1<T,N> > & x_i){}


	static const int length = -65536;
	static const int high = -65536;
	static const int size = -65536;
	static const int array_length = N;
};
