#pragma once
#include <imgui/imgui.h>

class style
{
public:
	static void render();
	static void MainWindow();
	static void theme_2(); //
	static void load_font();

	static ImFont* bahnschrift_16px;
	static ImFont* bahnschrift_32px;

	static ImFont* emoji_32px;
};

