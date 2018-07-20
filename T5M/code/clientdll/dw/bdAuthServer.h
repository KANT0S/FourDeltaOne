#pragma once

class bdAuthServer : public bdServiceServer
{
public:
	bdAuthServer();

	virtual const char* getHostName()
	{
		return "cod7-steam-auth.live.demonware.net";
	}
};

class bdSteamAuthService : public bdService
{
public:
	virtual uint8_t getType() { return 28; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdDediAuthService : public bdService
{
public:
	virtual uint8_t getType() { return 12; }

	virtual void callService(bdServiceServer* server, std::string& message);
};