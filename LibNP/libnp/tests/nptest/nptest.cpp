// nptest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <libnp.h>

bool done = false;

void WatHappen(NPAsync<NPAuthenticateResult>* async)
{
	NPAuthenticateResult* result = async->GetResult();
	printf("%d %llx\n", result->result, result->id);

	NP_SetExternalSteamID(0x1100001026753C3);

	done = true;
}

void RegisterServerCB(NPAsync<NPRegisterServerResult>* async)
{
	NPRegisterServerResult* result = async->GetResult();
	printf("license key %s; id %i\n", result->licenseKey, result->serverID);

	NPAsync<NPAuthenticateResult>* async2 = NP_AuthenticateWithLicenseKey(result->licenseKey);
	async2->SetCallback(WatHappen, NULL);
}

void FileResultCB(NPAsync<NPGetPublisherFileResult>* async)
{
	NPGetPublisherFileResult* result = async->GetResult();
	printf("returned %d\nread %d bytes\n", result->result, result->fileSize);
}

void GetSessionCB(NPAsync<bool>* async)
{
	printf("%d sessions\n", NP_GetNumSessions());

	for (int i = 0; i < NP_GetNumSessions(); i++)
	{
		NPSessionInfo info;
		NP_GetSessionData(i, &info);
		printf("%s\n", info.data.Get("hostname"));
	}
}

void SessionCB(NPAsync<NPCreateSessionResult>* async)
{
	NPCreateSessionResult* result = async->GetResult();
	printf("sid %llu\n", result->sid);

	NPDictionary dict;

	NPAsync<bool>* res = NP_RefreshSessions(dict);
	res->SetCallback(GetSessionCB, NULL);
}

void ProfileCB(NPAsync<NPGetProfileDataResult>* async)
{
	NPGetProfileDataResult* result = async->GetResult();

	printf("profile results %i\n", result->numResults);

	for (int i = 0; i < result->numResults; i++)
	{
		printf("npid %llx, experience %i, prestige %i\n", result->results[i].npID, result->results[i].experience, result->results[i].prestige);
	}
}

void Auth_VerifyIdentity();
char* Auth_GetSessionID();

int _tmain(int argc, _TCHAR* argv[])
{
	NP_Init();
	Auth_VerifyIdentity();

	/*NPDictionary dict;
	dict.Set("hey", "hello");
	dict.Set("hey", "helloo");
	dict.Set("heyho", "hellow");
	printf("%i %s\n", dict.Has("hey"), dict.Get("hey"));

	for (NPKeyValuePair& pair : dict)
	{
		printf("%s -> %s\n", pair.key, pair.value);
	}

	return 1;*/

	//NP_Connect("iw4.prod.fourdeltaone.net", 3025);
	NP_Connect("server.aiw3.net", 3036);

	//NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey("123456789012345678901234");
	//NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithDetails("xnp", "xnpxnp");
	NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithToken(Auth_GetSessionID());
	async->SetCallback(WatHappen, NULL);

	//NPAsync<NPRegisterServerResult>* async = NP_RegisterServer("hello_world.cfg");
	//async->SetCallback(RegisterServerCB, NULL);

	bool doneGroup = false;
	bool doneFriends = false;
	bool doneServers = true;
	bool doneProfiles = true;

	while (true)
	{
		Sleep(1);
		NP_RunFrame();

		if (!doneGroup && NP_GetUserGroup() > 0)
		{
			printf("group %d\n", NP_GetUserGroup());
			doneGroup = true;
		}

		if (done)
		{
			NPID npID;
			NP_GetNPID(&npID);

			printf("%llu\n", npID);

			static uint8_t worldFile[131072];

			//NPAsync<NPGetPublisherFileResult>* fileResult = NP_GetPublisherFile("theWorld.txt", worldFile, sizeof(worldFile));
			NPAsync<NPGetPublisherFileResult>* fileResult = NP_GetPublisherFile("online_mp.img", worldFile, sizeof(worldFile));
			/*NPGetPublisherFileResult* result = fileResult->Wait();
			printf("returned %d\nread %d bytes\n%s\n", result->result, result->fileSize, result->buffer);
			fileResult->Free();*/
			fileResult->SetCallback(FileResultCB, NULL);

			/*NPAsync<NPWriteUserFileResult>* writeResult = NP_WriteUserFile("hi.txt", npID, (const uint8_t*)"hii", 3);
			writeResult->Wait();

			NPAsync<NPGetUserFileResult>* readResult = NP_GetUserFile("hi.txt", npID, worldFile, sizeof(worldFile));
			//NPAsync<NPGetUserFileResult>* readResult = NP_GetUserFile("iw4.stat", npID, worldFile, sizeof(worldFile));
			NPGetUserFileResult* rResult = readResult->Wait();

			printf("blah %s", rResult->buffer);*/
			doneServers = false;
			done = false;
		}

		if (!doneServers)
		{
			NPSessionInfo info;
			//strcpy(info.hostname, "hello world");
			//strcpy(info.mapname, "my map");
			info.maxplayers = 18;
			info.players = 2;
			info.data.Set("hostname", "hello world");
			info.data.Set("mapname", "my map");

			NPAsync<NPCreateSessionResult>* sessionResult = NP_CreateSession(&info);
			sessionResult->SetCallback(SessionCB, NULL);

			doneServers = true;
			doneProfiles = false;
		}

		if (!doneProfiles)
		{
			static NPProfileData profileData[1024];
			static NPID npIDs[2] = 
			{
				0x110000100000002,
				0x11000010000004d,
			};

			NPAsync<NPGetProfileDataResult>* profileResult = NP_GetProfileData(2, npIDs, profileData);
			profileResult->SetCallback(ProfileCB, NULL);

			doneProfiles = true;
		}

		if (!doneFriends)
		{
			if (NP_FriendsConnected())
			{
				Sleep(5000);
				uint32_t numFriends = NP_GetNumFriends();

				for (uint32_t i = 0; i < numFriends; i++)
				{
					NPID npID = NP_GetFriend(i);
					printf("FRIEND %llx - %s %d\n", npID, NP_GetFriendName(npID), NP_GetFriendPresence(npID));

					if (NP_GetFriendPresence(npID) != PresenceStateOffline)
					{
						const char* map = NP_GetFriendRichPresence(npID, "connect");

						if (!map) map = "Unknown";

						printf("%s - %s\n", NP_GetFriendRichPresenceBody(npID), map);
					}
				}

				NP_SetRichPresence("connect", "85.24.129.115:28986");
				//NP_SetRichPresence("map", "mp_nuked");
				//NP_SetRichPresence("gametype", "dom");
				NP_SetRichPresenceBody("Playing Domination on Nuketown");
				NP_StoreRichPresence();

				char buffer[2048];
				NP_GetUserTicket(&buffer, sizeof(buffer), 0);

				doneFriends = true;
			}
		}
	}

	NP_Shutdown();
	return 0;
}

