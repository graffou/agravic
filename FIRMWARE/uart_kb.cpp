
#if 1


#include <linux/input.h>
#include <termios.h>

#include <SDL2/SDL.h>
//#include "mouse.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "unistd.h"

  #include "unistd.h"
 #include "linux/kd.h"
 #include "termios.h"
 #include "fcntl.h"
 #include "sys/ioctl.h"

 static struct termios tty_attr_old, new_settings;
 static int old_keyboard_mode;

 int setupKeyboard()
 {
     struct termios tty_attr;
     int flags;

     /* make stdin non-blocking */
     flags = fcntl(0, F_GETFL);
     flags |= O_NONBLOCK;
     fcntl(0, F_SETFL, flags);

     /* save old keyboard mode */
     if (ioctl(0, KDGKBMODE, &old_keyboard_mode) < 0) {
	 return 0;
     }

     tcgetattr(0, &tty_attr_old);

     /* turn off buffering, echo and key processing */
     tty_attr = tty_attr_old;
     tty_attr.c_lflag &= ~(ICANON | ECHO | ISIG);
     tty_attr.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF);
     tcsetattr(0, TCSANOW, &tty_attr);

     ioctl(0, KDSKBMODE, K_RAW);
     return 1;
 } 
void restoreKeyboard()
 {
     tcsetattr(0, TCSAFLUSH, &tty_attr_old);
     ioctl(0, KDSKBMODE, old_keyboard_mode);
 } 
//std::ofstream serial_port;// = open("/dev/ttyACM0", O_RDWR);
int serial_port = open("/dev/ttyACM0", O_RDWR);


 void readKeyboard()
 {
     unsigned char buf[1];
     int res;
// new_settings.c_cc[VMIN]=0;
//  tcsetattr(0, TCSANOW, &new_settings);
  //nread = read(0,&ch,1);
      res = read(0, &buf[0], 1);
// new_settings.c_cc[VMIN]=1;
//  tcsetattr(0, TCSANOW, &new_settings);
     /* read scan code from stdin */

     /* keep reading til there's no more*/
     while (res >= 0) {
	 switch (buf[0]) {
	 case 0x01:
             /* escape was pressed */
             break;
         case 0x81:
             /* escape was released */
             break;
	default:
	if (buf[0])
	{
		std::cerr << buf[0];
		serial_port << buf[0];
	}
         /* process more scan code possibilities here!  */
	 }
	 res = read(0, &buf[0], 1);
     }
 } 

int main(){
//serial_port.open("/dev/ttyACM0");
// Create new termios struc, we call it 'tty' for convention
struct termios tty;
memset(&tty, 0, sizeof tty);

tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
tty.c_cflag |= CS8; // 8 bits per byte (most common)
tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

tty.c_lflag &= ~ICANON;
tty.c_lflag &= ~ECHO; // Disable echo
tty.c_lflag &= ~ECHOE; // Disable erasure
tty.c_lflag &= ~ECHONL; // Disable new-line echo
tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
tty.c_cc[VMIN] = 0;
// Read in existing settings, and handle any error
if(tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
}
cfsetispeed(&tty, B115200);

if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
exit(0);
}
// Write to serial port
unsigned char msg[] = { 's', 's', 's', 's', 's', '\r' };
write(serial_port, "Hello, world!", sizeof(msg));

if(not setupKeyboard())
std::cerr << "FAILED KB setup\n";
while(1)
{
 readKeyboard();
}



}
















#elif




void keyboard_handler(SDL_Event * event);

static uint32_t last_key;
int main()
{

while(1){
	    SDL_Event event;
	    while(SDL_PollEvent(&event)) {


            //mouse_handler(&event);
            keyboard_handler(&event);
	if (last_key)
	std::cerr << char(last_key);
	    }

}



}

void keyboard_handler(SDL_Event * event)
{
    /* We only care about SDL_KEYDOWN and SDL_KEYUP events */
    switch(event->type) {
        case SDL_KEYDOWN:                       /*Button press*/
            last_key = event->key.keysym.sym;   /*Save the pressed key*/

            break;
        case SDL_KEYUP:                         /*Button release*/

            last_key = 0;    
	break;
        default:
            break;

    }
}
/*
void mouse_handler(SDL_Event *event)
{
	switch (event->type) {
        case SDL_MOUSEBUTTONUP:
            if (event->button.button == SDL_BUTTON_LEFT)
                left_button_down = false;
             //gprintf("@k<%g|%g>", last_x, last_y);

            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                left_button_down = true;
                last_x = event->motion.x / MAGNIFY;
                last_y = event->motion.y / MAGNIFY;
              //gprintf("@k<%m|%m>", last_x, last_y);
			//std::cerr << "< BD " << last_x << "," << last_y << ">";

            }
            break;
        case SDL_MOUSEMOTION:
            last_x = event->motion.x / MAGNIFY;
            last_y = event->motion.y / MAGNIFY;
			//td::cerr << "<" << last_x << "," << last_y << ">";
            //gprintf("@k<%r|%r>", last_x, last_y);
            break;
        case SDL_FINGERUP:
            if (1)
                left_button_down = false;
             //gprintf("@k<%g|%g>", last_x, last_y);
			//std::cerr << "< FU " << last_x << "," << last_y << ">";

            break;
            
        case SDL_FINGERDOWN:
            if (1) {
                left_button_down = true;
                last_x = event->tfinger.x / MAGNIFY;
                last_y = event->tfinger.y / MAGNIFY;
              //gprintf("@k<%m|%m>", last_x, last_y);
			//std::cerr << "< FD  " << last_x << "," << last_y << ">";

            }
            break;
        case SDL_FINGERMOTION:
            last_x = event->tfinger.x / MAGNIFY;
            last_y = event->tfinger.y / MAGNIFY;
			//std::cerr << "< FM  " << last_x << "," << last_y << ">";
            //gprintf("@k<%r|%r>", last_x, last_y);
            break;
    }


}
*/











#else



#include <iostream>
#include <stdio.h>     /* include for kbhit() and getch() functions */
#include <stdlib.h>



#include <termios.h>
#include <unistd.h>   // for read()


static struct termios initial_settings, new_settings;
static int peek_character = -1;

int readch()
{
  char ch;

  if(peek_character != -1)
    {
      ch = peek_character;
      peek_character = -1;
      return ch;
    }
  read(0,&ch,1);
  return ch;
}

void init_keyboard()
{
  tcgetattr(0,&initial_settings);
  new_settings = initial_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;
#ifndef rtl_433
  new_settings.c_lflag &= ~ISIG;
#endif
  new_settings.c_cc[VMIN] = 1;
  new_settings.c_cc[VTIME] = 0;
  tcsetattr(0, TCSANOW, &new_settings);
}

void restore_keyboard()
{
  tcsetattr(0, TCSANOW, &initial_settings);
}

int kbhit()
{
  unsigned char ch;
  int nread;

  if (peek_character != -1) return 1;
  new_settings.c_cc[VMIN]=0;
  tcsetattr(0, TCSANOW, &new_settings);
  nread = read(0,&ch,1);
  new_settings.c_cc[VMIN]=1;
  tcsetattr(0, TCSANOW, &new_settings);
  if(nread == 1)
    {
      peek_character = ch;
      return 1;
    }
  return 0;
}


int main()
{
	char c;	
	while (1)
	{
		if (c=readch())
		{
			std::cerr << c;
		}
	}
		
}
#endif
