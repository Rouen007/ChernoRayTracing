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
	Renderer() = default;
	
	~Renderer() = default;
	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& cam);
	std::shared_ptr<RayTracing::Image> GetFinalImage() const { return m_FinalImage; }
private:
	glm::vec4 TraceRay(const Scene& scene, const Ray& ray);
private:
	std::shared_ptr<RayTracing::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};