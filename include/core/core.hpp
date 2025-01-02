//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace Core
{
	using std::string;

	class VPN
	{
	public:
		static inline bool isVPNRunning;

		static inline string docsPath;
		static inline string configFilePath;

		static void Initialize();

		static bool IsThisProcessAlreadyRunning(const string& processName);
		static void CreateErrorPopup(const char* errorMessage = "INSERT ERROR MESSAGE");

		static void UpdateActivityTime();
		static bool IsUserIdle();

		static void Render();
		static void Shutdown();
	private:
		static bool IsInputActive();
	};
}