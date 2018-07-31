#pragma once

LIB_KATA_BEGIN

class LIB_ALIGNED(16) CVector2
{
public:
	CVector2();
	CVector2(f32 _x, f32 _y);
	CVector2(const f32* _pos);
	virtual ~CVector2();

public:
	void Set(f32 _x, f32 _y) { x = _x; y = _y; }
	void SetX(f32 _x) { x = _x; }
	void SetY(f32 _y) { y = _y; }

public:
	const f32* Get() const { return pos; }
	f32 GetX() const { return x; }
	f32 GetY() const { return y; }

public:
	f32 & operator [](u32 _idx);
	const f32 operator [](u32 _idx) const;
	//‘ã“ü‰‰Zq‚Ì’è‹`
	CVector2& operator=(const CVector2& v);
	//+=‚Ì’è‹`	v+=
	CVector2& operator+=(const CVector2& v);
	//-=‚Ì’è‹`	v-=
	CVector2& operator-=(const CVector2& v);
	//’è””{
	CVector2& operator*=(f32 _k);
	CVector2  operator*(f32 _k) const;
	// ’è”Š„
	CVector2& operator/=(f32 _k);
	CVector2 operator/(f32 _k) const;
	// ³•‰”½“]
	CVector2 operator+();
	CVector2 operator-();
	// ‘«‚µZAˆø‚«Z
	CVector2 operator+(const CVector2& _v) const;
	CVector2 operator-(const CVector2& _v) const;

public:
	static const CVector2 Zero;
	static const CVector2 Up;
	static const CVector2 Down;
	static const CVector2 Left;
	static const CVector2 Right;

private:
	union
	{
		f32 pos[4];
		struct
		{
			f32 x, y;
		};
	};
};

class LIB_ALIGNED(16) CVector3
{
public:
	CVector3();
	CVector3(f32 _x, f32 _y, f32 _z);
	CVector3(const f32* _pos);
	virtual ~CVector3();

public:
	void Set(f32 _x, f32 _y, f32 _z) { x = _x; y = _y; z = _z; }
	void SetX(f32 _x) { x = _x; }
	void SetY(f32 _y) { y = _y; }
	void SetZ(f32 _z) { y = _z; }

public:
	const f32* Get() const { return pos; }
	f32 GetX() const { return x; }
	f32 GetY() const { return y; }
	f32 GetZ() const { return z; }

public:
	f32& operator [](u32 _idx);
	const f32 operator [](u32 _idx) const;
	//‘ã“ü‰‰Zq‚Ì’è‹`
	CVector3& operator=(const CVector3& v);
	//+=‚Ì’è‹`	v+=
	CVector3& operator+=(const CVector3& v);
	//-=‚Ì’è‹`	v-=
	CVector3& operator-=(const CVector3& v);
	//’è””{
	CVector3& operator*=(f32 _k);
	CVector3 operator*(f32 _k) const;
	// ’è”Š„s
	CVector3& operator/=(f32 _k);
	CVector3 operator/(f32 _k) const;

	// ³•‰”½“]
	CVector3 operator+();
	CVector3 operator-();
	// ‘«‚µZAˆø‚«Z
	CVector3 operator+(const CVector3& _v) const;
	CVector3 operator-(const CVector3& _v) const;

public:
	static const CVector3 Zero;
	static const CVector3 Up;
	static const CVector3 Down;
	static const CVector3 Left;
	static const CVector3 Right;
	static const CVector3 Foward;
	static const CVector3 Back;

private:
	union
	{
		f32 pos[4];
		struct
		{
			f32 x, y, z;
		};
	};
};

class LIB_ALIGNED(16) CVector4
{
public:
	CVector4();
	CVector4(f32 _x, f32 _y, f32 _z, f32 _w);
	CVector4(const f32* _pos);
	virtual ~CVector4();

public:
	void Set(f32 _x, f32 _y, f32 _z, f32 _w) { x = _x; y = _y; z = _z; w = _w; }
	void SetX(f32 _x) { x = _x; }
	void SetY(f32 _y) { y = _y; }
	void SetZ(f32 _z) { z = _z; }
	void SetW(f32 _w) { w = _w; }

public:
	const f32* Get() const { return pos; }
	f32 GetX() const { return x; }
	f32 GetY() const { return y; }
	f32 GetZ() const { return z; }
	f32 GetW() const { return w; }

public:
	f32 & operator [](u32 _idx);
	const f32 operator [](u32 _idx) const;
	//‘ã“ü‰‰Zq‚Ì’è‹`
	CVector4& operator=(const CVector4& v);
	//+=‚Ì’è‹`	v+=
	CVector4& operator+=(const CVector4& v);
	//-=‚Ì’è‹`	v-=
	CVector4& operator-=(const CVector4& v);
	//’è””{
	CVector4& operator*=(f32 _k);
	CVector4 operator*(f32 _k) const;
	// ’è”Š„
	CVector4& operator/=(f32 _k);
	CVector4 operator/(f32 _k) const;
	// ³•‰”½“]
	CVector4 operator+();
	CVector4 operator-();
	// ‘«‚µZAˆø‚«Z
	CVector4 operator+(const CVector4& _v) const;
	CVector4 operator-(const CVector4& _v) const;

public:
	static const CVector4 Zero;
	static const CVector4 Up;
	static const CVector4 Down;
	static const CVector4 Left;
	static const CVector4 Right;
	static const CVector4 Foward;
	static const CVector4 Back;

private:
	union
	{
		f32 pos[4];
		struct
		{
			f32 x, y, z, w;
		};
	};

};

// ƒxƒNƒ^[‰‰Z—pŠÖ”ŒQ
class VectorUtil
{
public:
	// “àÏ
	static f32 Dot(const CVector2& _a, const CVector2& _b);
	static f32 Dot(const CVector3& _a, const CVector3& _b);
	static f32 Dot(const CVector4& _a, const CVector4& _b);

	// ŠOÏi–ß‚è’lˆø””Åj
	static f32& Cross(const CVector2& _a, const CVector2& _b, f32& _ret);
	static CVector3& Cross(const CVector3& _a, const CVector3& _b, CVector3& _ret);

	// ŠOÏ
	static f32 Cross(const CVector2& _a, const CVector2& _b);
	static CVector3 Cross(const CVector3& _a, const CVector3& _b);

	// ‚Qæ‚Ì’·‚³
	static f32 LengthSquare(const CVector2& _v);
	static f32 LengthSquare(const CVector3& _v);
	static f32 LengthSquare(const CVector4& _v);

	// ’·‚³
	static f32 Length(const CVector2& _v);
	static f32 Length(const CVector3& _v);
	static f32 Length(const CVector4& _v);

	// ³‹K‰»
	static CVector2 Normalize(const CVector2& _v);
	static CVector3 Normalize(const CVector3& _v);
	static CVector4 Normalize(const CVector4& _v);
};

LIB_KATA_END
