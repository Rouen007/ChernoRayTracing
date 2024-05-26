#include "Renderer.h"
#include "RayTracing/Random.h"
#include <execution>

namespace Utils
{
	uint32_t ConvertRGBA(glm::vec4 color)
	{
		uint8_t r = (uint8_t)( color.r * 255);
		uint8_t g = (uint8_t)( color.g * 255);
		uint8_t b = (uint8_t)( color.b * 255);
		uint8_t a = (uint8_t)( color.a * 255);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}
};

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<RayTracing::Image>(width, height, RayTracing::ImageFormat::RGBA);
	}
	
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);

	for (uint32_t i = 0; i < width; ++i)
		m_ImageHorizontalIter[i] = i;

	for (uint32_t i = 0; i < height; ++i)
		m_ImageVerticalIter[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& cam)
{
	m_ActiveScene = &scene;
	m_ActiveCam = &cam;
	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, sizeof(glm::vec4) * m_FinalImage->GetWidth() * m_FinalImage->GetHeight());
#define MT 1
#if MT
	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this](uint32_t y)
		{
#if 0
			std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
			[this, y](uint32_t x)
				{
					auto color = PerPixel(x, y);

					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

					glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
					accumulatedColor /= m_FrameIndex;
					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertRGBA(accumulatedColor);
				});
#else
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); ++x)
			{
				auto color = PerPixel(x, y);

				m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

				glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
				accumulatedColor /= m_FrameIndex;
				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertRGBA(accumulatedColor);
			}
#endif
		});

#else
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); ++y)
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); ++x)
		{
			auto color = PerPixel(x, y);
		
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

			glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
			accumulatedColor /= m_FrameIndex;
			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x+y*m_FinalImage->GetWidth()] = Utils::ConvertRGBA(accumulatedColor);
		}
#endif
	m_FinalImage->SetData(m_ImageData);
	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCam->GetPosition();
	ray.Direction = m_ActiveCam->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	int bounces = 5;
	float multiplier = 1.0f;
	glm::vec3 color(0.0f);
	for (int i = 0; i < bounces; ++i)
	{
		Renderer::HitPayLoad payLoad = TraceRay(ray);

		if (payLoad.HitDistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir(-1, -1, -1);
		lightDir = glm::normalize(lightDir);

		const Sphere& closestSphere = m_ActiveScene->Spheres[payLoad.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[closestSphere.MaterialIndex];

		glm::vec3 sphereColor = material.Albedo;
		float lightIntensity = glm::max(glm::dot(payLoad.WorldNormal, -lightDir), 0.0f);
		sphereColor *= lightIntensity;
		color += sphereColor * multiplier;

		multiplier *= 0.5f;

		ray.Origin = payLoad.WorldPosition + payLoad.WorldNormal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction, 
			payLoad.WorldNormal + material.Roughness * RayTracing::Random::Vec3(-0.5f, 0.5f));
	}

	return glm::vec4(color, 1.0f);
}

Renderer::HitPayLoad Renderer::TraceRay(const Ray& ray)
{

	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size();++i)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];

		glm::vec3 origin = ray.Origin - sphere.Position;
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		float discriminate = b * b - 4 * a * c;
		if (discriminate < 0.0f)
			continue;

		float closeT = (-b - glm::sqrt(discriminate)) / (2.0f * a);
		if (closeT >0.0f && closeT < hitDistance)
		{
			hitDistance = closeT;
			closestSphere = i;
		}
	}
	
	if (closestSphere < 0.0f)
		return  Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayLoad Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayLoad payLoad;
	payLoad.HitDistance = hitDistance;
	payLoad.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];
	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payLoad.WorldPosition = origin + ray.Direction * hitDistance;

	payLoad.WorldNormal = glm::normalize(payLoad.WorldPosition);
	payLoad.WorldPosition += closestSphere.Position;

	/*hit.x = std::max(0.0f, hit.x);
	hit.y = std::max(0.0f, hit.y);
	hit.z = std::max(0.0f, hit.z);*/

	return payLoad;
}

Renderer::HitPayLoad Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayLoad payLoad;
	payLoad.HitDistance = -1.0f;
	return payLoad;
}
