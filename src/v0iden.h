#pragma once

#include <openvpn/ovpncli.hpp>

#include "LYCF.h"

namespace v0iden
{
	class v0iden
	{
	public:
		static bool v_connect();
		static bool v_disconnect();

		static std::string prf_path;

		static bool WinTun_;

		// VPN INFO

		static std::string Server_Name;
		static std::string Server_Pub_IP;
		static std::string Port;
		static std::string VPN_Protocol;
		static std::string Tun_intf;
	};
}


