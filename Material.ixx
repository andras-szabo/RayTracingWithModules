#define OUT 

export module Material;

import <cmath>;

import Utility;
import Vec3;
import Ray;

export class Material
{
public:
					Material() = default;
					Material(const Vec3& albedo_);
	virtual			~Material() = default;
		
	virtual	bool	Scatter(const Ray& incomingRay, const Vec3 hitPoint, const Vec3 surfaceNormal, bool isFrontFace,
							OUT Vec3& attenuation, OUT Ray& scatteredRay) const;

	Vec3			albedo;
};

Material::Material(const Vec3& albedo_) : albedo{ albedo_ } {}

bool Material::Scatter(const Ray& incomingRay, const Vec3 hitPoint, const Vec3 surfaceNormal, bool isFrontFace,
					   OUT Vec3& attenuation, OUT Ray& scatteredRay) const
{
	return false;
}

export class MatLambertian : public Material
{
public:
					MatLambertian(const Vec3& albedo_) : Material(albedo_) {}
	virtual bool	Scatter(const Ray& incomingRay, const Vec3 hitPoint, const Vec3 surfaceNormal, bool isFrontFace,
							OUT Vec3& attenuation, OUT Ray& scatteredRay) const override;
};

export class MatMetal : public Material
{
public:
					MatMetal(const Vec3& albedo_, float fuzz_) :
							Material(albedo_), 
							fuzz{ fuzz_ < 1.0f ? fuzz_ : 1.0f } {}
	virtual bool	Scatter(const Ray& incomingRay, const Vec3 hitPoint, const Vec3 surfaceNormal, bool isFrontFace,
						OUT Vec3& attenuation, OUT Ray& scatteredRay) const override;

	float			fuzz{ 0.0f };
};

export class MatDielectric : public Material
{
public:
					MatDielectric(const Vec3& albedo_,
						float indexOfRefraction_) :
						Material(albedo_),
						indexOfRefraction{ indexOfRefraction_ } {}

					MatDielectric(float indexOfRefraction_) :
						Material(Vec3(1.0f, 1.0f, 1.0f)),
						indexOfRefraction{ indexOfRefraction_ } {}

	virtual bool	Scatter(const Ray& incomingRay, const Vec3 hitPoint, const Vec3 surfaceNormal, bool isFrontFace,
						OUT Vec3& attenuation, OUT Ray& scatteredRay) const override;

	static float	Reflectance(float cosine, float refractionRatio);
	float			indexOfRefraction{ 0.0f };
};

float MatDielectric::Reflectance(float cosine, float refractionRatio)
{
	// Using Schlick's approximation:
	float r0 = (1.0f - refractionRatio) / (1.0f + refractionRatio);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}


bool MatDielectric::Scatter(const Ray& incomingRay,
	const Vec3 hitPoint,
	const Vec3 surfaceNormal,
	bool isFrontFace,
	OUT Vec3& attenuation, OUT Ray& scatteredRay) const
{
	attenuation = albedo;

	const float refractionRatio = isFrontFace ? (1.0f / indexOfRefraction) : indexOfRefraction;
	const Vec3 unitDirection = Normalized(incomingRay.Direction());

	const float cosTheta = fminf(Dot(-unitDirection, surfaceNormal), 1.0f);
	const float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);

	const bool cannotRefract = refractionRatio * sinTheta > 1.0f;

	// What's this "cannotRefract"?
	// Well, when the ray is coming out of the material with the
	// higher refractive index, then there is no real solution
	// to Snell's law, so no refraction is possible:
	//
	// sin(theta') = (eta / eta') * sin(theta),
	// 
	// but the value of sin cannot be larger than 1, so if
	// (eta / eta') > 1, and (eta / eta') * sin(theta) > 1,
	// then we must reflect, because we cannot reflect.

	// Schlick approximation:
	// glass reflectivity varies with angle; we can approximate
	// it like so:

	if (cannotRefract || Reflectance(cosTheta, refractionRatio) > GetRandomFloat())
	{
		scatteredRay = Ray(hitPoint, Reflect(unitDirection, surfaceNormal));
	}
	else
	{
		const Vec3 refractedRayD = Refract(unitDirection, surfaceNormal, refractionRatio);
		scatteredRay = Ray(hitPoint, refractedRayD);
	}

	return true;
}

bool MatMetal::Scatter(const Ray& incomingRay,
	const Vec3 hitPoint,
	const Vec3 surfaceNormal,
	bool isFrontFace,
	OUT Vec3& attenuation, 
	OUT Ray& scatteredRay) const
{
	const Vec3 reflected = Reflect(Normalized(incomingRay.Direction()), surfaceNormal);
	scatteredRay = Ray(hitPoint, reflected + fuzz * GetRandomUnitVec3());
	attenuation = albedo;
	return Dot(scatteredRay.Direction(), surfaceNormal) > 0.0f;
}

bool MatLambertian::Scatter(const Ray& incomingRay, 
	const Vec3 hitPoint,
	const Vec3 surfaceNormal,
	bool isFrontFace,
	OUT Vec3& attenuation, 
	OUT Ray& scatteredRay) const
{
	auto scatterDirection = surfaceNormal + GetRandomUnitVec3();

	if (scatterDirection.SqrMagnitude() <= 0.0001f)
	{
		scatterDirection = surfaceNormal;
	}

	scatteredRay = Ray(hitPoint, scatterDirection);
	attenuation = albedo;
	return true;
}
