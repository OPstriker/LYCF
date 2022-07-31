#define IMGUI_DEFINE_MATH_OPERATORS


#include "style.h"
#include <imgui/imgui.h>
#include <imgui/imguipp.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

#include "v0iden.h"

#include <glad/glad.h>
#include "LYCF.h"
#include "style.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "font.h"

#include <chrono>
#include <thread>
#include <future>

#include <imgui/imgui_freetype.h>

#include <ShObjIdl_core.h>
#include <shlwapi.h>


ImFont* style::bahnschrift_16px;
ImFont* style::bahnschrift_32px;

ImFont* style::emoji_32px;


void style::load_font()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\bahnschrift.ttf", 17.0f);
	{
		static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
		static ImFontConfig cfg;
		cfg.OversampleH = cfg.OversampleV = 1;
		cfg.MergeMode = true;
		cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
		style::emoji_32px = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 28.0f, &cfg, ranges);
	}

	style::bahnschrift_16px = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\bahnschrift.ttf", 16.0f);

	style::bahnschrift_32px = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\bahnschrift.ttf", 65.0f);

	static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
	static ImFontConfig cfg;
	cfg.OversampleH = cfg.OversampleV = 1;
	cfg.MergeMode = true;
	cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
	style::emoji_32px = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 32.0f, &cfg, ranges);


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

void cs_ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay, ImVec4 col)
{
	using namespace ImGui;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 sizeval	= CalcItemSize(size_arg, CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
	ImRect bb(pos, pos + sizeval);
	ItemSize(sizeval, style.FramePadding.y);
	if (!ItemAdd(bb, 0))
		return;

	float bar_begin = 0.0f;
	float bar_end = ImSaturate(fraction);

	const bool indeterminate = fraction < 0.0f;
	if (indeterminate)
	{
		const float bar_fraction = 0.2f;
		bar_begin = ImFmod(-fraction, 1.0f) * (1.0f + bar_fraction) - bar_fraction;
		bar_end = bar_begin + bar_fraction;
	}

	// Render
	RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
	bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
	RenderRectFilledRangeH(window->DrawList, bb, GetColorU32(col), bar_begin, bar_end, style.FrameRounding); // GetColorU32(ImGuiCol_PlotHistogram)

	// Don't display text for indeterminate bars by default
	if (!indeterminate || overlay)
	{
		// Default displaying the fraction as percentage string, but user can override it
		char overlay_buf[32];
		if (!overlay)
		{
			ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction * 100 + 0.01f);
			overlay = overlay_buf;
		}

		const ImVec2 overlay_size = CalcTextSize(overlay, NULL);
		if (overlay_size.x > 0.0f)
		{
			const ImVec2 fill_br = ImVec2(indeterminate ? (bb.Min.x + bb.Max.x - overlay_size.x) * 0.5f : ImLerp(bb.Min.x, bb.Max.x, bar_end), bb.Max.y);
			RenderTextClipped(ImVec2(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max, overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
		}
	}
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

		auto val1 = 0.0f;
		auto val2 = std::sin(t * speed - i * degree_offset);

		const auto growth = std::max(val1, val2);
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

// Load 

bool LYCF::Display::LoadIcons = true;


GLuint LYCF::Display::minz;
GLuint LYCF::Display::close;
GLuint LYCF::Display::error;

static int close_width = 0;
static int close_height = 0;

static int minz_width = 0;
static int minz_height = 0;

static int error_width = 0;
static int error_height = 0;

//static bool showclosebutton = true;
bool LYCF::iBoss_LYCF::ibsapopup_exit = false;

static void showclsbtn()
{

	//showclosebutton = true;
	LYCF::iBoss_LYCF::ibsapopup_exit = true;

}

bool LYCF::iBoss_LYCF::ibsaclosex = false;

bool LYCF::iBoss_LYCF::ibsaction_eval = false;

bool LYCF::iBoss_LYCF::exec_eval = true;


void LYCF::Display::Prompt(const char* Text, bool* close)
{

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::SetCursorPos(ImVec2(550.0f, 350.0f));

	if (LYCF::iBoss_LYCF::iBossBlockDialog)
		ImGui::OpenPopup("Prompt");
	
	static ImU32 spincol_1 = ImGui::GetColorU32(ImVec4(0.85f, 0.85f, 0.85f, 0.8f));

	bool open = true;



	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16);
	ImGui::SetNextWindowSize(ImVec2(420.0f, 250.0f));
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Prompt", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		// Close Button...

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20, 0.35, 0.35, 0.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20, 0.35, 0.35, 1.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20, 0.35, 0.35, 0.0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);

		if (settings::dialog == 1) // iBoss Action Dialog (BLOCK = ENABLE / ENABLE = BLOCK)
		{

			ImGui::SetCursorPos(ImVec2((420.0f / 2) - 50.0f, (250.0f / 2) - 80.0f));
			LoadingIndicatorCircle("Circle", 50.0f, ImVec4(0.85f, 0.85f, 0.85f, 0.8f), ImVec4(0.85f, 0.85f, 0.85f, 1.0f), 8, 5.24f);

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6);

			ImGui::SetCursorPos(ImVec2((420.0f / 2) - 130, ImGui::GetCursorPosY() + 27));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.01f);

			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));


			std::string val = LYCF::iBoss_LYCF::ibs_stat ? "Blocking iBoss..." : "Enabling iBoss...";


			cs_ProgressBar(ImGui::GetTime() * -0.50f, ImVec2(260.0f, 45.0f), val.c_str(), ImVec4(0.20, 0.35, 0.35, 1.0)); // 0.31f, 0.78f, 0.47f, 1.0f


			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor();

			ImGui::SetCursorPos(ImVec2(90.0f, 170.0f));

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20, 0.35, 0.35, 0.0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20, 0.35, 0.35, 1.0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20, 0.35, 0.35, 0.0));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar(2);

			//run_test();

			if (LYCF::iBoss_LYCF::exec_eval)
			{
				std::thread t(LYCF::iBoss_LYCF::ibsa_action_eval);
				t.detach();

				LYCF::iBoss_LYCF::exec_eval = false;
			}

		}
		else if (settings::dialog == 2) // iBoss not Found....
		{
			static bool run = true;
			if (run)
			{
				LYCF::iBoss_LYCF::ibsaclosex = true;
				run = false;
			}


			//ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 125.5f, 70.5f));
			ImGui::SetCursorPos(ImVec2((420.0f / 2) - 48.0f, (250.0f / 2) - 78.0f));
			ImGui::Image((void*)(intptr_t)LYCF::Display::error, ImVec2(96.0f, 96.0f));

			
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6);

			ImGui::SetCursorPos(ImVec2((420.0f / 2) - 170.0f, (250.0f / 2) + 40.0f));
			ImGui::BeginChild("err_ibs", ImVec2(340.0f, 50.0f), true);

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 17.0f, ImGui::GetCursorPos().y + 6.0f));
			ImGui::Text("iBoss was Not Found, Try Again Later...");

			ImGui::EndChild();
			ImGui::PopStyleVar();

		}

		if (LYCF::iBoss_LYCF::ibsaclosex) {

			std::thread tst(&LYCF::iBoss_LYCF::ibsa_autocheck);
			LYCF::iBoss_LYCF::ibsaclosex = false;
			tst.detach();

		}


		if (LYCF::iBoss_LYCF::ibsapopup_exit)
		{
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 35.5, 8.5f));
			if (ImGui::Button(" ", ImVec2(26.5f, 25.0f)))
			{
				*close = false;
				LYCF::iBoss_LYCF::iBossBlockDialog = false;
				ImGui::CloseCurrentPopup();
				LYCF::iBoss_LYCF::iboss_status();
				LYCF::iBoss_LYCF::ibsapopup_exit = false;
				LYCF::iBoss_LYCF::ibsaclosex = true;
			}


			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20, 0.35, 0.35, 0.0));

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 35.5, 8.5f));
			ImGui::ImageButton((void*)(intptr_t)LYCF::Display::close, ImVec2(19.5f, 20.0f));


			ImGui::PopStyleColor();
		}

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

	//ImColor(35, 42, 47);

	style->Colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); //41, 42, 47
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); //41, 42, 47
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); //41, 42, 47
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


void LYCF::Display::caption()
{

	if (LYCF::Display::LoadIcons)
	{
	//	// Load Icons
		static bool error_ret = LoadTextureFromFile("icons//error.png", &LYCF::Display::error, &error_width, &error_height);

		if (!error_ret)
		{
			// TODO; make window display icons are not found in direcotry...
		}

		LYCF::Display::LoadIcons = error_ret ? false : true;
	}

	ImDrawList* drawl = ImGui::GetWindowDrawList();

	// BUTTON COLORS
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.77f, 0.17f, 0.11f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.16f, 0.18f, 1.0f)); // 
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.16f, 0.18f, 1.0f));

	// CLOSE BUTTON
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 74, 0.0f));
	ImGui::PushID("close-btn");
	if (ImGui::ButtonEx(" ", ImVec2(60.0f, 40.0f)))
	{
		LYCF::EN_LYCF::WindowClose();
	}
	ImGui::PopID();

	// MIN BUTTON
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 134.0f, 0.0f));
	ImGui::PushID("minz-btn");
	if (ImGui::ButtonEx(" ", ImVec2(60.0f, 40.0f)))
	{
		LYCF::EN_LYCF::WindowMinimize();
	}
	ImGui::PopID();

	ImGui::PopStyleColor(3);
	// X
	float col_thic_btn = 1.0f;
	ImU32 col_cls_sym = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	drawl->AddLine(ImVec2(795.0f, 15.0f), ImVec2(810.0f, 30.0f), col_cls_sym, col_thic_btn);
	drawl->AddLine(ImVec2(795.0f, 30.0f), ImVec2(810.0f, 15.0f), col_cls_sym, col_thic_btn);

	// --
	// 	drawl->AddLine(ImVec2(ImGui::GetWindowSize().x - 120.0f, 10.0f), ImVec2(ImGui::GetWindowSize().x - 138.0f, 10.0f), col_cls_sym, 1.0f);
	drawl->AddLine(ImVec2((ImGui::GetWindowSize().x - 120.0f) + 21.0f, 22.0f), ImVec2((ImGui::GetWindowSize().x - 138.0f) + 21.0f, 22.0f), col_cls_sym, 1.0f);


}

// Define Bools/Declare Bools...

bool LYCF::EN_LYCF::check_settings;
bool LYCF::EN_LYCF::check_cmd;
bool LYCF::EN_LYCF::check_TManager;


// iBoss Bool...

bool LYCF::iBoss_LYCF::ibs_stat;
bool LYCF::iBoss_LYCF::iBossBlockDialog;

bool LYCF::iBoss_LYCF::iboss_persis;

bool LYCF::iBoss_LYCF::iBossLoginpg;
bool LYCF::iBoss_LYCF::Exist;


// VPN bool...
bool LYCF::VPN_LYCF::vpnstat = false;

bool v0iden::v0iden::WinTun_ = false;

bool showprf_path = false;

bool v0iden::v0iden::connecting;


//Connection Info:

std::string v0iden::v0iden::prf_path;
std::string v0iden::v0iden::Server_Pub_IP;
std::string v0iden::v0iden::Server_Name;
std::string v0iden::v0iden::Port;
std::string v0iden::v0iden::VPN_Protocol;
std::string v0iden::v0iden::Tun_intf;


void style::render()
{
	//button_hovered_color_nothing();
	ImGuiStyle* style = &ImGui::GetStyle();
	//ImGui::ShowDemoWindow();

	//ImGui::Columns(2);
	ImGui::SetCursorPosY(0.0f);
	ImGui::BeginColumns("LYCF", 2, ImGuiColumnsFlags_NoResize);


	ImGui::SetColumnOffset(1, 240);
	{
		//Left Side
		//ImGui::SetScrollHereY(1.0);
		static ImVec4 Active			= ImVec4(0.17f, 0.19f, 0.21f, 1.0f); // ImVec4(0.14f, 0.16f, 0.18f, 1.0f);
		static ImVec4 InActive			= ImVec4(0.10f, 0.13f, 0.14f, 0.0f); //25, 33, 35, 255

		static ImVec4 HoveredInActive	= ImVec4(0.17f, 0.19f, 0.21f, 1.0f); // ImVec4(0.14f, 0.16f, 0.18f, 1.0f);
		static ImVec4 Hovered			= ImVec4(0.17f, 0.19f, 0.21f, 0.4f);

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
		ImGui::SetCursorPosX(10.0f);
		if (ImGui::Button("Enable", ImVec2(250 - 23.0f, 60)))
			settings::tab = 1;


		static auto IBOSS = 2; // iBoss...

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, settings::tab == IBOSS ? Active : InActive);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, settings::tab == IBOSS ? HoveredInActive : Hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, settings::tab == IBOSS ? Active : Hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == IBOSS ? Text : TextInactive);
		ImGui::SetCursorPosX(10.0f);
		if (ImGui::Button("iBoss", ImVec2(250 - 23.0f, 60)))
		{
			settings::tab = 2;
			LYCF::iBoss_LYCF::iboss_status(); // iBoss Status Updater...
			LYCF::iBoss_LYCF::DoesiBossExist();
		}

		static auto VPN = 3; // VPN...

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, settings::tab == VPN ? Active : InActive);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, settings::tab == VPN ? HoveredInActive : Hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, settings::tab == VPN ? Active : Hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == VPN ? Text : TextInactive);
		ImGui::SetCursorPosX(10.0f);
		if (ImGui::Button("VPN", ImVec2(250 - 23.0f, 60)))
			settings::tab = 3;

		static auto PREF = 4; // About...

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, settings::tab == PREF ? Active : InActive);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, settings::tab == PREF ? HoveredInActive : Hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, settings::tab == PREF ? Active : Hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == PREF ? Text : TextInactive);
		ImGui::SetCursorPosX(10.0f);
		if (ImGui::Button("Settings", ImVec2(250 - 23.0f, 60)))
			settings::tab = 4;

		// ICONS

		/*ENABLE*/
		ImGui::SetCursorPos(ImVec2(22.0f, 36.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == ENABLE ? Text : TextInactive);
		ImGui::Text("\xF0\x9F\x94\x93");
		ImGui::PopStyleColor();

		/*iBoss*/
		ImGui::SetCursorPos(ImVec2(22.0f, 106.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == IBOSS ? Text : TextInactive);
		ImGui::Text("\xF0\x9F\x91\xBD");
		ImGui::PopStyleColor();

		/*VPN*/
		ImGui::SetCursorPos(ImVec2(22.0f, 173.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == VPN ? Text : TextInactive);
		ImGui::Text("\xF0\x9F\x8C\x90");
		ImGui::PopStyleColor();
		/*About*/
		ImGui::SetCursorPos(ImVec2(22.0f, 241.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, settings::tab == PREF ? Text : TextInactive);
		ImGui::Text("\xe2\x9a\x99");
		ImGui::PopStyleColor();

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
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.1f);
			ImGui::PushStyleColor(ImGuiCol_Border, LYCF::EN_LYCF::check_settings ? ImVec4(0.31f, 0.78f, 0.47f, 0.5f) : ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 9, 50.0f));


			// Settings Option

			ImGui::BeginChild("Settings", ImVec2(ImGui::GetWindowWidth() - 285, 60), true);
			ImGui::SetCursorPos(ImVec2(20, 22));
			ImGui::Text("Settings & Control Panel");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 66.0, 15.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			//ImGui::Checkbox(" ", &LYCF::EN_LYCF::check_settings);
			LYCF::Display::ToggleButton("chck_stng", &LYCF::EN_LYCF::check_settings);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleVar(2);
			ImGui::EndChild();
			ImGui::PopStyleColor();
			
			// Set Setting Reg
			LYCF::EN_LYCF::SetDword_SettingReg_CheckBased(LYCF::EN_LYCF::check_settings);
			

			// CMD Option
			
			ImGui::PushStyleColor(ImGuiCol_Border, LYCF::EN_LYCF::check_cmd ? ImVec4(0.31f, 0.78f, 0.47f, 0.5f) : ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.1f);

			ImGui::Spacing();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9);
			ImGui::BeginChild("CMD", ImVec2(ImGui::GetWindowWidth() - 285, 60), true);
			ImGui::SetCursorPos(ImVec2(20, 22));
			ImGui::Text("CMD");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 66.0, 15.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			//ImGui::Checkbox(" ", &LYCF::EN_LYCF::check_cmd);
			LYCF::Display::ToggleButton("chck_cmd", &LYCF::EN_LYCF::check_cmd);
			//if (ImGui::IsItemClicked())
			//{
			//	LYCF::EN_LYCF::SetDword_CMDReg_CheckBased(LYCF::EN_LYCF::check_cmd);
			//}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}

			ImGui::PopStyleVar(2);
			ImGui::EndChild();
			ImGui::PopStyleColor();

			// Set CMD reg

			 LYCF::EN_LYCF::SetDword_CMDReg_CheckBased(LYCF::EN_LYCF::check_cmd);

			// TaskManager Option...

			ImGui::PushStyleColor(ImGuiCol_Border, LYCF::EN_LYCF::check_TManager ? ImVec4(0.31f, 0.78f, 0.47f, 0.5f) : ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.1f);

			ImGui::Spacing();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9);
			ImGui::BeginChild("TaskManager", ImVec2(ImGui::GetWindowWidth() - 285, 60), true);
			ImGui::SetCursorPos(ImVec2(20, 22));
			ImGui::Text("TaskManager");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 66.0f, 15.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			//ImGui::Checkbox(" ", &LYCF::EN_LYCF::check_TManager);
			LYCF::Display::ToggleButton("chck_tmgn", &LYCF::EN_LYCF::check_TManager);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::PopStyleColor();

			// Set TaskManager reg
			LYCF::EN_LYCF::SetDword_TManagerReg_CheckBased(LYCF::EN_LYCF::check_TManager);
			
			ImGui::PopStyleVar(2);
			//ImGui::PopStyleColor();
			
			//ImGui::ImageButton(path, ImVec2(20.0f, 20.0f));
		}

		//static bool iboss_persist;


		if (settings::tab == 2) // iBoss
		{

			// iBoss Dialogs...

			if (LYCF::iBoss_LYCF::iBossBlockDialog)
				LYCF::Display::Prompt("Hellow WOrld", &LYCF::iBoss_LYCF::iBossBlockDialog);
			
			//if (LYCF::iBoss_LYCF::iBossLoginpg)
			//	LYCF::Display::Prompt("If you Get an Error Please Try again later..", &LYCF::iBoss_LYCF::iBossLoginpg);


			const char* value = LYCF::iBoss_LYCF::ibs_stat ? "Active (Running)" : "Dead (Not Running)";

			//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 9);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.1f);

			// iBoss Status Indicator...
			ImGui::SetCursorPos(ImVec2(360.0f, -7.0f));
			ImGui::BeginChild("iBoss", ImVec2(295.0f, 50.0f), true);
			ImGui::SetCursorPos(ImVec2(21.5f, 19.0f));
			ImGui::Text("iBoss Status:");
			ImGui::SameLine();
			ImGui::TextColored(LYCF::iBoss_LYCF::ibs_stat ? ImVec4(0.86f, 0.08f, 0.24f, 1.0f) : ImVec4(0.31f, 0.78f, 0.47f, 1.0f), value); // Green : 0.31f, 0.78f, 0.47f, 1.0f, Red : 0.86f, 0.08f, 0.24f, 1.0f
			ImGui::EndChild();
			ImGui::PopStyleVar();

			// iBoss Options...
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 9, 90.0f));
			ImGui::BeginChild("iBoss-Auto", ImVec2(ImGui::GetWindowWidth() - 285, 60), true); // iBoss-Block start...
			ImGui::SetCursorPos(ImVec2(10.0f, 22.0f));
			ImGui::Text("iBoss");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 160, 13.0f));
			ImGui::PopStyleColor();


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.16f, 0.18f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.1f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
			if (ImGui::Button(LYCF::iBoss_LYCF::ibs_stat ? "Block iBoss" : "Enable iBoss", ImVec2(150.0f, 35.0f)))
			{
				//LYCF::Display::Prompt();
				//Prompt = true;
				LYCF::iBoss_LYCF::DoesiBossExist();

				if (LYCF::iBoss_LYCF::Exist) 
				{
					LYCF::iBoss_LYCF::iboss_action(); // iBoss Action....
				}

				LYCF::iBoss_LYCF::iBossBlockDialog = true;
				
				LYCF::iBoss_LYCF::Exist ? settings::dialog = 1 : settings::dialog = 2;
					

				//LYCF::Display::Prompt = true;
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

				ImGui::PushFont(style::bahnschrift_16px);
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25, 0.26, 0.27, 1.0f));
				ImGui::SetTooltip(LYCF::iBoss_LYCF::ibs_stat ? "Click to Block iBoss" : "Click to Enable iBoss");
				ImGui::PopStyleColor();
				ImGui::PopFont();

			}

			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(4);

			ImGui::EndChild(); //iBoss - Block End...

			
			// iBoss Login

			ImGui::Spacing();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9);
			ImGui::BeginChild("iBoss Login", ImVec2(ImGui::GetWindowWidth() - 285.0f, 60.0f), false);
			ImGui::SetCursorPos(ImVec2(10.0f, 22.0f));
			ImGui::Text("iBoss Login (BYOD)");
			ImGui::SameLine();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 120, 13.0f));

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.16f, 0.18f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.16f, 0.18f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.01f);
			if (ImGui::Button("Launch", ImVec2(110.0f, 35.0f)))
			{
				if (ShellExecute(0, 0, "http://connectivitycheck.gstatic.com/", 0, 0, SW_HIDE))
				{
					LYCF::iBoss_LYCF::iBossLoginpg = true;
				}
			}

			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(4);

			ImGui::EndChild();


			// iBoss Persist
			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Border, LYCF::iBoss_LYCF::iboss_persis ? ImVec4(0.31f, 0.78f, 0.47f, 0.5f) : ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9);
			ImGui::BeginChild("iBoss-Persist)", ImVec2(ImGui::GetWindowWidth() - 285.0f, 60.0f), true); // iBoss - Persis Begin...
			ImGui::SetCursorPos(ImVec2(10.0f, 22.0f));
			ImGui::Text("Persistent Block (optional)");
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 65, 15.0));
			//ImGui::Checkbox(" ", &LYCF::iBoss_LYCF::iboss_persist);
			LYCF::Display::ToggleButton("ibs_per", &LYCF::iBoss_LYCF::iboss_persis);
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::EndChild(); //iBoss - Persis End...

			// Set Reg

			LYCF::iBoss_LYCF::SetDword_ibsapersis_Checkbased(LYCF::iBoss_LYCF::iboss_persis);

			//ToggleButton("btn", &LYCF::iBoss_LYCF::iboss_persist);

			// iBoss Note
			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnOffset() + 50, ImGui::GetWindowHeight() - 60));
			ImGui::BeginChild("iBoss-Note", ImVec2(480.0f, 40.0f), true); // Start iBoss Note
			ImGui::SetCursorPos(ImVec2(20.0f, 10.0f));
			ImGui::Text("Note: iBoss Must be Disabled for the VPN to Work Properly");

			ImGui::EndChild(); // End of iBoss Note...

			//ImGui::Checkbox(" ", &LYCF::iBoss_LYCF::ibs_stat);


			ImGui::PopStyleVar();
			//ImGui::PopStyleColor();
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

			ImGui::SetCursorPos(LYCF::VPN_LYCF::vpnstat ? ImVec2(50.0f, 17.0f) : ImVec2(39.0f, 17.0f));
			ImGui::TextColored(LYCF::VPN_LYCF::vpnstat ? ImVec4(0.31f, 0.78f, 0.47f, 1.0f) : ImVec4(0.86f, 0.08f, 0.24f, 1.0f), LYCF::VPN_LYCF::vpnstat ? "CONNECTED" : "DISCONNECTED");

			ImGui::EndChild();

			std::string auto_vpnbtn;

			if (!v0iden::v0iden::prf_path.empty() && v0iden::v0iden::prf_path != "OpenVPN Profile Path")
			{
				auto_vpnbtn = LYCF::VPN_LYCF::vpnstat ? "Disable VPN" : "Enable VPN";
			}
			else
			{
				auto_vpnbtn = "Import Profile";
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);

			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnOffset() + 215.0f, 200.0f));

			if (v0iden::v0iden::connecting)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Button(auto_vpnbtn.c_str(), ImVec2(120.0f, 50.0f)))
			{

				if (v0iden::v0iden::prf_path.empty() || v0iden::v0iden::prf_path == "OpenVPN Profile Path")
				{
					std::thread start_imp_prf(LYCF::WND::GetFilPath, &v0iden::v0iden::prf_path);
					start_imp_prf.detach();
				}
				else 
				{
					if (LYCF::VPN_LYCF::vpnstat)
					{
						std::thread v_d(v0iden::v0iden::v_disconnect);
						v_d.detach();
					}
					else 
					{
						std::thread v_c(v0iden::v0iden::v_connect);
						v_c.detach();
					}
				}
			}
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			if (v0iden::v0iden::connecting)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			if (LYCF::VPN_LYCF::vpnstat)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			}
			ImGui::Spacing();
			ImGui::InputText("Profile Path", &v0iden::v0iden::prf_path);

			if (LYCF::VPN_LYCF::vpnstat)
			{
				ImGui::PopItemFlag();
			}

			ImGui::SameLine();
			if (ImGui::Button("\xe2\x9c\x95", ImVec2(30.0f, 30.0f)))
			{
				if (!LYCF::VPN_LYCF::vpnstat)
					v0iden::v0iden::prf_path = "";
			}


#ifdef IMPORT_PROFILE

			if(ImGui::Button("Import Profile", ImVec2(120.0f, 40.0f)))
			{
				if (!LYCF::VPN_LYCF::vpnstat)
				{
					std::thread imp_prf(LYCF::WND::GetFilPath, &v0iden::v0iden::prf_path);
					imp_prf.detach();
				}
			}
#endif

			//if (ImGui::Button("CLOSE", ImVec2(120.0f, 40.0f)))
			//{
			//	LYCF::WND::cmdl_quit();
			//}

#ifdef _DEBUG
			if (ImGui::Button("Clear Console", ImVec2(120.0f, 40.0f)))
			{
				system("CLS");
			}
#endif

			//ImGui::Text(v0iden::v0iden::prf_path.c_str());
			ImGui::Spacing();

			if (!LYCF::VPN_LYCF::vpnstat)
			{
				LYCF::Display::ToggleButton("WinTun", &v0iden::v0iden::WinTun_);
				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
				ImGui::Text("WinTun");
			}

			if (LYCF::VPN_LYCF::vpnstat)
			{

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

				std::string pub_name(
					" SERVER        \n\xe2\x86\xb3 " + v0iden::v0iden::Server_Name
				);
				ImGui::Text(pub_name.c_str());

				ImGui::SameLine();

				std::string pub_ip(
					" PUBLIC IP    \n\xe2\x86\xb3 " + v0iden::v0iden::Server_Pub_IP
				);
				ImGui::Text(pub_ip.c_str());

				ImGui::SameLine();

				std::string port(
					" PORT		   \n\xe2\x86\xb3 " + v0iden::v0iden::Port
				);
				ImGui::Text(port.c_str());

				ImGui::Spacing();

				std::string vpn_prot(
					" PROTOCOL	 \n\xe2\x86\xb3 " + v0iden::v0iden::VPN_Protocol
				);
				ImGui::Text(vpn_prot.c_str());

				ImGui::Spacing();

				std::string tun_intf(
					" DRIVER  \n\xe2\x86\xb3 " + v0iden::v0iden::Tun_intf
				);
				ImGui::Text(tun_intf.c_str());

			}

			ImGui::PopStyleColor(5);
			ImGui::PopStyleVar(3);
		}

		if (settings::tab == 4) // Settings
		{

			ImGui::PushFont(style::emoji_32px);
			ImGui::Text("\xe2\x9d\xa4 \xE2\x9C\x85 \xE2\x9D\x8C \xF0\x9F\x9A\x80");
			ImGui::PopFont();

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

			if (ImGui::Button("Un-Install", ImVec2(157.0f, 72.0f)))
			{
				TCHAR szFileName[MAX_PATH];

				if (GetModuleFileName(NULL, szFileName, MAX_PATH))
				{
					PathRemoveFileSpec(szFileName);

					std::string fl_nm_dir = szFileName;

					std::string uninstall_path = fl_nm_dir + "\\unins000.exe";

					DWORD file_atr = GetFileAttributes(uninstall_path.c_str());

					if (file_atr != INVALID_FILE_ATTRIBUTES)
					{
						LYCF::WND::cmdl_quit();
						LYCF::WND::start_exec(uninstall_path.c_str());
						std::cout << uninstall_path + "\n";
					}
					else
					{
						std::cout << "Could Not Find Uninstall File...\n";
					}

				}
				
			}
			ImGui::PopStyleVar();

			ImGui::Text("Made with \xe2\x9d\xa4");

		}

		// Caption....
		//settings::tab = 4;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.13f, 0.14f, 0.0f));
		//ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.40f, 0.73f, 0.34f, 1.0f));
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.10f, 0.13f, 0.14f, 0.0f));

		ImGui::SetCursorPos(ImVec2(244.0f, 0.0f));

		ImGui::BeginChild("Caption", ImVec2(472.0f, 40.0f), false);

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