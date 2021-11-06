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

#pragma once

#include "..\common.h"

namespace usse {
	namespace api {
		class BGSStringCache {
		private:
			std::list<std::string> cache;
		public:
			BGSStringCache(VOID) = default;
			~BGSStringCache(VOID) = default;
		public:
			inline DWORD Size(VOID) const { return cache.size(); }
			inline VOID Clear(VOID) { cache.clear(); }
			inline VOID Push(const std::string& s) { cache.push_back(s); }
			inline LPCSTR Last(VOID) const { return &cache.back()[0]; }
		};

		BGSStringCache StringCache;

		class BGSConvertorString {
		public:
			enum {
				MODE_ANSI = 0,
				MODE_UTF8
			};
		private:
			LPCSTR pre;
			BYTE mode;
		public:
			BGSConvertorString(VOID) = default;
			~BGSConvertorString(VOID) = default;
		private:
			inline BOOL IsValid(LPCSTR s) const {
				return ((s != NULL) && (s != LPSTR_TEXTCALLBACKA) && (strlen(s) > 0));
			}

			LPCSTR Utf8ToWinCP(LPCSTR src)
			{
				// Ansi verification is necessary because there are a lot of strings, especially short and system strings. 
				// The debug file without this option was more than 70 mb, compared to 2604 kb.
				// Translation of fallout4.esm has become significantly faster.

				if ((src == pre) || !IsValid(src) || !XUtil::Conversion::IsUtf8Valid(src))
					return src;

				std::string wincp_str = XUtil::Conversion::Utf8ToAnsi(src);

				// utf-8 takes up more memory than ansi, so I can use active memory
				pre = src;
				strcpy(const_cast<LPSTR>(pre), wincp_str.c_str());

				return pre;
			}

			LPCSTR WinCPToUtf8(LPCSTR src)
			{
				// Not all strings are translated during loading and remain in Utf-8. 
				// They are loaded after opening the dialog. As an example "Description".

				if (!IsValid(src) || XUtil::Conversion::IsUtf8Valid(src))
					return src;

				// in the Creation Kit code, the request to return a string occurs twice in a row.
				if ((pre == src) && (StringCache.Size() > 0))
					return StringCache.Last();

				// convert
				std::string utf8_str = XUtil::Conversion::AnsiToUtf8(src);

				// Unicode initially takes up more memory than ansi. 
				// Therefore, a heap is created that will store memory for the duration of saving.
				// Lists work quickly on adding items.
				StringCache.Push(utf8_str);

				// pointer to the memory of the contents of the last line, it is of course unique
				pre = src;
				return StringCache.Last();
			}
		public:
			inline BYTE GetMode(VOID) const { return mode; }
			inline VOID SetMode(BYTE m) {
				mode = m;
				StringCache.Clear();
			}

			LPCSTR Convert(LPCSTR s) {
				// Back to utf-8 (temporarily)
				if (mode == BGSConvertorString::MODE_UTF8)
					return WinCPToUtf8(s);
				// All APIs are implemented as ANSI. In this regard, utf-8 must be converted to the specified ANSI.
				else
					return Utf8ToWinCP(s);
			}
		public:
			PROPERTY(GetMode, SetMode) BYTE Mode;
		};

		BGSConvertorString ConvertorString;

		typedef LPSTR(*lpfnGetStr_t)(LPCSTR);
		static lpfnGetStr_t BGSLocalizedString_OldGetStrProc;

		class BGSLocalizedString {
		public:
			static LPCSTR GetStr(LPCSTR str) {
				return ConvertorString.Convert(BGSLocalizedString_OldGetStrProc(str));
			}
		};
	}
}