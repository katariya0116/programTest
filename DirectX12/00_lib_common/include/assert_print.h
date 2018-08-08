#pragma once

#include "type.h"
#include <cassert>

LIB_KATA_BEGIN

template<typename ... ARGS>
static void Print(const c8* _fmt, ARGS const& ...);

template<typename ... ARGS>
static void PrintError(const c8* _fmt, ARGS const& ...);

template<typename ... ARGS>
static void PrintWarning(const c8* _fmt, ARGS const& ...);


#ifdef DEBUG_MODE

#define PRINT(format, ...)			LIB_KATA::Print(format, __VA_ARGS__)
#define PRINT_ERROR(format, ...)	LIB_KATA::PrintError(format, __VA_ARGS__)
#define PRINT_WARNING(format, ...)	LIB_KATA::PrintWarning(format, __VA_ARGS__)

#define Assert(x) assert(x)

#else

#define PRINT(format, ...)			
#define PRINT_ERROR(format, ...)	
#define PRINT_WARNING(format, ...)	

#define Assert(x) 

#endif

LIB_KATA_END

#include "assert_print.inl"