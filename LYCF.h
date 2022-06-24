#pragma once
//#include "_ovpn.h"


#include <Windows.h>
#include <glad/glad.h>
#include <shellapi.h>
#include "tab.h"


namespace LYCF {

	class Display
	{
	private:

	public:
		static void caption();

		static void Prompt(const char* Text, bool* close);

		static void ToggleButton(const char* str_id, bool* v);


		static bool LoadIcons;

		static GLuint close;
		static GLuint minz;
		static GLuint error;
	};

	class EN_LYCF
	{

	public:
		static void h_System(LPCSTR xPath, LPCSTR xCmd); // Hidden ShellExecute...
		static void SetRegDword(HKEY key, LPCSTR path, LPCSTR v_name, DWORD data);
		static void LoadReg(HKEY hKey, LPCSTR path, LPCSTR v_name, DWORD dwFlag);

		static void SetDword_SettingReg_CheckBased(bool reg);
		static void SetDword_CMDReg_CheckBased(bool reg);
		static void SetDword_TManagerReg_CheckBased(bool reg);

		static void WindowMove();
		static void WindowClose();
		static void WindowMinimize();

		// Enable Tab
		static bool check_settings;
		static bool check_cmd;
		static bool check_TManager;
		static bool captionmove;
		static bool enable;
	};

	class iBoss_LYCF
	{
	public:

		// Functions...
		static void iboss_status();
		static void iboss_action();
		static void DoesiBossExist();

		static void ibsa_autocheck();
		static void ibsa_action_eval();

		static void SetDword_ibsapersis_Checkbased(bool reg);


		// BOOLS
		static bool Exist;
		static bool ibs_stat;
		static bool iboss_persis;
		static bool iBossBlockDialog;
		static bool iBossLoginpg;
		static bool ibsapopup_exit;
		static bool ibsaclosex;
		static bool ibsaction_eval;

		static bool ibsa_start_active;
		static bool ibsa_stop_active;

		static bool exec_eval;


		static std::string ibs_stat_log_class;

	};

	class VPN_LYCF
	{
	public:
		static bool vpnstat;

		static void vpnconnect();

	};

};


