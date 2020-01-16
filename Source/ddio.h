// HDMI output - december 2019
// this block transforms rgb and pixel clock inputs (2 bits at clk_hdmi)
// to DDR outputs
// In VHDL, this is performed by an altddioout cell


#include "../Include_libs/slv.h"

START_OF_FILE(ddio)
INCLUDES
USE_PACKAGE(altera)


ENTITY(ddio,
DECL_PORTS(
		PORT(clk_hdmi, CLK_TYPE, IN),
		PORT(reset_n, RST_TYPE, IN),
		PORT(pclk_i, UINT(2), IN),
		PORT(red_i, UINT(2), IN),
		PORT(green_i, UINT(2), IN),
		PORT(blue_i, UINT(2), IN),

		PORT(pclk_o, BIT_TYPE, OUT),
		PORT(red_o, BIT_TYPE, OUT),
		PORT(green_o, BIT_TYPE, OUT),
		PORT(blue_o, BIT_TYPE, OUT)
		)
);

#ifndef VHDL
BEGIN


PROCESS(0, clk_hdmi, reset_n)
BEGIN
IF (reset_n == BIT(0)) THEN
ELSEIF (EVENT(clk_hdmi)) THEN
	IF (clk_hdmi == BIT(0)) THEN
		pclk_o <= B(PORT_BASE(pclk_i), 0);
		red_o <= B(PORT_BASE(red_i), 0);
		green_o <= B(PORT_BASE(green_i), 0);
		blue_o <= B(PORT_BASE(blue_i), 0);
	ELSE
		pclk_o <= B(PORT_BASE(pclk_i), 1);
		red_o <= B(PORT_BASE(red_i), 1);
		green_o <= B(PORT_BASE(green_i), 1);
		blue_o <= B(PORT_BASE(blue_i), 1);
	ENDIF

ENDIF

END_PROCESS
BLK_END;


#else
// VHDL implementation using alt_ddio_out component


	signal datain_h_sig : std_logic_vector(3 downto 0);
	signal datain_l_sig : std_logic_vector(3 downto 0);
	signal dataout_sig : std_logic_vector(3 downto 0);

	component alt_ddio IS
		PORT
		(
			datain_h		: IN STD_LOGIC_VECTOR (3 DOWNTO 0);
			datain_l		: IN STD_LOGIC_VECTOR (3 DOWNTO 0);
			outclock		: IN STD_LOGIC ;
			dataout		: OUT STD_LOGIC_VECTOR (3 DOWNTO 0)
		);
	END component;

	begin

	// h is 0 and l is 1 ? strange ? Like in arduino example...
	datain_h_sig <= (pclk_i(0) & red_i(0) & green_i(0) & blue_i(0));
	datain_l_sig <= (pclk_i(1) & red_i(1) & green_i(1) & blue_i(1));

	alt_ddio_inst : alt_ddio port map (
			datain_h	 => datain_h_sig,
			datain_l	 => datain_l_sig,
			outclock	 => clk_hdmi,
			dataout	 => dataout_sig
		);

	pclk_o <= dataout_sig(3);
	red_o <= dataout_sig(2);
	green_o <= dataout_sig(1);
	blue_o <= dataout_sig(0);


	END rtl;


#endif
