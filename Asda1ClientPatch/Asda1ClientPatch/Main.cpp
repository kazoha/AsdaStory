#include <Windows.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "PEHook.h"

__declspec(naked) void SetIP()
{
	static sockaddr_in* m_SockAddr;

	__asm
	{
		LEA EDX, DWORD PTR SS:[EBP-0x18]
		MOV m_SockAddr, EDX
	}

	*(unsigned char*)&m_SockAddr->sin_port = 0xF;
	*(unsigned char*)(((int)&m_SockAddr->sin_port) + 1) = 0xAA;
	m_SockAddr->sin_addr.S_un.S_un_b.s_b1 = 192;
	m_SockAddr->sin_addr.S_un.S_un_b.s_b2 = 168;
	m_SockAddr->sin_addr.S_un.S_un_b.s_b3 = 1;
	m_SockAddr->sin_addr.S_un.S_un_b.s_b4 = 8;
	//can you see?  well, that is my dllpatch :) to change server ip into client.
	__asm
	{
		LEA EDX,DWORD PTR SS:[EBP-0x18]
		PUSH EDX
		MOV EAX,DWORD PTR SS:[EBP+8]

		PUSH 0x9D966F
		RETN
	}
}

int __stdcall DllMain(HINSTANCE hInstDLL, DWORD catchReason, LPVOID lpResrv)
{
	if (catchReason == DLL_PROCESS_ATTACH) 
	{
		DWORD dwOldProtectFlag_text;

		VirtualProtect((void*)0x401000, 0x7C8000, PAGE_READWRITE, &dwOldProtectFlag_text);

#pragma region Console dos capiroto
		AllocConsole();

		/* Output fix */
		int outParam = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		FILE *stdout_stream = _fdopen(outParam, "w");

		*stdout = *stdout_stream;

		/* Input fix */
		int inParam = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
		FILE *stdin_stream = _fdopen(inParam, "r");

		*stdin = *stdin_stream;
#pragma endregion

		// Set ip
		JMP_NEAR(0x9D9668, SetIP, 2);

		VirtualProtect((void*)0x401000, 0x7C8000, dwOldProtectFlag_text, &dwOldProtectFlag_text);
	}
	else if (catchReason == DLL_PROCESS_DETACH) 
	{
		FreeLibrary(hInstDLL);
	}

	return TRUE;
}