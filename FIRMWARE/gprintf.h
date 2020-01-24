


#define INT32_TYPE  0x80000000
#define INT16_TYPE  0x81000000
#define INT8_TYPE   0x82000000
#define UINT32_TYPE 0x83000000
#define UINT16_TYPE 0x84000000
#define UINT8_TYPE  0x85000000
#define BOOL_TYPE   0x86000000
#define FLOAT_TYPE  0x87000000
#define COLOR_TYPE  0xE0000000
#define END_PRINT   0xF0000000
#define SET_CONSOLE_MODE   0x03000000
#define PRINT_AT_XY   0x0F000000
#define CLS   0x02000000
#define FONT   0x01000000



void dbg_write(const char *ptr)
{
   while( (*ptr != 0) )
   {
    *dbg = *ptr;
    ptr++;
   }
    *dbg = 0; // tag end of char* print
}

struct color_type
{
    char code;
    color_type(char x)
    {
        code = x;
    }
};

// Hoping that all signed and unsigned types will automatically cast to one of these (including char)
void dbg_write(float x)
{
    *dbg = FLOAT_TYPE;
    *dbg = *reinterpret_cast<uint32_t*>(&x);
}

void dbg_write(uint32_t x)
{
    *dbg = UINT32_TYPE;
    *dbg = x;
}

void dbg_write(int32_t x)
{
    *dbg = INT32_TYPE;
    *dbg = x;
}
void dbg_write(uint16_t x)
{
	for (int k = 12; k >= 0;k-=4)
	{
		uint8_t c = (x >> k) & 15;
		*dbg = (c < 10 ? 48 : 55) + c;
	}
}
void dbg_write(color_type x)
{
    *dbg = COLOR_TYPE | uint32_t(x.code);
}

void dbg_write( char c)
{
    *dbg = c; 
}

void recurse_print(const char* toto)
{
    dbg_write(toto);
    // End of print: tag this
    *dbg = END_PRINT;
}


void till_percent_and_send_color(const char*& toto)
{

    while ( (*toto != '\0') and (*toto != '%') ) // Parse char string
    {
        *dbg = (*toto); // output char
        toto++;
    }  
    if (*toto != 0)
    {
        toto++;
        if (*(toto) > 65) // assume it's color code
        {
            dbg_write(color_type(*(toto++)));
        }    
    }
}

template <class T, class ...Args>
void recurse_print(const char* toto, const T& first, const Args&... args)
{

    till_percent_and_send_color(toto);   
 
    dbg_write(first);

    recurse_print(toto,  args...);
}


#define PRINT_AT(a, b, c) *dbg = (PRINT_AT_XY | (b << 8) | ((a) << 16) | (c));
#define READ_BUF() static_cast<char>(*dbg)
#define SET_CONSOLE(a) (*dbg = (SET_CONSOLE_MODE | a))
#define gprintf(...) recurse_print(__VA_ARGS__)
