#pragma once

class bdLSGServer : public bdServiceServer
{
public:
	bdLSGServer();

	virtual const char* getHostName()
	{
		return "cod7-pc-live.lsg.mmp3.demonware.net";
	}
};

class bdLobbyService : public bdService
{
public:
	virtual uint8_t getType() { return 7; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdTitleUtilities : public bdService
{
private:
	void getServerTime(bdServiceServer* server);

public:
	virtual uint8_t getType() { return 12; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdProfiles : public bdService
{
private:
	void getPublicInfos(bdServiceServer* server, bdByteBuffer& message);

	void setPublicInfo(bdServiceServer* server, bdByteBuffer& message);

public:
	virtual uint8_t getType() { return 8; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class MatchMakingInfo;

class bdMatchMaking : public bdService
{
private:
	NPSessionInfo sinfo;
	NPSID npSID;

	
private:
	void updateNPInfo(MatchMakingInfo& info);

	void updateSessionsFromNP(bdServiceServer* server, NPDictionary& dict);

	void updateSessionPlayers(bdServiceServer* server, bdByteBuffer& message);

	void createSession(bdServiceServer* server, bdByteBuffer& message);

	void updateSession(bdServiceServer* server, bdByteBuffer& message);

	void inviteToSession(bdServiceServer* server, bdByteBuffer& message);

	void deleteSession(bdServiceServer* server, bdByteBuffer& message);

	void findSessionsPaged(bdServiceServer* server, bdByteBuffer& message);

	void findSessionsByEntityIDs(bdServiceServer* server, bdByteBuffer& message);

	void findSessionFromID(bdServiceServer* server, bdByteBuffer& message);
	
public:
	virtual uint8_t getType() { return 21; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdStorage : public bdService
{
private:
	void uploadUserFile(bdServiceServer* server, bdByteBuffer& message);
	void getUserFile(bdServiceServer* server, bdByteBuffer& message);
	void getPublisherFile(bdServiceServer* server, bdByteBuffer& message);

public:
	virtual uint8_t getType() { return 10; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdMessaging : public bdService
{
private:
	void sendGlobalInstantMessage(bdServiceServer* server, bdByteBuffer& message);
	void sendInstantMessage(bdServiceServer* server, bdByteBuffer& message);

public:
	virtual uint8_t getType() { return 6; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdService50 : public bdService
{
private:
	void writeContentServerFile(bdServiceServer* server, bdByteBuffer& message);

	void confirmFile(bdServiceServer* server, bdByteBuffer& message);

public:
	virtual uint8_t getType() { return 50; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdContentSearch : public bdService
{
private:
	void searchByTags(bdServiceServer* server, bdByteBuffer& message);

public:
	virtual uint8_t getType() { return 52; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

class bdService58 : public bdService
{
private:
	void getFileSummaries(bdServiceServer* server, bdByteBuffer& message);

	void writeContentServerFile(bdServiceServer* server, bdByteBuffer& message);

	void confirmFile(bdServiceServer* server, bdByteBuffer& message);

	void setFileSummary(bdServiceServer* server, bdByteBuffer& message);

	void confirmFileSummary(bdServiceServer* server, bdByteBuffer& message);

	void getFileSummary(bdServiceServer* server, bdByteBuffer& message);

	void downloadFile(bdServiceServer* server, bdByteBuffer& message);

public:
	virtual uint8_t getType() { return 58; }

	virtual void callService(bdServiceServer* server, std::string& message);
};

/*class bdSteamAuthService : public bdService
{
public:
	virtual uint8_t getType() { return 28; }

	virtual void callService(bdServiceServer* server, std::string& message);
};*/