#include "v0iden.h"

#include <iostream>

#include <openvpn/client/clievent.hpp>

using namespace openvpn;
class Client : public ClientAPI::OpenVPNClient
{
private:

	virtual void event(const ClientAPI::Event& ev) override {
		//std::cout << ev.info;

		LYCF::VPN_LYCF::vpnstat = ev.name == "CONNECTED" ? true : false;

		if (!LYCF::VPN_LYCF::vpnstat && ev.name != "DISCONNECTED")
		{
			v0iden::v0iden::connecting = true;
		}
		else {
			v0iden::v0iden::connecting = false;
		}
		

		v0iden::v0iden::Server_Pub_IP = connection_info().serverIp;
		v0iden::v0iden::Server_Name = connection_info().serverHost;
		v0iden::v0iden::Port = connection_info().serverPort;
		v0iden::v0iden::VPN_Protocol = connection_info().serverProto;
		v0iden::v0iden::Tun_intf = connection_info().tunName;

#ifdef _DEBUG
		//std::cout << ev.name + "\n";
#endif // 

		
	}

	virtual void log(const ClientAPI::LogInfo& loginf) override {

#ifdef _DEBUG

		std::cout << loginf.text;

#endif

	}

	virtual void external_pki_cert_request(ClientAPI::ExternalPKICertRequest& certreq) override {
	}

	virtual void external_pki_sign_request(ClientAPI::ExternalPKISignRequest& signreq) override {

	}

	virtual bool pause_on_connection_timeout() override {
		return false;
	}

};

static Client* the_client = nullptr;

bool v0iden::v0iden::v_connect()
{
	using namespace openvpn::ClientAPI;

	if (!v0iden::prf_path.empty())
	{
		ClientAPI::Config config;

		MergeConfig mc;

		//mc = OpenVPNClientHelper::merge_config("C:\\Users\\skill\\Desktop\\Experiments\\LYCF - DirectX\\LYCF\\Build\\x64\\Release\\profiles\\OPstriker.ovpn", true);

		mc = OpenVPNClientHelper::merge_config(v0iden::prf_path, true);

		config.content = mc.profileContent;
		config.wintun = v0iden::WinTun_;

		Client client;

		the_client = &client;

		ClientAPI::EvalConfig eval = client.eval_config(config);


		ClientAPI::Status connect_status = client.connect();

		if (connect_status.error)
		{
			return false;
		}

		return true;

	}

}

bool v0iden::v0iden::v_disconnect()
{
	the_client->stop();

	if (LYCF::VPN_LYCF::vpnstat)
	{
		return false;
	}
	return true;
}
