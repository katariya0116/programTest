#pragma once

#include "assert_print.h"
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>

LIB_KATA_BEGIN

template<typename ... ARGS>
void Print(const c8* _fmt, ARGS const& ... args)
{
	::printf(_fmt, args...);

	c8 text[1024];
	sprintf_s(text, _fmt, args...);
	::OutputDebugString(text);
}

template<typename ... ARGS>
void PrintWarning(const c8* _fmt, ARGS const& ... args)
{
	printf(_fmt, args...);

	c8 text[1024];
	sprintf_s(text, _fmt, args...);
	::OutputDebugString(text);
}

template<typename ... ARGS>
void PrintError(const c8* _fmt, ARGS const& ... args)
{
	printf(_fmt, args...);

	c8 text[1024];
	sprintf_s(text, _fmt, args...);
	::OutputDebugString(text);
}

LIB_KATA_END