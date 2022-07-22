 // Have to call first else causes issues... with WinSock...

// Boost.asio
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "tab.h"
#include "LYCF.h"
#include <iostream>

#include <glad/glad.h>
#include <string.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <fstream>
#include <iostream>
#include "v0iden.h"


//#include <openvpn/common/platform.hpp>
//#include <openvpn/options/merge.hpp>
//#include <openvpn/client/cliconstants.hpp>

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
    DWORD data{};
    DWORD sizeData = sizeof(data);

    
        RegGetValueA(hKey,
        path,
        v_name,
        dwFlag,
        NULL,
        &data,
        &sizeData);

    // Update Setting At Load...
    static bool ibsapProceed = true;
    if (v_name == "Start")
    {
        if (ibsapProceed)
            if (data == 4 ? LYCF::iBoss_LYCF::iboss_persis = true : LYCF::iBoss_LYCF::iboss_persis = false)
                ibsapProceed = false;
    }

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

    LYCF::EN_LYCF::LoadReg(HKEY_CURRENT_USER, path, v_name, REG_DWORD);
}

void LYCF::EN_LYCF::SetDword_CMDReg_CheckBased(bool reg)
{
    static bool En_Done = false;
    static bool Dis_Done = false;

    LPCSTR path = "SOFTWARE\\Policies\\Microsoft\\Windows\\System";
    LPCSTR v_name = "DisableCMD";

    //LYCF::EN_LYCF::LoadReg(HKEY_CURRENT_USER, path, v_name, REG_DWORD);

    if (reg)
    {
        if (En_Done)
        {
            // Enable CMD
            // 
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

    LYCF::EN_LYCF::LoadReg(HKEY_CURRENT_USER, path, v_name, REG_DWORD);

}

void LYCF::EN_LYCF::SetDword_TManagerReg_CheckBased(bool reg)
{
    static bool En_Done = false;
    static bool Dis_Done = false;

    LPCSTR path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
    LPCSTR v_name = "DisableTaskMgr";

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

    LYCF::EN_LYCF::LoadReg(HKEY_CURRENT_USER, path, v_name, REG_DWORD);

}

void LYCF::iBoss_LYCF::SetDword_ibsapersis_Checkbased(bool reg)
{
    static bool En_Done_p = false;
    static bool Dis_Done_p = false;

    LPCSTR path = "SYSTEM\\CurrentControlSet\\Services\\ibsaService.exe";
    LPCSTR v_name = "Start";

    if (reg)
    {
        if (En_Done_p)
        {
            // Enable iBoss on Startup
            LYCF::EN_LYCF::SetRegDword(HKEY_LOCAL_MACHINE,
                path,
                v_name,
                00000002); // Automatic
            En_Done_p = false;
        }
        else {
            Dis_Done_p = true;
        }
    }

    if (!reg)
    {
        if (Dis_Done_p)
        {
            // Disable iBoss on Startup
            LYCF::EN_LYCF::SetRegDword(HKEY_LOCAL_MACHINE,
                path,
                v_name,
                00000004);
            Dis_Done_p = false;
        }
        else {
            En_Done_p = true;
        }
    }

    LYCF::EN_LYCF::LoadReg(HKEY_LOCAL_MACHINE, path, v_name, REG_DWORD);
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

    CloseServiceHandle(path);
    CloseServiceHandle(iboss);
}

void LYCF::iBoss_LYCF::DoesiBossExist()
{
    SC_HANDLE Manger = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);

    SC_HANDLE iboss_hndle = OpenService(Manger, "ibsaservice.exe", SC_MANAGER_ALL_ACCESS);

    if (iboss_hndle == NULL)
    {
        DWORD ibs_error = GetLastError();

        if (ibs_error == ERROR_SERVICE_DOES_NOT_EXIST || ERROR_INVALID_HANDLE || ERROR_INVALID_NAME)
        {
            LYCF::iBoss_LYCF::Exist = false; // Must be FALSE
        }
    }
    else
    {
        LYCF::iBoss_LYCF::Exist = true;
    }

    CloseServiceHandle(Manger);
    CloseServiceHandle(iboss_hndle);
}


void ibsa_stat_eval(const boost::system::error_code&)
{

    SC_HANDLE eval_path;
    SC_HANDLE ibs_evalhdl;
    SERVICE_STATUS ibseval_ret;

    eval_path = OpenSCManager(
        NULL,
        SERVICES_ACTIVE_DATABASE,
        SC_MANAGER_ALL_ACCESS
    );

    ibs_evalhdl = OpenService(
        eval_path,
        "ibsaservice.exe",
        SC_MANAGER_ALL_ACCESS
    );



    if (!QueryServiceStatus(ibs_evalhdl, &ibseval_ret))
    {
        goto eval_cleanup;
    }


    if (ibseval_ret.dwCurrentState == SERVICE_STOPPED && LYCF::iBoss_LYCF::ibsa_stop_active)
    {
        LYCF::iBoss_LYCF::exec_eval = false;
        LYCF::iBoss_LYCF::iBossBlockDialog = false;
        settings::dialog = 0;
        LYCF::iBoss_LYCF::iboss_status();

        LYCF::iBoss_LYCF::ibsa_stop_active = false;
    }
    else if (ibseval_ret.dwCurrentState == SERVICE_RUNNING && LYCF::iBoss_LYCF::ibsa_start_active)
    {
        LYCF::iBoss_LYCF::exec_eval = false;
        LYCF::iBoss_LYCF::iBossBlockDialog = false;
        settings::dialog = 0;
        LYCF::iBoss_LYCF::iboss_status();

        LYCF::iBoss_LYCF::ibsa_start_active = false;
    }
    else if (ibseval_ret.dwCurrentState == SERVICE_STOP_PENDING && LYCF::iBoss_LYCF::ibsa_stop_active)
    {
        LYCF::iBoss_LYCF::exec_eval = true;

    }
    else if (ibseval_ret.dwCurrentState == SERVICE_START_PENDING && LYCF::iBoss_LYCF::ibsa_start_active)
    {
        LYCF::iBoss_LYCF::exec_eval = true;
    }


eval_cleanup: 
    {
    CloseServiceHandle(eval_path);
    CloseServiceHandle(ibs_evalhdl);
    }
}

bool LYCF::iBoss_LYCF::ibsa_start_active = false;
bool LYCF::iBoss_LYCF::ibsa_stop_active = false;


void LYCF::iBoss_LYCF::iboss_action()
{

    DWORD state{};
    SC_HANDLE path_2;
    SC_HANDLE iboss_handle;
    SERVICE_STATUS iboss_ret;

    path_2 = OpenSCManager(
        NULL,
        SERVICES_ACTIVE_DATABASE,
        SC_MANAGER_ALL_ACCESS
    );

    iboss_handle = OpenService(
        path_2,
        "ibsaservice.exe",
        SC_MANAGER_ALL_ACCESS
    );

    if (LYCF::iBoss_LYCF::ibs_stat)
    {
        if (!ControlService(iboss_handle, SERVICE_CONTROL_STOP, &iboss_ret))
        {
            goto cleanup;
        }
        else {
            LYCF::iBoss_LYCF::ibsa_stop_active = true;
        }
    }
    else
    {
        if (!StartService(iboss_handle, 0, NULL))
        {
            goto cleanup;
        }
        else {
            LYCF::iBoss_LYCF::ibsa_start_active = true;
        }
    }

    goto cleanup;
cleanup:
    {
        CloseServiceHandle(path_2);
        CloseServiceHandle(iboss_handle);
    }
}



void LYCF::iBoss_LYCF::ibsa_action_eval()
{
    boost::asio::io_service io;

    boost::asio::deadline_timer ti(io, boost::posix_time::seconds(4));
    ti.async_wait(ibsa_stat_eval);

    io.run();

    LYCF::iBoss_LYCF::exec_eval = true;
}


void setbool(const boost::system::error_code&)
{
    //LYCF::iBoss_LYCF::ibsapopup_exit = true;
    LYCF::iBoss_LYCF::iBossBlockDialog = false;
    LYCF::iBoss_LYCF::ibsaclosex = true;
    ImGui::CloseCurrentPopup();
}


void LYCF::iBoss_LYCF::ibsa_autocheck()
{

    using namespace std::chrono_literals;

    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::milliseconds(1650));
    
    t.async_wait(&setbool);

    io.run();
}


void LYCF::Display::ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    float height = ImGui::GetFrameHeight() + 7.0f; // 4.9f
    float width = height * 1.75f; //1.75 //1.55f
    float radius = height * 0.46f; //0.46f //0.50f
    
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

void LYCF::VPN_LYCF::vpn_disc()
{
    if (LYCF::VPN_LYCF::vpnstat)
    {
        v0iden::v0iden::v_disconnect();
    }

    while (LYCF::VPN_LYCF::vpnstat)
    {
        Sleep(1);

        if (!LYCF::VPN_LYCF::vpnstat)
            break;
    }
}

void LYCF::WND::start_exec(LPCSTR pName)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateProcess(pName,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
