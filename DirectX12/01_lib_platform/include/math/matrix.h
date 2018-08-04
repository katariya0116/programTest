#pragma once

#include "math/vector.h"

LIB_KATA_BEGIN

// 一旦4x4のみ用意
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

// 行列ユーティリティ
class MatrixUtil
{
public:
	static CMatrix44& Identity(CMatrix44& _m);

	// スケール
	static CMatrix44& Scale(CMatrix44& _m, const CVector3& _scale);
	static CMatrix44& Scale(CMatrix44& _m, const f32 _scale);

	static CMatrix44& ScaleX(CMatrix44& _m, const f32 _scale);
	static CMatrix44& ScaleY(CMatrix44& _m, const f32 _scale);
	static CMatrix44& ScaleZ(CMatrix44& _m, const f32 _scale);

	// 回転行列
	static CMatrix44& RotX(CMatrix44& _m, f32 _rad);
	static CMatrix44& RotY(CMatrix44& _m, f32 _rad);
	static CMatrix44& RotZ(CMatrix44& _m, f32 _rad);

	// 移動
	static CMatrix44& SetTrans(CMatrix44& _m, const CVector3& _vec);
	static CMatrix44& SetTransX(CMatrix44& _m, const f32 _vec);
	static CMatrix44& SetTransY(CMatrix44& _m, const f32 _vec);
	static CMatrix44& SetTransZ(CMatrix44& _m, const f32 _vec);

	// 行列の掛け算
	static CMatrix44& Mul(CMatrix44& _ret, const CMatrix44& _m0, const CMatrix44& _m1);

	// 行列によるベクトル変化
	static CVector3& Transform(CVector3& _ret, const CMatrix44& _m0, const CVector3& _vec);
	static CVector4& Transform(CVector4& _ret, const CMatrix44& _m0, const CVector4& _vec);

	// 転置行列
	static CMatrix44& Transpose(CMatrix44& _ret, const CMatrix44& _m0);
	
	// 逆行列
	static CMatrix44& Inverse(CMatrix44& _ret, const CMatrix44& _m0);

	// プリント
	static void PrintMatrix(const CMatrix44& _mtx);
};

LIB_KATA_END