#define OUT

export module Camera;

import Vec3;
import Ray;
import Hittable;
import Interval;
import PPMUtility;
import Utility;
import Material;

import <vector>;
import <iostream>;
import <thread>;
import <chrono>;

export class Camera
{
public:
	void	Render(const Hittable& world, bool multiThreaded);

	float	aspectRatio{ 16.0f / 9.0f };
	int		imageWidth{ 100 };
	int		imageHeight{ 100 };
	int		samplesPerPixel{ 10 };
	int		bounceLimit{ 12 };
	float	reflectance{ 0.7f };

	Vec3	viewCentre;
	Vec3	viewLookTarget{ 0.0f, 0.0f, 1.0f };

	Vec3	viewUp{ 0.0f, 1.0f, 0.0f };
	Vec3	viewRight{ 1.0f, 0.0f, 0.0f };
	Vec3	viewForward{ 0.0f, 0.0f, 1.0f };

	Vec3	pixel00_loc;
	Vec3	pixel_delta_u;
	Vec3	pixel_delta_v;
	float	verticalFOVDegrees{ 90.0f };

	float	defocusAngle{ 0.0f };
	float	focusDistance{ 10.0f };
	Vec3	defocusDiskU;
	Vec3	defocusDiskV;

private:
	void	Initialize();
	Ray		GetRandomCameraRayForPixel(const Vec3& pixelCentre, int i, int j) const;
	Vec3	GetRandomPointInSquareAroundOriginPixel() const;
	Vec3	GetRayColor(const Ray& ray, const Hittable& world, int bounceCount = 0) const;
	void	RenderSingleThreaded(std::vector<Vec3>& colors, const Hittable& world) const;
	void	RenderMultiThreaded(std::vector<Vec3>& colors, const Hittable& world);
	void	RenderPart(std::vector<Vec3>& colors, int startRow, int endRowPlusOne);
	Vec3	GetDefocusDiskSample() const;

	bool				_isInitialized{ false };
	const Hittable*		_world { nullptr };
};

Vec3 Camera::GetDefocusDiskSample() const
{
	const auto p = GetRandomVectorInUnitDisk();
	return viewCentre + (p.X() * defocusDiskU) + (p.Y() * defocusDiskV);
}

Ray Camera::GetRandomCameraRayForPixel(const Vec3& pixelCentre, int i, int j) const
{
	const Vec3 pixelSample = pixelCentre + GetRandomPointInSquareAroundOriginPixel();

	const Vec3 rayOrigin = defocusAngle <= 0.0f ? viewCentre : GetDefocusDiskSample();
	const Vec3 rayDirection = pixelSample - rayOrigin;

	return Ray(rayOrigin, rayDirection);
}

Vec3 Camera::GetRandomPointInSquareAroundOriginPixel() const
{
	const float px = -0.5f + GetRandomFloat();
	const float py = -0.5f + GetRandomFloat();

	return (px * pixel_delta_u) + (py * pixel_delta_v);
}

void Camera::Initialize()
{
	imageHeight = static_cast<int>(imageWidth / aspectRatio);

	//const float focal_length = (viewLookTarget - viewCentre).Length(); // distance from camera centre to viewport

	float viewport_height = tanf(ToRadians(verticalFOVDegrees / 2.0f)) * focusDistance;

	const float viewport_width = viewport_height * (static_cast<float>(imageWidth) / imageHeight);

	const Vec3 worldUpVector{ 0.0f, 1.0f, 0.0f };

	viewForward = Normalized(viewLookTarget - viewCentre);
	viewRight = Normalized(Cross(worldUpVector, viewForward));
	viewUp = Cross(viewForward, viewRight);

	const Vec3 viewport_u = viewRight * viewport_width;
	const Vec3 viewport_v = viewUp * -viewport_height;

	pixel_delta_u = viewport_u / static_cast<float>(imageWidth);
	pixel_delta_v = viewport_v / static_cast<float>(imageHeight);

	const Vec3 viewport_upper_left = viewCentre + (focusDistance * viewForward)
		- viewport_u / 2.0f
		- viewport_v / 2.0f;

	pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

	// Camera defocus disk basis vectors
	const float defocusRadius = focusDistance * tanf(ToRadians(defocusAngle / 2.0f));
	defocusDiskU = viewRight * defocusRadius;
	defocusDiskV = viewUp * defocusRadius;

	_isInitialized = true;
}

void Camera::RenderSingleThreaded(std::vector<Vec3>& colors, const Hittable& world) const
{
	for (int j = 0; j < imageHeight; ++j)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
		for (int i = 0; i < imageWidth; ++i)
		{
			Vec3 pixelColor;
			const Vec3 pixelCentre = pixel00_loc + (static_cast<float>(i) * pixel_delta_u) + (static_cast<float>(j) * pixel_delta_v);

			for (int sample = 0; sample < samplesPerPixel; ++sample)
			{
				const Ray r = GetRandomCameraRayForPixel(pixelCentre, i, j);
				pixelColor += GetRayColor(r, world);
			}

			colors.emplace_back(pixelColor);
		}
	}
}

void Camera::RenderPart(std::vector<Vec3>& colors,
						int startRow,
						int endRowPlusOne)
{
	for (int j = startRow; j < endRowPlusOne; ++j)
	{
		for (int i = 0; i < imageWidth; ++i)
		{
			const auto index = j * imageWidth + i;
			if (index >= colors.size())
			{
				continue;
			}

			Vec3 pixelColor;
			const Vec3 pixelCentre = pixel00_loc + (static_cast<float>(i) * pixel_delta_u) + (static_cast<float>(j) * pixel_delta_v);
			for (int sample = 0; sample < samplesPerPixel; ++sample)
			{
				const Ray r = GetRandomCameraRayForPixel(pixelCentre, i, j);
				pixelColor += GetRayColor(r, *_world);
			}

			colors[index] = std::move(pixelColor);
		}
	}
}

void Camera::RenderMultiThreaded(std::vector<Vec3>& colors, const Hittable& world)
{
	const auto maxHardwareThreadCount = std::thread::hardware_concurrency();

	if (maxHardwareThreadCount <= 1)
	{
		RenderSingleThreaded(colors, world);
	}
	else
	{
		_world = &world;

		const auto sideThreadCount = maxHardwareThreadCount - 1;
		std::vector<std::thread> threads(sideThreadCount);

		const int rowsPerThread = imageHeight / maxHardwareThreadCount;

		for (int i = 0; i < sideThreadCount; ++i)
		{
			const int startRow = i * rowsPerThread;
			const int endRowPlusOne = startRow + rowsPerThread + 1;
			threads[i] = (std::thread(&Camera::RenderPart, this, std::ref(colors), startRow, endRowPlusOne));
		}

		const int mainThreadStartRow = rowsPerThread * sideThreadCount;
		RenderPart(colors, mainThreadStartRow, imageHeight);

		for (auto& threadItem : threads)
		{
			threadItem.join();
		}
	}
}

void  Camera::Render(const Hittable& world, bool multiThreaded)
{
	if (!_isInitialized)
	{
		Initialize();
	}

	std::vector<Vec3> colors(imageWidth * imageHeight, Vec3{});
	const auto start = std::chrono::steady_clock::now();

	if (multiThreaded) { RenderMultiThreaded(colors, world); }
	else { RenderSingleThreaded(colors, world); }

	const auto finish = std::chrono::steady_clock::now();
	const std::chrono::duration<double> duration = finish - start;

	std::cout << "\nElapsed: " << duration << "\n";

	PPMUtil::TrySaveAsPPM(
		imageWidth, 
		imageHeight, 
		colors.data(),
		samplesPerPixel,
		"Test");

	std::clog << "\rDone.                                    \n";
}
 
Vec3 Camera::GetRayColor(const Ray& r, const Hittable& world, int bounceCount) const
{

	const Interval tRange { 0.001f, infinity };

	HitRecord hit;
	if (++bounceCount < bounceLimit && world.DoesHit(r, tRange, OUT hit))
	{
		Ray scatteredRay;
		Vec3 attenuation;

		if (hit.hitMaterial->Scatter(r, hit.hitPoint, hit.surfaceNormal, hit.isFrontFace,
			OUT attenuation, OUT scatteredRay))
		{
			return attenuation * GetRayColor(scatteredRay, world, bounceCount);
		}

		return Vec3(0.0f, 0.0f, 0.0f);
	}

	const Vec3 one{ 1.0f, 1.0f, 1.0f };
	const Vec3 targetColor{ 0.5f, 0.7f, 1.0f };

	const Vec3 unit_direction = Normalized(r.Direction());
	const float a = 0.5f * (unit_direction.Y() + 1.0f);
	return (1.0f - a) * one + a * targetColor;
}