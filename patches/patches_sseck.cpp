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

#include "..\api\BGString.h"
#include "UnicodePatchOffset.h"

#include "TESDialogSpell.h"

using namespace usse;
using namespace usse::api;

BOOL USSEAPI hk_BeginPluginSave(VOID) {
	ConvertorString.Mode = BGSConvertorString::MODE_UTF8;
	return TRUE;
}

VOID USSEAPI hk_EndPluginSave(HCURSOR hCursor) {
	ConvertorString.Mode = BGSConvertorString::MODE_ANSI;
	SetCursor(hCursor);
}

BOOL USSEAPI hk_SetDlgItemTextA(HWND hDlg, INT nIDDlgItem, LPCSTR lpString) {
	switch (nIDDlgItem)
	{
	case 1024:
	case 1025:
	{
		if (!lpString || !XUtil::Conversion::IsUtf8Valid(lpString))
			goto SetTextDef;

		std::string wincp_str = XUtil::Conversion::Utf8ToAnsi(lpString);
		return SetDlgItemTextA(hDlg, nIDDlgItem, wincp_str.c_str());
	}
	default:
	SetTextDef:
		return SetDlgItemTextA(hDlg, nIDDlgItem, lpString);
	}
}

BOOL USSEAPI hk_SendDlgItemMessageA(HWND hDlg, INT nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (Msg != WM_GETTEXT && Msg != WM_GETTEXTLENGTH)
		MsgTextDef:
		return SendDlgItemMessageA(hDlg, nIDDlgItem, Msg, wParam, lParam);

	HWND hCtrlWnd;

	switch (nIDDlgItem)
	{
	case 1024:
	case 1025:
	{
		hCtrlWnd = GetDlgItem(hDlg, nIDDlgItem);
		INT32 maxlen = GetWindowTextLengthA(hCtrlWnd) << 2;

		if (maxlen <= 0)
			goto MsgTextDef;

		std::string ansi_str;
		ansi_str.resize(maxlen);
		ansi_str.resize(GetWindowTextA(hCtrlWnd, &ansi_str[0], maxlen));

		if (XUtil::Conversion::IsUtf8Valid(ansi_str))
			goto MsgTextDef;

		std::string utf8_str = XUtil::Conversion::AnsiToUtf8(ansi_str);

		if (Msg == WM_GETTEXT)
			strncpy((LPSTR)(lParam), utf8_str.c_str(), wParam);

		return utf8_str.length() + 1;
	}
	default:
		goto MsgTextDef;
	}
}

VOID USSEAPI MainFix_CreationKitSSE(VOID) {
	if (_stricmp((LPSTR)(g_ModuleBase + 0x3062CC8), "1.5.73.0")) {
		CHAR modulePath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandleA(NULL), modulePath, ARRAYSIZE(modulePath));

		CHAR message[1024];
		sprintf_s(message,
			"Unknown Creation Kit version detected. Unicode patch are disabled.\n\n"
			"Required versions:\n"
			"CreationKit.exe 1.5.73 released on 2019-03-13\n"
			"\nExecutable path: %s", modulePath);

		MessageBoxA(NULL, message, "Version Check", MB_ICONERROR);
		return;
	}

	//
	// Convert Utf-8 to WinCP when loading and back when saving
	//
#if SKYRIMSE_LAZ_UNICODE_PLUGIN
	// Initialization CreationKitUnicodePlugin.dll
	BOOL bRes = XUtil::Conversion::LazUnicodePluginInit();
	if (!bRes)
		_MESSAGE("Library 'CreationKitUnicodePlugin.dll' no found. Unicode support don't patched.");
	else {
		
		// Initial mode
		ConvertorString.Mode = BGSConvertorString::MODE_ANSI;

		// Since the fixes change AT, I replace the function call, however, need to save its contents code.
		auto call = OFFSET(0x1215D80);
		*(uintptr_t*)&BGSLocalizedString_OldGetStrProc = Detours::X64::DetourFunctionClass(call, &BGSLocalizedString::GetStr);
		DWORD protection;
		
		VirtualProtect((LPVOID)g_CodeBase, g_CodeEnd - g_CodeBase, PAGE_READWRITE, &protection);
		
		// Restore call (cut out ssefixes)
		uintptr_t loff;
		DWORD roff;
		for (auto i : UnicodePatch::rav) {
			loff = OFFSET(i);
			memcpy((LPVOID)loff, "\xE8", 1);
			roff = DWORD(call - (loff + 5));
			memcpy((LPVOID)(loff + 1), &roff, 4);
		}

		VirtualProtect((LPVOID)g_CodeBase, g_CodeEnd - g_CodeBase, protection, &protection);
		FlushInstructionCache(GetCurrentProcess(), (LPVOID)g_CodeBase, g_CodeEnd - g_CodeBase);
		
		// Also delete it message "You must close all Dialoge Boxes",
		// which has problems with programs that work with multiple monitors.
		XUtil::DetourCall(OFFSET(0x13FC051), &hk_BeginPluginSave);
		// I don't quite understand the meaning of calling SetCursor in this function, which deals with saving
		// But we'll make the call in hook.
		XUtil::DetourCall(OFFSET(0x13FC0F2), &hk_EndPluginSave);
		
		// Fix charset Spell/Scroll/Ench etc dialoges
		*(uintptr_t*)&spell::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x1486620), &spell::DlgProc);
		
		// In the "Data" dialog box, the "author" and "description" controls are independent, and I'm forced to make a trap for WinAPI calls
		XUtil::DetourCall(OFFSET(0x13E32BB), &hk_SetDlgItemTextA);
		XUtil::DetourCall(OFFSET(0x13E32E8), &hk_SetDlgItemTextA);
		XUtil::DetourCall(OFFSET(0x13E3313), &hk_SetDlgItemTextA);
		XUtil::DetourCall(OFFSET(0x13E3079), &hk_SendDlgItemMessageA);
		XUtil::DetourCall(OFFSET(0x13E30DA), &hk_SendDlgItemMessageA);
		XUtil::DetourCall(OFFSET(0x13E3113), &hk_SendDlgItemMessageA);
		XUtil::DetourCall(OFFSET(0x13E3174), &hk_SendDlgItemMessageA);
	}
#else
	_MESSAGE("Unfortunately, your compiled version does not support the 'Experimental::Unicode' option.");
#endif // !SKYRIMSE_LAZ_UNICODE_PLUGIN
}