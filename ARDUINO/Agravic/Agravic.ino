//#include <arduino.h>
#include <SPI.h>
#include "jtag.h"

// For High level functions such as pinMode or digitalWrite, you have to use FPGA_xxx
// Low level functions (in jtag.c file) use other kind of #define (TDI,TDO,TCK,TMS) with different values
#define FPGA_TDI                            (26u)
#define FPGA_TDO                            (29u)
#define FPGA_TCK                            (27u)
#define FPGA_TMS                            (28u)

// Clock send by SAMD21 to the FPGA
#define FPGA_CLOCK                        (30u)

// SAMD21 to FPGA control signal (interrupt ?)
#define FPGA_MB_INT                       (31u)

// FPGA to SAMD21 control signal (interrupt ?)
#define FPGA_INT                          (33u) //B2 N2

// set Giorno boot mode
#define GIORNO_BOOT_MODE                          (7u) //B2 N2

// For MKR pinout assignments see : https://systemes-embarques.fr/wp/brochage-connecteur-mkr-vidor-4000/

extern void enableFpgaClock(void);

#define no_data    0xFF, 0xFF, 0xFF, 0xFF, \
          0xFF, 0xFF, 0xFF, 0xFF, \
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
          0xFF, 0xFF, 0xFF, 0xFF, \
          0x00, 0x00, 0x00, 0x00  \

#define NO_BOOTLOADER   no_data
#define NO_APP        no_data
#define NO_USER_DATA    no_data

__attribute__ ((used, section(".fpga_bitstream_signature")))
const unsigned char signatures[4096] = {
  #include "signature.h"
};
__attribute__ ((used, section(".fpga_bitstream")))
const unsigned char bitstream[] = {
  #include "app.h"
};


// the setup function runs once when you press reset or power the board
void setup() {

  int ret;
  uint32_t ptr[1];

  enableFpgaClock();

  //Init Jtag Port
  ret = jtagInit();
  mbPinSet();

  // Load FPGA user configuration
  ptr[0] = 0 | 3;
  mbEveSend(ptr, 1);

  // Give it delay
  delay(1000);

  // Configure onboard LED Pin as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Disable all JTAG Pins (usefull for USB BLASTER connection)
  pinMode(FPGA_TDO, INPUT);
  pinMode(FPGA_TMS, INPUT);
  pinMode(FPGA_TDI, INPUT);
  pinMode(FPGA_TCK, INPUT);

  // Configure other share pins as input too
  pinMode(FPGA_INT, INPUT);

  pinMode(GIORNO_BOOT_MODE, OUTPUT);
  digitalWrite(GIORNO_BOOT_MODE, LOW);
  
  Serial1.begin(115200);  // Baudrate is hardware code in FPGA.
  Serial.begin(115200);  // Baudrate is hardware code in FPGA.

}

bool toggle = 0;
bool load = 0;
bool preload = 0;
char c, cp;
int bcnt = 0;
uint32_t cnt;
uint32_t byte_cnt;
// the loop function runs over and over again forever
void loop() {

  if (load)
  {
      Serial1.write(c);
      /*
     Serial.print(int(bcnt), HEX);
     Serial.print(" ");
    
     Serial.print(int(c), HEX);
     Serial.print("\n");
     */
     //delay(1);
      while (not Serial.available() )
      {
        
        cnt++;
        if (cnt > 0x10000)
        {
          load = 0;
          preload = 0;
          delay(10);
          digitalWrite(GIORNO_BOOT_MODE, LOW);
          break;
        }
        
      }
      if (load)
        c = Serial.read();
      cnt = 0;

  }
  else if (Serial.available()) 
  {      // If anything comes in Serial (USB),
    cnt = 0;
    bcnt = 0;
    c = Serial.read();
#if 0    // rough method: if text goes through uart, this will fail (0x6F is 'o')
    if ((c == 0x13) || (c == 0x6F) )// old and new first byte // must secure that, 6F is 'o'
    {
      load = 1;
      digitalWrite(GIORNO_BOOT_MODE, HIGH);
      delay(1);
    }
    else
          Serial1.write(c); //post load
#else // refined method: catch 13 or 6F, then confirm with following value 0 to confirm it's code
  // When it's code, it starts with 13 00 (old crappy crt0) or 6F 00 (clean one)
    if ( preload and (c == 0) )// secured with preload
    {
      load = 1;
      digitalWrite(GIORNO_BOOT_MODE, HIGH);
      delay(1);
      // no, this has been fifoed Serial1.write(cp); //resend leftover code byte, with load mode activated 
    }
    else
    {
          preload = ( (c == 0x13 ) or (c == 0x6F) );// old and new first byte of code data
          Serial1.write(c); //post load
          cp = c; // store in case this is code
    }

#endif
    //Serial.println(int(c), HEX);
    //Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }
  
  /*
  if (toggle)
    digitalWrite(GIORNO_BOOT_MODE, HIGH);
  else
    digitalWrite(GIORNO_BOOT_MODE, LOW);
  

  delay(5000);
  toggle = ! toggle;
  */
                     
}
