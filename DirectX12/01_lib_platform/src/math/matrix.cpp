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

// 拡大行列
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

// 回転
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

// 移動
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

// 行列同士の掛け算
CMatrix44& MatrixUtil::Mul(CMatrix44& _ret, const CMatrix44& _m0, const CMatrix44& _m1)
{
	u32 num = 4;
	CMatrix44 temp;
	for (u32 idx = 0; idx < num; ++idx)
	{
		for (u32 idx2 = 0; idx2 < num; ++idx2)
		{
			for (u32 idx3 = 0; idx3 < num; ++idx3)
			{
				temp[idx][idx2] += _m0[idx][idx3] * _m1[idx3][idx2];
			}
		}
	}
	_ret = temp;
	return _ret;
}

// ベクトルの返還
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

	// 平行移動成分は必ず反映
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
		_ret[idx] = VectorUtil::Dot(_m0[idx], _vec);
	}
	return _ret;
}

// 転置
CMatrix44& MatrixUtil::Transpose(CMatrix44& _ret, const CMatrix44& _m0)
{
	u32 num = 4;
	for (u32 idx = 0; idx < num; ++idx)
	{
		for (u32 idx2 = 0; idx2 < num; ++idx2)
		{
			_ret[idx][idx2] = _m0[idx2][idx];
		}
	}
	return _ret;
}

// 逆行列
CMatrix44& MatrixUtil::Inverse(CMatrix44& _ret, const CMatrix44& _m0)
{
	// 戻り値を単位行列
	Identity(_ret);
	CMatrix44 gm = _m0;

	u32 num = 4;
	for (u32 colum = 0; colum < num; ++colum)
	{
		u32 pivotRow = 0;
		f32 max = 0.0f;

		// ピボットの選定
		// 前の行は終わってるので次の項目からいく
		for (u32 idx2 = colum; idx2 < num; ++idx2)
		{
			if (fabs(gm[idx2][colum]) > max)
			{
				max = fabs(gm[idx2][colum]);
				pivotRow = idx2;
			}
		}
		if (max == 0.0) { return _ret; }

		// 対角線以外なら入れ替え
		if (colum != pivotRow) 
		{
			// 結果と入ってきた行列どちらも入れ替え
			for (u32 i = 0; i < num; ++i)
			{
				f32 temp = gm[colum][i];
				gm[colum][i] = gm[pivotRow][i];
				gm[pivotRow][i] = temp;
				
				temp = _ret[colum][i];
				_ret[colum][i] = _ret[pivotRow][i];
				_ret[pivotRow][i] = temp;
			}
		}

		// 対角成分を1,0にして他を合わす
		f32 invPivot = 1.0f / gm[colum][colum];
		for (u32 j = 0; j < num; ++j) 
		{
			gm[colum][j] *= invPivot;
			_ret[colum][j] *= invPivot;
		}

		// 最終的に逆行列にするためピボット列を０にしていく（対角線除く）
		for (u32 i = 0; i < num; ++i)
		{
			if (i == colum) { continue; }

			f32 temp = gm[i][colum];
			for (u32 j = 0; j < num; ++j)
			{
				gm[i][j] -= temp * gm[colum][j];
				_ret[i][j] -= temp * _ret[colum][j];
			}
		}
	}

	return _ret;
}

void MatrixUtil::CreateLookAt(const CVector3& _cameraPosition, const CVector3& _cameraTarget, const CVector3& _cameraUpVector, CMatrix44& _result)
{
	CVector3 zaxis = _cameraPosition - _cameraTarget;
	zaxis = VectorUtil::Normalize(zaxis);

	CVector3 xaxis = VectorUtil::Cross(_cameraUpVector, zaxis);
	xaxis = VectorUtil::Normalize(xaxis);

	CVector3 yaxis = VectorUtil::Cross(zaxis, xaxis);
	yaxis = VectorUtil::Normalize(yaxis);

	_result[0][0] = xaxis.GetX();
	_result[0][1] = yaxis.GetX();
	_result[0][2] = zaxis.GetX();
	_result[0][3] = 0.0f;

	_result[1][0] = xaxis.GetY();
	_result[1][1] = yaxis.GetY();
	_result[1][2] = zaxis.GetY();
	_result[1][3] = 0.0f;

	_result[2][0] = xaxis.GetZ();
	_result[2][1] = yaxis.GetZ();
	_result[2][2] = zaxis.GetZ();
	_result[2][3] = 0.0f;

	_result[3][0] = -VectorUtil::Dot(xaxis, _cameraPosition);
	_result[3][1] = -VectorUtil::Dot(yaxis, _cameraPosition);
	_result[3][2] = -VectorUtil::Dot(zaxis, _cameraPosition);
	_result[3][3] = 1.0f;
}

void MatrixUtil::CreatePerspective(const f32 _width, const f32 _height, const f32 _nearClip, const f32 _farClip, CMatrix44& _result)
{
	Assert(_width != 0.0f);
	Assert(_height != 0.0f);
	register f32 diff = _nearClip - _farClip;
	Assert(diff != 0.0f);

	_result[0][0] = 2.0f * _nearClip / _width;
	_result[0][1] = 0.0f;
	_result[0][2] = 0.0f;
	_result[0][3] = 0.0f;

	_result[1][0] = 0.0f;
	_result[1][1] = 2.0f * _nearClip / _height;
	_result[1][2] = 0.0f;
	_result[1][3] = 0.0f;

	_result[2][0] = 0.0f;
	_result[2][1] = 0.0f;
	_result[2][2] = _farClip / diff;
	_result[2][3] = -1.0f;

	_result[3][0] = 0.0f;
	_result[3][1] = 0.0f;
	_result[3][2] = (_nearClip * _farClip) / diff;
	_result[3][3] = 0.0f;
}

void MatrixUtil::CreatePerspectiveFieldOfView(const f32 fieldOfView, const f32 aspectRatio, const f32 _nearClip, const f32 _farClip, CMatrix44& _result)
{
	assert(aspectRatio != 0.0f);
	register f32 diff = _nearClip - _farClip;
	assert(diff != 0.0f);
	register f32 yScale = 1.0f / tanf(fieldOfView / 2.0f);
	register f32 xScale = yScale / aspectRatio;

	_result[0][0] = xScale;
	_result[0][1] = 0.0f;
	_result[0][2] = 0.0f;
	_result[0][3] = 0.0f;

	_result[1][0] = 0.0f;
	_result[1][1] = yScale;
	_result[1][2] = 0.0f;
	_result[1][3] = 0.0f;

	_result[2][0] = 0.0f;
	_result[2][1] = 0.0f;
	_result[2][2] = _farClip / diff;
	_result[2][3] = -1.0f;

	_result[3][0] = 0.0f;
	_result[3][1] = 0.0f;
	_result[3][2] = (_nearClip * _farClip) / diff;
	_result[3][3] = 0.0f;

}

void MatrixUtil::CreateOrthographic(const f32 _width, const f32 _height, const f32 _nearClip, const f32 _farClip, CMatrix44 &_result)
{
	Assert(_width != 0.0f);
	Assert(_height != 0.0f);
	register f32 diffNF = _nearClip - _farClip;
	Assert(diffNF != 0.0f);

	_result[0][0] = 2.0f * _width;
	_result[0][1] = 0.0f;
	_result[0][2] = 0.0f;
	_result[0][3] = 0.0f;

	_result[1][0] = 0.0f;
	_result[1][1] = 2.0f * _height;
	_result[1][2] = 0.0f;
	_result[1][3] = 0.0f;

	_result[2][0] = 0.0f;
	_result[2][1] = 0.0f;
	_result[2][2] = 1.0f / diffNF;
	_result[2][3] = 0.0f;

	_result[3][0] = 0.0f;
	_result[3][1] = 0.0f;
	_result[3][2] = _nearClip / diffNF;
	_result[3][3] = 1.0f;
}

void MatrixUtil::PrintMatrix(const CMatrix44& _mtx)
{
	for (u32 idx = 0; idx < 4; ++idx)
	{
		PRINT("%d: %f, %f, %f, %f\n", idx, _mtx[idx][0], _mtx[idx][1], _mtx[idx][2], _mtx[idx][3]);
	}
}

LIB_KATA_END
