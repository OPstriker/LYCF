#include "style.h"
#include <imgui/imgui.h>
#include <imgui/imguipp.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include "tab.h"
#include "LYCF.h"
#include <stdlib.h>
//#include "icons.h"
//#include "iconcpp.h"
#include "IconsFontAwesome4.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "font.h"


void style::load_font()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\bahnschrift.ttf", 20.0f);

}

void font::loadfont(const char* TFFdir, float size)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.Fonts->AddFontFromFileTTF(TFFdir, size);
}

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
	using namespace ImGui;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	// Render
	window->DrawList->PathClear();

	int num_segments = 30;
	int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
			centre.y + ImSin(a + g.Time * 8) * radius));
	}

	window->DrawList->PathStroke(color, false, thickness);
	return false;
}

void LoadingIndicatorCircle(const char* label, const float indicator_radius,
	const ImVec4& main_color, const ImVec4& backdrop_color,
	const int circle_count, const float speed) {
	using namespace ImGui;
	ImGuiStyle &style = ImGui::GetStyle();
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems) {
		return;
	}

	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(label);

	const ImVec2 pos = window->DC.CursorPos;
	const float circle_radius = indicator_radius / 10.0f;
	const ImRect bb(pos, ImVec2(pos.x + indicator_radius * 2.0f,
		pos.y + indicator_radius * 2.0f));
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, id)) {
		return;
	}
	const float t = g.Time;
	const auto degree_offset = 2.0f * IM_PI / circle_count;
	for (int i = 0; i < circle_count; ++i) {
		const auto x = indicator_radius * std::sin(degree_offset * i);
		const auto y = indicator_radius * std::cos(degree_offset * i);

		auto val1 = 0;
		auto val2 = std::sin(t * speed - i * degree_offset);

		const auto growth = max(val1, val2);
		ImVec4 color;
		color.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
		color.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
		color.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
		color.w = 1.0f;
		window->DrawList->AddCircleFilled(ImVec2(pos.x + indicator_radius + x,
			pos.y + indicator_radius - y),
			circle_radius + growth * circle_radius,
			GetColorU32(color));
	}
}


// Block Dialog
void LYCF::Display::Prompt(const char* Text, bool* close)
{

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::SetCursorPos(ImVec2(550.0f, 350.0f));

	static bool popup1;

	if (popup1)
	{
		ImGui::OpenPopup("Prompt");
		popup1 = false;
	}
	

	bool open = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16);
	ImGui::SetNextWindowSize(ImVec2(420.0f, 250.0f));
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Prompt", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		int my_image_width = 0;
		int my_image_height = 0;
		GLuint my_image_texture = 0;
		bool ret = LoadTextureFromFile("icons//close.png", &my_image_texture, &my_image_width, &my_image_height);
		IM_ASSERT(ret);


		//ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));

		ImGui::Text(Text);
		static bool loading = true;

		if (loading)
		{
			LoadingIndicatorCircle("Circle", 50.0f, ImVec4(0.85f, 0.85f, 0.85f, 0.8f), ImVec4(0.85f, 0.85f, 0.85f, 1.0f), 8, 5.24f);
		}


		//ImGui::ProgressBar(ImGui::GetTime() * -0.25f);
		//BufferingBar("Blocking", );

		//static ImU32 spin_colEN = ImGui::GetColorU32(ImVec4(0.85f, 0.85f, 0.85f, 0.8f));
		//Spinner("spin", 10.0f, 10.0f, spin_colEN);
		//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100);

		//Spinner("lbl", 10, 5, spin_colEN);



		ImGui::SetCursorPos(ImVec2(90.0f, 170.0f));
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20, 0.35, 0.35, 1.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20, 0.35, 0.35, 0.6));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20, 0.35, 0.35, 0.6));

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);

		if (ImGui::Button("Close", ImVec2(120.0f, 50.0f)))
		{

			//loading = false;
			//*close = false;
			ImGui::CloseCurrentPopup();
		}

		//ImGui::EndPopup();

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}


void style::MainWindow()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowBorderSize = 0;

	//style->Colors[ImGuiCol_Button] = ImColor(35, 42, 47, 255);
	//style->Colors[ImGuiCol_ButtonActive] = ImColor(35, 42, 47, 255);
	//style->Colors[ImGuiCol_ButtonHovered] = ImColor(35, 42, 47, 255);

	style->Colors[ImGuiCol_Separator] = ImColor(35, 42, 47); //41, 42, 47
	style->Colors[ImGuiCol_SeparatorActive] = ImColor(35, 42, 47); //41, 42, 47
	style->Colors[ImGuiCol_SeparatorHovered] = ImColor(35, 42, 47); //41, 42, 47
	//style->Colors[ImGuiCol_ButtonHovered] = ImColor(35, 42, 47, 255);

	style->Colors[ImGuiCol_ModalWindowDimBg] = ImColor(25, 33, 35, 155); // 
	style->Colors[ImGuiCol_PopupBg] = ImColor(35, 42, 47, 250);

}


void style::theme_2()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	// Child
	style->Colors[ImGuiCol_ChildBg] = ImColor(35, 42, 47, 100); // Cyan 58, 132, 132, 255, // Good One...(160, 193, 209) // 124, 152, 179
	style->Colors[ImGuiCol_CheckMark] = ImColor(80, 200, 120);

}

// close
bool LYCF::Display::LoadIcons = true;


GLuint LYCF::Display::minz;
GLuint LYCF::Display::close;

static int close_width = 0;
static int close_height = 0;

static int minz_width = 0;
static int minz_height = 0;


void LYCF::Display::caption()
{

	if (LYCF::Display::LoadIcons)
	{
		// Load Icons

		bool close_ret = LoadTextureFromFile("icons//Close.png", &LYCF::Display::close, &close_width, &close_height);

		bool minz_ret = LoadTextureFromFile("icons//Minimize.png", &LYCF::Display::minz, &minz_width, &minz_height);

		LYCF::Display::LoadIcons = false;
	}

	ImGuiStyle* style = &ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//style->Colors[ImGuiCol_Button] = ImVec4(0.77f, 0.17f, 0.11f, 0.0f); // 35, 42, 47, 255
	//style->Colors[ImGuiCol_ButtonActive] = ImColor(35, 42, 47, 255);
	//style->Colors[ImGuiCol_ButtonHovered] = ImColor(35, 42, 47, 255);

	style->Colors[ImGuiCol_ChildBg] = ImColor(0.77f, 0.17f, 0.11f, 0.0f);


	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 78, 0.0f));

	// Manually Assing COlors
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.77f, 0.17f, 0.11f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.16f, 0.18f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.16f, 0.18f, 1.0f));

	ImGui::PushID("hide-btn");
	if (ImGui::Button(" ", ImVec2(60.0f, 40.0f)))
	{
		LYCF::EN_LYCF::WindowClose();
	}
	ImGui::PopID();

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 138.0f, 0.0f));
	ImGui::PushID("Minz-btn");
	if (ImGui::Button(" ", ImVec2(60.0f, 40.0f)))
	{
		LYCF::EN_LYCF::WindowMinimize();
	}
	ImGui::PopID();

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 61.5, 8.5f));
	ImGui::ImageButton((void*)(intptr_t)LYCF::Display::close, ImVec2(19.5f, 20.0f));

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 121.5, 10.0f));

	ImGui::ImageButton((void*)(intptr_t)LYCF::Display::minz, ImVec2(20.0f, 17.0f));
	ImGui::PopStyleColor(3);
}

// Define Bools/Declare Bools...

bool LYCF::EN_LYCF::check_settings;
bool LYCF::EN_LYCF::check_cmd;
bool LYCF::EN_LYCF::check_TManager;



// iBoss Bool...

bool LYCF::iBoss_LYCF::ibs_stat;
bool LYCF::iBoss_LYCF::iBossBlockDialog;
bool LYCF::iBoss_LYCF::iboss_persist;
bool LYCF::iBoss_LYCF::iBossLoginpg;


// VPN bool...
bool LYCF::VPN_LYCF::vpnstat = false;


void style::render()
{
	//button_hovered_color_nothing();
	ImGuiStyle* style = &ImGui::GetStyle();
	//ImGui::ShowDemoWindow();

	//ImGui::Columns(2);
	ImGui::SetCursorPosY(0.0f);
	ImGui::BeginColumns("LYCF", 2, ImGuiColumnsFlags_NoResize);
	//ImGui::SetCursorPosY(70.0f);



	ImGui::SetColumnOffset(1, 250);
	{
		//Left Side
		//ImGui::SetScrollHereY(1.0);
		static ImVec4 Active			= ImVec4(0.14f, 0.16f, 0.18f, 1.0f);
		static ImVec4 InActive			= ImVec4(0.10f, 0.13f, 0.14f, 0.0f); //25, 33, 35, 255

		static ImVec4 HoveredInActive	= ImVec4(0.14f, 0.16f, 0.18f, 1.0f);
		static ImVec4 Hovered			= ImVec4(0.14f, 0.16f, 0.18f, 0.4f);

		static ImVec4 Text				= ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		static ImVec4 TextInactive		= ImVec4(1.0f, 1.0f, 1.0f, 0.2f); // 0.33, 0.35, 0.36, 1.0

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.16f, 0.18f, 0.35f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.16f, 0.18f, 0.35f));


		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8); // Rounding
		ImGui::SetCursorPosY(10.0f);

		static auto ENABLE = 1; // Enable...

		ImGui::PushStyleColor(ImGuiCol_Button, settings::tab == ENABLE ? Active : InActive);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, settings::tab == ENABLE ? HoveredInActive : Hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, settings::tab == ENABLE ? Active : Hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == ENABLE ? Text : TextInactive);
		if (ImGui::Button("Enable", ImVec2(250 - 16.3, 60)))
			settings::tab = 1;
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (settings::tab == 1)
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}
		//ImGui::PopStyleColor(3);

		static auto IBOSS = 2; // iBoss...

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, settings::tab == IBOSS ? Active : InActive);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, settings::tab == IBOSS ? HoveredInActive : Hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, settings::tab == IBOSS ? Active : Hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == IBOSS ? Text : TextInactive);
		if (ImGui::Button("iBoss", ImVec2(250 - 16.3, 60)))
		{
			settings::tab = 2;
			LYCF::iBoss_LYCF::iboss_status(); // iBoss Status Updater...
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (settings::tab == 2)
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}
		//ImGui::PopStyleColor(3);

		static auto VPN = 3; // VPN...

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, settings::tab == VPN ? Active : InActive);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, settings::tab == VPN ? HoveredInActive : Hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, settings::tab == VPN ? Active : Hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == VPN ? Text : TextInactive);
		if (ImGui::Button("VPN", ImVec2(250 - 16.3, 60)))
			settings::tab = 3;
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (settings::tab == 3)
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}
		//ImGui::PopStyleColor(3);

		static auto ABOUT = 4; // About...

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, settings::tab == ABOUT ? Active : InActive);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, settings::tab == ABOUT ? HoveredInActive : Hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, settings::tab == ABOUT ? Active : Hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == ABOUT ? Text : TextInactive);
		if (ImGui::Button("About", ImVec2(250 - 16.3, 60)))
			settings::tab = 4;
		if (ImGui::IsItemHovered()) {
			//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, )
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (settings::tab == 4)
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}
		//ImGui::PopStyleColor(3);


		ImGui::SetCursorPos(ImVec2(42.5f, ImGui::GetWindowHeight() - 45.2));
		ImGui::TextColored(ImVec4(0.83f, 0.66f, 0.69f, 1.0f), "Use At Your Own Risk"); // Let Your Computer Free!
		// 0.86f, 0.08f, 0.24f, 1.0f
		ImGui::PopStyleColor(18);



		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 10);


		ImGui::PopStyleVar(); // Pop-Rounding...
	}


	ImGui::NextColumn();
	{


		LYCF::Display::caption(); // Load in Custom Caption Bars...

		theme_2(); // Theme for Enable Option style...


		if (settings::tab == 1) // Enable
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6);
			ImGui::SetCursorPosY(50.0f);


			// Settings Option

			ImGui::BeginChild("Settings", ImVec2(ImGui::GetWindowWidth() - 285, 60), false);
			ImGui::SetCursorPos(ImVec2(20, 20));
			ImGui::Text("Settings");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 66.0, 15.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			//ImGui::Checkbox(" ", &LYCF::EN_LYCF::check_settings);
			LYCF::Display::ToggleButton("chck_stng", &LYCF::EN_LYCF::check_settings);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleVar();
			ImGui::EndChild();
			
			// Set Setting Reg
			LYCF::EN_LYCF::SetDword_SettingReg_CheckBased(LYCF::EN_LYCF::check_settings);
			

			// CMD Option


			ImGui::Spacing();
			ImGui::BeginChild("CMD", ImVec2(ImGui::GetWindowWidth() - 285, 60), false);
			ImGui::SetCursorPos(ImVec2(20, 20));
			ImGui::Text("CMD");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 66.0, 15.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			//ImGui::Checkbox(" ", &LYCF::EN_LYCF::check_cmd);
			LYCF::Display::ToggleButton("chck_cmd", &LYCF::EN_LYCF::check_cmd);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleVar();
			ImGui::EndChild();

			// Set CMD reg

			LYCF::EN_LYCF::SetDword_CMDReg_CheckBased(LYCF::EN_LYCF::check_cmd);

			// TaskManager Option

			ImGui::Spacing();
			ImGui::BeginChild("TaskManager", ImVec2(ImGui::GetWindowWidth() - 285, 60), false);
			ImGui::SetCursorPos(ImVec2(20, 20));
			ImGui::Text("TaskManager");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 66, 15.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			//ImGui::Checkbox(" ", &LYCF::EN_LYCF::check_TManager);
			LYCF::Display::ToggleButton("chck_tmgn", &LYCF::EN_LYCF::check_TManager);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleVar();
			ImGui::EndChild();

			// Set TaskManager reg
			LYCF::EN_LYCF::SetDword_TManagerReg_CheckBased(LYCF::EN_LYCF::check_TManager);
			
			ImGui::PopStyleVar();
			
			//ImGui::ImageButton(path, ImVec2(20.0f, 20.0f));
		}

		//static bool iboss_persist;


		if (settings::tab == 2) // iBoss
		{

			// iBoss Dialogs...

			if (LYCF::iBoss_LYCF::iBossBlockDialog)
				LYCF::Display::Prompt("Hellow WOrld", &LYCF::iBoss_LYCF::iBossBlockDialog);

			if (LYCF::iBoss_LYCF::iBossLoginpg)
				LYCF::Display::Prompt("If you Get an Error Please Try again later..", &LYCF::iBoss_LYCF::iBossLoginpg);


			const char* value = LYCF::iBoss_LYCF::ibs_stat ? "Active (Running)" : "Dead (Not Running)";

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 9);
			// iBoss Status Indicator...
			ImGui::SetCursorPos(ImVec2(360.0f, -7.0f));
			ImGui::BeginChild("iBoss", ImVec2(295.0f, 50.0f), false);
			ImGui::SetCursorPos(ImVec2(21.5f, 17.0f));
			ImGui::Text("iBoss Status:");
			ImGui::SameLine();
			ImGui::TextColored(LYCF::iBoss_LYCF::ibs_stat ? ImVec4(0.86f, 0.08f, 0.24f, 1.0f) : ImVec4(0.31f, 0.78f, 0.47f, 1.0f), value); // Green : 0.31f, 0.78f, 0.47f, 1.0f, Red : 0.86f, 0.08f, 0.24f, 1.0f
			ImGui::EndChild();


			// iBoss Options...
			ImGui::SetCursorPosY(90.0f);
			ImGui::BeginChild("iBoss-Block", ImVec2(ImGui::GetWindowWidth() - 285, 60)); // iBoss-Block start...
			ImGui::SetCursorPos(ImVec2(10.0f, 20.0f));
			ImGui::Text("iBoss");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 160, 13.0f));

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.16f, 0.18f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
			if (ImGui::Button(LYCF::iBoss_LYCF::ibs_stat ? "Block iBoss" : "Enable iBoss", ImVec2(150.0f, 35.0f)))
			{
				//LYCF::Display::Prompt();
				//Prompt = true;
				LYCF::iBoss_LYCF::iBossBlockDialog = true;
				//LYCF::Display::Prompt = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);
			ImGui::EndChild(); //iBoss - Block End...

			// iBoss Login

			ImGui::Spacing();
			ImGui::BeginChild("iBoss Login", ImVec2(ImGui::GetWindowWidth() - 285.0f, 60.0f));
			ImGui::SetCursorPos(ImVec2(10.0f, 20.0f));
			ImGui::Text("iBoss Login (BYOD)");
			ImGui::SameLine();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 120, 13.0f));

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.16f, 0.18f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
			if (ImGui::Button("Launch", ImVec2(110.0f, 35.0f)))
			{
				if (ShellExecute(0, 0, "http://connectivitycheck.gstatic.com/", 0, 0, SW_HIDE))
				{
					LYCF::iBoss_LYCF::iBossLoginpg = true;
				}
			}

			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);

			ImGui::EndChild();


			// iBoss Persist
			ImGui::Spacing();
			ImGui::BeginChild("iBoss-Persist)", ImVec2(ImGui::GetWindowWidth() - 285.0f, 60.0f)); // iBoss - Persis Begin...
			ImGui::SetCursorPos(ImVec2(10.0f, 20.0f));
			ImGui::Text("Persistent Block (optional)");
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 65, 15.0));
			//ImGui::Checkbox(" ", &LYCF::iBoss_LYCF::iboss_persist);
			LYCF::Display::ToggleButton("btn", &LYCF::iBoss_LYCF::iboss_persist);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleVar();
			ImGui::EndChild(); //iBoss - Persis End...

			//ToggleButton("btn", &LYCF::iBoss_LYCF::iboss_persist);

			// iBoss Note
			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnOffset() + 50, ImGui::GetWindowHeight() - 60));
			ImGui::BeginChild("iBoss-Note", ImVec2(480.0f, 40.0f), false); // Start iBoss Note
			ImGui::SetCursorPos(ImVec2(20.0f, 10.0f));
			ImGui::Text("Note: iBoss Must be Disabled for the VPN to Work Properly");

			ImGui::EndChild(); // End of iBoss Note...

			//ImGui::Checkbox(" ", &LYCF::iBoss_LYCF::ibs_stat);


			ImGui::PopStyleVar();

		}

		//static bool vpnstat = false;

		if (settings::tab == 3) // VPN
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.93f, 0.50f, 0.13f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.93f, 0.50f, 0.13f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.90f, 0.56f, 0.27f, 1.0f));


			//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.62f, 0.25f, 0.33f, 1.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 7);


			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnOffset() + 169.0f, 49.0f));
			ImGui::PushStyleColor(ImGuiCol_ChildBg, LYCF::VPN_LYCF::vpnstat ? ImVec4(0.31f, 0.78f, 0.47f, 1.0f) : ImVec4(0.86f, 0.08f, 0.24f, 1.0f));

			ImGui::BeginChild("VPN-STAT-INDBGND", ImVec2(197.0f, 52.0f));



			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnOffset() + 170.0f, 50.0f));
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.15f, 0.17f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6);
			ImGui::BeginChild("VPN-STAT", ImVec2(195.0f, 50.0f));

			ImGui::SetCursorPos(LYCF::VPN_LYCF::vpnstat ? ImVec2(50.0f, 14.0f) : ImVec2(39.0f, 14.0f));
			ImGui::TextColored(LYCF::VPN_LYCF::vpnstat ? ImVec4(0.31f, 0.78f, 0.47f, 1.0f) : ImVec4(0.86f, 0.08f, 0.24f, 1.0f), LYCF::VPN_LYCF::vpnstat ? "CONNECTED" : "DISCONNECTED");

			ImGui::EndChild();

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);

			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnOffset() + 215.0f, 200.0f));
			if (ImGui::Button("Enable VPN", ImVec2(120.0f, 50.0f)))
			{
				LYCF::VPN_LYCF::vpnstat = true;
				LYCF::VPN_LYCF::vpnconnect();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);


			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnOffset() + 215.0f, 260.0f));
			if (ImGui::Button("Disable VPN", ImVec2(120.0f, 50.0f)))
			{
				LYCF::VPN_LYCF::vpnstat = false;
			}

			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			ImGui::PopStyleColor(5);
			ImGui::PopStyleVar(3);
		}

		if (settings::tab == 4) // About
		{
			ImGui::SetCursorPosX(250);
			ImGui::BeginChild("Frame");

			//ImGui::Button("test");
			ImGui::BeginChild("About");

			ImGui::SetCursorPos(ImVec2(250.0f, 250.0f));
			ImGui::Text("About");

			static bool idk;



			ImGui::Checkbox(" ", &idk);

			ImGui::SetCursorPosX(ImGui::GetColumnOffset() + 50.0f);
			if (idk) {
				LoadingIndicatorCircle("Circle", 50.0f, ImVec4(0.85f, 0.85f, 0.85f, 0.2f), ImVec4(0.85f, 0.55f, 0.95f, 0.0f), 8, 10.24f);

				static ImU32 spin_colEN = ImGui::GetColorU32(ImVec4(0.85f, 0.85f, 0.85f, 0.8f));

				ImGui::SetCursorPosX(ImGui::GetColumnOffset() + 100.0f);
				Spinner("heh", 22.0f, 10, spin_colEN);
			}


			ImGui::EndChild();

			ImGui::EndChild();
		}

		// Caption....

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.13f, 0.14f, 0.0f));
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.10f, 0.13f, 0.14f, 0.0f));

		ImGui::SetCursorPos(ImVec2(212.0f, 0.0f));

		ImGui::BeginChild("Caption", ImVec2(500.0f, 40.0f), false);

		ImGui::EndChild();


		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{
			
			LYCF::EN_LYCF::captionmove = true;
		}
		else 
		{
			LYCF::EN_LYCF::captionmove = false;
		}
	}
}