#include "Pipeline.h"
#include "Utils/Random.h"

#include <thread>
#include <chrono>

#define NES_WIDTH 256
#define NES_HEIGHT 240

namespace NESEmu {
	const int TARGET_FPS = 60; // <- NES Runs at 60 FPS
	const double TARGET_FRAME_TIME = 1000.0 / TARGET_FPS;

	float CurrentTime = glfwGetTime();
	float Frametime = 0.0f;
	float DeltaTime = 0.0f;

	uint8_t NESFrameBuffer[NES_WIDTH * NES_HEIGHT * 3];

	class RayTracerApp : public NESEmu::Application
	{
	public:

		bool vsync;

		RayTracerApp()
		{
			m_Width = 800;
			m_Height = 600;
		}

		void OnUserCreate(double ts) override
		{

		}

		void OnUserUpdate(double ts) override
		{
			glfwSwapInterval((int)vsync);

			GLFWwindow* window = GetWindow();

		}

		void OnImguiRender(double ts) override
		{
			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::Begin("Internal Debug")) {


			
			} ImGui::End();
		}

		void OnEvent(NESEmu::Event e) override
		{
			ImGuiIO& io = ImGui::GetIO();

			if (e.type == NESEmu::EventTypes::MousePress && !ImGui::GetIO().WantCaptureMouse && GetCurrentFrame() > 32)
			{

			}

			if (e.type == NESEmu::EventTypes::MouseMove && GetCursorLocked())
			{
			}


			if (e.type == NESEmu::EventTypes::MouseScroll && !ImGui::GetIO().WantCaptureMouse)
			{
			}

			if (e.type == NESEmu::EventTypes::WindowResize)
			{
			}

			if (e.type == NESEmu::EventTypes::KeyPress && e.key == GLFW_KEY_ESCAPE) {
				exit(0);
			}

			if (e.type == NESEmu::EventTypes::KeyPress && e.key == GLFW_KEY_F1)
			{
				this->SetCursorLocked(!this->GetCursorLocked());
			}

			if (e.type == NESEmu::EventTypes::KeyPress && e.key == GLFW_KEY_F2 && this->GetCurrentFrame() > 5)
			{
				NESEmu::ShaderManager::RecompileShaders();
			}

			if (e.type == NESEmu::EventTypes::KeyPress && e.key == GLFW_KEY_F3 && this->GetCurrentFrame() > 5)
			{
				NESEmu::ShaderManager::ForceRecompileShaders();
			}


		}


	};

	void Pipeline::StartPipeline()
	{
		// Application
		RayTracerApp app;
		app.Initialize();
		app.SetCursorLocked(false);

		// Load custom TrueType font
		ImGuiIO& io = ImGui::GetIO();
		const char* systemFontPath = "C:\\Windows\\Fonts\\calibri.ttf";
		io.Fonts->AddFontFromFileTTF(systemFontPath, 24.0f);
		io.Fonts->Build();

		// Create VBO and VAO for drawing the screen-sized quad.
		GLClasses::VertexBuffer ScreenQuadVBO;
		GLClasses::VertexArray ScreenQuadVAO;

		// Setup screensized quad for rendering
		{
			unsigned long long CurrentFrame = 0;
			float QuadVertices_NDC[] =
			{
				-1.0f,  1.0f,  0.0f, 1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
				 1.0f, -1.0f,  1.0f, 0.0f, -1.0f,  1.0f,  0.0f, 1.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,  1.0f,  1.0f,  1.0f, 1.0f
			};

			ScreenQuadVAO.Bind();
			ScreenQuadVBO.Bind();
			ScreenQuadVBO.BufferData(sizeof(QuadVertices_NDC), QuadVertices_NDC, GL_STATIC_DRAW);
			ScreenQuadVBO.VertexAttribPointer(0, 2, GL_FLOAT, 0, 4 * sizeof(GLfloat), 0);
			ScreenQuadVBO.VertexAttribPointer(1, 2, GL_FLOAT, 0, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
			ScreenQuadVAO.Unbind();
		}

		// Create Shaders 
		ShaderManager::CreateShaders();

		// Shaders
		GLClasses::Shader& BlitShader = ShaderManager::GetShader("BLIT");
		Random RNG;


		// Custom Blit Texture
		GLuint NESTexture = 0;
		glGenTextures(1, &NESTexture);
		glBindTexture(GL_TEXTURE_2D, NESTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, NES_WIDTH, NES_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		for (int y = 0; y < NES_HEIGHT; ++y) {
			for (int x = 0; x < NES_WIDTH; ++x) {
				//bool isWhite = ((x / 8 + y / 8) % 2) == (0);

				float X = (float)x / (float)NES_WIDTH;
				float Y = (float)y / (float)NES_HEIGHT;
				X = X * 2. - 1.;
				Y = Y * 2. - 1.;
				X *=2.5f; Y *= 2.5f;

				if (X * X + Y * Y < 1.0f) {
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 0] = 255; // R
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 1] = 255; // G
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 2] = 255; // B
				}
				else {
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 0] = 0; // R
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 1] = 0; // G
					NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 2] = 0; // B
				}

				//NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 0] = (RNG.Float() * 255.); // R
				//NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 1] = (RNG.Float() * 255.); // G
				//NESFrameBuffer[(y * NES_WIDTH + x) * 3 + 2] = (RNG.Float() * 255.); // B
			}
		}

		while (!glfwWindowShouldClose(app.GetWindow())) {

			app.vsync = true;

			// Update : 
			app.OnUpdate();
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, NESTexture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NES_WIDTH, NES_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, NESFrameBuffer);

			// Blit Final Result 
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);


			// Render via imgui
			ImGui::Begin("NES Display", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::Image((void*)(intptr_t)NESTexture, ImVec2(NES_WIDTH * 2.5, NES_HEIGHT * 2.5));
			ImGui::End();



			BlitShader.Use();
			ScreenQuadVAO.Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			ScreenQuadVAO.Unbind();

			glUseProgram(0);
			glFinish();
			app.FinishFrame();

			CurrentTime = glfwGetTime();
			DeltaTime = CurrentTime - Frametime;
			Frametime = glfwGetTime();

			GLClasses::DisplayFrameRate(app.GetWindow(), "NESEmu ");
		}
	}
}