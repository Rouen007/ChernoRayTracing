#include "RayTracing/Application.h"
#include "RayTracing/EntryPoint.h"

#include "RayTracing/Image.h"

class ExampleLayer : public RayTracing::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Hello");
		ImGui::Button("Button");
		ImGui::End();

		ImGui::ShowDemoWindow();
	}
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