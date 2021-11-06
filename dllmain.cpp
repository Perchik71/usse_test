//////////////////////////////////////////
/*
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
#include "api/runhandler.h"

using namespace usse::api;

VOID USSEAPI Sys_DumpDisableBreakpoint(VOID);
VOID USSEAPI Sys_DumpEnableBreakpoint(VOID);
VOID USSEAPI MainFix_CreationKitSSE(VOID);

/*
==================
Sys_ApplyPatches

The main function for code injection, called from dump.cpp
==================
*/
VOID USSEAPI Sys_ApplyPatches(VOID) {
	if (RunHandler.WhoRuns() == BSTRunHandler::CREATIONKIT_SKYRIMSE)
		MainFix_CreationKitSSE();
}

/*
==================
DllMain

==================
*/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		// For now, skip everything except the CK
		switch (RunHandler.Visit(hModule)) {
		case BSTRunHandler::CREATIONKIT_SKYRIMSE:
			Sys_DumpEnableBreakpoint();
			break;
		}
	}

	return TRUE;
}