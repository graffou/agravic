// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
//#include <cmath>
#include "misc.h"
#include "gprintf.h"
//#include <cstddef>



#define GAME_ACCELERATION 0
#define REPEAT_FIRE 1
#define BOMB_MASK  0xC0000000;

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
	0b00000000,
	0b00000000,
	0b00000000,
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
unsigned char shipm[12]={
	0b00011000,
	0b00011000,
	0b01111110,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b00000000,
	0b00000000
	};



unsigned char missile1[12]={
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
	};

unsigned char crash[12]={
	0b011100110,
	0b00011110,
	0b01101110,
	0b11111111,
	0b11100111,
	0b11111111,
	0b00110011,
	0b11001111,
	0b11110011,
	0b10011110,
	0b00111100,
	0b01100000
	};

unsigned char missile1b[12]={

	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,	};
unsigned char hole[12]={
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b10000001,
	0b10000001,
	0b11000011,
	0b11000011,
	0b11111111,
	0b11111111
};


unsigned char saucer1[12]={
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000111,
	0b00111111,
	0b11100110,
	0b11100110,
	0b00111111,
	0b00000111,
	0b00000000,
	0b00000000,
	0b00000000
};

unsigned char saucer2[12]={
	0b00000000,
	0b00000000,
	0b00000111,
	0b11111111,
	0b11111111,
	0b01100110,
	0b01100110,
	0b11111111,
	0b11111111,
	0b00000111,
	0b00000000,
	0b00000000
};


uint32_t alt = 0; // alternate form of aliens
//int32_t left_to_right = 1; 
int8_t left_to_right = 1; // 
uint8_t lifes = 2;

struct saucer
{
	int8_t left_to_right = 1;
	int8_t pos = -1;
	uint8_t cnt;
	void init(uint32_t rnd_i)
	{

		if ((pos < 0) and ((rnd_i>>28) == 0))
		{
			if (rnd_i & 0x00800000)
				left_to_right = -1;
			else
				left_to_right = 1;
			if (left_to_right == 1)
				pos = 0;
			else
				pos = 76;
			cnt = 0;
		}


	}

	void display()
	{
		if ( (pos>=0) )
		{
			if (cnt == 2)
			{
				PRINT_AT(3,pos,0);
				gprintf(" opqr ");
				pos = pos + left_to_right;

				if ((pos == 0) or (pos == 76))
				{
					PRINT_AT(3,pos,0);
					gprintf("      ");
					pos = -1;
				}
				//else
				//	gprintf("      ");
				cnt = 0;
			}
			else cnt++;
		}

	}
	void erase()
	{
		PRINT_AT(3,pos,0);
		gprintf("      ");
		pos = -1;
	}
};

saucer the_saucer;

struct alien{
	//unsigned char first = 66;
	bool dead = 0;
	bool display(unsigned char& x, unsigned char& y, unsigned char first, unsigned char row)
	{
		if (dead)
		{
			PRINT_AT(x, y, 32); y++;
			PRINT_AT(x, y, 32); y++;
			PRINT_AT(x, y, 32); y++;
			PRINT_AT(x, y, 32); y++;
		}
		else
		{
			unsigned char c = first + alt;
			PRINT_AT(x, y, 32); y++;
			PRINT_AT(x, y, c++); y++;
			PRINT_AT(x, y, c++); y++;
			PRINT_AT(x, y, 32); y++;
		}
		return dead;
	}
	
	void init()
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
	uint16_t display(unsigned char& line, unsigned char & col, unsigned char& first_alien, unsigned char& last_alien, unsigned char& dead_row)
	{
		//unsigned char col = first_col;

		uint16_t dead = 0;
		dead_row = 1;
		for (int i = first_alien; i <= last_alien; i++)
		{
			bool dead_alien = row[i].display(line, col, first_char,i+48);
			dead = (dead << 1) | dead_alien;
			if (not dead_alien)
				dead_row = 0;
		}
		line++;
		return dead;
	}
	
	void init()
	{
		for (int i = 0; i <= 11; i++)
			row[i].init();

	}

};

unsigned char ship_x = 1;
uint16_t score;
uint16_t timeout = 20000;
uint32_t super_timeout = 20000*25;


void erase_missiles();

void new_ship()
{
	usleep(300000);
	PRINT_AT(36, ship_x,0);
	gprintf("     ");

	PRINT_AT(39, (lifes+1)*5, 0);
	gprintf("   ");
	ship_x = 0;
	erase_missiles();
	PRINT_AT(36, ship_x,0);
	gprintf(" xyz ");
	usleep(4000000);
}

void init_invaders();

void init_game()
{
	erase_missiles();
	init_invaders();

  	for (int k=0; k < 80; k++)
	  	for (int l=0; l < 40; l++)
		PRINT_AT(l,k,' ');
  	for (int k=0; k < 80; k++)
		PRINT_AT(37,k,'a');

  	score = 0;

	PRINT_AT(39, 5, 0);
	gprintf("xyz");//ship
	PRINT_AT(39, 10, 0);
	gprintf("xyz");//ship
	PRINT_AT(0,0,0);
	gprintf(" AGRAVIC INVADERS");
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

	PRINT_AT(35,0,0)
	gprintf("]]]]");
	lifes = 2;
	super_timeout = (timeout * 25) >>  GAME_ACCELERATION;
	ship_x = 0;
}

void game_over()
{
	for (int i = 0; i < 10; i++)
	{
		PRINT_AT(19,35,0);
		gprintf("GAME OVER");
		usleep(500000);
		PRINT_AT(19,35,0);
		gprintf("          ");
		usleep(500000);
	}
	init_game();
}

void destroy(unsigned char x, unsigned char y);

// Missile --------------------------------------------------------
struct missile{
	unsigned char xpos;
	unsigned char ypos;
	unsigned char altm = 0;
	unsigned char upwards = 1;
	unsigned char visible = 0;

	void init(unsigned char xpos_i, unsigned char ypos_i, unsigned char upwards_i)
	{
		if (visible) PRINT_AT(ypos, xpos, 32); //erase block

		xpos = xpos_i; ypos = ypos_i; upwards = upwards_i; altm = upwards_i; visible = 1;
	}


	void display()
	{
		if (visible)
		{
			altm = altm ? 0 : 1;
			char c;
			if (upwards xor (not altm)) // move position
			{
				PRINT_AT(ypos, xpos, ' ');
				c = READ_BUF(); // sure that it's done
				ypos = upwards ? ypos - 1 : ypos + 1;
				//visible = ((ypos < 36) and (ypos>5));
				//PRINT_AT(ypos, xpos, 1);
				//c = READ_BUF();
				c = 't' + altm;//READ_BUF(); // This returns the overwritten char
				PRINT_AT(ypos, xpos, c);
				//PRINT_AT(yposp, xpos+2, 'X');
				c = READ_BUF();
				//PRINT_AT(1,(35-ypos)*5,0);
				//gprintf("%", uint16_t(c));
				//gprintf("%", (c));
				if (c == 93) // shield
				{
					visible = 0;
					PRINT_AT(ypos, xpos, 93); // confirm shield

				}
				if (( (c > 93) and (c <= 'a')) or not ((ypos <= 36) and (ypos>1)) )//block
				{
					if (ypos < 35) // don't destroy the ground
					{
						if (c == 'a')
						{
							PRINT_AT(ypos, xpos, 95-upwards); //erase block
						}
						else
						{
							PRINT_AT(ypos, xpos, 32); //erase block
						}
					}
					else
						PRINT_AT(ypos, xpos, 'a'); //confirm ground

					visible = 0;
				}
				if ( (c > 'a') and (c < 105) ) //alien
				{
					destroy(xpos, ypos);
					visible = 0;
					if (c&1) // left part of alien
					{
						PRINT_AT(ypos,xpos-1,0);
					}
					else
					{
						PRINT_AT(ypos,xpos,0);
					}
					gprintf("lm");
					//gprintf("C=%", (c));
					score += (( (score&15) == 9) ? 7 : 1);
					PRINT_AT(0,76,0);
					gprintf("%", score);
				}
				if ((c > 'o') and (c<= 'r') ) //saucer
				{
					score += 32;
					PRINT_AT(0,76,0);
					gprintf("%", score);
					the_saucer.erase();
				}

				if ( (c >= 'x') and (c <= 'z') ) //ship hit
				{
					PRINT_AT(ypos,xpos-1,0);
					gprintf("lm ");
					if (lifes > 0)
					{
						lifes --;
						new_ship();
					}
					else
					{
						game_over();
						init_game();
					}
				}
			}
			else
			{
				c = 't' + altm;//READ_BUF(); // This returns the overwritten char
				PRINT_AT(ypos, xpos, c);
			}
		}

	}

	void erase()
	{
		PRINT_AT(ypos, xpos, 32); //erase block
		visible = 0;
	}
};

missile ship_missile;

struct alien_matrix
{
	alien_row matrix[5];
	missile alien_missiles[16];
	unsigned char first_alien = 0;
	unsigned char last_alien = 11;
	unsigned char last_row = 4;
	unsigned char start_line = 4;
	unsigned char start_col = 0;
	unsigned char line = 4;
	bool stop = 0;
	bool endline = 0;
	uint32_t mask = BOMB_MASK;
	uint16_t dead;
	
	void blank_line()
	{
		for (int i = 0; i < 79; i++)
			PRINT_AT(line, i, ' ');
		line++;	
	}

	void init()
	{
		 first_alien = 0;
		 last_alien = 11;
		last_row = 4;
		start_line = 4;
		start_col = 0;
		line = 4;
		stop = 0;
		for (int i = 0; i <= last_row; i++)
		{
			matrix[i].init();

		}
		timeout = 20000;
	}

	void display()
	{
		unsigned char col = start_col;
		if (not stop) 
		{
			line = start_line;
			dead = 0xffff;
			for (int i = 0; i <= last_row; i++)
			{
				col = start_col;
				blank_line(); // not optimal...
				unsigned char dead_row;
				uint16_t tmp = matrix[i].display(line, col, first_alien, last_alien, dead_row);
				dead = dead & tmp;
				if ((i == last_row) and (dead_row) )
					last_row --;
				//PRINT_AT(i, 30, 0);
				//gprintf("%", tmp);
			}
			//PRINT_AT(0, 40, 0);
			//gprintf("%", dead);
			if ( ( (col == 79) and (left_to_right == 1) )or ((start_col == 0) and ( left_to_right == -1) ) ) 
			{
				left_to_right = - left_to_right;
				if (start_line == 35)
					game_over();
				//start_col+=left_to_right;
				start_line ++;
				timeout = timeout - (timeout >> 4);
			}
			else 
			{
				start_col+=left_to_right;
			}

			if ( (dead >> (last_alien - first_alien)) & 1) // First column decimated
			{
				first_alien++; // narrow matrix
				start_col += 4; // otherwise they jump leftwards !
			}
			if (dead & 1) // Last column decimated
			{
				last_alien--; // narrow matrix
			}
			if (char(last_alien)< char(first_alien)) // win
			{
				PRINT_AT(19,35,0);
				gprintf("YOU WIN");
				usleep(1000000);

				PRINT_AT(19,35,0);
				gprintf("NEXT STAGE");
				usleep(1000000);
				init_game();
			}
		}		
		if ( (line == 36) and (col == 79)) // stop movement
		{
			stop = 1;
			game_over();
		}
		alt = (alt == 0) ? 2 : 0;
	}
	
	void (destroy(unsigned char x, unsigned char y)) // Look for the alien at this podition and destroy it
	{
		unsigned char idx_x = ((x - start_col)>>2) + first_alien;
		unsigned char idx_y = ((y - start_line)>>1);
		matrix[idx_y].row[idx_x].dead = 1;
		//PRINT_AT(2,10,0);
		//gprintf("killed % %", char(idx_x+48), char(idx_y+48));
	}

	void launch_missile(uint32_t rnd_i)
	{
		if ( (rnd_i & mask) == 0 )
		{
			int8_t new_missile = -1;
			for (int i = 0; i < 16; i++) // look for available missile
			{
				if (alien_missiles[i].visible == 0)
					new_missile = i;
			}

			if (new_missile > 0)
			{
				// random column
				uint8_t vcol = first_alien;
				uint8_t diff = (last_alien - first_alien) >> 1;
				for (int i = 0; i < 4; i++)
				{
					if ( (rnd_i >> i)&1)
						vcol = vcol + (diff);
					diff >>= 1;
				}

				if (vcol > last_alien)
					vcol = last_alien;

				int8_t idx = -1;

				while (idx < 0)
				{
					for (int k = 0; k <= last_row; k++)
					{
						if (not matrix[k].row[vcol].dead)
							idx = k;
					}
					if (idx < 0) //not found
					{
						if (vcol != last_alien) vcol++;
						else vcol = first_alien;
					}
				}
				unsigned char mcol = start_col + ( (vcol - first_alien) << 2) + 2 - (left_to_right == 1);
				unsigned char mline = start_line + (idx << 1) + 2;

				alien_missiles[new_missile].init(mcol, mline, 0);

			}
		}

	}

	void move_missiles()
	{
		for (int i = 0; i < 16; i++)
		{
			if (alien_missiles[i].visible)
				alien_missiles[i].display();
		}
	}

	void erase_missiles()
	{
		for (int i = 0; i < 16; i++)
		{
			alien_missiles[i].erase();
		}
	}


};

alien_matrix aliens;


void init_invaders()
{
	aliens.init();
}

	void destroy(unsigned char x, unsigned char y)
	{
		aliens.destroy(x, y);
	}

	void erase_missiles()
	{
		aliens.erase_missiles();
		ship_missile.erase();
	}

uint32_t rng()
{
	static uint32_t reg = 0xFAFAFAFA;
	uint32_t fb = ((reg << 30) ^ (reg << 26) ^ (reg << 25) ^(reg)) & 0x80000000;
	reg = (reg >> 1) | fb;
	return reg;
}



int main(int argc, char **argv) {
  // Any data written to the stack segment will connect the lowest four bits to
  // the board leds






 #if 1 
  	//*dbg = TOGGLE_BLINK;
	SET_CONSOLE(0);
   //while ( (*uart & 0x100) == 0); 
 	//*dbg = CLS;
 	//while ( (*uart & 0x100) == 0); 

	for (int i = 0; i < 12; i++)
 	{
		*dbg = FONT | (((93-32)*12+i) << 8) | 0xFF;
		*dbg = FONT | ((('a'-32)*12+i) << 8) | 0xFF;
		*dbg = FONT | (((95-32)*12+i) << 8) | hole[i];
		*dbg = FONT | (((94-32)*12+i) << 8) | hole[11-i];
		*dbg = FONT | ((('l'-32)*12+i) << 8) | (crash[i]);
		*dbg = FONT | ((('m'-32)*12+i) << 8) | BIT_REVERSE(crash[i]);
		*dbg = FONT | ((('o'-32)*12+i) << 8) | (saucer1[i]);
		*dbg = FONT | ((('p'-32)*12+i) << 8) | (saucer2[i]);
		*dbg = FONT | ((('q'-32)*12+i) << 8) | BIT_REVERSE(saucer2[i]);
		*dbg = FONT | ((('r'-32)*12+i) << 8) | BIT_REVERSE(saucer1[i]);

		*dbg = FONT | ((('x'-32)*12+i) << 8) | (ship[i]);
		*dbg = FONT | ((('y'-32)*12+i) << 8) | (shipm[i]);
		*dbg = FONT | ((('z'-32)*12+i) << 8) | BIT_REVERSE(ship[i]);
		*dbg = FONT | ((('b'-32)*12+i) << 8) | (alien1[i]);
		*dbg = FONT | ((('c'-32)*12+i) << 8) | BIT_REVERSE(alien1[i]);
		*dbg = FONT | ((('e'-32)*12+i) << 8) | BIT_REVERSE(alien1b[i]);
		*dbg = FONT | ((('d'-32)*12+i) << 8) | (alien1b[i]);
		*dbg = FONT | ((('t'-32)*12+i) << 8) | (missile1[i]);
		*dbg = FONT | ((('u'-32)*12+i) << 8) | (missile1b[i]);
		
		*dbg = FONT | ((('k'-32)*12+i) << 8) | (alien1b[i]); // bug on last write
	
	}
 	gprintf("TAGADA\n");
    *uart = 0x01A10100;   
    //*uart = 0x01A10100;   
// while ( (*uart & 0x100) == 0); 
 // gprintf("#VRead %Y from UART\n", uint32_t(*uart & 0xff));  
   	
    init_game();

 	while(1) //for (int k=0; k < 1000; k++)
 	{
		aliens.display();
		uint32_t rnd;// = rng();
		//PRINT_AT(1,1,0);
		//gprintf("%", uint16_t(rnd>>16));
		//gprintf("%", uint16_t(rnd));

		aliens.launch_missile(rnd);
		// Right numbers: i <25 and usleep 20000
		//for (int i = 0; i < (25 >> GAME_ACCELERATION); i++)
		the_saucer.init(rnd);
		uint32_t time = 0;
		while (time < super_timeout)
		{
			rnd = rng();
			//usleep(timeout);
			msleep(timeout);
			time += timeout;
			char c = 0;
			if ( *uart & 0x100)
			 c = static_cast<char>(*uart);
			if ((c == 's') and (ship_x > 0)) ship_x--;
			if ((c == 'd') and (ship_x < 76)) ship_x++;
			if ((c == ' ') and ( (ship_missile.visible == 0) or REPEAT_FIRE)) ship_missile.init(ship_x+2, 35,1);
			PRINT_AT(36, ship_x,0);
			gprintf(" xyz ");
			*uart = 0x01A10100;   
			ship_missile.display();
			aliens.move_missiles();
			the_saucer.display();
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
