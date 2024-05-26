#include "RayTracing/Application.h"
#include "RayTracing/EntryPoint.h"

#include "RayTracing/Image.h"
#include "RayTracing/Random.h"
#include "RayTracing/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include "glm/gtc/type_ptr.hpp"

using namespace RayTracing;

class ExampleLayer : public RayTracing::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.f, 0.1f, 100.0f)
	{
		{
			Sphere sphere;
			sphere.Postion = glm::vec3(0.f, 0.f, 0.f);
			sphere.Albedo = glm::vec3(1.0f, 0.2f, 0.1f);
			sphere.Radius = 0.5f;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Postion = glm::vec3(0.f, 0.f, 0.f);
			sphere.Albedo = glm::vec3(0.2f, 1.0f, 0.1f);
			sphere.Radius = 1.f;
			m_Scene.Spheres.push_back(sphere);
		}
	}
	virtual void OnUpdate(float ts) override
	{
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render Time: %0.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i=0;i<2;++i)
		{
			ImGui::PushID(i);
			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Postion), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::ColorEdit3("Color", glm::value_ptr(sphere.Albedo));
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight()}, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
		ImGui::PopStyleVar();
		//ImGui::ShowDemoWindow();

		Render();

	}
	void Render() 
	{
		Timer timer;
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);
		
		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};

RayTracing::Application* RayTracing::CreateApplication(int argc, char** argv)
{
	RayTracing::ApplicationSpecification spec;
	spec.Name = "RayTracing Example";

	RayTracing::Application* app = new RayTracing::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}