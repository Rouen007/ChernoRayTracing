#pragma once
#include "RayTracing/Image.h"
#include <memory> 
#include "glm/glm.hpp"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
		bool SlowRandom = true;
	};
	Renderer() = default;
	
	~Renderer() = default;
	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& cam);
	std::shared_ptr<RayTracing::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }
private:
	struct HitPayLoad
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;
		int ObjectIndex;
	};
	glm::vec4 PerPixel(uint32_t x, uint32_t y); // ray gen

	HitPayLoad TraceRay(const Ray& ray);
	HitPayLoad ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayLoad Miss(const Ray& ray);
private:
	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCam = nullptr;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
	std::shared_ptr<RayTracing::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
};