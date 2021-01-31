# January'21 updates

* This risc-V SoC has been used for 6 months to test BTLE/802.15.4 IPs in C++.
* Verilog IPs can easily be compiled with verilator and be integrated to this C++ SoC. The cycle eval method provided by verilator is easily scheduled by the Agravic simulator.
The SoC has been running the verilated IPs and the ~500kB BTLE/MAC link layer firmware, which heavily runs on IRQ processing with WFI core stalling.
This FW is compiled using a regular risc-V toolchain, and: so far, so good.
Of course, these BTLE/MAC IPs are not provided here.
* FPGA operation is still demonstrated with the "Agravic invaders" game.
* What is new here is that the preferred development platform is the DE10-lite one. This implies that VGA output has been added to the project.
* On the DE10, the firmware is loaded using a USB-to-UART interface, physically wired to GPIOs 11 and 13 of the DE10 board.
On the 2x20 expansion connector, we have:
pin 14 -> uart_rx of SoC
pin 16 -> uart_tx of SoC
* To load the FW, put board switch 0 on (the screen turns blue). Then go to FIRMWARE folder and do 
    make PROGRAM=invaders 
    ./load_uart invaders.bin 
 Then put board switch 0 off, the FW starts.

# Agravic 

HW development platform in C++, NOT HLS but hardware-level C++ (VHDL generation via C preprocessor, HW simulation in C++).
Arduino Vidor target (with HDMI console output).
Risc-V small SoC (simple UART + DMA + HDMI console peripherals).
Video demo of the SoC running on the Arduino Vidor: https://youtu.be/4wAXtRFF0tc
(blurry video and awful speech inside ;)

https://www.youtube.com/watch?v=dMSiSrPnSbE
## Latest updates

Too many, I should have committed a long time ago.

### Platform improvements
* Most important change is that Agravic now implements an event-driven simulator (Include_libs/scheduler2.h)
* As a result, main.cpp and tb.h are way cleaner than before
* Non-synchronous clocks can be created using FOREVER_PROCESS (see tb.h)
* FOREVER_PROCESS can generate clocks. Though, it is a very simple piece of code that can't handle more complicated logic (e.g. conditions)
* Despite the event-driven scheme, true combinational logic is not possible yet.
* Though, the number of processes (synchronous or pseudo-combinational) per module is now 8 instead of 4.
* FOREVER_PROCESSES create base clocks. Derived clocks can be created in regular processes (see Source/clk_gen.h)
* Every <= operation on a clock signal creates 3 delta cycles (compute flop inputs, assign flop outputs, update combinational logic)
* Delta cycles can be converted to ps delays in vcd output (define REPORT_DELTAS_AS_PS in scheduler2.h)
* Arrays of arrays are now possible
* Flops outputs update is only performed over modified flop inputs (improves simulation time)

* One generic parameter per block is available. This is not enough, but this enables generic configuration of register base address and address span. 
* Clean-up started in Include_libs/slv.h (C macros and arrays are now separate files)

### Risc-V SoC improvements
* Some bugs corrected (rv core, dma...)
* CSR are now implemented as a memory device. CSR instructions generate memory accesses. CSR tests now pass!
* Timer and IRQs are implemented inside the csr_irq block. However, this feature is not fully implemented (HW and FW).
* Thanks to flop-update only performed on modified flops, memory models are now common to C++ model and VHDL implementations.
* Memory size is a generic parameter. This enables defining different instruction and data memory sizes.
* Cleaner register configuration using block generic parameter. Register configuration is shared by HW and FW (Source.reg_defs.h).

## Agravic has reached some level of maturity
The Risc-V tiny SoC HW can now be downloaded to the Arduino Vidor FPGA development platform and is running properly so far.
HDMI console mode (with programmable fonts) available via the micro-HDMI connector
The test firmware runs on the risc-V core on the cyclone-10 FPGA.
The HW HDMI console offers some means of SW and HW debug. 
"agravic invaders" is a space invaders basic clone FW, fully written in bare-metal C++, and is now playable.

### To build the HW
* A linux system is preferred
* You need a native g++ compiler, verions 7.4 to 8
* You need to have quartus installed
* You need the Arduino IDE
* Download and install quartus prime lite (this is free from Intel website. This requires creating an account) 
* In agravic folder, run make. This builds the linux executable and VHDL files as well
* Go to the RTL/Source folder. You should find the quartus project (top.qpf)
* Open top.qpf in quartus and run compilation flow (hit start compilation button)
    After a few minutes (depending on HW configuration), compilation should complete with generation of a top.ttf file
* Go to the ARDUINO/Agravic folder
* Install the GO language package (this is just for the Arduino ttf->.h converter. This is Arduino's choice, I don't approve using an extra language to generate this kind of files: sudo apt-get install golang)
* Generate .h files for the arduino project :
    go run make_composite_binary.go  -i ../../RTL/Source/top.ttf:1:512 -o app.h -t 1 > signature.h   
* In the same directory, open Agravic.ino in Arduino IDE
* Make sure you have the Arduino libraries for the Arduino mkr4000 Vidor board. If not, run the Arduino IDE board manager and download them. 
* When done, select the Arduino mkr4000 vidor board, /dev/ttyACM0 port (or whatever), and programmer as USBAsp. Then, upload the code.
* You should see the GIORNO CORE prompt out of the Vidor HDMI interface (requires a micro HDMI -> HDMI cable)

### To build and load the risc-V firmware
* Go to the FIRMWARE folder
* Edit the Makefile to set the actual path of your risc-V toolchain
* Type make
* The led.bin firware binary is available
* Alternatively, build the invader example. Type:
* make -f Makefile_invaders
* Install putty (sudo apt-get install putty on ubuntu platforms)
* Open putty. Configure putty for a Serial, 115200 bauds session using /dev/ttyACM0 device (the Vidor board might show up as the /dev/ttyACM1 or /dev/ttyUSB* device). Opening putty is sometimes required for proper serial configuration.
* The FW is loaded via the USB serial port of the Arduino Vidor (the arduino FW mirrors it to the HW UART of the Risc-V SoC)
* from the FIRMWARE directory:
* use the load script to download the binary file: 
  load invaders.bin
* The HDMI console screen should (shortly) turn blue during FW upload
* If the firmware does not load properly, the risc-V core might go into trap. The HDMI console then shows the execution trace, and might even turn red. Reset the board, then check that putty is open and set properly.
* Code can be reloaded without resetting the Vidor board. However, I am aware that there might be some glitches (the HW is not properly resetted after code loading)
* To play agravic invaders, bring the putty window to the front. Use the 's' and 'd' keys to move your ship, space bar to fire.

## What is Agravic ?
Agravic is a framework that enables writing VHDL-like code inside a C++ program.
It uses heavy preprocessor abuse for that, but the counterpart is that synthesizable VHDL code can be preprocessor-generated from the C++ code!

The syntax has been chosen to enable VHDL and C++ code generation using the C preprocessor and sed. At first, it might seem confusing for both C++ developers and rtl designers, but in the end, the syntax is so close to VHDL's that I feel this is not a problem.

**So, agravic enables rtl code writing without the expense of EDA tools.**
Furthermore, C++ simulation of agravic designs should be faster than classical rtl simulation. 
But the platform is not optimized yet, and it is sure that designs with low flip-flop activity are slower than necessary.
This is why memory models are not the same for simulation as for VHDL generation (altera memory inferring requires describing memories as wide arrays of flip-flops, with inherently low toggling rate). 

### Limitations
* Unlike mentor ac_int types, arbitrary length types in Agravic only cover the 1-64 bits range. This is generally enough for most designs and enables using native int64 types as a base type.
* Agravic does not make a true difference between bit-types and vector bit-types with size 1. So, some designs might compile in C++, but not the auto-generated VHDL files.
* True combinational logic does not exist in Agravic. Traditional uses of combinational logic can be achieved this way:
    - Use in-process variables for intermediate calculations
    - Be careful of the non-retained nature of agravic variables. VHDL variables are retained, this can lead to a different behavior betwwen simulation and generated VHDL.
    - Use clock-driven combinational processes for output assignments
    - The risc-V SoC example shows how to do this
* For now, a single clock signal is supported. Though, gated clocks can be created.
=> The SoC runs on three clocks, all derived from the master clock (240MHz) using a gating clock technique in simulation. The HW makes use of true clocks (24MHz pixel clk, 48MHz core clk, and 120MHz HDMI clk, all derived from the master 48MHz clk of the Arduino Vidor)

## Requirements
* A linux distribution, with a recent gcc ( > 7.4 ).
* Ubuntu on windows, cygwin might do the job as well.
* Gtkwave is highly recommended for waveform viewing.

## Provided example (small risc-V SoC based on Giorno core)
A risc-V SoC example is provided to demonstrate the platform capabilities, for simulation and rtl generation as well.

The SoC is composed of the Giorno risc-V core (custom design based on rv32i instruction set), two 24kB RAMs (instruction and data), a peripheral block (used as printf port for now), a very simple UART interface (half duplex), DMA controller (for the UART only) and an HDMI console.
An SDRAM controller is provided but is not functional yet, its projected use is for HDMI bitmap mode.

* The Giorno core does not support interruptions and timers yet. 
* However, building binaries using a standard bare-metal gcc toolchain seems to be working so far.

**93 verification tests are provided** (the rv32i subset of the risc-V compliance tests), which all pass except of the CSR, FENCE and MISALIGN ones.

Running the non-regression suite is statisfyingly fast: it obviously deserves being run every time you change a single line of the rtl code. 

A simple bare-metal gcc C++ code can be found in the FIRMWARE directory.

The generated rtl synthesizes in quartus, and should run at 48MHz on an Arduino Vidor board (using 27% of the Cyclone10LP Logic elements).

## Directory organization

* Main.cpp and all rtl blocks (.h files) are found in the Source directory.

* Include_libs contains the Agravic framework.

* Include_rtl contains the C++ compilable versions of the rtl blocks found in Source. These are generated by the Makefile.

* RTL contains the VHDL versions of the rtl blocks found in Source. These are generated by the Makefile. You'll find the quartus project as well.

* Tests contains the non-regression tests (binaries and references). 

* FIRMWARE contains the c++ sample code and utilities for firmware development. 

* ARDUINO/Agravic contains the arduino project that loads the SoC FPGA image and the firmware binary to the risc-V core.

* ARDUINO/Agravic contains the arduino 'go' code that generates the *.h file that contains the FPGA image.


## Run the risc-V SoC example1
### Generate executable and rtl:

    make
### Generate non-regression executable and non-regression executable:
    make nonreg

The non-regression executable differs from the previous one by its ability to abort at the end of the test and check the results (it compares memory content to the reference one, which is expected by the risc-V compliance tests).
### Build the bare-metal C++ example:

The C++ example shows the features that seem to work on this platform:

* C++ classes and methods
* C++ templates (and recursive ones as well)
* A C++ printf clone that enables color printing on terminals from inside the simulation platform
* Bit field structures manipulation -hardware configuration API will be based on bit field structures in the future- 
* Global and static variables initialization, function calls...
* Floating point operations and displaying
* UART transfers
* DMA UART transfers

To build the example:

    install the risc-V gcc toolchain (riscv-gnu-toolchain on github, compile the newlib toolchain)
    cd FIRMWARE
    !! change risc-V compiler path in the Makefile according to your settings (default is /opt/riscv... )
    make (-f Makefile_invaders for the invaders demo)
    cd ..
To run the example (simulation):

    make
    ./dut -ncycles 200000 -bin_file 'FIRMWARE/led.bin'   
        
### Select internal signals to be probed in dut.vcd:
edit vcd.scn file.

A single signal is probed by invoking its full hierarchy:

    tb:dut:u0_peripherals:gpios_o  
       
Adding bin to signal name probes it in binary format:

    tb:dut:u0_peripherals:gpios_o bin
    
Adding all signals inside a hierarchy level:

    tb:dut:u0_peripherals:  
    
**Once a probe is declared, its format is definitive. It also conditions the format of all connected nets (ports etc.). So, all 'bin' probes must be declared first in vcd.scn.**     
### Run the SoC
    make
    
    ./dut -ncycles 200000 -bin_file 'FIRMWARE/led.bin'   

    gtkwave dut.vcd

Then navigate inside the hierarchy to watch internal signals.
This example toggles the GPIO values every 1ms (see gpios signal in the peripherals block).
### Have a look to printf outputs
gprintf is a simple printf implementation in C++ language.

It outputs text using the gpio output, which is hacked in peripherals.h to produce console output.

It makes use of recursive templates to implement multiple arguments printf, so any gprintf with a new set of argument types has a cost in terms of program memory.

Since argument types does not need to be specified, the formatting argument is used to specify the output color.

Basic print colors are black, blue, magenta, cyan, red, green and yellow, respecting the color code of matlab (k, b, m, c, r, g, and y)
For bold print, color codes must be specified in capital letters (K, B, M etc.)
Some print codes with background color are T, U, V and W. These are an arbitrary choice of FG/BG colors.

The base color of any gprintf is specified at the beginning of the formatting string, beginning with '#':

    gprintf("#R Yo! %Y and be %G", "man", "cool");
will print  the text in the formatting string with default color R (bold red) and the aguments ("man" and "cool") in bold yellow color.
For now, only uint32, int32, char and bool arguments are supported.

Check FIRMWARE/led.c or FIRMWARE/invaders.cpp for examples.

To see gprintf outputs, just launch:

	tail -f dbg_file !!in a new terminal!!

where the executable is launched.

gprintf outputs show up in the terminal as you launch simulations.
gprintf ouputs show up in the HDMI console as well, but anything but chars is ignored (colors and other types).
This explains the slightly corrupt output seens on the HDMI console.

### Run the non-regression tests:
    make nonreg
    ./nonreg
    more test_results
The test_results file only contains the list of failed tests.
### Run any of the non-regression tests (for example xor instruction test):
    ./dut -ncycles 10000 -bin_file 'tests/xor.elf.bin'

