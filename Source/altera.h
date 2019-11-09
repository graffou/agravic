 #include "slv.h"

START_OF_FILE(altera)
INCLUDES

PACKAGE(altera)

// Not required by C++, since gated clocks use a native scheme
#ifdef VHDL

component clk_gating_cell is
        port (
            inclk  : in  std_logic := 'X'; -- inclk
            ena    : in  std_logic := 'X'; -- ena
            outclk : out std_logic         -- outclk
        );
end component clk_gating_cell;

#endif

END_PACKAGE(altera)
