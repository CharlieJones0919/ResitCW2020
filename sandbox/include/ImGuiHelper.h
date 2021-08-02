#pragma once
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <array>
#include <string> // Shouldn't need to be here - should inherit this include from imgui.h.

class ImGuiHelper
{
public:
	static void init();
	static void begin();
	static void end();
	static void shutdown();
	static void writeToConsole(const std::string & text);
	static char * getConsoleString();
private:
	static std::array<std::string, 50> m_consoleLines;
	static unsigned int m_consoleLinesLength;
	static unsigned int m_consolePosition;
};