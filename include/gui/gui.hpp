//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace GUI
{
	class GUI_VPN
	{
	public:
		static void Initialize();
		static void Render();
		static void Shutdown();
	private:
		static inline bool isImguiInitialized;
	};
}