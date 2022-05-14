#include "_ovpn.h" // Have to call first else causes issues... with WinSock...
#include "LYCF.h"
#include <iostream>
#include "tab.h"
#include <glad/glad.h>
#include "stb_image.h"
#include <string.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


#include <fstream>
#include <iostream>

#include <openvpn/common/platform.hpp>
//#include <openvpn/options/merge.hpp>
#include <openvpn/client/cliconstants.hpp>

// Caption...




bool LYCF::EN_LYCF::captionmove = false;


void LYCF::EN_LYCF::h_System(LPCSTR xPath, LPCSTR xCmd) // MS-WINDOWS System CMD hidden...
{
	ShellExecuteA(NULL, "open", xPath, xCmd, NULL, SW_HIDE);
}

void LYCF::EN_LYCF::SetRegDword(HKEY key, LPCSTR path, LPCSTR v_name, DWORD data)
{

    HKEY hKey;
    DWORD dwData = data; // Disabled~
    RegCreateKeyEx(
        key,
        path,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS | KEY_WOW64_32KEY,
        NULL,
        &hKey,
        NULL);

    RegSetValueEx(
        hKey,
        v_name,
        NULL,
        REG_DWORD,
        (LPBYTE)&dwData,
        sizeof(dwData)
        );
    RegCloseKey(hKey);

}

void LYCF::EN_LYCF::LoadReg(HKEY hKey, LPCSTR path, LPCSTR v_name, DWORD dwFlag)
{
    DWORD data = 0;
    DWORD sizeData = sizeof(data);

    
        RegGetValueA(hKey,
        path,
        v_name,
        dwFlag,
        NULL,
        &data,
        &sizeData);

    // Update Setting At Load...
    static bool settingProceed = true;
    if (v_name == "NoControlPanel")
    {
        if (settingProceed)
            if (data == 0 ? LYCF::EN_LYCF::check_settings = true : LYCF::EN_LYCF::check_settings = false)
                settingProceed = false;
    }

    static bool cmdProceed = true;
    if (v_name == "DisableCMD")
    {
        if (cmdProceed)
            if (data == 0 ? LYCF::EN_LYCF::check_cmd = true : LYCF::EN_LYCF::check_cmd = false)
                cmdProceed = false;
    }

    static bool TManagerProceed = true;
    if (v_name == "DisableTaskMgr")
    {
        if (TManagerProceed)
            if (data == 0 ? LYCF::EN_LYCF::check_TManager = true : LYCF::EN_LYCF::check_TManager = false)
                TManagerProceed = false;
    }
}

void LYCF::EN_LYCF::SetDword_SettingReg_CheckBased(bool reg)
{
    static bool En_Done = false;
    static bool Dis_Done = false;

    LPCSTR path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";
    LPCSTR v_name = "NoControlPanel";

    LYCF::EN_LYCF::LoadReg(HKEY_CURRENT_USER, path, v_name, REG_DWORD);


    if (reg)
    {
        if (En_Done)
        {
            // Enable Settings
            LYCF::EN_LYCF::SetRegDword(HKEY_CURRENT_USER,
                path,
                v_name,
                00000000);
            En_Done = false;
        }
        else {
            Dis_Done = true;
        }
    }
    
    if (!reg)
    {
        if (Dis_Done)
        {
            // Disable setting
            LYCF::EN_LYCF::SetRegDword(HKEY_CURRENT_USER,
                path,
                v_name,
                00000001);
            Dis_Done = false;
        }
        else {
            En_Done = true;
        }
    }
}

void LYCF::EN_LYCF::SetDword_CMDReg_CheckBased(bool reg)
{
    static bool En_Done = false;
    static bool Dis_Done = false;

    LPCSTR path = "SOFTWARE\\Policies\\Microsoft\\Windows\\System";
    LPCSTR v_name = "DisableCMD";

    LYCF::EN_LYCF::LoadReg(HKEY_CURRENT_USER, path, v_name, REG_DWORD);

    if (reg)
    {
        if (En_Done)
        {
            // Enable CMD
            LYCF::EN_LYCF::SetRegDword(HKEY_CURRENT_USER,
                path,
                v_name,
                00000000);
            En_Done = false;
        }
        else {
            Dis_Done = true;
        }
    }

    if (!reg)
    {
        if (Dis_Done)
        {
            // Disable CMD
            LYCF::EN_LYCF::SetRegDword(HKEY_CURRENT_USER,
                path,
                v_name,
                00000001);
            Dis_Done = false;
        }
        else {
            En_Done = true;
        }
    }
}

void LYCF::EN_LYCF::SetDword_TManagerReg_CheckBased(bool reg)
{
    static bool En_Done = false;
    static bool Dis_Done = false;

    LPCSTR path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
    LPCSTR v_name = "DisableTaskMgr";

    LYCF::EN_LYCF::LoadReg(HKEY_CURRENT_USER, path, v_name, REG_DWORD);

    if (reg)
    {
        if (En_Done)
        {
            // Enable CMD
            LYCF::EN_LYCF::SetRegDword(HKEY_CURRENT_USER,
                path,
                v_name,
                00000000);
            En_Done = false;
        }
        else {
            Dis_Done = true;
        }
    }

    if (!reg)
    {
        if (Dis_Done)
        {
            // Disable CMD
            LYCF::EN_LYCF::SetRegDword(HKEY_CURRENT_USER,
                path,
                v_name,
                00000001);
            Dis_Done = false;
        }
        else {
            En_Done = true;
        }
    }
}

void LYCF::iBoss_LYCF::iboss_status()
{
    SC_HANDLE path;
    SERVICE_STATUS iboss_stat;
    SC_HANDLE iboss;
    DWORD value{};
    
    path = OpenSCManager(NULL, // Path
        SERVICES_ACTIVE_DATABASE,
        SC_MANAGER_ALL_ACCESS);
    
    iboss = OpenServiceW(path, // iBoss Path
        L"ibsaservice.exe",
        SC_MANAGER_ALL_ACCESS);
    
    if (QueryServiceStatus(iboss, &iboss_stat))
    
    value = iboss_stat.dwCurrentState;
    
    if (value == SERVICE_RUNNING)
    {
        LYCF::iBoss_LYCF::ibs_stat = true;
    }
    else {
        LYCF::iBoss_LYCF::ibs_stat = false;
    }
}

void LYCF::Display::ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight() + 4.9f;
    float width = height * 1.75f; //1.55f
    float radius = height * 0.46f; //0.50f

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;

    float t = *v ? 1.0f : 0.0f;

    ImGuiContext& g = *GImGui;
    float ANIM_SPEED = 0.16f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
    {
        float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }

    ImU32 col_bg;
    ImU32 toggle_col = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::IsItemHovered())
    {
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 0.8f), ImVec4(0.31f, 0.78f, 0.47f, 0.9f), t));
        toggle_col = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.85f));
        //col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.31f, 0.78f, 0.47f, 0.92f), t)); // 2 - 0.64f, 0.83f, 0.34f, 1.0f
    }
    else {
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 0.8f), ImVec4(0.31f, 0.78f, 0.47f, 0.9f), t));
        //col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.31f, 0.78f, 0.47f, 1.0f), t)); // 1.0f, 0.56f, 0.83f, 0.26f, 1.0f
    }

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(p.x + 1.2 + radius + t * (width - radius * 2.15f), p.y + radius + 1.2), radius - 2.0f, toggle_col); //M_COL32(255, 255, 255, 255)
}


class Client : public openvpn::ClientAPI::OpenVPNClient
{
public:
    virtual void event(const openvpn::ClientAPI::Event&) override
    {  // events delivered here

        
    }

    virtual void log(const openvpn::ClientAPI::LogInfo&) override
    {  // logging delivered here

    }

};



void LYCF::VPN_LYCF::vpnconnect()
{


    //std::string path = "/profiles/OPstriker.ovpn";
    //
    //openvpn::ProfileMerge pm("/profiles/OPstriker.ovpn", "ovpn", "", openvpn::ProfileMerge::FOLLOW_FULL, openvpn::ProfileParseLimits::MAX_LINE_SIZE, openvpn::ProfileParseLimits::MAX_PROFILE_SIZE);
    //
    //openvpn::ClientAPI::Config config;
    //
    //config.content = pm.profile_content();
    //
    //Client client;
    //
    //client.eval_config(config);
    //client.connect();

}
