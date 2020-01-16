// Clock generator
// For RTL code, use 48MHz input clk and generate c0 and C1 with ratios 1:2 and 5:2
// C0 is core clock (48MHz)
// C1 is pixel clock (24MHz)
// C2 is hdmi clock (120MHz)
//
// For Agravic platform, generate these clocks from a 240MHz base clock (1:10, 1:20, 1:2)

#include "slv.h"
START_OF_FILE(clk_gen)
USE_PACKAGE(structures)
USE_PACKAGE(altera)

#ifdef VHDL


LIBRARY ieee;
USE ieee.std_logic_1164.all;

LIBRARY altera_mf;
USE altera_mf.all;

ENTITY clk_gen IS
	PORT
	(
		areset		: IN STD_LOGIC  := '0';
		inclk0		: IN STD_LOGIC  := '0';
		c0		: OUT STD_LOGIC ;
		c1		: OUT STD_LOGIC ;
		c2		: OUT STD_LOGIC ;
		locked		: OUT STD_LOGIC
	);
END clk_gen;


ARCHITECTURE SYN OF clk_gen IS

	SIGNAL sub_wire0	: STD_LOGIC_VECTOR (4 DOWNTO 0);
	SIGNAL sub_wire1	: STD_LOGIC ;
	SIGNAL sub_wire2	: STD_LOGIC ;
	SIGNAL sub_wire3	: STD_LOGIC ;
	SIGNAL sub_wire4	: STD_LOGIC ;
	SIGNAL sub_wire5	: STD_LOGIC ;
	SIGNAL sub_wire6	: STD_LOGIC_VECTOR (1 DOWNTO 0);
	SIGNAL sub_wire7_bv	: BIT_VECTOR (0 DOWNTO 0);
	SIGNAL sub_wire7	: STD_LOGIC_VECTOR (0 DOWNTO 0);
	CONSTANT ZERO : std_logic := '0';


	COMPONENT altpll
	GENERIC (
		bandwidth_type		: STRING;
		clk0_divide_by		: NATURAL;
		clk0_duty_cycle		: NATURAL;
		clk0_multiply_by		: NATURAL;
		clk0_phase_shift		: STRING;
		clk1_divide_by		: NATURAL;
		clk1_duty_cycle		: NATURAL;
		clk1_multiply_by		: NATURAL;
		clk1_phase_shift		: STRING;
		clk2_divide_by		: NATURAL;
		clk2_duty_cycle		: NATURAL;
		clk2_multiply_by		: NATURAL;
		clk2_phase_shift		: STRING;
		compensate_clock		: STRING;
		inclk0_input_frequency		: NATURAL;
		intended_device_family		: STRING;
		lpm_hint		: STRING;
		lpm_type		: STRING;
		operation_mode		: STRING;
		pll_type		: STRING;
		port_activeclock		: STRING;
		port_areset		: STRING;
		port_clkbad0		: STRING;
		port_clkbad1		: STRING;
		port_clkloss		: STRING;
		port_clkswitch		: STRING;
		port_configupdate		: STRING;
		port_fbin		: STRING;
		port_inclk0		: STRING;
		port_inclk1		: STRING;
		port_locked		: STRING;
		port_pfdena		: STRING;
		port_phasecounterselect		: STRING;
		port_phasedone		: STRING;
		port_phasestep		: STRING;
		port_phaseupdown		: STRING;
		port_pllena		: STRING;
		port_scanaclr		: STRING;
		port_scanclk		: STRING;
		port_scanclkena		: STRING;
		port_scandata		: STRING;
		port_scandataout		: STRING;
		port_scandone		: STRING;
		port_scanread		: STRING;
		port_scanwrite		: STRING;
		port_clk0		: STRING;
		port_clk1		: STRING;
		port_clk2		: STRING;
		port_clk3		: STRING;
		port_clk4		: STRING;
		port_clk5		: STRING;
		port_clkena0		: STRING;
		port_clkena1		: STRING;
		port_clkena2		: STRING;
		port_clkena3		: STRING;
		port_clkena4		: STRING;
		port_clkena5		: STRING;
		port_extclk0		: STRING;
		port_extclk1		: STRING;
		port_extclk2		: STRING;
		port_extclk3		: STRING;
		self_reset_on_loss_lock		: STRING;
		width_clock		: NATURAL
	);
	PORT (
			areset	: IN STD_LOGIC ;
			inclk	: IN STD_LOGIC_VECTOR (1 DOWNTO 0);
			clk	: OUT STD_LOGIC_VECTOR (4 DOWNTO 0);
			locked	: OUT STD_LOGIC
	);
	END COMPONENT;

BEGIN
	sub_wire7_bv(0 DOWNTO 0) <= "0";
	sub_wire7    <= To_stdlogicvector(sub_wire7_bv);
	sub_wire3    <= sub_wire0(2);
	sub_wire2    <= sub_wire0(1);
	sub_wire1    <= sub_wire0(0);
	c0    <= sub_wire1;
	c1    <= sub_wire2;
	c2    <= sub_wire3;
	locked    <= sub_wire4;
	sub_wire5    <= inclk0;
	sub_wire6    <= sub_wire7(0 DOWNTO 0) & sub_wire5;

	altpll_component : altpll
	GENERIC map (
		bandwidth_type => "AUTO",
		clk0_divide_by => 1,
		clk0_duty_cycle => 50,
		clk0_multiply_by => 1,
		clk0_phase_shift => "0",
		clk1_divide_by => 2,
		clk1_duty_cycle => 50,
		clk1_multiply_by => 1,
		clk1_phase_shift => "0",
		clk2_divide_by => 2,
		clk2_duty_cycle => 50,
		clk2_multiply_by => 5,
		clk2_phase_shift => "0",
		compensate_clock => "CLK0",
		inclk0_input_frequency => 20833,
		intended_device_family => "Cyclone 10 LP",
		lpm_hint => "CBX_MODULE_PREFIX=clk_gen",
		lpm_type => "altpll",
		operation_mode => "NORMAL",
		pll_type => "AUTO",
		port_activeclock => "PORT_UNUSED",
		port_areset => "PORT_USED",
		port_clkbad0 => "PORT_UNUSED",
		port_clkbad1 => "PORT_UNUSED",
		port_clkloss => "PORT_UNUSED",
		port_clkswitch => "PORT_UNUSED",
		port_configupdate => "PORT_UNUSED",
		port_fbin => "PORT_UNUSED",
		port_inclk0 => "PORT_USED",
		port_inclk1 => "PORT_UNUSED",
		port_locked => "PORT_USED",
		port_pfdena => "PORT_UNUSED",
		port_phasecounterselect => "PORT_UNUSED",
		port_phasedone => "PORT_UNUSED",
		port_phasestep => "PORT_UNUSED",
		port_phaseupdown => "PORT_UNUSED",
		port_pllena => "PORT_UNUSED",
		port_scanaclr => "PORT_UNUSED",
		port_scanclk => "PORT_UNUSED",
		port_scanclkena => "PORT_UNUSED",
		port_scandata => "PORT_UNUSED",
		port_scandataout => "PORT_UNUSED",
		port_scandone => "PORT_UNUSED",
		port_scanread => "PORT_UNUSED",
		port_scanwrite => "PORT_UNUSED",
		port_clk0 => "PORT_USED",
		port_clk1 => "PORT_USED",
		port_clk2 => "PORT_USED",
		port_clk3 => "PORT_UNUSED",
		port_clk4 => "PORT_UNUSED",
		port_clk5 => "PORT_UNUSED",
		port_clkena0 => "PORT_UNUSED",
		port_clkena1 => "PORT_UNUSED",
		port_clkena2 => "PORT_UNUSED",
		port_clkena3 => "PORT_UNUSED",
		port_clkena4 => "PORT_UNUSED",
		port_clkena5 => "PORT_UNUSED",
		port_extclk0 => "PORT_UNUSED",
		port_extclk1 => "PORT_UNUSED",
		port_extclk2 => "PORT_UNUSED",
		port_extclk3 => "PORT_UNUSED",
		self_reset_on_loss_lock => "OFF",
		width_clock => 5
	)
	PORT map (
		areset => ZERO, //PG !!!!!!! must not get reset_n. Otherwise does not start !!!!!!!!!!!!!!!!!!!
		inclk => sub_wire6,
		clk => sub_wire0,
		locked => sub_wire4
	);



END SYN;


#else

ENTITY( clk_gen,
	DECL_PORTS
	(
		PORT(areset, RST_TYPE, IN),// 		: IN STD_LOGIC  := '0';
		PORT(inclk0, CLK_TYPE, IN),	//	: IN STD_LOGIC  := '0';
		PORT(c0, CLK_TYPE, OUT),	//	: OUT STD_LOGIC ;
		PORT(c1, CLK_TYPE, OUT),	//	: OUT STD_LOGIC ;
		PORT(c2, CLK_TYPE, OUT),	//	: OUT STD_LOGIC ;
		PORT(locked, BIT_TYPE, OUT)	//	: OUT STD_LOGIC
	)
);

SIG(cnt, UINT(4));
SIG(clk_en_24, BIT_TYPE);
SIG(clk_en_48, BIT_TYPE);
SIG(clk_en_120, BIT_TYPE);

//SIG(init_done, BIT_TYPE) := BIT(0);
bool init_done = 0; // we don't rtl here

BEGIN
//GATED_CLK(c0 ,clk_inclk0, clk_en_48);
//GATED_CLK(c1 ,clk_inclk0, clk_en_24);
//GATED_CLK(c2 ,clk_inclk0, clk_en_120);


PROCESS(0, inclk0, areset)
BEGIN
IF (areset == BIT(0)) THEN
	clk_en_24 <= BIT(0);
	clk_en_48 <= BIT(0);
	clk_en_120 <= BIT(0);
	RESET(cnt);
	// This is a C++ trick to register all generated clocks as gated clocks
	if (init_done == 0)
	{
		PORT_BASE(c0).p_en = &clk_en_48;
		PORT_BASE(inclk0).children.push_back(&PORT_BASE(c0));
		PORT_BASE(c1).p_en = &clk_en_24;
		PORT_BASE(inclk0).children.push_back(&PORT_BASE(c1));
		PORT_BASE(c2).p_en = &clk_en_120;
		PORT_BASE(inclk0).children.push_back(&PORT_BASE(c2));
		init_done = 1;
	}
	// !!!!!!!!!!!!!! Compute enable on falling edge, otherwise no falling edge detection on gated clocks !!!!!!!!!!!!!!!!!!
ELSEIF (EVENT(inclk0) and (inclk0 == BIT(0))) THEN
	IF ( not (cnt == BIN(1001)) ) THEN
		cnt <= cnt + 1;
	ELSE
		cnt <= BIN(0000);
	ENDIF

	IF (B(cnt, 0) == BIT(0)) THEN
		clk_en_120 <= BIT(1);
	ELSE
		clk_en_120 <= BIT(0);
	ENDIF

	IF ( (cnt == BIN(0000)) or (cnt == BIN(0101)) ) THEN
		clk_en_48 <= BIT(1);
	ELSE
		clk_en_48 <= BIT(0);
	ENDIF

	IF ( (cnt == BIN(0000)) ) THEN
		clk_en_24 <= BIT(1);
	ELSE
		clk_en_24 <= BIT(0);
	ENDIF
ENDIF
END_PROCESS

BLK_END;
#endif
