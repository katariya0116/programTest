#pragma once

#include "math/vector.h"

LIB_KATA_BEGIN

// êU4x4ÌÝpÓ
class LIB_ALIGNED(16) CMatrix44
{
public:
	CMatrix44();
	~CMatrix44();

public:
	f32 * operator [] (u32 _idx);
	const f32* operator [] (u32 _idx) const;
	f32& operator () (u32 _idx, u32 _idx2);

	CMatrix44& operator=(const CMatrix44& _m);

	CMatrix44 operator+();
	CMatrix44 operator-();

	CMatrix44& operator+=(const CMatrix44& _m);
	CMatrix44& operator-=(const CMatrix44& _m);
	CMatrix44& operator*=(const f32 _k);
	CMatrix44& operator/=(const f32 _k);

private:
	CVector4 column[4];
};

// sñ[eBeB
class MatrixUtil
{
public:
	static CMatrix44& Identity(CMatrix44& _m);

	// XP[
	static CMatrix44& Scale(CMatrix44& _m, const CVector3& _scale);
	static CMatrix44& Scale(CMatrix44& _m, const f32 _scale);

	static CMatrix44& ScaleX(CMatrix44& _m, const f32 _scale);
	static CMatrix44& ScaleY(CMatrix44& _m, const f32 _scale);
	static CMatrix44& ScaleZ(CMatrix44& _m, const f32 _scale);

	// ñ]sñ
	static CMatrix44& RotX(CMatrix44& _m, f32 _rad);
	static CMatrix44& RotY(CMatrix44& _m, f32 _rad);
	static CMatrix44& RotZ(CMatrix44& _m, f32 _rad);

	// Ú®
	static CMatrix44& SetTrans(CMatrix44& _m, const CVector3& _vec);
	static CMatrix44& SetTransX(CMatrix44& _m, const f32 _vec);
	static CMatrix44& SetTransY(CMatrix44& _m, const f32 _vec);
	static CMatrix44& SetTransZ(CMatrix44& _m, const f32 _vec);

	// sñÌ|¯Z
	static CMatrix44& Mul(CMatrix44& _ret, const CMatrix44& _m0, const CMatrix44& _m1);

	// sñÉæéxNgÏ»
	static CVector3& Transform(CVector3& _ret, const CMatrix44& _m0, const CVector3& _vec);
	static CVector4& Transform(CVector4& _ret, const CMatrix44& _m0, const CVector4& _vec);

	// ]usñ
	static CMatrix44& Transpose(CMatrix44& _ret, const CMatrix44& _m0);
	
	// tsñ
	static CMatrix44& Inverse(CMatrix44& _ret, const CMatrix44& _m0);

	// ViewsñÌì¬
	static void CreateLookAt(const CVector3& _cameraPosition, const CVector3& _cameraTarget, const CVector3& _cameraUpVector, CMatrix44& _result);

	// §esñÌì¬
	static void CreatePerspective(const f32 _width, const f32 _height, const f32 _nearClip, const f32 _farClip, CMatrix44& _result);
	static void CreatePerspectiveFieldOfView(const f32 fieldOfView, const f32 aspectRatio, const f32 _nearClip, const f32 _farClip, CMatrix44& _result);

	// ½sesñÌì¬
	static void CreateOrthographic(const f32 _width, const f32 _height, const f32 _nearClip, const f32 _farClip, CMatrix44 &_result);

	// vg
	static void PrintMatrix(const CMatrix44& _mtx);
};

LIB_KATA_END