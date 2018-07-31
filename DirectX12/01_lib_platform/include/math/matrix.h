#pragma once

#include "math/vector.h"

LIB_KATA_BEGIN

class LIB_ALIGNED(16) CMatrix32
{
private:
	CVector3 row[2];
};

class LIB_ALIGNED(16) CMatrix33
{
private:
	CVector3 row[3];
};

class LIB_ALIGNED(16) CMatrix44
{
private:
	CVector4 row[4];
};

class MatrixUtil
{
};

LIB_KATA_END