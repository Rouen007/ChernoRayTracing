#include "RayTracing/Application.h"
#include "RayTracing/EntryPoint.h"

#include "RayTracing/Image.h"
#include "RayTracing/Random.h"
#include "RayTracing/Timer.h"

using namespace RayTracing;

class ExampleLayer : public RayTracing::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render Time: %0.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		if (m_Image)
			ImGui::Image(m_Image->GetDescriptorSet(), { (float)m_Image->GetWidth(), (float)m_Image->GetHeight()});
		ImGui::End();
		ImGui::PopStyleVar();
		//ImGui::ShowDemoWindow();
	}
	void Render() 
	{
		Timer timer;
		if (!m_Image || m_ViewportHeight != m_Image->GetHeight() || m_ViewportWidth != m_Image->GetWidth())
		{
			m_Image = std::make_shared<Image>(m_ViewportWidth, m_ViewportHeight, ImageFormat::RGBA);
			delete[] m_ImageData;
			m_ImageData = new uint32_t[m_ViewportHeight * m_ViewportWidth];
		}
		for (uint32_t i = 0; i < m_ViewportWidth * m_ViewportHeight; ++i)
		{
			m_ImageData[i] = Random::UInt();
			m_ImageData[i] |= 0xff000000; // set alpha to 255
		}
		m_Image->SetData(m_ImageData);
		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	std::shared_ptr<Image> m_Image;
	uint32_t* m_ImageData = nullptr;
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