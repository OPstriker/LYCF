#include "v0iden.h"

#include <iostream>

using namespace openvpn;
class Client : public ClientAPI::OpenVPNClient
{
private:

	virtual void event(const ClientAPI::Event& ev) override {
		std::cout << ev.info;
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
	LYCF::VPN_LYCF::vpnstat = false;

	using namespace openvpn::ClientAPI;

	ClientAPI::Config config;

	MergeConfig mc;

	//mc = OpenVPNClientHelper::merge_config("C:\\Users\\skill\\Desktop\\Experiments\\LYCF - DirectX\\LYCF\\Build\\x64\\Release\\profiles\\OPstriker.ovpn", true);
	mc = OpenVPNClientHelper::merge_config("OPstriker.ovpn", true);

	config.content = mc.profileContent;
	config.autologinSessions = true;

	config.tunPersist = true;

	Client client;

	the_client = &client;

	ClientAPI::EvalConfig eval = client.eval_config(config);

	ClientAPI::Status connect_status = client.connect();
}

void v0iden::v0iden::v_disconnect()
{
	the_client->stop();
}
