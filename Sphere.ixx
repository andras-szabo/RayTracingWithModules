#define OUT
export module Sphere;

import <cmath>;
import <memory>;
import <vector>;

import Hittable;
import Vec3;
import Material;

export class Sphere : public Hittable
{
public:
			Sphere(const Vec3& origin_,
				float radius_,
				std::shared_ptr<Material> material_) : 
				origin{ origin_ }, 
				radius{ radius_ },
				material{ material_ },
				radiusSquared{ radius_ * radius_ } {}

	Vec3						origin;
	float						radius;
	float						radiusSquared;
	std::shared_ptr<Material>	material { nullptr };

	bool						DoesHit(const Ray& ray,
									Interval tRange,
									OUT HitRecord& hit) const override;

};

export class SphereWorld : public Hittable
{
public:

	SphereWorld() = default;
	SphereWorld(std::shared_ptr<Sphere> item);

	void	Clear();
	void	Add(std::shared_ptr<Sphere> item);
	bool	DoesHit(const Ray& ray,
		Interval tRange,
		OUT HitRecord& hit) const override;

	std::vector<std::shared_ptr<Sphere>> items;

};

SphereWorld::SphereWorld(std::shared_ptr<Sphere> item)
{
	Add(item);
}

void SphereWorld::Clear()
{
	items.clear();
}

void SphereWorld::Add(std::shared_ptr<Sphere> item)
{
	items.push_back(item);
}

bool SphereWorld::DoesHit(const Ray& ray,
	Interval tRange,
	OUT HitRecord& hit) const
{
	HitRecord tmpHit;
	bool didHitAnything{ false };

	for (const auto& item : items)
	{
		if (item->DoesHit(ray, tRange, OUT tmpHit))
		{
			didHitAnything = true;
			tRange.max = tmpHit.t;
			hit = tmpHit;
		}
	}

	return didHitAnything;
}

bool Sphere::DoesHit(const Ray& ray,
	Interval tRange, OUT HitRecord& hit) const
{
	const Vec3 dir = ray.Direction();
	const Vec3 oc = ray.Origin() - origin;
	const float a = dir.SqrMagnitude();
	const float half_b = Dot(oc, dir);
	const float c = oc.SqrMagnitude() - radiusSquared;
	const float discriminant = half_b * half_b - a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}
	
	const float dRoot = sqrtf(discriminant);

	const float root1 = (-half_b - dRoot) / a;
	const bool root1OK = tRange.DoesSurround(root1);

	const float root2 = (-half_b + dRoot) / a;
	const bool root2OK = tRange.DoesSurround(root2);

	if (!root1OK && !root2OK)
	{
		return false;
	}

	const float t = root1OK ? (root2OK ? fminf(root1, root2) : root1) : root2;

	hit.t = t;
	hit.hitPoint = ray.At(hit.t);
	const Vec3 outwardNormal = (hit.hitPoint - origin) / radius;
	hit.SetSurfaceNormal(ray, outwardNormal);
	hit.hitMaterial = material.get();

	return true;
}