#pragma once

#ifndef __TYPE_H__
#define __TYPE_H__

#include "lib_def.h"

typedef bool				b8;
typedef char				c8;
typedef unsigned char		u8;
typedef	short				s16;
typedef unsigned short		u16;
typedef char16_t			c16;
typedef wchar_t				w16;
typedef int					s32;
typedef unsigned int		u32;
typedef char32_t			c32;
typedef long long			s64;
typedef unsigned long long	u64;
typedef float				f32;
typedef double				f64;

#define RCast reinterpret_cast
#define SCast static_cast
#define DCast dynamic_cast
#define CCast const_cast

#endif