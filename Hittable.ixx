#define OUT

export module Hittable;

import <cmath>;
import <stdexcept>;
import <memory>;

import Vec3;
import Ray;
import Interval;
import Material;

export struct HitRecord
{
	Vec3						hitPoint;
	Vec3						surfaceNormal;
	float						t { 0.0f };
	bool						isFrontFace{ false };
	Material const*				hitMaterial;

	void						SetSurfaceNormal(const Ray& ray, const Vec3& surfaceNormal);
};

export class Hittable
{
public:
	virtual ~Hittable() = default;

	virtual bool DoesHit(const Ray& ray,
		Interval tRange,
		OUT HitRecord& hit) const = 0;
};

void HitRecord::SetSurfaceNormal(const Ray& ray, const Vec3& outwardNormal)
{
#if _DEBUG
	const float normalLength = outwardNormal.Length();
	const float d = fabs(normalLength - 1.0f);
	if (d > 0.1f)
	{
		throw std::runtime_error("[Hittable] surfaceNormal is expected to be of length 1.");
	}
#endif

	isFrontFace = Dot(ray.Direction(), outwardNormal) < 0.0f;
	surfaceNormal = isFrontFace ? outwardNormal : -outwardNormal;
}