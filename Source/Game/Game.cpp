#include "Game.h"
#include "Window.h"
#include "Input.h"
#include "Matrix4x4.h"
#include "LineRenderer.h"
#include "VertexBinding.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture2D.h"
#include "TextureSampler.h"
#include "BspFile.h"
#include "BspFlags.h"
#include "MdxFile.h"
#include "Md2/Md2File.h"
#include "EntityLump.h"
#include "FpsTimer.h"
#include "FreeCamera.h"
#include "SpriteBatch.h"
#include "Font.h"
#include "DynamicModel.h"
#include "Util.h"
#include "Map.h"
#include "Paths.h"
#include "Nav/NavFile.h"
#include "PakFileSystem.h"
#include "PhysicalFileSystem.h"
#include "Renderer.h"
#include "TimeUtil.h"
#include "Audio/AudioClip.h"
#include "Audio/AudioDevice.h"
#include "Skybox.h"
#include "BillboardBatch.h"
#include "RenderTarget.h"
#include <glad/gl.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <array>

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_opengl3.h"
#include "ThirdParty/imgui/imgui_impl_sdl.h"

namespace Freeking
{
	Game::Game(int argc, char** argv)
	{
		FileSystem::AddFileSystem(PhysicalFileSystem::Create(std::filesystem::current_path() / "Assets"));
		FileSystem::AddFileSystem(PhysicalFileSystem::Create(Paths::KingpinDir() / "main"));
		FileSystem::AddFileSystem(PakFileSystem::Create(Paths::KingpinDir() / "main/Pak0.pak"));

		static const std::string windowTitle = "Kingpin";
		_viewportWidth = 2880;
		_viewportHeight = 1620;
		_window = std::make_unique<Window>(windowTitle, _viewportWidth, _viewportHeight);
		_mouseLocked = false;
	}

	Game::~Game()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		_window.reset();
		SDL_Quit();
	}

	void Game::OnResize(int width, int height)
	{
		glViewport(0, 0, width, height);

		_viewportWidth = width;
		_viewportHeight = height;
	}

	void Game::LockMouse(bool lockMouse)
	{
		if (_mouseLocked == lockMouse)
		{
			return;
		}

		_mouseLocked = lockMouse;

		SDL_SetRelativeMouseMode(lockMouse ? SDL_TRUE : SDL_FALSE);

		if (lockMouse)
		{
			int mousePosX, mousePosY;
			SDL_GetMouseState(&mousePosX, &mousePosY);
		}

		int w, h;
		SDL_GetWindowSize(static_cast<SDL_Window*>(*_window), &w, &h);
		SDL_WarpMouseInWindow(static_cast<SDL_Window*>(*_window), w / 2, h / 2);

		Input::ResetMouseDelta();
	}

	static void ImGuiDebugAssetLibrary()
	{
		ImGui::SetNextWindowSize(ImVec2(800, 1000), ImGuiCond_Once);
		ImGui::Begin("Asset Library");
		ImGui::BeginTabBar("tabs");

		if (ImGui::BeginTabItem("Textures"))
		{
			int windowWidth = static_cast<int>(ImGui::GetWindowContentRegionWidth());
			int lineWidth = 0;
			int i = 0;

			for (auto const& [name, texture] : Texture2D::Library.GetPathAssets())
			{
				if ((lineWidth + texture->GetWidth()) < windowWidth)
				{
					lineWidth += (texture->GetWidth() + 10);
				}
				else
				{
					lineWidth = texture->GetWidth() + 10;

					if (i > 0)
					{
						ImGui::NewLine();
					}
				}

				ImGui::Image(
					(ImTextureID)(intptr_t)texture->GetId(),
					ImVec2((float)texture->GetWidth(), (float)texture->GetHeight()),
					ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 0, 1));

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("%s", name.c_str());
				}

				ImGui::SameLine();

				i++;
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		ImGui::End();
	}

	void Game::Run()
	{
		Time::SetTimeApplicationStart();

		uint64_t now = SDL_GetPerformanceCounter();
		uint64_t last = 0;
		double deltaTime = 0.0;
		FpsTimer timer;

		SDL_Event e;
		bool running = true;

		SDL_GetWindowSize(*_window, &_viewportWidth, &_viewportHeight);
		glViewport(0, 0, _viewportWidth, _viewportHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_window->Swap();

		Shader::Initialize();

		ImGui::CreateContext();
		ImGui_ImplSDL2_InitForOpenGL(static_cast<SDL_Window*>(*_window), static_cast<SDL_GLContext*>(*_window));
		ImGui_ImplOpenGL3_Init("#version 130");
		ImGuiIO& io = ImGui::GetIO();

		AudioDevice audio;

		auto crosshair = Texture2D::Library.Get("Textures/crosshair.tga");

		BillboardBatch billboards;
		LightFlares::Billboards = &billboards;

		auto viewmodel = DynamicModel::Library.Get("models/weapons/shotgun/shotgun.mdx");
		auto viewmodel2 = DynamicModel::Library.Get("models/weapons/shotgun/hand.mdx");
		FrameAnimator animator;
		animator.SetAnimation(6);

		for (const auto& frameAnimation : viewmodel->GetFrameAnimations())
		{
			animator.AddAnimation(frameAnimation.name, frameAnimation.firstFrame, frameAnimation.numFrames);
		}

		std::string mapName("sr1");

		auto lineRenderer = std::make_shared<LineRenderer>(2000000);
		auto spriteBatch = std::make_shared<SpriteBatch>(10000);
		SpriteBatch::Debug = spriteBatch;
		LineRenderer::Debug = lineRenderer;
		FreeCamera camera;
		camera.MoveTo(Vector3f::Up * 100.0f);
		auto font = Font::Library.Get("Fonts/Roboto-Bold.json");
		auto map = std::make_shared<Map>(mapName);

		std::unique_ptr<Skybox> skybox;

		auto navData = FileSystem::GetFileData("navdata/" + mapName + ".nav");
		auto navNodes = NavFile::ReadNodes(navData.data());

		for (const auto& entDef : map->GetEntityProperties())
		{
			std::string classname = entDef.GetClassnameProperty();

			if (classname == "worldspawn")
			{
				std::string skyname;
				if (entDef.TryGetString("sky", skyname))
				{
					skybox = std::make_unique<Skybox>(skyname);
				}
			}
		}

		TraceResult tr;

		while (running)
		{
			Time::Update();

			last = now;
			now = SDL_GetPerformanceCounter();
			deltaTime = ((now - last) / (double)SDL_GetPerformanceFrequency());
			timer.Update(deltaTime);

			Input::PreEvent();

			while (SDL_PollEvent(&e))
			{
				switch (e.type)
				{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						OnResize(e.window.data1, e.window.data2);
					}
					break;
				}

				ImGui_ImplSDL2_ProcessEvent(&e);

				Input::HandleEvent(e);
			}

			LockMouse(Input::IsDown(Button::MouseRight));

			float mouseDeltaX = Input::GetMouseDeltaX();
			float mouseDeltaY = Input::GetMouseDeltaY();

			if (_mouseLocked)
			{
				camera.LookDelta(mouseDeltaX * 0.25f, mouseDeltaY * 0.25f);
			}

			if (Input::JustPressed(Button::F1))
			{
				Renderer::DebugDraw = !Renderer::DebugDraw;
			}

			auto inputForce = Vector3f(0.0f, 0.0f, 0.0f);
			if (Input::IsDown(Button::KeyW)) inputForce += Vector3f(0.0f, 0.0f, 1.0f);
			if (Input::IsDown(Button::KeyS)) inputForce -= Vector3f(0.0f, 0.0f, 1.0f);
			if (Input::IsDown(Button::KeyA)) inputForce += Vector3f(1.0f, 0.0f, 0.0f);
			if (Input::IsDown(Button::KeyD)) inputForce -= Vector3f(1.0f, 0.0f, 0.0f);
			
			if (inputForce.SquaredLength() > 0.0f)
			{
				inputForce = inputForce.Normalise();
				inputForce *= Input::IsDown(Button::KeyLSHIFT) ? 1000.0f : (Input::IsDown(Button::KeyLCONTROL) ? 50.0f : 500.0f);
			}

			camera.Move(inputForce, static_cast<float>(deltaTime));

			audio.SetListenerTransform(camera.GetPosition(), camera.GetRotation());
			audio.FlushQueue();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(static_cast<SDL_Window*>(*_window));
			ImGui::NewFrame();

			ImGuiDebugAssetLibrary();

			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 8.0f, io.DisplaySize.y - 8.0f), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
			ImGui::SetNextWindowBgAlpha(0.35f);
			if (ImGui::Begin("Camera position overlay", NULL,
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
			{
				auto const& camPos = camera.GetPosition();
				ImGui::Text("Camera Position: %s", camPos.ToString().c_str());
			}
			ImGui::End();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glCullFace(GL_BACK);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Matrix4x4 projectionMatrix = Matrix4x4::Perspective(80, (float)_viewportWidth / (float)_viewportHeight, 0.1f, 5000.0f);
			Matrix4x4 viewMatrix = camera.GetTransform();
			Matrix4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;

			Renderer::ProjectionMatrix = projectionMatrix;
			Renderer::ViewMatrix = viewMatrix;
			Renderer::ViewportWidth = static_cast<float>(_viewportWidth);
			Renderer::ViewportHeight = static_cast<float>(_viewportHeight);

			Shader::GlobalUniforms.Uniforms.viewMatrix = viewMatrix;
			Shader::GlobalUniforms.Uniforms.projectionMatrix = projectionMatrix;
			Shader::GlobalUniforms.Uniforms.viewProjectionMatrix = viewProjectionMatrix;
			Shader::GlobalUniforms.Update();

			if (Input::JustPressed(Button::MouseLeft))
			{
				Vector2f normalisedPoint = _mouseLocked ? 0.0f : Util::PixelPositionToScreenSpace(Input::GetMousePosition(), Vector4i(0, 0, _viewportWidth, _viewportHeight));
				auto direction = camera.NormalisedScreenPointToDirection(projectionMatrix, normalisedPoint);
				tr = map->LineTrace(camera.GetPosition(), camera.GetPosition() + direction * 10000.0f, BspContentFlags::MaskSolid);

				if (tr.hit && tr.entity)
				{
					tr.entity->Trigger();
				}
			}

			if (tr.hit)
			{
				lineRenderer->DrawArrow(
					tr.endPosition,
					tr.endPosition + tr.planeNormal * 25.0f,
					tr.axisU,
					tr.axisV,
					4.0f,
					LinearColor((tr.planeNormal.x + 1.0f * 0.5f), (tr.planeNormal.y + 1.0f * 0.5f), (tr.planeNormal.z + 1.0f * 0.5f), 1.0f));
			}

			map->Tick(deltaTime);
			map->Render();

			if (skybox)
			{
				glDepthMask(GL_FALSE);
				glDepthFunc(GL_LEQUAL);
				Matrix4x4 skyboxView = viewMatrix;
				skyboxView.Translate(0);

				Shader::GlobalUniforms.Uniforms.viewMatrix = skyboxView;
				Shader::GlobalUniforms.Update();

				skybox->Draw();

				Shader::GlobalUniforms.Uniforms.viewMatrix = viewMatrix;
				Shader::GlobalUniforms.Update();

				glDepthFunc(GL_LESS);
				glDepthMask(GL_TRUE);
			}

			billboards.Draw(deltaTime, camera.GetPosition(), camera.GetRotation().Forward());

			{
				glClear(GL_DEPTH_BUFFER_BIT);

				const auto& shader = Shader::Library.DynamicModel;
				shader->Bind();

				animator.Tick(deltaTime);

				size_t frame = animator.GetFrame();
				size_t nextFrame = animator.GetNextFrame();

				Matrix4x4 viewmodelProjectionMatrix = Matrix4x4::Perspective(73, (float)_viewportWidth / (float)_viewportHeight, 0.1f, 100.0f);

				Shader::GlobalUniforms.Uniforms.projectionMatrix = viewmodelProjectionMatrix;
				Shader::GlobalUniforms.Uniforms.viewProjectionMatrix = viewmodelProjectionMatrix;
				Shader::GlobalUniforms.Update();

				shader->SetParameterValue("delta", animator.GetFrameDelta());
				shader->SetParameterValue("modelMatrix", Matrix4x4::Translation(camera.GetViewModelOffset()) * Matrix3x3::RotationY(Math::DegreesToRadians(90)).ToMatrix4x4());
				shader->SetParameterValue("normalBuffer", DynamicModel::GetNormalBuffer().get());
				shader->SetParameterValue("cubemap", skybox->GetCubemap(), TextureSampler::Library.Get({ TextureWrapMode::ClampEdge, TextureFilterMode::Linear }).get());

				shader->SetParameterValue("diffuse", Texture2D::Library.Get(viewmodel->Skins[0]).get());
				shader->SetParameterValue("frameVertexBuffer", viewmodel->GetFrameVertexBuffer().get());
				shader->SetParameterValue("frames[0].index", (int)(frame * viewmodel->GetFrameVertexCount()));
				shader->SetParameterValue("frames[0].translate", viewmodel->FrameTransforms[frame].translate);
				shader->SetParameterValue("frames[0].scale", viewmodel->FrameTransforms[frame].scale);
				shader->SetParameterValue("frames[1].index", (int)(nextFrame * viewmodel->GetFrameVertexCount()));
				shader->SetParameterValue("frames[1].translate", viewmodel->FrameTransforms[nextFrame].translate);
				shader->SetParameterValue("frames[1].scale", viewmodel->FrameTransforms[nextFrame].scale);

				viewmodel->Draw();

				shader->SetParameterValue("diffuse", Texture2D::Library.Get(viewmodel2->Skins[0]).get());
				shader->SetParameterValue("frameVertexBuffer", viewmodel2->GetFrameVertexBuffer().get());
				shader->SetParameterValue("frames[0].index", (int)(frame * viewmodel2->GetFrameVertexCount()));
				shader->SetParameterValue("frames[0].translate", viewmodel2->FrameTransforms[frame].translate);
				shader->SetParameterValue("frames[0].scale", viewmodel2->FrameTransforms[frame].scale);
				shader->SetParameterValue("frames[1].index", (int)(nextFrame * viewmodel2->GetFrameVertexCount()));
				shader->SetParameterValue("frames[1].translate", viewmodel2->FrameTransforms[nextFrame].translate);
				shader->SetParameterValue("frames[1].scale", viewmodel2->FrameTransforms[nextFrame].scale);

				viewmodel2->Draw();

				Shader::GlobalUniforms.Uniforms.projectionMatrix = viewProjectionMatrix;
				Shader::GlobalUniforms.Uniforms.viewProjectionMatrix = viewProjectionMatrix;
				Shader::GlobalUniforms.Update();
			}

			if (Renderer::DebugDraw)
			{
				for (const auto& entDef : map->GetEntityProperties())
				{
					if (!entDef.GetOriginProperty())
					{
						continue;
					}

					auto origin = *entDef.GetOriginProperty();
					origin = Vector3f(origin.x, origin.z, -origin.y);
					float distance = origin.LengthBetween(camera.GetPosition());

					if (distance >= 512.0f)
					{
						continue;
					}

					Vector2f screenPosition;
					if (Util::WorldPointToNormalisedScreenPoint(origin, screenPosition, projectionMatrix, viewMatrix, 512.0f))
					{
						float alpha = 1.0f - (distance / 512.0f);
						lineRenderer->DrawSphere(origin, 4.0f, 4, 4, LinearColor(0, 1, 1, alpha));
						screenPosition = Util::ScreenSpaceToPixelPosition(screenPosition, Vector4i(0, 0, _viewportWidth, _viewportHeight));
						screenPosition.x = Math::Round(screenPosition.x);
						screenPosition.y = Math::Round(screenPosition.y);
						auto text = *entDef.GetClassnameProperty() + " (" + *entDef.GetNameProperty() + ")";
						spriteBatch->DrawText(font.get(), text, screenPosition + Vector2f(2, 2), LinearColor(0, 0, 0, alpha), 0.5f);
						spriteBatch->DrawText(font.get(), text, screenPosition, LinearColor(1, 1, 1, alpha), 0.5f);
					}
				}

				for (int i = 0; i < navNodes.size(); ++i)
				{
					const auto& navpoint = navNodes[i];
					auto origin = Vector3(navpoint.Position.y, navpoint.Position.w, -navpoint.Position.z);

					float distance = origin.LengthBetween(camera.GetPosition());

					if (distance >= 512.0f)
					{
						continue;
					}

					Vector2f screenPosition;
					if (Util::WorldPointToNormalisedScreenPoint(origin, screenPosition, projectionMatrix, viewMatrix, 512.0f))
					{
						float alpha = 1.0f - (distance / 512.0f);
						lineRenderer->DrawAABBox(origin, Vector3f(-5, -5, -5), Vector3f(5, 5, 5), LinearColor(0, 1, 0, alpha));
						screenPosition = Util::ScreenSpaceToPixelPosition(screenPosition, Vector4i(0, 0, _viewportWidth, _viewportHeight));
						screenPosition.x = Math::Round(screenPosition.x);
						screenPosition.y = Math::Round(screenPosition.y);
						auto text = "node #" + std::to_string(i);
						spriteBatch->DrawText(font.get(), text, screenPosition + Vector2f(2, 2), LinearColor(0, 0, 0, alpha), 0.5f);
						spriteBatch->DrawText(font.get(), text, screenPosition, LinearColor(1, 1, 1, alpha), 0.5f);
					}
				}

				glDisable(GL_DEPTH_TEST);
				lineRenderer->Flush();
				glEnable(GL_DEPTH_TEST);;
			}

			spriteBatch->Draw(crosshair.get(), Vector2f((_viewportWidth * 0.5f) - 16, (_viewportHeight * 0.5f) - 16), Vector2f(32), LinearColor::White.WithAlpha(0.75f));

			auto fps = timer.GetFps();
			LinearColor fpsColor = LinearColor(0, 1, 0, 1);

			if (fps < 30)
			{
				fpsColor = LinearColor(1, 0, 0, 1);
			}
			else if (fps < 60)
			{
				fpsColor = LinearColor(1, 1, 0, 1);
			}

			auto orthoProjection = Matrix4x4::Ortho(0, (float)_viewportWidth, (float)_viewportHeight, 0, -1.0f, 1.0f);

			Shader::GlobalUniforms.Uniforms.projectionMatrix = orthoProjection;
			Shader::GlobalUniforms.Update();

			spriteBatch->DrawText(font.get(), std::to_string(fps), Vector2f(10, 2), LinearColor(0, 0, 0, 1), 1.0f);
			spriteBatch->DrawText(font.get(), std::to_string(fps), Vector2f(8, 0), fpsColor, 1.0f);
			spriteBatch->Flush();

			lineRenderer->Clear();
			spriteBatch->Clear();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			_window->Swap();
		}
	}
}
