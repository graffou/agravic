// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
//#include <cmath>
#include "misc.h"
#include "gprintf.h"






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
#define TOGGLE_BLINK   0x03000000
#define PRINT_AT_XY   0x0F000000
#define CLS   0x02000000
#define FONT   0x01000000

//char test_font[12] = {0xAA,0x55,0x11,0x22,0x33,0x44,0xAA,0x55,0x11,0x22,0x33,0x44};

unsigned char alien1[12]={
	0b00110000,
	0b00001000,
	0b00011111,
	0b00110111,
	0b01111011,
	0b11111111,
	0b11111100,
	0b00011000,
	0b00100111,
	0b01000000,
	0b10000000,
	0b00000000};
/*	
unsigned char alien1b[12]={
	0b00001100,
	0b00001000,
	0b00011111,
	0b00110111,
	0b01111011,
	0b11111111,
	0b11111100,
	0b00011010,
	0b00100111,
	0b00100000,
	0b00010000,
	0b00000000};*/
unsigned char alien1b[12]={
	0b00001100,
	0b00001000,
	0b00011111,
	0b00110111,
	0b01111011,
	0b11111111,
	0b11111010,
	0b00011000,
	0b00100111,
	0b00100000,
	0b00010000,
	0b00000000};	
unsigned char ship[12]={
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000011,
	0b00001111,
	0b00111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b01111000,
	0b00110000
	};	
	
	
uint32_t alt = 0; // alternate form of aliens
int32_t left_to_right = 1; 
 //int8_t left_to_right = 1; // 
#if 1


struct alien{
	//unsigned char first = 66;
	bool dead = 0;
	bool display(unsigned char& x, unsigned char& y, unsigned char first, unsigned char row)
	{
		if (dead)
		{
			PRINT_AT(x, y++, 32);
			PRINT_AT(x, y++, 32);
			PRINT_AT(x, y++, 32);
			PRINT_AT(x, y++, 32);
		}
		else
		{
			unsigned char c = first + alt;
			PRINT_AT(x, y++, 32);
			PRINT_AT(x, y++, c++);
			PRINT_AT(x, y++, c++);
			PRINT_AT(x, y++, 32);
		}
		return dead;
	}
	
	void reset()
	{
		dead = 0;
	}
};

struct alien_row
{
	unsigned char first_char = 98;
	//unsigned char first_idx = 0;
	//unsigned char last_idx = 11;
	alien row[12];
	void conf(unsigned char first_i) {first_char = first_i;}
	uint16_t display(unsigned char& line, unsigned char & col, unsigned char& first_alien, unsigned char& last_alien)
	{
		//unsigned char col = first_col;

		uint16_t dead = 0;
		for (int i = first_alien; i <= last_alien; i++)
		{
			dead = (dead << 1) | row[i].display(line, col, first_char,i+48);
		}
		line++;
		return dead;
	}
	
};

struct alien_matrix
{
	alien_row matrix[5];
	unsigned char first_alien = 0;
	unsigned char last_alien = 11;
	unsigned char last_row = 4;
	unsigned char start_line = 4;
	unsigned char start_col = 0;
	unsigned char line = 4;
	bool stop = 0;
	bool endline = 0;
	
	void blank_line()
	{
		for (int i = 0; i < 79; i++)
			PRINT_AT(line, i, ' ');
		line++;	
	}
	void display()
	{
		unsigned char col = start_col;
		if (not stop) 
		{
			line = start_line;
			uint16_t dead = 0;
			for (int i = 0; i <= last_row; i++)
			{
				col = start_col;
				blank_line(); // not optimal...
				dead = dead & matrix[i].display(line, col, first_alien, last_alien);

			}
			if ( ( (col == 79) and (left_to_right == 1) )or ((start_col == 0) and ( left_to_right == -1) ) ) 
			{
				left_to_right = - left_to_right;
				//start_col+=left_to_right;
				start_line ++;
			}
			else 
			{
				start_col+=left_to_right;
			}	
		}		
		if ( (line == 36) and (col == 79)) // stop movement
		{
			stop = 1;
		}
		alt = (alt == 0) ? 2 : 0;
	}
	
	
};
#endif
int main(int argc, char **argv) {
  // Any data written to the stack segment will connect the lowest four bits to
  // the board leds

	unsigned char ship_x = 39;

 #if 1 
  	*dbg = TOGGLE_BLINK;
   //while ( (*uart & 0x100) == 0); 
 	//*dbg = CLS;
 	//while ( (*uart & 0x100) == 0); 

	for (int i = 0; i < 12; i++)
 	{
		*dbg = FONT | ((('a'-32)*12+i) << 8) | 0xFF;
		*dbg = FONT | ((('y'-32)*12+i) << 8) | (ship[i]);
		*dbg = FONT | ((('z'-32)*12+i) << 8) | BIT_REVERSE(ship[i]);
		*dbg = FONT | ((('b'-32)*12+i) << 8) | (alien1[i]);
		*dbg = FONT | ((('c'-32)*12+i) << 8) | BIT_REVERSE(alien1[i]);
		*dbg = FONT | ((('e'-32)*12+i) << 8) | BIT_REVERSE(alien1b[i]);
		*dbg = FONT | ((('d'-32)*12+i) << 8) | (alien1b[i]);
		
		*dbg = FONT | ((('k'-32)*12+i) << 8) | (alien1b[i]); // bug on last write
	
	}
 	gprintf("TAGADA\n");
    *uart = 0x01A10100;   
    //*uart = 0x01A10100;   
// while ( (*uart & 0x100) == 0); 
 // gprintf("#VRead %Y from UART\n", uint32_t(*uart & 0xff));  
   	
 	alien_matrix aliens;
 
  	for (int k=0; k < 80; k++)
	  	for (int l=0; l < 40; l++)
		PRINT_AT(l,k,' ');
  	for (int k=0; k < 80; k++)
		PRINT_AT(37,k,'a');		
		
	//PRINT_AT(36, 55, 0);
	//gprintf("yz");//ship	
	PRINT_AT(39, 5, 0);
	gprintf("yz");//ship	
	PRINT_AT(39, 10, 0);
	gprintf("yz");//ship	
	PRINT_AT(0,0,0);
	gprintf("AGRAVIC INVADERS");
	PRINT_AT(0,69,0);
	gprintf("SCORE 00000");
		
	//Create blocks
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			PRINT_AT((30-j),(40+(i)*15-22-5),0);
			gprintf("aaaaaaaa");
		}
	}	
		
/*	
 		//PRINT_AT(10,10,'Z');
		//PRINT_AT(20,20,'O');
		//PRINT_AT(30,30,'U');
		for (int i = 0; i< 16; i++)
			PRINT_AT(i, i, 65+i);
		for (int i = 0; i< 16; i++)
			PRINT_AT(0, i, 65+i);
*/
/*	
 	while (1)
 	{
	}
 */
 
 	for (int k=0; k < 1000; k++)
 	{
		aliens.display();
		for (int i = 0; i < 5; i++)
		{
			usleep(100000);
			char c = 0;
			if ( *uart & 0x100)
			 c = static_cast<char>(*uart);
			if ((c == 's') and (ship_x > 0)) ship_x--;
			if ((c == 'd') and (ship_x < 76)) ship_x++;
			PRINT_AT(36, ship_x,0);
			gprintf(" yz ");
			*uart = 0x01A10100;   

		}
	}
 	
 	 //while ( (*uart & 0x100) == 0); 
 	 
 	 
 #endif	 
	gprintf("\x80\x80\x80\n");
	PRINT_AT(19,35,0);
  	gprintf("GAME OVER");
 while (1) {
   *uart = 0x01A10100;   
   while ( (*uart & 0x100) == 0);//{*dbg='*';}; 
	char c = char(*uart);
	if (c == 13) c = 10;
	if (c == 0xA3) 
	{
		*dbg = CLS;
		/*
		for (int i = 0; i < 80;i++)
		{
			for (int j = 0; j < 40;j++)
			{
				*dbg = (PRINT_AT_XY | (i << 8) | ((j) << 16) | (i+j+64));
			}	
		}	
		*/
		PRINT_AT(20,0,128);
	}
	else
	dbg_write(c);
	
	

  /*
   for (int i = 0; i < 80;i++)
   {
		for (int j = 0; j < 40;j++)
		{
			*dbg = (PRINT_AT | (i << 8) | (j << 16) | (i+j));
		}	
	}
	*/
/*
	gprintf("\n");	
	char c = static_cast<char>(*uart);
	for (int i = 7; i >= 0; i--)
		if ( c >>i )
			gprintf("1");
		else
			gprintf("0");
*/	
	//gprintf("popo %\n", char(*uart));
   }
}
