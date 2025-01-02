//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>
#include <filesystem>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//vpn
#include "gui.hpp"
#include "core.hpp"
#include "render.hpp"

using std::cout;
using std::string;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::current_path;

using Core::VPN;

namespace GUI
{
	void GUI_VPN::Initialize()
	{
		cout << "Initializing ImGui.\n\n";

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = (path(VPN::docsPath) / "imgui.ini").string();
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		string currPath = current_path().generic_string();
		string fontPath = (path(currPath) / "files" / "fonts" / "coda" / "Coda-Regular.ttf").string();
		if (exists(fontPath))
		{
			io.Fonts->Clear();
			io.Fonts->AddFontFromFileTTF(((path(currPath) / "files" / "fonts" / "coda" / "Coda-Regular.ttf")).string().c_str(), 16.0f);
		}
		else
		{
			cout << "Error: Font " << fontPath << " does not exist!\n\n";
		}

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		style.TabRounding = 6;
		style.FramePadding = ImVec2(6, 2);
		style.ItemSpacing = ImVec2(0, 5);
		io.FontGlobalScale = 1.5f;

		isImguiInitialized = true;
	}

	void GUI_VPN::Render()
	{
		if (isImguiInitialized)
		{
			if (!VPN::IsUserIdle())
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				ImGuiDockNodeFlags dockFlags =
					ImGuiDockNodeFlags_PassthruCentralNode;

				ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

				//imgui windows here

				ImGui::Render();
			}

			ImDrawData* drawData = ImGui::GetDrawData();
			if (drawData != nullptr)
			{
				ImGui_ImplOpenGL3_RenderDrawData(drawData);
			}
		}
	}

	void GUI_VPN::Shutdown()
	{
		if (isImguiInitialized)
		{
			isImguiInitialized = false;

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	}
}