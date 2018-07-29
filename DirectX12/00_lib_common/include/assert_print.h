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


#ifdef _DEBUG

#define PRINT(format, ...)			Print(format, __VA_ARGS__)
#define PRINT_ERROR(format, ...)	PrintError(format, __VA_ARGS__)
#define PRINT_WARNING(format, ...)	PrintWarning(format, __VA_ARGS__)

#define Assert(x) assert(x)

#else
#define PRINT(format, ...)			
#define PRINT_ERROR(format, ...)	
#define PRINT_WARNING(format, ...)	

#define Assert(x) 

#endif

LIB_KATA_END