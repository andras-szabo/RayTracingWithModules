export module Ray;

import Vec3;

export class Ray
{
public:
					Ray() = default;
					Ray(const Vec3& origin, const Vec3& direction);

	inline Vec3		Origin() const { return _origin; }
	inline Vec3		Direction() const { return _direction; }
	inline Vec3		At(float t) const {return _origin + t * _direction;}

private:
	Vec3	_origin;
	Vec3	_direction;
};

Ray::Ray(const Vec3& origin, const Vec3& direction)
	: _origin(origin), _direction(direction)
{}
