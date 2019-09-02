# Agravic platform makefile
# Creates C++ executable and VHDL files
SOURCEDIR = ./Source/
RTL_SRC = ./RTL/Source
DINCS = ./Include_libs 
INCS_conv = ./Include_rtl/Source


INCLUDES = -I./Include_libs -I./Include_rtl/Source
INCLUDES_VHD = -I./Include_libs -I./Source

CC_FLAGS = -O3 -std=gnu++17

INCS = $(wildcard $(DINCS)*.h)

SRCS = $(wildcard $(SOURCEDIR)*.cpp)
RTL = $(wildcard $(SOURCEDIR)*.h)
RTL_SOURCES = $(wildcard $(RTL_SRC)*)
RTL_CONV = $(patsubst %.h, %.hpp, $(RTL))
RTL_VHDL = $(patsubst %.h, %.vhd, $(RTL))



All: dut

# Creates executable and generates VHDL files
dut:  $(RTL_VHDL) $(RTL_CONV) $(SOURCEDIR)main.cpp  
	@echo $(RTL)
	@echo $(RTL_CONV)
	sed -e 's/:=/=/g' Source/main.cpp | g++ $(CC_FLAGS) $(INCLUDES) -o dut -g3 -xc++ - 

# Shows preprocessor output for macro debug
show:  $(RTL_VHDL) $(RTL_CONV) $(SOURCEDIR)main.cpp  
	@echo $(RTL)
	@echo $(RTL_CONV)
	sed -e 's/:=/=/g' Source/main.cpp | g++ $(CC_FLAGS) $(INCLUDES) -P -E -g3 -xc++ - 


# Fake rule for hpp, indeed replaces := with = and puts it in Include_rtl dir.
%.hpp	: %.h 
	sed -e ''s/:=/=/g'' $< > ./Include_rtl/$<

# Rule for vhdl code generation (in RTL/Source dir.)
# Gets preprocessor output with -DVHDL option, replaces == with =, Then keeps 100000 lines after the Agravic keyword is found (start of VHDL block)
%.vhd	: %.h 
	g++ $(CC_FLAGS) $(INCLUDES) -E -P -IInclude_libs -DVHDL $< | sed -e 's/==/=/g' -e 's/!=/~=/g' | grep -A 100000 -e 'Agravic' > ./RTL/$@

clean:
	rm -f -r  $(RTL_SOURCES)/* $(INCS_conv)/* dut; 
