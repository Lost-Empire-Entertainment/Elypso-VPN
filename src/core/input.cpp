//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//vpn
#include "input.hpp"
#include "core.hpp"

namespace Core
{
	void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		VPN::UpdateActivityTime();
	}

	void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		VPN::UpdateActivityTime();
	}

	void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		VPN::UpdateActivityTime();
	}

	void Input::MouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
	{
		VPN::UpdateActivityTime();
	}
}