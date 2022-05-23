// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <filesystem>

//0F 84 ?? ?? ?? ?? 33 C9
//  \x0F\x84\x00\x00\x00\x00\x33\xC9

//0F 84 ?? ?? ?? ?? 48 8D 8C
//   \x0F\x84\x00\x00\x00\x00\x48\x8D\x8C


//0F 84 ?? ?? ?? ?? 66 0F 6F 05
//    \x0F\x84\x00\x00\x00\x00\x66\x0F\x6F\x05

//0F 85 ?? ?? ?? ?? 66
//    \x0F\x85\x00\x00\x00\x00\x66


void Patch(char* dst, char* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void nop(BYTE* dst,  unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}


MODULEINFO GetModuleInfo(const char* szModule)
{
	MODULEINFO modInfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule != 0)
		GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
	return modInfo;
}


void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int numberOfBytes)
{
	unsigned long old_protection;
	VirtualProtect((LPVOID)addressToWrite, numberOfBytes, PAGE_EXECUTE_READWRITE, &old_protection);
	memcpy((LPVOID)addressToWrite, valueToWrite, numberOfBytes);
	VirtualProtect((LPVOID)addressToWrite, numberOfBytes, old_protection, NULL);
}

void CreateConsole()
{
	if (!AllocConsole()) {
		// Add some error handling here.
		// You can call GetLastError() to get more info about the error.
		return;
	}
}

void bypass()
{
	//vanguard = 0x30925
	//build-1.exe
	//\x0F\x85\x9F\x01\x00\x00

	//wz: 1ZSBD.exe
	//wz: \x0F\x85\x8F\x01\x00\x00
	//wz: 0x30685

	//spf: 0x2DAD5
	//jhS16.exe
	//\x0F\x85\x9F\x01\x00\x00
	//MessageBoxA(NULL, "done", "done", MB_OK);

	uintptr_t moduleBase = (uintptr_t)GetModuleHandleA("jhS16.exe");

	
	//Patch((char*)(moduleBase + 0x38289), (char*)"\x0F\x85\x9F\x01\x00\x00", 6);

	nop((BYTE*)(moduleBase + 0x2DAD5), 2);

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        bypass();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

