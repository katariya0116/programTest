#include "math/vector.h"
#include <string.h>
#include <math.h>

LIB_KATA_BEGIN

//===================================================================================================
// CVector2

// ƒxƒNƒgƒ‹’è”
const CVector2 CVector2::Zero	= CVector2(0.0f, 0.0f);
const CVector2 CVector2::Up		= CVector2(0.0f, 1.0f);
const CVector2 CVector2::Down	= CVector2(0.0f, -1.0f);
const CVector2 CVector2::Left	= CVector2(-1.0f,0.0f);
const CVector2 CVector2::Right	= CVector2(1.0f, 0.0f);

CVector2::CVector2()
{
	memset(pos, 0.0f, sizeof(pos));
}

CVector2::CVector2(f32 _x, f32 _y)
	:x(_x), y(_y)
{
}

CVector2::CVector2(const f32* _pos)
{
	memcpy(pos, _pos, sizeof(f32) * 2);
}

CVector2::~CVector2()
{
}

f32& CVector2::operator [](u32 _idx)
{
	return pos[_idx];
}

const f32 CVector2:: operator [](u32 _idx) const
{
	return pos[_idx];
}

//‘ã“ü‰‰ŽZŽq‚Ì’è‹`
CVector2& CVector2::operator=(const CVector2& v)
{
	this->x = v.x;
	this->y = v.y;
	return *this;
}

//+=‚Ì’è‹`	v+=
CVector2& CVector2::operator+=(const CVector2& v)
{
	this->x += v.x;
	this->y += v.y;
	return *this;
}

//-=‚Ì’è‹`	v-=
CVector2& CVector2::operator-=(const CVector2& v)
{
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

//’è””{
CVector2& CVector2::operator*=(f32 _k)
{
	this->x *= _k;
	this->y *= _k;
	return *this;
}

CVector2 CVector2::operator*(f32 _k) const
{
	CVector2 ret;
	ret.x = this->x * _k;
	ret.y = this->y * _k;
	return ret;
}

// ’è”Š„
CVector2& CVector2::operator/=(f32 _k)
{
	Assert(_k != 0.0f);
	this->x /= _k;
	this->y /= _k;
	return *this;
}

CVector2 CVector2::operator/(f32 _k) const
{
	Assert(_k != 0.0f);
	CVector2 ret;
	ret.x = this->x / _k;
	ret.y = this->y / _k;
	return ret;
}
// ³•‰”½“]
CVector2 CVector2::operator+()
{
	return *this;
}
CVector2 CVector2::operator-()
{
	return CVector2(-x, -y);
}

// ‘«‚µŽZˆø‚«ŽZ
CVector2 CVector2::operator+(const CVector2& _v) const
{
	CVector2 ret;
	ret.x = this->x + _v.x;
	ret.y = this->y + _v.y;
	return ret;
}

CVector2 CVector2::operator-(const CVector2& _v) const
{
	CVector2 ret;
	ret.x = this->x - _v.x;
	ret.y = this->y - _v.y;
	return ret;
}

//===================================================================================================
//

// ƒxƒNƒgƒ‹’è”
const CVector3 CVector3::Zero	= CVector3(0.0f, 0.0f, 0.0f);
const CVector3 CVector3::Up		= CVector3(0.0f, 1.0f, 0.0f);
const CVector3 CVector3::Down	= CVector3(0.0f, -1.0f, 0.0f);
const CVector3 CVector3::Left	= CVector3(-1.0f, 0.0f, 0.0f);
const CVector3 CVector3::Right	= CVector3(1.0f, 0.0f, 0.0f);
const CVector3 CVector3::Foward = CVector3(0.0f, 0.0f, 1.0f);
const CVector3 CVector3::Back	= CVector3(0.0f, 0.0f, -1.0f);

CVector3::CVector3()
{
	memset(pos, 0.0f, sizeof(pos));
}

CVector3::CVector3(f32 _x, f32 _y, f32 _z)
	:x(_x), y(_y), z(_z)
{
}

CVector3::CVector3(const f32* _pos)
{
	memcpy(pos, _pos, sizeof(f32) * 3);
}

CVector3::~CVector3()
{
}

f32& CVector3::operator [](u32 _idx)
{
	return pos[_idx];
}

const f32 CVector3:: operator [](u32 _idx) const
{
	return pos[_idx];
}

//‘ã“ü‰‰ŽZŽq‚Ì’è‹`
CVector3& CVector3::operator=(const CVector3& v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;

	return *this;
}

//+=‚Ì’è‹`	v+=
CVector3& CVector3::operator+=(const CVector3& v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;

	return *this;
}

//-=‚Ì’è‹`	v-=
CVector3& CVector3::operator-=(const CVector3& v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;

	return *this;
}

//’è””{
CVector3& CVector3::operator*=(f32 _k)
{
	this->x *= _k;
	this->y *= _k;
	this->z *= _k;
	return *this;
}
CVector3 CVector3::operator*(f32 _k) const
{
	CVector3 ret;
	ret.x = this->x * _k;
	ret.y = this->y * _k;
	ret.z = this->z * _k;
	return ret;
}

// ’è”Š„
CVector3& CVector3::operator/=(f32 _k)
{
	Assert(_k != 0.0f);
	this->x /= _k;
	this->y /= _k;
	this->z /= _k;

	return *this;
}
CVector3 CVector3::operator/(f32 _k) const
{
	Assert(_k != 0.0f);
	CVector3 ret;
	ret.x = this->x / _k;
	ret.y = this->y / _k;
	ret.z = this->z / _k;
	return ret;
}

// ³•‰”½“]
CVector3 CVector3::operator+()
{
	return *this;
}
CVector3 CVector3::operator-()
{
	return CVector3(-x, -y, -z);
}

// ‘«‚µŽZˆø‚«ŽZ
CVector3 CVector3::operator+(const CVector3& _v) const
{
	CVector3 ret;
	ret.x = this->x + _v.x;
	ret.y = this->y + _v.y;
	ret.z = this->z + _v.z;
	return ret;
}

CVector3 CVector3::operator-(const CVector3& _v) const
{
	CVector3 ret;
	ret.x = this->x - _v.x;
	ret.y = this->y - _v.y;
	ret.z = this->z - _v.z;
	return ret;
}

//===================================================================================================
//

// ƒxƒNƒgƒ‹’è”
const CVector4 CVector4::Zero	= CVector4(0.0f, 0.0f, 0.0f, 0.0f);
const CVector4 CVector4::Up		= CVector4(0.0f, 1.0f, 0.0f, 0.0f);
const CVector4 CVector4::Down	= CVector4(0.0f, -1.0f, 0.0f, 0.0f);
const CVector4 CVector4::Left	= CVector4(-1.0f, 0.0f, 0.0f, 0.0f);
const CVector4 CVector4::Right	= CVector4(1.0f, 0.0f, 0.0f, 0.0f);
const CVector4 CVector4::Foward = CVector4(0.0f, 0.0f, 1.0f, 0.0f);
const CVector4 CVector4::Back	= CVector4(0.0f, 0.0f, -1.0f, 0.0f);

CVector4::CVector4()
{
	memset(pos, 0.0f, sizeof(pos));
}

CVector4::CVector4(f32 _x, f32 _y, f32 _z, f32 _w)
	:x(_x), y(_y), z(_z), w(_w)
{
}

CVector4::CVector4(const f32* _pos)
{
	memcpy(pos, _pos, sizeof(f32) * 4);
}

CVector4::~CVector4()
{
}

f32& CVector4::operator [](u32 _idx)
{
	return pos[_idx];
}

const f32 CVector4:: operator [](u32 _idx) const
{
	return pos[_idx];
}

CVector4& CVector4::operator=(const CVector4& v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = v.w;

	return *this;
}

//+=‚Ì’è‹`	v+=
CVector4& CVector4::operator+=(const CVector4& v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	this->w += v.w;

	return *this;
}

//-=‚Ì’è‹`	v-=
CVector4& CVector4::operator-=(const CVector4& v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	this->w += v.w;

	return *this;
}

//’è””{
CVector4& CVector4::operator*=(f32 _k)
{
	this->x *= _k;
	this->y *= _k;
	this->z *= _k;
	this->w *= _k;
	return *this;
}
CVector4 CVector4::operator*(f32 _k) const
{
	CVector4 ret;
	ret.x = this->x * _k;
	ret.y = this->y * _k;
	ret.z = this->z * _k;
	ret.w = this->w * _k;
	return ret;
}


// ’è”Š„
CVector4& CVector4::operator/=(f32 _k)
{
	Assert(_k != 0.0f);
	this->x /= _k;
	this->y /= _k;
	this->z /= _k;
	this->w *= _k;

	return *this;
}
CVector4 CVector4::operator/(f32 _k) const
{
	Assert(_k != 0.0f);
	CVector4 ret;
	ret.x = this->x / _k;
	ret.y = this->y / _k;
	ret.z = this->z / _k;
	ret.w = this->w / _k;
	return ret;
}

// ³•‰”½“]
CVector4 CVector4::operator+()
{
	return *this;
}
CVector4 CVector4::operator-()
{
	return CVector4(-x, -y, -z, -w);
}

// ‘«‚µŽZˆø‚«ŽZ
CVector4 CVector4::operator+(const CVector4& _v) const
{
	CVector4 ret;
	ret.x = this->x + _v.x;
	ret.y = this->y + _v.y;
	ret.z = this->z + _v.z;
	ret.w = this->w + _v.w;
	return ret;
}

CVector4 CVector4::operator-(const CVector4& _v) const
{
	CVector4 ret;
	ret.x = this->x - _v.x;
	ret.y = this->y - _v.y;
	ret.z = this->z - _v.z;
	ret.w = this->w - _v.w;
	return ret;
}

//===================================================================================================
//
f32 VectorUtil::Dot(const CVector2& _a, const CVector2& _b)
{
	return _a[0] * _b[0] + _a[1] * _b[1];
}

f32 VectorUtil::Dot(const CVector3& _a, const CVector3& _b)
{
	return _a[0] * _b[0] + _a[1] * _b[1] + _a[2] * _b[2];
}

f32 VectorUtil::Dot(const CVector4& _a, const CVector4& _b)
{
	return _a[0] * _b[0] + _a[1] * _b[1] + _a[2] * _b[2] + _a[3] * _b[3];
}

f32& VectorUtil::Cross(const CVector2& _a, const CVector2& _b, f32& _ret)
{
	_ret = _a[0] * _b[1] - _a[1] * _b[0];
	return _ret;
}

CVector3& VectorUtil::Cross(const CVector3& _a, const CVector3& _b, CVector3& _ret)
{
	_ret[0] = _a[1] * _b[2] - _a[2] * _b[1];
	_ret[1] = _a[2] * _b[0] - _a[0] * _b[2];
	_ret[2] = _a[0] * _b[1] - _a[1] * _b[0];
	return _ret;
}

f32 VectorUtil::Cross(const CVector2& _a, const CVector2& _b)
{
	f32 ret = 0.0f;
	ret = _a[0] * _b[1] - _a[1] * _b[0];
	return ret;
}

CVector3 VectorUtil::Cross(const CVector3& _a, const CVector3& _b)
{
	CVector3 ret;
	ret[0] = _a[1] * _b[2] - _a[2] * _b[1];
	ret[1] = _a[2] * _b[0] - _a[0] * _b[2];
	ret[2] = _a[0] * _b[1] - _a[1] * _b[0];
	return ret;
}

f32 VectorUtil::LengthSquare(const CVector2& _v)
{
	return _v[0] * _v[0] + _v[1] * _v[1];
}

f32 VectorUtil::LengthSquare(const CVector3& _v)
{
	return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2];
}

f32 VectorUtil::LengthSquare(const CVector4& _v)
{
	return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3];
}

f32 VectorUtil::Length(const CVector2& _v)
{
	return ::sqrtf(LengthSquare(_v));
}

f32 VectorUtil::Length(const CVector3& _v)
{
	return ::sqrtf(LengthSquare(_v));
}

f32 VectorUtil::Length(const CVector4& _v)
{
	return ::sqrtf(LengthSquare(_v));
}

CVector2 VectorUtil::Normalize(const CVector2& _v)
{
	f32 len = Length(_v);
	Assert(len != 0);
	return _v / len;
}

CVector3 VectorUtil::Normalize(const CVector3& _v)
{
	f32 len = Length(_v);
	Assert(len != 0);
	return _v / len;
}
CVector4 VectorUtil::Normalize(const CVector4& _v)
{
	f32 len = Length(_v);
	Assert(len != 0);
	return _v / len;
}

LIB_KATA_END