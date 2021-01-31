// various slv manipulation utilities

#include "../Include_libs/slv.h"

START_OF_FILE(slv_utils)
INCLUDES
// -------------------------------------------------------------------------
#ifdef VHDL


PACKAGE slv_utils IS

function BOOL2BIT(x : boolean) return std_logic;
function BOOL2BIN(x : boolean) return UNSIGNED;
function BIT2BOOL(x : std_logic) return boolean ;
function BIT2BIN(x : std_logic) return unsigned ;
function BIN2BIT(x : unsigned) return std_logic ;

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

function BIT2BOOL(x : std_logic) return boolean is
		begin
		return (x = '1');
		end;

function BIT2BIN(x : std_logic) return unsigned is
		begin
		return unsigned'(""&x);
		end;

function BIN2BIT(x : unsigned) return std_logic is
		begin
			return (x(0));
		end;

END slv_utils;



#else
// ---------------------------------------------------------------------------------
#define BOOL2BIN(x) slv<1>(x)
#define BOOL2BIT(x) slv<1>(x)
#define BIT2BOOL(x) (x == BIT(1))
#define BIT2BIN(x) x
#define BIN2BIT(x) x

#endif
