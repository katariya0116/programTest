#pragma once

#include "type.h"

LIB_KATA_BEGIN

// 念のためdefineを切っておく（C++11環境じゃないところで切った時よう）
#define LIB_ALIGNED(x) alignas(x)

struct CHandle
{
public:
	CHandle();
	~CHandle();

private:
	union
	{
		u32 hdl;
		u16 idx[2];
	};
};
/// 色構造体
struct Color
{
public:
	union
	{
		f32 color[4];
		struct
		{
			f32 r;
			f32 g;
			f32 b;
			f32 a;
		};
	};

public:
	Color()
		:r(0.0f), g(0.0f), b(0.0f), a(0.0f)
	{
	}

	Color(f32 _r, f32 _g, f32 _b, f32 _a)
		:r(_r), g(_g), b(_b), a(_a)

	{
	}

	const f32 operator [](const s32 _idx) const
	{
		return color[_idx];
	};
	f32& operator [](const s32 _idx)
	{
		return color[_idx];
	};

};

LIB_KATA_END