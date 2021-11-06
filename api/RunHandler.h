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
#include "..\profiler_internal.h"

namespace usse {
	namespace api {
		class BSTRunHandler {
		public:
			enum {
				UNKNOWN = 0,
				LAUNCHER_SKYRIMSE,
				GAME_SKYRIMSE,
				CREATIONKIT_SKYRIMSE
			};
		private:
			BYTE who_runs;
			HMODULE handle;
			HMODULE exe_handle;
		public:
			inline BYTE WhoRuns(VOID) const { return who_runs; }
			inline HMODULE GetThisDllHandle(VOID) const { return handle; }
			inline HMODULE GetThisExeHandle(VOID) const { return exe_handle; }

			BYTE Visit(HMODULE hModule);
		public:
			BSTRunHandler(VOID) = default;
			~BSTRunHandler(VOID) = default;
		};

		extern BSTRunHandler RunHandler;
	}
}

