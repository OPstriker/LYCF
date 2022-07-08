#include "v0iden.h"

#include <iostream>

#include <openvpn/client/clievent.hpp>

using namespace openvpn;
class Client : public ClientAPI::OpenVPNClient
{
private:

	virtual void event(const ClientAPI::Event& ev) override {
		std::cout << ev.info;

		LYCF::VPN_LYCF::vpnstat = ev.name == "CONNECTED" ? true : false;
	}

	virtual void log(const ClientAPI::LogInfo& loginf) override {

		std::cout << loginf.text;

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

void v0iden::v0iden::v_connect()
{
	system("CLS");
	using namespace openvpn::ClientAPI;

	ClientAPI::Config config;

	MergeConfig mc;

	//mc = OpenVPNClientHelper::merge_config("C:\\Users\\skill\\Desktop\\Experiments\\LYCF - DirectX\\LYCF\\Build\\x64\\Release\\profiles\\OPstriker.ovpn", true);

	if (v0iden::prf_path.empty())
		v0iden::prf_path = "OPstriker.ovpn";

	mc = OpenVPNClientHelper::merge_config(v0iden::prf_path, true);

	config.content = mc.profileContent;

	Client client;

	the_client = &client;

	ClientAPI::EvalConfig eval = client.eval_config(config);

	ClientAPI::Status connect_status = client.connect();

}

void v0iden::v0iden::v_disconnect()
{
	system("CLS");
	the_client->stop();

}
