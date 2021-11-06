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

#include <strsafe.h>

std::string __stdcall XUtil::Str::GetLastErrorToStr(DWORD err, const std::string& namefunc)
{
	// Retrieve the system error message for the last-error code

	std::string str;
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlenA((LPCSTR)lpMsgBuf) + lstrlenA(namefunc.c_str()) + 40) * sizeof(CHAR));
	StringCchPrintfA((LPSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(CHAR),
		"%s failed with error %d: %s",
		namefunc.c_str(), err, lpMsgBuf);
	str = (LPSTR)lpDisplayBuf;

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

	return str;
}

std::string __stdcall XUtil::Str::GetLastErrorToStr(const std::string& namefunc)
{
	return GetLastErrorToStr(GetLastError(), namefunc);
}

VtableIndexUtil *VtableIndexUtil::GlobalInstance;

VtableIndexUtil *VtableIndexUtil::Instance()
{
	if (!GlobalInstance)
	{
		GlobalInstance = new VtableIndexUtil();
		GlobalInstance->ForceVtableReference();

		// Overwrite this class's vtable pointers
		static const VtableIndexFn VtableIndexArray[] =
		{
#define MAKE_ENTRY(Index) []() -> int { return Index; }
			MAKE_ENTRY(0), MAKE_ENTRY(1), MAKE_ENTRY(2),
			MAKE_ENTRY(3), MAKE_ENTRY(4), MAKE_ENTRY(5),
			MAKE_ENTRY(6), MAKE_ENTRY(7), MAKE_ENTRY(8),
			MAKE_ENTRY(9), MAKE_ENTRY(10), MAKE_ENTRY(11),
			MAKE_ENTRY(12), MAKE_ENTRY(13), MAKE_ENTRY(14),
			MAKE_ENTRY(15), MAKE_ENTRY(16), MAKE_ENTRY(17),
			MAKE_ENTRY(18), MAKE_ENTRY(19), MAKE_ENTRY(20),
			MAKE_ENTRY(21), MAKE_ENTRY(22), MAKE_ENTRY(23),
			MAKE_ENTRY(24), MAKE_ENTRY(25), MAKE_ENTRY(26),
			MAKE_ENTRY(27), MAKE_ENTRY(28), MAKE_ENTRY(29),
			MAKE_ENTRY(30), MAKE_ENTRY(31), MAKE_ENTRY(32),
			MAKE_ENTRY(33), MAKE_ENTRY(34), MAKE_ENTRY(35),
			MAKE_ENTRY(36), MAKE_ENTRY(37), MAKE_ENTRY(38),
			MAKE_ENTRY(39), MAKE_ENTRY(40), MAKE_ENTRY(41),
			MAKE_ENTRY(42), MAKE_ENTRY(43), MAKE_ENTRY(44),
			MAKE_ENTRY(45), MAKE_ENTRY(46), MAKE_ENTRY(47),
			MAKE_ENTRY(48), MAKE_ENTRY(49), MAKE_ENTRY(50),
			MAKE_ENTRY(51), MAKE_ENTRY(52), MAKE_ENTRY(53),
			MAKE_ENTRY(54), MAKE_ENTRY(55), MAKE_ENTRY(56),
			MAKE_ENTRY(57), MAKE_ENTRY(58), MAKE_ENTRY(59),
			MAKE_ENTRY(60), MAKE_ENTRY(61), MAKE_ENTRY(62),
			MAKE_ENTRY(63), MAKE_ENTRY(64), MAKE_ENTRY(65),
			MAKE_ENTRY(66), MAKE_ENTRY(67), MAKE_ENTRY(68),
			MAKE_ENTRY(69), MAKE_ENTRY(70), MAKE_ENTRY(71),
			MAKE_ENTRY(72), MAKE_ENTRY(73), MAKE_ENTRY(74),
			MAKE_ENTRY(75), MAKE_ENTRY(76), MAKE_ENTRY(77),
			MAKE_ENTRY(78), MAKE_ENTRY(79), MAKE_ENTRY(80),
			MAKE_ENTRY(81), MAKE_ENTRY(82), MAKE_ENTRY(83),
			MAKE_ENTRY(84), MAKE_ENTRY(85), MAKE_ENTRY(86),
			MAKE_ENTRY(87), MAKE_ENTRY(88), MAKE_ENTRY(89),
			MAKE_ENTRY(90), MAKE_ENTRY(91), MAKE_ENTRY(92),
			MAKE_ENTRY(93), MAKE_ENTRY(94), MAKE_ENTRY(95),
			MAKE_ENTRY(96), MAKE_ENTRY(97), MAKE_ENTRY(98),
			MAKE_ENTRY(99), MAKE_ENTRY(100), MAKE_ENTRY(101),
			MAKE_ENTRY(102), MAKE_ENTRY(103), MAKE_ENTRY(104),
			MAKE_ENTRY(105), MAKE_ENTRY(106), MAKE_ENTRY(107),
			MAKE_ENTRY(108), MAKE_ENTRY(109), MAKE_ENTRY(110),
			MAKE_ENTRY(111), MAKE_ENTRY(112), MAKE_ENTRY(113),
			MAKE_ENTRY(114), MAKE_ENTRY(115), MAKE_ENTRY(116),
			MAKE_ENTRY(117), MAKE_ENTRY(118), MAKE_ENTRY(119),
			MAKE_ENTRY(120),
#undef MAKE_ENTRY
		};

		*(uintptr_t *)GlobalInstance = (uintptr_t)&VtableIndexArray;
	}

	return GlobalInstance;
}

int VtableIndexUtil::ForceVtableReference()
{
	return 0;
}

void XUtil::SetThreadName(uint32_t ThreadID, const char *ThreadName)
{
	if (ThreadID == GetCurrentThreadId())
	{
#if SKYRIM64_USE_VTUNE
		__itt_thread_set_name(ThreadName);
#endif

#if SKYRIM64_USE_TRACY
		tracy::SetThreadName(GetCurrentThread(), ThreadName);
#endif
	}

#pragma pack(push, 8)  
	const DWORD MS_VC_EXCEPTION = 0x406D1388;

	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType;		// Must be 0x1000.  
		LPCSTR szName;		// Pointer to name (in user addr space).  
		DWORD dwThreadID;	// Thread ID (-1=caller thread).  
		DWORD dwFlags;		// Reserved for future use, must be zero.  
	} THREADNAME_INFO;
#pragma pack(pop)

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = ThreadName;
	info.dwThreadID = ThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

size_t XUtil::Str::findCaseInsensitive(std::string data, std::string toSearch, size_t pos)
{
	// Convert complete given String to lower case
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	// Convert complete given Sub String to lower case
	std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);
	// Find sub string in given string
	return data.find(toSearch, pos);
}

void XUtil::Str::replaceAll(std::string& source, const std::string& from, const std::string& to)
{
	// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string

	std::string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while (std::string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
}

std::string XUtil::Str::format(const char* fmt, ...)
{
	va_list va;
	char message[2048];

	va_start(va, fmt);
	_vsnprintf(&message[0], _TRUNCATE, fmt, va);
	va_end(va);

	return message;
}

void XUtil::Trim(char *Buffer, char C)
{
	size_t len = strlen(Buffer);

	if (len > 0 && Buffer[len - 1] == C)
		Buffer[len - 1] = '\0';
}

void XUtil::XAssert(const char *File, int Line, const char *Format, ...)
{
	char buffer[4096];
	char message[4096];

	va_list ap;
	va_start(ap, Format);

	_vsnprintf_s(buffer, _TRUNCATE, Format, ap);
	sprintf_s(message, "%s(%d):\n\n%s", File, Line, buffer);

	MessageBoxA(nullptr, message, "ASSERTION", MB_ICONERROR);

	if (IsDebuggerPresent())
	{
		OutputDebugStringA(message);
		__debugbreak();
	}

	TerminateProcess(GetCurrentProcess(), 1);
	__assume(0);
}

uint64_t XUtil::MurmurHash64A(const void *Key, size_t Len, uint64_t Seed)
{
	/*-----------------------------------------------------------------------------
	// https://github.com/abrandoned/murmur2/blob/master/MurmurHash2.c#L65
	// MurmurHash2, 64-bit versions, by Austin Appleby
	//
	// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment
	// and endian-ness issues if used across multiple platforms.
	//
	// 64-bit hash for 64-bit platforms
	*/
	const uint64_t m = 0xc6a4a7935bd1e995ull;
	const int r = 47;

	uint64_t h = Seed ^ (Len * m);

	const uint64_t *data = (const uint64_t *)Key;
	const uint64_t *end = data + (Len / 8);

	while (data != end)
	{
		uint64_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char *data2 = (const unsigned char *)data;

	switch (Len & 7)
	{
	case 7: h ^= ((uint64_t)data2[6]) << 48;
	case 6: h ^= ((uint64_t)data2[5]) << 40;
	case 5: h ^= ((uint64_t)data2[4]) << 32;
	case 4: h ^= ((uint64_t)data2[3]) << 24;
	case 3: h ^= ((uint64_t)data2[2]) << 16;
	case 2: h ^= ((uint64_t)data2[1]) << 8;
	case 1: h ^= ((uint64_t)data2[0]);
		h *= m;
	}

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

uintptr_t XUtil::FindPattern(uintptr_t StartAddress, uintptr_t MaxSize, const char *Mask)
{
	std::vector<std::pair<uint8_t, bool>> pattern;

	for (size_t i = 0; i < strlen(Mask);)
	{
		if (Mask[i] != '?')
		{
			pattern.emplace_back((uint8_t)strtoul(&Mask[i], nullptr, 16), false);
			i += 3;
		}
		else
		{
			pattern.emplace_back(0x00, true);
			i += 2;
		}
	}

	const uint8_t *dataStart = (uint8_t *)StartAddress;
	const uint8_t *dataEnd = (uint8_t *)StartAddress + MaxSize + 1;

	auto ret = std::search(dataStart, dataEnd, pattern.begin(), pattern.end(),
		[](uint8_t CurrentByte, std::pair<uint8_t, bool>& Pattern)
	{
		return Pattern.second || (CurrentByte == Pattern.first);
	});

	if (ret == dataEnd)
		return 0;

	return std::distance(dataStart, ret) + StartAddress;
}

std::vector<uintptr_t> XUtil::FindPatterns(uintptr_t StartAddress, uintptr_t MaxSize, const char *Mask)
{
	std::vector<uintptr_t> results;
	std::vector<std::pair<uint8_t, bool>> pattern;

	for (size_t i = 0; i < strlen(Mask);)
	{
		if (Mask[i] != '?')
		{
			pattern.emplace_back((uint8_t)strtoul(&Mask[i], nullptr, 16), false);
			i += 3;
		}
		else
		{
			pattern.emplace_back(0x00, true);
			i += 2;
		}
	}

	const uint8_t *dataStart = (uint8_t *)StartAddress;
	const uint8_t *dataEnd = (uint8_t *)StartAddress + MaxSize + 1;

	for (const uint8_t *i = dataStart;;)
	{
		auto ret = std::search(i, dataEnd, pattern.begin(), pattern.end(),
			[](uint8_t CurrentByte, std::pair<uint8_t, bool>& Pattern)
		{
			return Pattern.second || (CurrentByte == Pattern.first);
		});

		// No byte pattern matched, exit loop
		if (ret == dataEnd)
			break;

		uintptr_t addr = std::distance(dataStart, ret) + StartAddress;
		results.push_back(addr);

		i = (uint8_t *)(addr + 1);
	}

	return results;
}

bool XUtil::GetPESectionRange(uintptr_t ModuleBase, const char *Section, uintptr_t *Start, uintptr_t *End)
{
	PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)(ModuleBase + ((PIMAGE_DOS_HEADER)ModuleBase)->e_lfanew);
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

	// Assume PE header if no section
	if (!Section || strlen(Section) <= 0)
	{
		if (Start)
			*Start = ModuleBase;

		if (End)
			*End = ModuleBase + ntHeaders->OptionalHeader.SizeOfHeaders;

		return true;
	}

	for (uint32_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++)
	{
		// Name might not be null-terminated
		char sectionName[sizeof(IMAGE_SECTION_HEADER::Name) + 1] = { };
		memcpy(sectionName, section->Name, sizeof(IMAGE_SECTION_HEADER::Name));

		if (!strcmp(sectionName, Section))
		{
			if (Start)
				*Start = ModuleBase + section->VirtualAddress;

			if (End)
				*End = ModuleBase + section->VirtualAddress + section->Misc.VirtualSize;

			return true;
		}
	}

	return false;
}

void XUtil::WrnMsgFmt(const char* Format, ...) {
	char buffer[4096];

	va_list ap;
	va_start(ap, Format);
	_vsnprintf_s(buffer, _TRUNCATE, Format, ap);
	va_end(ap);

	MessageBoxA(nullptr, buffer, "WARNING", MB_ICONWARNING | MB_OK);
}

void XUtil::PatchMemory(uintptr_t Address, uint8_t *Data, size_t Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (uintptr_t i = Address; i < (Address + Size); i++)
		*(volatile uint8_t *)i = *Data++;

	VirtualProtect((LPVOID)Address, Size, d, &d);
	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

void XUtil::PatchMemory(uintptr_t Address, std::initializer_list<uint8_t> Data)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Data.size(), PAGE_EXECUTE_READWRITE, &d);

	uintptr_t i = Address;
	for (auto value : Data)
		*(volatile uint8_t *)i++ = value;

	VirtualProtect((LPVOID)Address, Data.size(), d, &d);
	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Data.size());
}

void XUtil::PatchMemoryNop(uintptr_t Address, size_t Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (uintptr_t i = Address; i < (Address + Size); i++)
		*(volatile uint8_t *)i = 0x90;

	VirtualProtect((LPVOID)Address, Size, d, &d);
	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

void XUtil::DetourJump(uintptr_t Target, uintptr_t Destination)
{
	Detours::X64::DetourFunction(Target, Destination, Detours::X64Option::USE_REL32_JUMP);
}

void XUtil::DetourCall(uintptr_t Target, uintptr_t Destination)
{
	Detours::X64::DetourFunction(Target, Destination, Detours::X64Option::USE_REL32_CALL);
}