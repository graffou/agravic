
#include <stdint.h>

#include "misc.h"
#include "gprintf.h"

int main(int argc, char **argv) {
 

  while (1) {
   *uart = 0x01A10100;   
   while ( (*uart & 0x100) == 0); 
	char c = char(*uart);
	if (c == 13) c = 10;
	if (c == 0xA3) 
	{

		for (int i = 0; i < 80;i++)
		{
			for (int j = 0; j < 40;j++)
			{
				*dbg = (PRINT_AT_XY | (i << 8) | ((j) << 16) | (i+j+64));
			}	
		}	
	}
	else
	dbg_write(c);
	
	
   }
}
