// various slv manipulation utilities

#include "../Include_libs/slv.h"

START_OF_FILE(slv_utils)
INCLUDES
// -------------------------------------------------------------------------
#ifdef VHDL


PACKAGE slv_utils IS

function BOOL2BIT(x : boolean) return std_logic;
function BOOL2BIN(x : boolean) return UNSIGNED;

END slv_utils;

PACKAGE body slv_utils IS


function BOOL2BIT(x : boolean) return std_logic is
        variable z : std_logic;
        begin
            if x then
                z := '1';
            else
                z := '0';
            end if;
            return z;
        end;

function BOOL2BIN(x : boolean) return UNSIGNED is
        variable z : UNSIGNED(0 downto 0);
                begin
                    if x then
                        z := "1";
                    else
                        z := "0";
                    end if;
                    return z;
                end;


END slv_utils;


#else
// ---------------------------------------------------------------------------------
#define BOOL2BIN(x) slv<1>(x)
#define BOOL2BIT(x) slv<1>(x)

#endif
