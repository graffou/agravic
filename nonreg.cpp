#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>


#define DEBUG
#include "mbprt.h"

int main()
{
	std::ifstream test_list;
	shell("\\rm test_list;");
	shell("ls tests/*.bin >> test_list");
	test_list.open("test_list");
	std::ofstream test_results;
	test_results.open("test_results");

	std::stringstream cmd;
	int n = 0;
	while (not test_list.eof())
	{
		gstring s;
		test_list >> s;
		if (not test_list.eof() )//and (n < 2))
		{
			gkprintf("#MTest % : %", n, s);
			gkprintf(cmd, "./dut -ncycles 10000 -bin_file '%s' 2>&1 | grep FAILED > result", s);
			shell(cmd);
			std::ifstream result;
			result.open("result");
			std::string x;
			result >> x;
			if (x.size() > 10)
				gkprintf(test_results, "% failed\n", s);

		}
		n++;

	}

}
