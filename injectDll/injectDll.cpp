// injectDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "injectDll.h"


#include "injectDll.h"
#include <tchar.h>
unsigned char flow[50] = "\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c";


MYDLL_API int funmydll(void)
{
	char temp[26];
	__asm {
		mov eax, eax;
		mov eax, eax;
		mov eax, eax;

	}
	strcpy(temp, (char*)flow);

	return 2;
}