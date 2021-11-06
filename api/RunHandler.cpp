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

#include "runhandler.h"

using namespace usse::api;

BSTRunHandler usse::api::RunHandler;

BYTE BSTRunHandler::Visit(HMODULE hModule) {
	// Force this dll to be loaded permanently
	HMODULE temp;
	handle = hModule;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)hModule, &temp);

	exe_handle = GetModuleHandleA(NULL);

	// Then determine which exe is being loaded
	who_runs = UNKNOWN;

	CHAR modulePath[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), modulePath, MAX_PATH);

	CHAR executableName[MAX_PATH];
	_strlwr_s(modulePath);
	_splitpath_s(modulePath, NULL, 0, NULL, 0, executableName, ARRAYSIZE(executableName), NULL, 0);

	switch (CRC32(executableName))
	{
	case CRC32("skyrimselauncher"):
		who_runs = LAUNCHER_SKYRIMSE;
		break;

	case CRC32("skyrimse"):
	case CRC32("skyrimse_dump"):
		who_runs = GAME_SKYRIMSE;
		break;

	case CRC32("creationkit"):
	case CRC32("creationkit_1573"):
		who_runs = CREATIONKIT_SKYRIMSE;
		break;
	}

	return who_runs;
}


