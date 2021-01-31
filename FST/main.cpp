// g++ -std=c++17 fastlz.c lz4.c fstapi.c main.cpp -o dut -lz
// g++ -std=c++17 -DLIB main.cpp -o fst_lib.o -O3 -g3 -c
#ifdef LIB
#include "fastlz.h"
#include "fastlz.c"
#include "lz4.h"
#include "lz4.c"
#include "fstapi.h"
#include "fstapi.c"

#else
#include "fstapi.h"
//#include "fstapi.c"

int main()
{
	void* m_fst = fstWriterCreate("dut.fst", 1);
	fstWriterSetPackType(m_fst, FST_WR_PT_LZ4);
	fstWriterSetTimescaleFromString(m_fst, "1ps"); 
	
	fstWriterSetScope(m_fst, FST_ST_VCD_SCOPE, "top", NULL);
	fstWriterSetScope(m_fst, FST_ST_VCD_SCOPE, "tb", NULL);
	fstWriterSetScope(m_fst, FST_ST_VCD_SCOPE, "dut/zob.bite", NULL);
	//p.first->second = fstWriterCreateVar(m_fst, vartype, vardir, len, name_str.c_str(), 0); -> for aliases
	//fstWriterCreateVar(m_fst, vartype, vardir, len, name_str.c_str(), p.first->second);
	int handle = fstWriterCreateVar(m_fst, FST_VT_VCD_REG, FST_VD_MIN, 16, "zozo.toto", 0);
	int handle2 = fstWriterCreateVar(m_fst, FST_VT_VCD_REG, FST_VD_OUTPUT, 16, "toto_o", handle);
	int handle5 = fstWriterCreateVar(m_fst, FST_VT_VCD_REG, FST_VD_OUTPUT, 16, "toto1_o", handle2);
	int handle6 = fstWriterCreateVar(m_fst, FST_VT_VCD_REG, FST_VD_OUTPUT, 16, "toto2_o", handle5);
	int handle3 = fstWriterCreateVar(m_fst, FST_VT_VCD_REG, FST_VD_MIN, 1, "\\zozo.toto_en", 0);
	int handle4 = fstWriterCreateVar(m_fst, FST_VT_VCD_REG, FST_VD_MIN, 16, "tab(0)", 0);
	
	
	fstWriterSetUpscope(m_fst);
	fstWriterSetUpscope(m_fst);
	fstWriterSetUpscope(m_fst);

	
	for (int i = 0; i < 1000000; i++)
	{
		fstWriterEmitTimeChange(m_fst, i*100);
		if (i < 300000)
		fstWriterEmitValueChange64(m_fst, handle, 16, i);
		else if (i < 600000)
		fstWriterEmitValueChange64(m_fst, handle5, 16, i);
		else
		fstWriterEmitValueChange64(m_fst, handle6, 16, i);
			
		fstWriterEmitValueChange64(m_fst, handle4, 16, i*500);

		fstWriterEmitValueChange(m_fst, handle3, i&1 ? "1" : "0");
	}
	fstWriterClose(m_fst);

}
#endif