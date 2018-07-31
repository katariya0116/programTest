#pragma once

#ifndef __TYPE_H__
#define __TYPE_H__

// ライブラリネームスペース
#define LIB_KATA_NAMESPACE kataLib
#define LIB_KATA_NAMESPACE_NAME #LIB_KATA_NAMESPACE
#define LIB_KATA_BEGIN namespace LIB_KATA_NAMESPACE {
#define LIB_KATA_END };

typedef bool				b8;
typedef char				c8;
typedef unsigned char		u8;
typedef	short				s16;
typedef unsigned short		u16;
typedef int					s32;
typedef unsigned int		u32;
typedef long long			s64;
typedef unsigned long long	u64;
typedef float				f32;
typedef double				f64;

#define RCast reinterpret_cast
#define SCast static_cast
#define DCast dynamic_cast
#define CCast const_cast

#endif