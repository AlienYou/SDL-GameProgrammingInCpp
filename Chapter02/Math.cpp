#include "Math.h"

const Vector2 Vector2::Zero(0.0f, 0.0f);
const Vector2 Vector2::UnitX(1.0f, 0.0f);
const Vector2 Vector2::UnitY(0.0f, 1.0f);
const Vector2 Vector2::NegUnitX(-1.0f, 0.0f);
const Vector2 Vector2::NegUnitY(0.0f, -1.0f);

const Vector3 Vector3::Zero(0.0f, 0.0f, 0.f);
const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::NegUnitX(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::NegUnitY(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::NegUnitZ(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::Infinity(Math::Infinity, Math::Infinity, Math::Infinity);
const Vector3 Vector3::NegInfinity(Math::NegInfinity, Math::NegInfinity, Math::NegInfinity);

static float m3Ident[3][3] =
{
	{ 1.0, 0.0f, 0.0f, },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f }
};

const Matrix3 Matrix3::Identity(m3Ident);

static float m4Ident[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
};

const Matrix4 Matrix4::Identity(m4Ident);

const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

Vector2 Vector2::Transform(const Vector2& vec, const Matrix3& mat, float w /*= 1.0f*/)
{
	Vector2 retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] + w * mat.mat[2][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] + w * mat.mat[2][1];
	//ignore w since we aren't returning a new value for it...
	return retVal;
}

Vector3 Vector3::Transform(const Vector3& vec, const Matrix4& mat, float w /*= 1.0f*/)
{
	Vector3 retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
		vec.z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
		vec.z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
		vec.z * mat.mat[2][2] + w * mat.mat[3][2];
	//ignore w since we aren't returning a new value for it...
	return retVal;
}

// This will transform the vector and renormalize the w component
Vector3 Vector3::TransformWithPerspDiv(const Vector3& vec, const Matrix4& mat, float w /*= 1.0f*/)
{
	Vector3 retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
		vec.z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
		vec.z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
		vec.z * mat.mat[2][2] + w * mat.mat[3][2];
	float transformedW = vec.x * mat.mat[0][3] + vec.y * mat.mat[1][3] +
		vec.z * mat.mat[2][3] + w * mat.mat[3][3];
	if (!Math::NearZero(Math::Abs(transformedW)))
	{
		transformedW = 1.0f / transformedW;
		retVal *= transformedW;
	}
	return retVal;
}

// Transform a Vector3 by a quaternion
Vector3 Vector3::Transform(const Vector3& v, const Quaternion& q)
{
	// v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
	Vector3 qv(q.x, q.y, q.z);
	Vector3 retVal = v;
	retVal += 2.0f * Vector3::Cross(qv, Vector3::Cross(qv, v) + q.w * v);
	return retVal;
}

Matrix4 Matrix4::CreateFromQuaternion(const class Quaternion& q)
{
	float mat[4][4];
	
	mat[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
	mat[0][1] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
	mat[0][2] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
	mat[0][3] = 0.0f;

	mat[1][0] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
	mat[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	mat[1][2] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
	mat[1][3] = 0.0f;

	mat[2][0] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
	mat[2][1] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
	mat[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
	mat[2][3] = 0.0f;

	mat[3][0] = 0.0f;
	mat[3][1] = 0.0f;
	mat[3][2] = 0.0f;
	mat[3][3] = 1.0f;

	return Matrix4(mat);
}
