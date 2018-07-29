#pragma once

#include "type.h"

LIB_KATA_BEGIN

/// êFç\ë¢ëÃ
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

	f32 operator [](const s32 _idx) const
	{
		return color[_idx];
	};
};

LIB_KATA_END