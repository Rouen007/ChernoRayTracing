#include "Renderer.h"
#include "RayTracing/Random.h"



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
}

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); ++y)
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); ++x)
		{
			glm::vec2 coord = { (float)x / m_FinalImage->GetWidth(), (float)y / m_FinalImage->GetHeight() };
			m_ImageData[x+y*m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	coord.x = coord.x * 2 - 1;
	coord.y = coord.y * 2 - 1;
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	// x = ax+bx*t
	// y = ay+by*t
	// x^2 + y^2 <= r^2

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	float discriminate = b * b - 4 * a * c;
	if (discriminate >= 0.0f)
	{
		return 0xffff00ff;
	}
	else return 0xff000000;
	
	return 0xff000000 | (g << 8) | r;
}
