export module Vec3;

import Utility;

import <cmath>;
import <iostream>;
import <array>;

export struct Vec3
{
			Vec3() = default;
			Vec3(float x, float y, float z);

	float	e[3]{ 0.0f, 0.0f, 0.0f };

	float	X() const { return e[0]; }
	float	Y() const { return e[1]; }
	float	Z()	const { return e[2]; }

	float	R() const { return e[0]; }
	float	G()	const { return e[1]; }
	float	B() const { return e[2]; }

	Vec3	operator-() const;
	float	operator[](int i) const;
	float&	operator[](int i);

	Vec3&	operator+=(const Vec3& rhs);
	Vec3&	operator*=(float t);
	Vec3&	operator/=(float t);

	float	Length() const;
	float	SqrMagnitude() const;
};

export inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

export inline Vec3 operator+(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.e[0] + b.e[0],
				 a.e[1] + b.e[1],
				 a.e[2] + b.e[2] };
}

export inline Vec3 operator-(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.e[0] - b.e[0],
				 a.e[1] - b.e[1],
				 a.e[2] - b.e[2] };
}

export inline Vec3 operator*(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.e[0] * b.e[0],
				 a.e[1] * b.e[1],
				 a.e[2] * b.e[2] };
}

export inline Vec3 operator*(float t, const Vec3& v)
{
	return Vec3{ v.e[0] * t,
				 v.e[1] * t,
				 v.e[2] * t };
}

export inline Vec3 operator*(const Vec3& v, float t)
{
	return t * v;
}

export inline Vec3 operator/(const Vec3& v, float t)
{
	return (1 / t) * v;
}

export inline float Dot(const Vec3& a, const Vec3& b)
{
	return	a.e[0] * b.e[0] +
			a.e[1] * b.e[1] +
			a.e[2] * b.e[2];
}

export inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.e[1] * b.e[2] - a.e[2] * b.e[1],
				 a.e[2] * b.e[0] - a.e[0] * b.e[2],
				 a.e[0] * b.e[1] - a.e[1] * b.e[0] };
}

export inline Vec3 Normalized(const Vec3& v)
{
	return v / v.Length();
}

Vec3& Vec3::operator+=(const Vec3& rhs)
{
	for (int i = 0; i < 3; ++i)
	{
		e[i] += rhs.e[i];
	}

	return *this;
}

Vec3& Vec3::operator*=(float t)
{
	for (int i = 0; i < 3; ++i)
	{
		e[i] *= t;
	}

	return *this;
}

Vec3& Vec3::operator/=(float t)
{
	return *this *= 1 / t;
}

float Vec3::Length() const
{
	return std::sqrtf(SqrMagnitude());
}

float Vec3::SqrMagnitude() const
{
	return	e[0] * e[0] +
			e[1] * e[1] +
			e[2] * e[2];
}

Vec3::Vec3(float x, float y, float z)
	: e{ x, y, z }
	{}

Vec3 Vec3::operator-() const
{
	return Vec3(-e[0], -e[1], -e[2]);
}

float Vec3::operator[](int i) const
{
	return e[i];
}

float& Vec3::operator[](int i)
{
	return e[i];
}

export Vec3 GetRandomVec3()
{
	return Vec3(GetRandomFloat(), GetRandomFloat(), GetRandomFloat());
}

Vec3 GetRandomVec3(float min, float max)
{
	return Vec3(GetRandomFloat(min, max),
		GetRandomFloat(min, max),
		GetRandomFloat(min, max));
}

Vec3 GetRandomVec3InUnitSphere()
{
	const Vec3 p = GetRandomVec3(-1.0f, 1.0f);
	if (p.SqrMagnitude() < 1.0f)
	{
		return p;
	}

	return Normalized(p);
}

bool didCache{ false };
std::array<Vec3, 256> randomVec3s;
int randomVecIndex{ 0 };

void InitializeArrayCache()
{
	for (int i = 0; i < 256; ++i)
	{
		randomVec3s[i] = Normalized(GetRandomVec3InUnitSphere());
	}

	didCache = true;
}

export Vec3 Reflect(const Vec3& v, const Vec3& n)
{
	return v - 2 * Dot(v, n) * n;
}

/// <summary>
/// OK, so how does this work?
/// Snell's law: given an incoming ray with angle theta
/// to the normal, and and outgoing ray (which we're looking for)
/// with an angle theta' to the normal, and the refractive indices
/// eta and eta' (eta on the incoming side (e.g. air), eta' on the
/// outgoing side (e.g. inside glas)), the following is true:
/// eta * sin(theta) = eta' * sin(theta')
/// 
/// Solving for sin(theta') (looking for the outgoing angle):
/// sin(theta') = (eta / eta') * sin(theta)
/// 
/// We can split the outgoing ray (R') into (R'pe) and (R'pa), where
/// (R'pe) is the ray's component perpendicular to the surface normal,
/// and (R'pa) is the component parallel with the surface normal.
/// 
/// R'pe = (eta / eta')*(R + cos(theta) * n), and
/// R'pa = -sqrtf(1 - (R'pe)^2) * n
///
/// But, assuming that R is a unit vector, cos(theta) can be
/// expressed as: Dot(-R, n), hence:
/// 
/// R'pe = (eta / eta') * (R + Dot(-R, n) * n)
/// </summary>
/// <param name="uv"></param>
/// <param name="normal"></param>
/// <param name="etaI_over_etaT"></param>
/// <returns></returns>
export Vec3 Refract(const Vec3& uv, const Vec3& normal,
	float etaI_over_etaT)
{
	const float cosTheta = fminf(Dot(-uv, normal), 1.0f);
	const Vec3 rOutPerp = etaI_over_etaT * (uv + cosTheta * normal);
	const Vec3 rOutPar = -sqrtf(fabsf(1.0f - rOutPerp.SqrMagnitude())) * normal;
	return rOutPerp + rOutPar;
}

export Vec3 GetRandomUnitVec3()
{
	if (!didCache)
	{
		InitializeArrayCache();
	}

	const Vec3 vec = randomVec3s[randomVecIndex];
	randomVecIndex = (randomVecIndex + 1) % 256;
	return vec;
}

export Vec3 GetRandomVec3OnHemisphere(const Vec3 surfaceNormal)
{
	const Vec3 unitVector = GetRandomUnitVec3();
	if (Dot(unitVector, surfaceNormal) > 0.0f)
	{
		return unitVector;
	}

	return -unitVector;
}

export Vec3 GetRandomVectorInUnitDisk()
{
	const Vec3 vec = GetRandomVec3();

	if (vec.SqrMagnitude() > 1.0f)
	{
		const float length = vec.Length();
		const float randomDiv = 1.0f + GetRandomFloat();
		return vec / length;
	}

	return vec;
}