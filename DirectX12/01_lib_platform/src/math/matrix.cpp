#include "math/matrix.h"
#include <cmath>

LIB_KATA_BEGIN

//===================================================================================================
// Matrix4x4

CMatrix44::CMatrix44()
{
	for (u32 idx = 0; idx < 4; ++idx)
	{
		column[idx] = CVector4::Zero;
	}
}

CMatrix44::~CMatrix44()
{
}

f32* CMatrix44::operator [] (u32 _idx)
{
	return column[_idx].Get();
}

const f32* CMatrix44::operator [] (u32 _idx) const
{
	return column[_idx].Get();
}

f32& CMatrix44::operator () (u32 _idx, u32 _idx2)
{
	return column[_idx][_idx2];
}

CMatrix44& CMatrix44::operator=(const CMatrix44& _m)
{
	for (int i = 0; i < 4; i++)
	{
		this->column[i] = _m.column[i];
	}
	return *this;
}

CMatrix44 CMatrix44::operator+()
{
	return *this;
}

CMatrix44 CMatrix44::operator-()
{
	CMatrix44 ret;
	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			ret[i][j] = -*this[i][j];
		}
	}
	return ret;
}

CMatrix44& CMatrix44::operator+=(const CMatrix44& _m)
{
	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			this->column[i][j] += _m[i][j];
		}
	}
	return *this;
}

CMatrix44& CMatrix44::operator-=(const CMatrix44& _m)
{
	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			this->column[i][j] -= _m[i][j];
		}
	}
	return *this;
}

CMatrix44& CMatrix44::operator*=(const f32 _k)
{
	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			this->column[i][j] *= _k;
		}
	}
	return *this;
}

CMatrix44& CMatrix44::operator/=(const f32 _k)
{
	Assert(_k != 0);
	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			this->column[i][j] /= _k;
		}
	}
	return *this;
}

//===================================================================================================
// MatrixUtil

CMatrix44& MatrixUtil::Identity(CMatrix44& _m)
{
	_m = CMatrix44();
	_m[0][0] = _m[1][1] = _m[2][2] = _m[3][3] = 1.0f;
	return _m;
}

// ägëÂçsóÒ
CMatrix44& MatrixUtil::Scale(CMatrix44& _m, const CVector3& _scale)
{
	_m[0][0] = _scale[0];
	_m[1][1] = _scale[1];
	_m[2][2] = _scale[2];
	return _m;
}

CMatrix44& MatrixUtil::Scale(CMatrix44& _m, const f32 _scale)
{
	_m[0][0] = _scale;
	_m[1][1] = _scale;
	_m[2][2] = _scale;
	return _m;

}

CMatrix44& MatrixUtil::ScaleX(CMatrix44& _m, const f32 _scale)
{
	_m[0][0] = _scale;
	return _m;
}

CMatrix44& MatrixUtil::ScaleY(CMatrix44& _m, const f32 _scale)
{
	_m[1][1] = _scale;
	return _m;
}

CMatrix44& MatrixUtil::ScaleZ(CMatrix44& _m, const f32 _scale)
{
	_m[2][2] = _scale;
	return _m;
}

// âÒì]
CMatrix44& MatrixUtil::RotX(CMatrix44& _m, f32 _rad)
{
	_m[1][1] = ::cosf(_rad);
	_m[1][2] = ::sinf(_rad);
	_m[2][1] = -::sinf(_rad);
	_m[2][2] = ::cosf(_rad);
	return _m;
}

CMatrix44& MatrixUtil::RotY(CMatrix44& _m, f32 _rad)
{
	_m[0][0] = ::cosf(_rad);
	_m[0][2] = -::sinf(_rad);
	_m[2][0] = ::sinf(_rad);
	_m[2][2] = ::cosf(_rad);
	return _m;
}

CMatrix44& MatrixUtil::RotZ(CMatrix44& _m, f32 _rad)
{
	_m[0][0] = ::cosf(_rad);
	_m[0][1] = ::sinf(_rad);
	_m[1][0] = -::sinf(_rad);
	_m[1][1] = ::cosf(_rad);
	return _m;
}

// à⁄ìÆ
CMatrix44& MatrixUtil::SetTrans(CMatrix44& _m, const CVector3& _vec)
{
	_m[3][0] = _vec[0];
	_m[3][1] = _vec[1];
	_m[3][2] = _vec[2];
	return _m;
}

CMatrix44& MatrixUtil::SetTransX(CMatrix44& _m, const f32 _vec)
{
	_m[3][0] = _vec;
	return _m;
}

CMatrix44& MatrixUtil::SetTransY(CMatrix44& _m, const f32 _vec)
{
	_m[3][1] = _vec;
	return _m;
}

CMatrix44& MatrixUtil::SetTransZ(CMatrix44& _m, const f32 _vec)
{
	_m[3][2] = _vec;
	return _m;
}

// çsóÒìØémÇÃä|ÇØéZ
CMatrix44& MatrixUtil::Mul(CMatrix44& _ret, const CMatrix44& _m0, const CMatrix44& _m1)
{
	u32 num = 4;
	for (u32 idx = 0; idx < num; ++idx)
	{
		for (u32 idx2 = 0; idx2 < num; ++idx2)
		{
			for (u32 idx3 = 0; idx3 < num; ++idx3)
			{
				_ret[idx][idx2] += _m0[idx][idx3] * _m1[idx3][idx2];
			}
		}
	}
	return _ret;
}

// ÉxÉNÉgÉãÇÃï‘ä“
CVector3& MatrixUtil::Transform(CVector3& _ret, const CMatrix44& _m0, const CVector3& _vec)
{
	u32 num = 3;
	for (u32 idx = 0; idx < num; ++idx)
	{
		for (u32 idx2 = 0; idx2 < num; ++idx2)
		{
			_ret[idx] += _m0[idx][idx2] * _vec[idx2];
		}
	}

	// ïΩçsà⁄ìÆê¨ï™ÇÕïKÇ∏îΩâf
	_ret[0] += _m0[3][0];
	_ret[1] += _m0[3][1];
	_ret[2] += _m0[3][2];
	return _ret;
}

CVector4& MatrixUtil::Transform(CVector4& _ret, const CMatrix44& _m0, const CVector4& _vec)
{
	u32 num = 4;
	for (u32 idx = 0; idx < num; ++idx)
	{
		for (u32 idx2 = 0; idx2 < num; ++idx2)
		{
			_ret[idx] += _m0[idx][idx2] * _vec[idx2];
		}
	}
	return _ret;
}

LIB_KATA_END
