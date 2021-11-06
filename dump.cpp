//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#include "common.h"
#include <atomic>

#include "api/runhandler.h"

CHAR TempNTSIT[16];
ULONG_PTR TempNTSITAddress;
std::atomic_uint32_t g_DumpTargetThreadId;

typedef BOOL(USSEAPI* StartFunc_t)(DWORD dwUnk);

StartFunc_t g_StartFunc;

LONG(NTAPI * NtSetInformationThread)(HANDLE ThreadHandle, LONG ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength);

VOID USSEAPI Sys_DumpDisableBreakpoint(VOID);
VOID USSEAPI Sys_DumpEnableBreakpoint(VOID);
VOID USSEAPI Sys_ApplyPatches(VOID);

BOOL WINAPI hk_StartFunc(DWORD dwUnk) {
	// Restore the original pointer
	Sys_DumpDisableBreakpoint();

	// Notify debugger
	__try {
		__debugbreak();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{}
	
	Sys_ApplyPatches();
	return g_StartFunc(dwUnk);
}

VOID USSEAPI Sys_DumpEnableBreakpoint(VOID) {
	uintptr_t moduleBase = (uintptr_t)usse::api::RunHandler.GetThisExeHandle();
	PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)(moduleBase + ((PIMAGE_DOS_HEADER)moduleBase)->e_lfanew);

	// Get the load configuration section which holds the security cookie address
	auto dataDirectory	= ntHeaders->OptionalHeader.DataDirectory;
	auto sectionRVA		= dataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress;
	auto sectionSize	= dataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size;
	auto loadConfig		= (PIMAGE_LOAD_CONFIG_DIRECTORY)(moduleBase + sectionRVA);

	Assert(sectionRVA > 0 && sectionSize > 0);
	AssertMsg(loadConfig->SecurityCookie, "SecurityCookie is a null pointer!");

	// Determine the module/code section addresses and sizes
	g_ModuleBase = moduleBase;
	g_ModuleSize = ntHeaders->OptionalHeader.SizeOfImage;

	Assert(XUtil::GetPESectionRange(moduleBase, ".text", &g_CodeBase, &g_CodeEnd));
	Assert(XUtil::GetPESectionRange(moduleBase, ".rdata", &g_RdataBase, &g_RdataEnd));
	Assert(XUtil::GetPESectionRange(moduleBase, ".data", &g_DataBase, &g_DataEnd));

	uintptr_t tempBssStart;
	uintptr_t tempBssEnd;

	if (XUtil::GetPESectionRange(moduleBase, ".textbss", &tempBssStart, &tempBssEnd)) {
		g_CodeBase = std::min(g_CodeBase, tempBssStart);
		g_CodeEnd = std::max(g_CodeEnd, tempBssEnd);
	}

	*(uintptr_t*)&g_StartFunc = Detours::X64::DetourFunctionClass(OFFSET(0x2E47188), &hk_StartFunc);
}

VOID USSEAPI Sys_DumpDisableBreakpoint(VOID) {
	// Restore
	Detours::X64::DetourFunctionClass(OFFSET(0x2E47188), g_StartFunc);
}