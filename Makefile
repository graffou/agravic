# Agravic platform makefile
# Creates C++ executable and VHDL files
SOURCEDIR = ./Source/
RTL_SRC = ./RTL/Source
DINCS = ./Include_libs 
RINCS = ./Include/
INCS_conv = ./Include_rtl/Source


INCLUDES_C = -I./Include_libs -I./Include_rtl/Source -I./Include_rtl/Include
INCLUDES_VHD = -I./Include_libs -I./Source -I./Include

# -Wfatal-errors should stop gcc on 1st error
CC_FLAGS = -O3 -std=gnu++17 -Wfatal-errors


INCS = $(wildcard $(DINCS)*.h)

SRCS = $(wildcard $(SOURCEDIR)*.cpp)
RTL = $(wildcard $(SOURCEDIR)*.h)
NOT_RTL = $(wildcard $(RINCS)*.h)
RTL_SOURCES = $(wildcard $(RTL_SRC)*)
RTL_CONV = $(patsubst %.h, %.hpp, $(RTL))
CONV = $(patsubst %.h, %.hpp, $(NOT_RTL))
RTL_VHDL = $(patsubst %.h, %.vhd, $(RTL))



All: dut

regs: genregs.cpp
	g++ -IInclude_libs genregs.cpp -o genregs

# Creates executable 
exe:  $(RTL_CONV) $(CONV) $(SOURCEDIR)main.cpp  
	sed -e 's/:=/=/g' Source/main.cpp | g++ $(CC_FLAGS) $(INCLUDES_C) -o dut -g3 -xc++ - 
	
# Creates executable for risc-V test suite
nonreg:  $(RTL_CONV) $(CONV) $(SOURCEDIR)main.cpp  
	sed -e 's/:=/=/g' Source/main.cpp | g++ $(CC_FLAGS) $(INCLUDES_C) -DNONREG -o dut -g3 -xc++ - 
	g++ -IInclude_libs nonreg.cpp -o nonreg
# Creates executable and generates VHDL files:;!/
dut:  $(RTL_VHDL) $(RTL_CONV) $(CONV) $(SOURCEDIR)main.cpp  
	sed -e 's/:=/=/g' Source/main.cpp | g++ -fdiagnostics-show-option $(CC_FLAGS) $(INCLUDES_C) -o dut -g3 -xc++ - 

# Shows preprocessor output for macro debug
show:  $(RTL_VHDL) $(RTL_CONV)  $(CONV) $(SOURCEDIR)main.cpp  
	sed -e 's/:=/=/g' Source/main.cpp | g++ $(CC_FLAGS) $(INCLUDES_C) -P -E -g3 -xc++ - 


# Fake rule for hpp, indeed replaces := with = and puts it in Include_rtl dir.
%.hpp	: %.h 
	sed -e ''s/:=/=/g'' $< > ./Include_rtl/$<

# Rule for vhdl code generation (in RTL/Source dir.)
# Gets preprocessor output with -DVHDL option, replaces == with =, Then keeps 100000 lines after the Agravic keyword is found (start of VHDL block)
%.vhd	: %.h 
	g++ $(CC_FLAGS) $(INCLUDES_VHD) -I$(RINCS) -E -P -w -IInclude_libs -DVHDL $< | sed -e 's/==/=/g' -e 's/!=/~=/g' | grep -A 100000 -e 'Agravic' > ./RTL/$@

clean:
	rm -f -r  $(RTL_SOURCES)/* $(INCS_conv)/* dut; 
