#pragma once
#include "RayTracing/Image.h"
#include <memory> 
#include "glm/glm.hpp"

class Renderer
{
public:
	Renderer() = default;
	
	~Renderer() = default;
	void OnResize(uint32_t width, uint32_t height);
	void Render();
	std::shared_ptr<RayTracing::Image> GetFinalImage() const { return m_FinalImage; }
private:
	glm::vec4 PerPixel(glm::vec2 coord);
private:
	std::shared_ptr<RayTracing::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};