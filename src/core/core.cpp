//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#include <TlHelp32.h>
#elif __linux__
#include <unistd.h>
#endif
#include <iostream>
#include <filesystem>
#include <fstream>

//external
#include "glfw3.h"

//vpn
#include "core.hpp"
#include "configfile.hpp"
#include "render.hpp"
#include "gui.hpp"

using std::cout;
using std::wstring;
using std::filesystem::create_directory;
using std::filesystem::path;
using std::filesystem::exists;
using std::ofstream;

using GUI::Render;
using GUI::GUI_VPN;

namespace Core
{
	void VPN::Initialize()
	{
#ifdef _WIN32
		string name = "Elypso VPN.exe";
#elif __linux__
		string name = "Elypso VPN";
#endif
		if (IsThisProcessAlreadyRunning(name))
		{
			CreateErrorPopup((name + " is already running!").c_str());
		}

		cout << "\n==================================================\n"
			<< "\n"
			<< "ENTERED ELYPSO VPN\n"
			<< "\n"
			<< "==================================================\n"
			<< ".\n"
			<< ".\n"
			<< ".\n\n";

		cout << "Initializing Elypso VPN...\n";

#ifdef _WIN32
		PWSTR pwstrPath;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pwstrPath);
		if (SUCCEEDED(result))
		{
			wstring wPath(pwstrPath);
			CoTaskMemFree(pwstrPath); //free the allocated memory

			//get the required buffer size
			int size_needed = WideCharToMultiByte(
				CP_UTF8,
				0,
				wPath.c_str(),
				static_cast<int>(wPath.length()),
				NULL,
				0,
				NULL,
				NULL);

			//convert wide string to utf-8 encoded narrow string
			string narrowPath(size_needed, 0);
			WideCharToMultiByte(
				CP_UTF8,
				0,
				wPath.c_str(),
				static_cast<int>(wPath.length()),
				&narrowPath[0],
				size_needed,
				NULL,
				NULL);

			size_t pos = 0;
			docsPath = (path(narrowPath) / "Elypso VPN").string();
		}

		if (!exists(docsPath)) create_directory(docsPath);
#elif __linux__
		const char* homeDir = getenv("HOME");
		cout << "home dir: " << homeDir << "\n";
		if (!homeDir)
		{
			CreateErrorPopup("HOME environment is not set!");
		}

		string documentsFolder = path(homeDir) / "Documents";
		if (!exists(documentsFolder)) create_directory(documentsFolder);

		docsPath = path(documentsFolder) / "Elypso VPN";
		if (!exists(docsPath)) create_directory(docsPath);
		cout << "docs path: " << docsPath << "\n";
#endif

		configFilePath = (path(docsPath) / "config.txt").string();
		cout << "config file path: " << configFilePath << "\n";
		if (!exists(configFilePath))
		{
			ofstream configFile(configFilePath);
			if (!configFile.is_open())
			{
				CreateErrorPopup("Failed to create config file.");
			}
			configFile.close();
		}
		ConfigFile::LoadConfigFile();

		Render::Initialize();
	}

	bool VPN::IsThisProcessAlreadyRunning(const string& processName)
	{
#ifdef _WIN32
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32{};
		bool processFound = false;
		DWORD currentProcessId = GetCurrentProcessId();

		//take a snapshot of all processes
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			cout << "Error: CreateToolhelp32Snapshot failed!\n";
			return false;
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32))
		{
			cout << "Error: Process32First failed!\n";
			CloseHandle(hProcessSnap);
			return false;
		}

		do
		{
			//compare the current process name with the one to check
			if (strcmp(pe32.szExeFile, processName.c_str()) == 0)
			{
				//check if this is not the current process
				if (pe32.th32ProcessID != currentProcessId)
				{
					processFound = true;
					break;
				}
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return processFound;
#elif __linux__
		//get the process id
		pid_t currentPID = getpid();

		//construct the command to find processes by name, excluding the currend PID
		string command = "pgrep -x \"" + processName + "\" | grep -v " + to_string(currentPID) + " > /dev/null";

		//execute the command and return the result
		return (system(command.c_str()) == 0);
#endif
	}

	void VPN::CreateErrorPopup(const char* errorMessage)
	{
		string title = "Elypso VPN has shut down";

		cout << "\n"
			<< "===================="
			<< "\n"
			<< "VPN SHUTDOWN"
			<< "\n\n"
			<< errorMessage
			<< "\n"
			<< "===================="
			<< "\n";

#ifdef _WIN32
		int result = MessageBoxA(nullptr, errorMessage, title.c_str(), MB_ICONERROR | MB_OK);

		if (result == IDOK) Shutdown();
#elif __linux__
		string command = "zenity --error --text=\"" + (string)errorMessage + "\" --title=\"" + title + "\"";
		int result = system(command.c_str());
		(void)result;
		Shutdown();
#endif
	}

	//reset last idle activity timer
	static double lastActivityTime = 0.0f;
	void VPN::UpdateActivityTime()
	{
		lastActivityTime = glfwGetTime();
	}
	//check if any input has occured within the idle time
	bool VPN::IsInputActive()
	{
		const double idleThreshold = 1.0;
		double currentTime = glfwGetTime();
		double idleTime = currentTime - lastActivityTime;

		bool inputActive = idleTime <= idleThreshold;
		return inputActive;
	}
	//counts as idle if minimized
	//or unfocused and not compiling
	//or if focused and no input was detected and not compiling
	bool VPN::IsUserIdle()
	{
		//checks if window is minimized
		int width, height;
		glfwGetWindowSize(Render::window, &width, &height);
		if (width == 0 || height == 0) return true;

		//checks if window is unfocused and user is not compiling
		if (glfwGetWindowAttrib(Render::window, GLFW_FOCUSED) == GLFW_FALSE)
		{
			return true;
		}

		//checks if not compiling and no input is detected
		return !IsInputActive();
	}

	void VPN::Render()
	{
		cout << "Successfully reached render loop!\n\n";
		cout << "==================================================\n\n";

		isVPNRunning = true;

		while (isVPNRunning)
		{
			Render::Run();
			// Check if the window should close (e.g., user closed the window)
			if (glfwWindowShouldClose(Render::window))
			{
				isVPNRunning = false;
			}
		}
	}

	void VPN::Shutdown()
	{
		cout << "Shutting down Elypso VPN...\n";

		isVPNRunning = false;

		ConfigFile::SaveConfigFile();
		GUI_VPN::Shutdown();
		glfwTerminate();
		quick_exit(EXIT_SUCCESS);
	}
}