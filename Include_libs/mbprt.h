#pragma once

#include <iostream>
#include <sstream>

//extern void printf(...);
//#include <stdio.h>
#if 1 //def DEBUG
#include <string>
   using std::string;

#ifdef PC_GRUIK
#include<iostream>
#endif

#define MSG(a) //std::cerr << a << "\n";
struct color
{
  int v;
  
  color(const int x)
    {
      //v = x > 31 ? 31 : x;
      //v = 31 + (x&7) + 6*((x&4) != 0) + 256*((x&8) != 0) + 16*((x&16) != 0);
      v = x > 63 ? 63 : x;
      v = x > 19 ? (x + 267) : x + 31;
      v = x == 63 ? 0 : v;
      v = x == 62 ? 257 : v;
    }

};

template <class T>
 int color_code(T supto)
    {
        //std::cerr << " ! " << supto << " ! ";
      int code = 1024;
	      if (supto == 'r')
		code = 0;
	      if (supto == 'g')
		code = 1;
	      if (supto == 'y')
		code = 2;
	      if (supto == 'b')
		code = 3;
	      if (supto == 'm')
		code = 4;
	      if (supto == 'c')
		code = 5;
	      if (supto == 'k')
		code = 63;

	      if (supto == 'R')
		code = 20;
	      if (supto == 'G')
		code = 21;
	      if (supto == 'Y')
		code = 22;
	      if (supto == 'B')
		code = 23;
	      if (supto == 'M')
		code = 24;
	      if (supto == 'C')
		code = 25;	      
	      if (supto == 'K')
		code = 62;
	      if (supto == 'T')
		code = (15<<8)+22;
	      if (supto == 'U')
		code = (14<<8)+21;
	      if (supto == 'V')
		code = (13<<8)+20;
	      if (supto == 'W')
		code = (12<<8)+24;
        //std::cerr << " !! " << code << " !! ";
	      return code;
    }


template <class T>
string to_string(const T& x)
{
    //return static_cast<string>(x);
	std::stringstream s;
	s << x;
	return (s.str());
}


template<>
string to_string(const short int& x)
{
    char buf[12];
    sprintf(buf, "%d", x);
    return string(buf);
}


template<>
string to_string(const int& x)
{
    char buf[12];
    sprintf(buf, "%d", x);
    return string(buf);
}

template<>
string to_string(const color& x)
{
    string os;
    if (x.v & 256)
    {
        os += "\033[1;";
    }
    else
    {
        os += "\033[0;";
    }
    
    os += to_string(x.v&255);
    os += "m";
    return os;
}

struct gstring : public string
{
    //using string::size;
  bool color_change;
  bool no_color_change;
  int base_color;
  int rcnt;

    gstring()
        {
            base_color = 1024;
            rcnt = 0;
        }

    template <class T>
    gstring(T x) : string(x)
        {
            base_color = 1024;
            rcnt = 0;
        }
    
    bool set_color(gstring& os, int code)
        {
            //std::cerr << ":" << code<<":";
            if (code != 1024)
            {
                if (code < 256)
                {
                    os += to_string(color(code));
                }
                else
                {
                    os += to_string(color(code >> 8));
                    os += to_string(color(code & 0xff));
                }
                return true;
            }
      
            return false;
        }
    
    bool set_color(std::ostream& os, int code)
        {
            //std::cerr << ":" << code<<":";
            if (code != 1024)
            {
                if (code < 256)
                {
                    os << to_string(color(code));
                }
                else
                {
                    os << to_string(color(code >> 8));
                    os << to_string(color(code & 0xff));
                }
                return true;
            }
      
            return false;
        }

    char get_char(int init = 0)
        {
            //std::cerr << "#" << rcnt << " " << size() << "\n";
            char c;
            if (init == 1)
            {rcnt = 0; return ' ';}
            else
            {
                c = string::operator[](rcnt);
                if (!init)
                    rcnt++;
                return c;
            }
	
        }
    

    // Find a character
    bool upto_char(gstring& os, char x)
        {
            //std::cerr << "&" << rcnt;
            char c;
            int code = 1024;
            while (rcnt < size())
            {
                c = this->get_char();
                //std::cerr << c << rcnt;


                if (rcnt == 1)
                {
                    if (c == '#'||c == '@')
                    {
                        if (c == '@')
                            no_color_change = 1;
                        else
                            no_color_change = 0;
		  
                        c = this->get_char();
                        code = color_code(c);
                        base_color = code;
		  
                        if (code != 1024)
                        {
                            set_color(os, base_color);
                            color_change = 1;
                        }
		      
		  
                        c = this->get_char();
                    }
                    else
                    {
                        base_color = 1024;
                        color_change = 0;
                        no_color_change = 0;		
                    }
                    //std::cerr << " " << base_color << " ";
                }
	  
                if (c == x){//std::cerr << "]";
                    return true;}
                os += c;
            }
            return false;	      
        }

    // Find a character
    bool upto_char(std::ostream& os, char x)
        {
            //std::cerr << "&" << rcnt;
            char c;
            int code = 1024;
            while (rcnt < size())
            {
                c = this->get_char();
                //std::cerr << c << rcnt;


                if (rcnt == 1)
                {
                    if (c == '#'||c == '@')
                    {
                        if (c == '@')
                            no_color_change = 1;
                        else
                            no_color_change = 0;
		  
                        c = this->get_char();
                        code = color_code(c);
                        base_color = code;
		  
                        if (code != 1024)
                        {
                            set_color(os, base_color);
                            color_change = 1;
                        }
		      
		  
                        c = this->get_char();
                    }
                    else
                    {
                        base_color = 1024;
                        color_change = 0;
                        no_color_change = 0;		
                    }
                    //std::cerr << " " << base_color << " ";
                }
	  
                if (c == x){//std::cerr << "]";
                    return true;}
                os << c;
            }
            return false;	      
        }    
    // returns a string from start up to next % sign. next_start is the next pos to scan from
    void till_percent(gstring& os, int start, int& next_start)
        {
            //std::cerr << " " << base_color << " ";
            //res = "";
            //int upto;

            //bool found = this->find_char('%',start, upto);
            if (base_color != 1024)
                set_color(os, base_color);
	
            bool found = this->upto_char(os, '%');


            next_start = start; // PG 0514, not used anymore but avoid warnings
      
      
            if (!found || rcnt == size())
            {
                next_start = size();
            }
            else
            {
                char supto = this->get_char(4);//s[upto+1];
                //std::cerr << ':' << supto << ':';
                if (supto == ' ' || supto == '\n' || supto == '%' ||
                    supto == ',' || supto == '.' ||supto == ';'||supto == ')' ||supto == ']' )
                {
                     MSG("CZ");
                   
                }
                else
                {
                    MSG("CC");
                    color_change |= set_color(os, color_code(this->get_char()));
                }
            }

            //this->clone(res, start, upto - start);
            //os << res;
            // std::cerr << "//TP";
            //return res;
            
        }

    // returns a string from start up to next % sign. next_start is the next pos to scan from
    void till_percent(std::ostream& os, int start, int& next_start)
        {
            //std::cerr << " " << base_color << " ";
            //res = "";
            //int upto;

            //bool found = this->find_char('%',start, upto);
            if (base_color != 1024)
                set_color(os, base_color);
	
            bool found = this->upto_char(os, '%');


            next_start = start; // PG 0514, not used anymore but avoid warnings
      
      
            if (!found || rcnt == size())
            {
                next_start = size();
            }
            else
            {
                char supto = this->get_char(4);//s[upto+1];
                //std::cerr << ':' << supto << ':';
                if (supto == ' ' || supto == '\n' || supto == '%' ||
                    supto == ',' || supto == '.' ||supto == ';'||supto == ')' ||supto == ']' )
                {
                     MSG("CZ");
                   
                }
                else
                {
                    MSG("CC");
                    color_change |= set_color(os, color_code(this->get_char()));
                }
            }

            //this->clone(res, start, upto - start);
            //os << res;
            // std::cerr << "//TP";
            //return res;
            
        }   
    
    
// display string from position start till ther next % sign. start is updated w/ the next pos to scan from
    void display_till_percent(gstring& os, int& start)
        {
            //std::cerr << "DTP";
            //gstring lala;
            int next_start = 0; //PG 0515, not used trying to avoid warnings

            if (size() > rcnt)
            {
      
                this->till_percent(os, start, next_start);
                //os << lala;
                start = next_start;
            }
            //std::cerr << "//DTE" << rcnt << " " << size();
            //std::cerr << "//DTP";
        }

    void display_till_percent(std::ostream& os, int& start)
        {
            //std::cerr << "DTP";
            //gstring lala;
            int next_start = 0; //PG 0515, not used trying to avoid warnings

            if (size() > rcnt)
            {
      
                this->till_percent(os, start, next_start);
                //os << lala;
                start = next_start;
            }
            //std::cerr << "//DTE" << rcnt << " " << size();
            //std::cerr << "//DTP";
        }
// Display the string from start til the end
    void display_till_end(gstring& os, int& start)
        {
            //std::cerr << "DTE" << rcnt << " " << size();
            //int next_start; -- !!!!!!
            if (size() > rcnt)
            {
                this->upto_char(os, '%');
                //std::cerr << "(" << rcnt << ")";
                //this->clone(lala, start, size() - start);// - 1);
                //os << lala;
                //start = next_start;-- !!!!!!
            }
  
            if (color_change && !no_color_change)
            {
#ifndef NO_COLOR      
                os += "\033[0m\n";
                //os += std::endl;
#else
                os += "\n";//std::endl;
#endif
      
            }
#ifdef DISCO
            pc.printf(os.c_str());
#else
            //printf(os.c_str());
            std::cerr << os;

#endif
            //std::cerr << "//DTE";
  
        }
    // Display the string from start til the end
        void display_till_end(std::ostream& os, int& start)
            {
                //std::cerr << "DTE" << rcnt << " " << size();
                //int next_start; -- !!!!!!
                if (size() > rcnt)
                {
                    this->upto_char(os, '%');
                    //std::cerr << "(" << rcnt << ")";
                    //this->clone(lala, start, size() - start);// - 1);
                    //os << lala;
                    //start = next_start;-- !!!!!!
                }
      
                if (color_change && !no_color_change)
                {
    #ifndef NO_COLOR      
                    os << "\033[0m\n";
                    //os += std::endl;
    #else
                    os << "\n";//std::endl;
    #endif
          
                }
 
            }
    template <class T>
    void to_stream(gstring& os,  T& x)
        {
            os += to_string(x);
            //std::cerr << "//TS";
        }
    template <class T>
    void to_stream(std::ostream& os,  T& x)
        {
            os << (x);
            //std::cerr << "//TS";
        }
    void append_char(char x)
       {
         this->operator+=(x);
       }

    template<class T>
    bool find_string(const T& x_i, int& nn)
    {
     size_t u = std::string::find(x_i);
     if (u != std::string::npos)
     {
    	 nn = u;
    	 return true;
     }
     else
     {
    	 nn = size();
    	 return false;
     }
    }   
    
 template<class T>
 bool contains(const T& x_i)
 {
	 return std::string::find(x_i) != std::string::npos;
 }
 template <class T1, class T2>
 void replace(const T1&x_i, const T2& y_i)
 {
	 string x(x_i);
	 string y(y_i);
	 int idx = 0;
	 while (find_string(x, idx))
	 {
		 string::replace(idx, x.length(), y);
	 }
 }
/*
 template<class T>
 bool operator==(const T& x_i)
 {
	 //return std::string::compare(x_i) != 0;

 }
*/
 char operator()(int idx)
 {
	 if (idx < std::string::size())
		 return std::string::operator[](idx);
	 else
	 {
		 std::cerr << "ERROR! accessing element " << idx << " of string of size " << std::string::size(); 
		 return char(0);
	 }
 }
 
 // Get word from 
 gstring get_word(int& idx)
 {
	 gstring res;
	 char c = 33;
	 if (idx < std::string::size())
	 {
		 while ( (c > 32) and ( idx < std::string::size() ) )
		 {
			 c = std::string::operator[](idx);
			 if (c > 32)
				 res.append_char(c);
			 idx++;
		 }
		 return res;
	 }
	 else return(res);
 }

 gstring get_line(int& idx)
  {
 	 gstring res;
 	 char c = 33;
 	 if (idx < std::string::size())
 	 {
 		 while ( (c > 13) and ( idx < std::string::size() ) )
 		 {
 			 c = std::string::operator[](idx);
 			 if (c > 13)
 				 res.append_char(c);
 			 idx++;
 		 }
 		 return res;
 	 }
 	 else return(res);
  }

 
 // get string elt
 char get(int idx)
 {
	 return operator()(idx);
 }
 
 // convert hex to int
 unsigned long long int to_dec() // T can only be a gstring, this is to force templatization
     {
         unsigned long long int val = 0;
         int nn = 0;
         if (find_string("0x", nn))
             nn += 2;
         for (int i = nn; i<size(); i++)
         {
             int titi = uint(operator()(i))-(48+7*(operator()(i)>57)+32*(operator()(i)>96));
             if ((titi < 16) && (titi >= 0)) // valid hex char
                 val = (val<<4) + titi;
             else i = size(); // abort
             //gprintf(" - %   .  %  .  %  . %  -", y(i), int(y(i)), 48+7*(int(y(i))>57), val);
         }

         return val;
     }
 
 // convert to int
 long long int to_int()
     {
         if (contains("0x")) // hex string, call to_dec()
             return (long long int)(to_dec());
 
         long long int res = 0;
         int minus = 1;
         for (int i = 0; i< size(); i++)
             if (get(i) == '-')
                 minus = -1;
             else if ((operator()(i) > 47) && (operator()(i) < 58))
                 res = res*10 + (operator()(i) - 48);
         return minus * res;
 
     }
 
 // get numerical value
 int get_num(int idx = 0)
 {
	 gstring s = get_word(idx);
	 return s.to_int();	 
 }
 
 // test if is numerical
 bool is_num(int idx = 0)
 {
	 int i = idx;
	 char c = 33;
	 bool b = 1;
	 if (i < std::string::size())
	 {
		 while ( ( i < std::string::size() ) )
		 {
			 c = std::string::operator[](i);
			 
			 if ( (c < 48) or (c > 57) ) // not integer
			 {
				 b = (c > 32); // consider only valid chars to invalide numeric option
				 break;
			 }
		 }
		 return b;
	 }
	 else return(0);
	 
 }
 
 friend std::istream& operator>> (std::istream& is, gstring& x)
 {
	 char c = 32;
	 do
	 {
		 is.get(c);
		 //std::cerr << int(c) << " / ";//
		 x += c;
	 }
	 while (not is.eof() and not ( (c == 10) or (c == 13) ));
	 return is;
 }
};

//template <class T, class T0>
template <class T00>
//void gprintf(T& os, T0 x)
void gkprintf(T00& x)
{
    gstring os;
    os = to_string(x);
#ifndef PC_GRUIK
    printf(os.c_str());
#else
    std::cerr << os;
#endif
}


template <class T1, class T2, class T, class ...Args>
void recurse_prt(T1& os, T2& toto, int start, const T& first, const Args&... args)
{
	toto.to_stream(os, first); toto.display_till_percent(os, start);
    recurse_prt(os, toto, start, args...);
}
template<class T1, class T2>
void recurse_prt(T1& os, T2& toto, int start)
{
}
template < class ...Args>
void gkprintf(const char* format, const Args&... args)
{
	 gstring toto = format; toto.get_char(true);
	    gstring os;

	  int start=0;

	  // before 1st %
	  toto.display_till_percent(os, start);
	  recurse_prt(os, toto, start, args...);

	  // after last %
	  toto.display_till_end(os, start);

}

static void gkprintf(const char* format)
{
  int start=0;
    gstring os;
  gstring toto = format; toto.get_char(true);
  toto.display_till_percent(os, start);
  toto.display_till_end(os, start);
}

template < class ...Args>
void gkprintf(std::ostream& os,const char* format, const Args&... args)
{
	 gstring toto = format; toto.get_char(true);

	  int start=0;

	  // before 1st %
	  toto.display_till_percent(os, start);
	  recurse_prt(os, toto, start, args...);

	  // after last %
	  toto.display_till_end(os, start);

}

static void gkprintf(std::ostream& os,const char* format)
{
  int start=0;
  gstring toto = format; toto.get_char(true);
  toto.display_till_percent(os, start);
  toto.display_till_end(os, start);
}


template<class T>
void shell(T& s)//stringstream& s)
{

	std::string ss = s.str();
    gkprintf("\nsh: %K",ss);
    system(ss.c_str());
    s.str("");//Clear shell cmd
}

template <class T> // for char*
void shell(const T* s)//stringstream& s)
{
	std::string ss(s);
    gkprintf("\nsh: %K",ss);
    system(ss.c_str());
    //s.str("");//Clear shell cmd
}
template <class T>
gstring to_bin(T val, int nbins = 0)
{

gstring x;
unsigned int z = val;

if (nbins == 0)
  {
    while (z)
{
  z >>= 1;
  nbins++;
}
  }

z = val;

int nn = 1;
while ( (nn <= nbins) || (nn == 1) )
  {
    int u = (z >> (nbins-1)) & 1;
    //gprintf("%R % - ", u);
    x.append_char(char(u + 48));
    z = z << 1;
    nn++;
  }
//gprintf("dec2hex % nbins % : % \n", val, nbins, x);
return x;
}


template <class T>
gstring to_hex(T val, int nbins = 0)
{

gstring x;
unsigned int z = val;

if (nbins == 0)
  {
    while (z)
{
  z >>= 4;
  nbins++;
}
  }

z = val;

int nn = 1;
while ( (nn <= nbins) || (nn == 1) )
  {
    int u = (z >> 4*(nbins-1)) & 15;
    //gprintf("%R % - ", u);
    x.append_char(char(u + 48 + 7*(u>9)));
    z = z << 4;
    nn++;
  }
//gprintf("dec2hex % nbins % : % \n", val, nbins, x);
return x;
}
#endif //DEBUG
