# agravic 
# agravic make generates the executable file and the rtl files
# risc-V core is not tested at all!!!!!
make
# launch executable, for example lw instruction test
./dut -bin_file 'tests/lw.elf.bin'
# Use gtkwave to watch waves
sudo apt-get install gtkwave
gtkwave dut.vcd

# See rtl files generated by agravic
ls RTL/Source/

