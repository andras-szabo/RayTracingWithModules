#define NOT_HIT -1.0f;
#define OUT

import Utility;
import PPMUtility;
import Vec3;
import Ray;
import Hittable;
import HittableList;
import Sphere;
import Camera;
import Material;

#include <iostream>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <time.h>

void DoRender(const HittableList& world)
{
	// Camera
	Camera cam;

	// Render
	cam.aspectRatio = 16.0f / 9.0f;
	cam.imageWidth = 1024;
	cam.samplesPerPixel = 256;
	cam.bounceLimit = 64;
	cam.reflectance = 0.5f;

	cam.viewCentre = Vec3(13.0f, 2.0f, -3.0f);
	Vec3 viewTarget{ 0.0f, 0.0f, 0.0f };
	cam.viewLookTarget = viewTarget;
	cam.verticalFOVDegrees = 32.0f;

	cam.defocusAngle = 0.6f;
	cam.focusDistance = 10.0f;

	const bool multiThreaded = true;
	cam.Render(world, multiThreaded);
}

void DoFinalRender()
{
	HittableList world;
	const auto matGround = std::make_shared<MatLambertian>(Vec3(0.5f, 0.5f, 0.5f));
	world.Add(std::make_shared<Sphere>(Vec3(0.0f, -1000.0f, 0.0f), 1000.f, matGround));

	const Vec3 p(4.0f, 0.2, 0.0f);
	const float pdSquared = 0.9f * 0.9f;

	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			const float materialSelector = GetRandomFloat();
			Vec3 centre(a + 0.9f * GetRandomFloat(), 0.2f, b + 0.9f * GetRandomFloat());

			if ((centre - p).SqrMagnitude() > pdSquared)
			{
				std::shared_ptr<Material> sphereMaterial;
				if (materialSelector < 0.8f)	// Diffuse
				{
					const auto albedo = GetRandomVec3();
					sphereMaterial = std::make_shared<MatLambertian>(albedo);
					world.Add(std::make_shared<Sphere>(centre, 0.2f, sphereMaterial));
				}
				else if (materialSelector < 0.95f)	// Metal
				{
					const auto albedo = GetRandomVec3();
					const float fuzz = GetRandomFloat(0.0f, 0.5f);
					sphereMaterial = std::make_shared<MatMetal>(albedo, fuzz);
					world.Add(std::make_shared<Sphere>(centre, 0.2f, sphereMaterial));
				}
				else // glass
				{
					sphereMaterial = std::make_shared<MatDielectric>(1.5f);
					world.Add(std::make_shared<Sphere>(centre, 0.2f, sphereMaterial));
				}
			}
		}
	}


	const auto material1 = std::make_shared<MatDielectric>(1.5);
	world.Add(std::make_shared<Sphere>(Vec3(0.0f, 1.0f, 0.0f), 1.0, material1));

	const auto material2 = std::make_shared<MatLambertian>(Vec3(0.4f, 0.2f, 0.1f));
	world.Add(std::make_shared<Sphere>(Vec3(-4.0f, 1.0f, 0.0f), 1.0, material2));

	const auto material3 = std::make_shared<MatMetal>(Vec3(0.7f, 0.6f, 0.5f), 0.0f);
	world.Add(std::make_shared<Sphere>(Vec3(4.0f, 1.0f, 0.0f), 1.0f, material3));

	DoRender(world);
}

void DoStuff()
{
	// World
	HittableList world;

	MatLambertian	red(Vec3(1.0f, 0.2f, 0.2f));
	MatLambertian	blue(Vec3(0.2f, 0.2f, 1.0f));
	MatMetal		chrome(Vec3(0.8f, 0.8f, 0.8f), 0.2f);
	MatMetal		gold(Vec3(0.8f, 0.6f, 0.2f), 0.5f);
	MatDielectric	glass(Vec3(1.0f, 1.0f, 1.0f), 1.5f);
	MatDielectric	tintedGlass(Vec3(0.2f, 0.2f, 0.2f), 1.4f);

	Sphere redSphere(Vec3(0.4f, 0.0f, 4.0f), 0.5f, std::make_shared<MatLambertian>(red));
	Sphere blueSphere(Vec3(0.0f, -100.5f, 1.0f), 100.0f, std::make_shared<MatLambertian>(blue));
	Sphere chromeSphere(Vec3(-0.6f, 0.0f, 1.0f), 0.3f, std::make_shared<MatMetal>(chrome));
	Sphere chromeSphere2(Vec3(-0.2f, 0.6f, 1.0f), 0.3f, std::make_shared<MatMetal>(gold));
	Sphere glassSphere(Vec3(0.3f, 0.0f, 0.8f), 0.3f, std::make_shared<MatDielectric>(glass));
	Sphere tintedSphere(Vec3(-0.1f, 0.0f, 4.0f), 0.2f, std::make_shared<MatDielectric>(tintedGlass));

	world.Add(std::make_shared<Sphere>(redSphere));
	world.Add(std::make_shared<Sphere>(blueSphere));
	world.Add(std::make_shared<Sphere>(chromeSphere));
	world.Add(std::make_shared<Sphere>(chromeSphere2));
	world.Add(std::make_shared<Sphere>(glassSphere));
	world.Add(std::make_shared<Sphere>(tintedSphere));

	DoRender(world);
}

void Init()
{
	srand(time(nullptr));
}

int main()
{
	Init();
	//DoStuff();
	DoFinalRender();
	return 0;
}
