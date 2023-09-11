#define OUT

export module HittableList;

import Hittable;

import <memory>;
import <vector>;

export class HittableList : public Hittable
{
public:
	
			HittableList() = default;
			HittableList(std::shared_ptr<Hittable> item);

	void	Clear();
	void	Add(std::shared_ptr<Hittable> item);
	bool	DoesHit(const Ray& ray,
			Interval tRange,
			OUT HitRecord& hit) const override;

	std::vector<std::shared_ptr<Hittable>> items;

};

HittableList::HittableList(std::shared_ptr<Hittable> item)
{
	Add(item);
}

void HittableList::Clear()
{
	items.clear();
}

void HittableList::Add(std::shared_ptr<Hittable> item)
{
	items.push_back(item);
}

bool HittableList::DoesHit(const Ray& ray,
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