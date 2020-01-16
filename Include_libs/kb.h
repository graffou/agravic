#include <stdio.h>     /* include for kbhit() and getch() functions */
#include <termios.h>
#include <unistd.h>   // for read()

void terminal_lnbuf(int yn) {
   struct termios oldt, newt;
   tcgetattr(0, &oldt);
   newt = oldt;
   if (!yn) newt.c_lflag &= ~ICANON;
   else newt.c_lflag |= ICANON;
   tcsetattr(0, TCSANOW, &newt);
}

void terminal_echo(int yn) {
   struct termios oldt, newt;
   tcgetattr(0, &oldt);
   newt = oldt;
   if (!yn) newt.c_lflag &= ~ECHO;
   else newt.c_lflag |= ECHO;
   tcsetattr(0, TCSANOW, &newt);
}

void gotoxy(int x, int y) { printf("\x1B[%d;%df", y, x); }

void clrscr() { printf("\x1B[2J\x1B[0;0f"); }

int readch() {
    int ch;
   terminal_lnbuf(0);
   terminal_echo(0);
   ch = getchar();
   terminal_lnbuf(1);
   terminal_echo(1);
   return ch;
}

int getche() {
    int ch;
   terminal_lnbuf(0);
   ch = getchar();
   terminal_lnbuf(1);
   return ch;
}

int kbhit() {
    int ret;
   fd_set fds;
   terminal_lnbuf(0);
   terminal_echo(0);
   struct timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 0;
   FD_ZERO(&fds);
   FD_SET(0, &fds);
   select(1, &fds, 0, 0, &tv);
   ret = FD_ISSET(0, &fds);
   terminal_lnbuf(1);
   terminal_echo(1);
   return ret;
}

void init_vcd()
{
	bool in_port = 0;
	gmodule* pmodule; // current module
	gmodule* pdrv_module; // module of driver of current entry
	int vcd_drv_idx = 0;



	gprintf("#VInit VCD: Copying arrays or records drivers ------------------------------------------------");
	// First, find and copy drivers of members of array ports and records (whole module hierarchy)
	for (int i = 0; i < gmodule::module_list.size(); i++)
	{
		for (int j = 0; j < gmodule::module_list[i]->vcd_list.size(); j++)
		{
			gprintf("#Mptr %", gmodule::module_list[i]->vcd_list[j]);
			bool is_port = ( gmodule::module_list[i]->vcd_list[j]->name[0] == '>' or gmodule::module_list[i]->vcd_list[j]->name[0] == '<' );
			bool is_self_driven = ( gmodule::module_list[i]->vcd_list[j]->driver == gmodule::module_list[i]->vcd_list[j] );
			// If no mistake, only top entries of arrays/records have these nbits values
			// arrays of records or record containing an array should have -65536 - 2048 etc.
			// Okay, 32 nested records have nbits = -65536 as well...
			bool is_array_entry = (gmodule::module_list[i]->vcd_list[j]->nbits == -65536);
			bool is_record_entry = (gmodule::module_list[i]->vcd_list[j]->nbits == -2048);
			bool is_regular_entry = (gmodule::module_list[i]->vcd_list[j]->nbits >= 0);
			gprintf("#G-->entry %m port %R self %R array %R record %R regular %R nbits %R drv name %M",gmodule::module_list[i]->vcd_list[j]->name,
					is_port,is_self_driven,is_array_entry,is_record_entry,is_regular_entry,gmodule::module_list[i]->vcd_list[j]->nbits, gmodule::module_list[i]->vcd_list[j]->driver->name);

			if (is_port and (is_array_entry or is_record_entry) )
			{
				if (in_port) gprintf("#UEND COPY1");
				if (not is_self_driven)
				{
					//start reporting drivers and copying IDs of array / record members
					// (the following entries are obviously those of these members)
					// Get current module and module of driver
					pmodule = gmodule::module_list[i];
					pdrv_module = gmodule::module_list[i]->vcd_list[j]->driver->pmodule;
					// Find current entry in vcd list of cur. module, find driver entry in vcd list of drv. module
					vcd_drv_idx = gmodule::module_list[i]->vcd_list[j]->driver->find_entry(gmodule::module_list[i]->vcd_list[j]->driver->name, pdrv_module);
					if (vcd_drv_idx == -1) // not found !
					{
						gprintf("#RERROR! driver of array/record % not found", gmodule::module_list[i]->vcd_list[j]->name);
					}
					in_port = 1;
					gprintf("#Ucopying array or record drivers % drv %", gmodule::module_list[i]->vcd_list[j]->driver->name, pdrv_module);

				}
				else
				{
					in_port = 0; //PG: forgot that! Why do I have a self driven record port? (s0_sUART, uart_dma_i)
				}
			}
			else if ( (not is_port) or (is_regular_entry) ) // current array / record finished
			{
				if (in_port)
					gprintf("#UEND COPY2");
				in_port = 0;
			}
			else if ( in_port )// We are actually in an array/record port not self driven
			{
				vcd_drv_idx++; //
				if (vcd_drv_idx >= pdrv_module->vcd_list.size()) // Overflow, this should not occur so there is a bug
				{
					gprintf("#RERROR! OVF in vcd list of driver signal when reporting drivers idx % sz % here % there %", vcd_drv_idx, pdrv_module->vcd_list.size(), gmodule::module_list[i]->name, pdrv_module->name); exit(0);
				}
				gmodule::module_list[i]->vcd_list[j]->driver = pdrv_module->vcd_list[vcd_drv_idx];
			}
		}
	}

	gprintf("#VInit VCD: reading vcd.scn configuration file ------------------------------------------------");
	std::ifstream conf_vcd("vcd.scn");
	char line[256];
	bool comment = 0;
	while (not conf_vcd.eof())
	{
		conf_vcd.getline(line, 256);
		if (line[0] != '#')
		{
			if (not comment)
			{
				if ( (line[0] == '/') and (line[1] == '*') )
				{
					comment = 1;
				}
				else
					activate_vcd(line);
			}
			else if ( (line[0] == '*') and (line[1] == '/') )
			{
				comment = 0;
			}
		}
	}

	// Set nbits=0 for non-binary entries
	//clean_vcd();
}

void kb()
{
	if (kbhit())
	{
		char c = readch();
		while (c != 'k')
		{
			if (c == 'q')
			{
				//estore_keyboard();
				exit(0);
			}
			else if (c == '*')
			{

			}

			while (not kbhit()){}

			c  = readch();

		}
		std::cerr << "exit kb loop\n";
	}

}
