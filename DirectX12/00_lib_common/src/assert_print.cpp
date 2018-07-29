#include "assert_print.h"
#include <stdarg.h>

LIB_KATA_BEGIN

template<typename ... ARGS>
void Print(const c8* _fmt, ARGS const& ... args)
{
	printf(_fmt, args...);
}

template<typename ... ARGS>
void PrintWarning(const c8* _fmt, ARGS const& ... args)
{
	printf(_fmt, args...);
}

template<typename ... ARGS>
void PrintError(const c8* _fmt, ARGS const& ... args)
{
	printf(_fmt, args...);
}

LIB_KATA_END