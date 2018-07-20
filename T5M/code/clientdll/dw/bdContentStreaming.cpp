#include "StdInc.h"
#include "DW.h"
#include "bdLSGServer.h"

#include "cpprest/json.h"

using namespace web;

// fileshare tag
struct wcsfID
{
	uint64_t category;
	uint64_t item;
};

#pragma region utility functions
json::value callToNP(json::value& request)
{
	std::wstring reqBuffer = L"theater\n" + request.to_string();
	std::string req(reqBuffer.begin(), reqBuffer.end());

	static char responseFiller[131072];

	auto async = NP_SendRandomStringExt(req.c_str(), responseFiller, sizeof(responseFiller));
	auto result = async->Wait(7500);

	if (!result)
	{
		return json::value::null();
	}

	std::stringstream str;
	str << responseFiller;

	return json::value::parse(str);
}

json::value generateTags(uint32_t tagCount, wcsfID* tags)
{
	json::value tagArray = json::value::array();

	int tagi = 0;

	for (int i = 0; i < tagCount; i++)
	{
		Trace(__FUNCTION__, "category %lli item %lli", tags[i].category, tags[i].item);

		// skip bots (guid 0)
		if (tags[i].category == 4 && !tags[i].item)
		{
			continue;
		}

		std::wstringstream tagValue;
		tagValue << std::hex << tags[i].item;

		json::value tag;
		tag[0] = (int)tags[i].category;
		tag[1] = json::value::string(tagValue.str());

		tagArray[tagi] = tag;
		tagi++;
	}

	return tagArray;
}
#pragma endregion

#pragma region data types
class bdSummaryMetaHandler : public bdResult
{
public:
	char url[384];
	uint32_t unknownInt;

	virtual void deserialize(bdByteBuffer& buffer)
	{
		buffer.readString(url, sizeof(url));
		buffer.readUInt32(&unknownInt);
	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeString(url);
		buffer.writeUInt32(unknownInt);
	}
};

class bdFileMetaData : public bdResult
{
public:
	uint64_t fileID;

	uint32_t unknownInt1;
	uint32_t unknownInt2;
	uint32_t fileSize;

	uint64_t ownerID;

	char unknownString[64]; // uploader name?

	uint16_t ddlVersion; // ?
	char serverFileName[128]; //?

	char summaryURL[384]; // is it actually summary?

	uint16_t type; // ?!?

	std::string metaData;

	uint32_t summarySize;

	std::vector<wcsfID> tags; // !!

	virtual void deserialize(bdByteBuffer& buffer)
	{

	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeUInt64(fileID);
		buffer.writeUInt32(unknownInt1);
		buffer.writeUInt32(unknownInt2);
		buffer.writeUInt32(fileSize);

		buffer.writeUInt64(ownerID);

		buffer.writeString(unknownString);

		buffer.writeUInt16(ddlVersion);
		buffer.writeString(serverFileName);

		buffer.writeString(summaryURL);

		buffer.writeUInt16(type);

		buffer.writeBlob(metaData);

		buffer.writeUInt32(summarySize);

		buffer.writeArray(10, tags.size(), sizeof(tags[0]), &tags[0]);
	}
};

class bdURL : public bdResult
{
private:

public:
	char url[384];
	uint16_t serverID;
	char serverFilename[128];
	uint64_t serverNPID;

public:
	bdURL()
	{

	}

	virtual void deserialize(bdByteBuffer& buffer)
	{

	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeString(url);
		buffer.writeUInt16(serverID);
		buffer.writeString(serverFilename);
		buffer.writeUInt64(serverNPID);
	}

	void fromJSON(json::value& result)
	{
		wcstombs(url, result[L"url"].as_string().c_str(), sizeof(url));

		serverNPID = result[L"serverNPID"].as_integer();
		serverID = result[L"serverID"].as_integer();

		wcstombs(serverFilename, result[L"serverFilename"].as_string().c_str(), sizeof(serverFilename));
	}
};

class bdFileID : public bdResult
{
public:
	uint64_t fileID;

	bdFileID()
	{
		fileID = 0;
	}

	bdFileID(uint64_t fileID)
		: fileID(fileID)
	{

	}

	virtual void deserialize(bdByteBuffer& buffer)
	{

	}

	virtual void serialize(bdByteBuffer& buffer)
	{
		buffer.writeUInt64(fileID);
	}
};
#pragma endregion

void bdService50::writeContentServerFile(bdServiceServer* server, bdByteBuffer& message)
{
	char fileName[129];
	uint16_t unknownShort1;
	uint32_t fileSize;
	uint16_t unknownShort2;
	std::string dataChecksum;

	message.readString(fileName, sizeof(fileName));
	message.readUInt16(&unknownShort1);
	message.readUInt32(&fileSize);
	message.readUInt16(&unknownShort2);
	
	if (message.readBlob(dataChecksum))
	{
		Trace("bdService50", "call 5 - blob was present");
	}
	
	Trace("bdService50", "'writing' %s", fileName);

	// NP callout
	std::string fnameS = fileName;
	std::wstring fname(fnameS.begin(), fnameS.end());

	json::value request;
	request[L"type"] = json::value::string(U("write"));
	request[L"name"] = json::value::string(fname);

	json::value result = callToNP(request);

	if (result.is_null())
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	// formulate an example reply
	bdURL url;
	url.fromJSON(result);

	auto reply = server->makeReply(0);
	reply->addResult(&url);

	reply->send();
}

void bdService50::confirmFile(bdServiceServer* server, bdByteBuffer& message)
{
	char unknownString1[129];
	uint16_t fileSlot;
	uint16_t serverID;
	char serverFileName[129];
	uint32_t fileSize;
	uint16_t unknownShort2;
	std::string metaData;

	wcsfID* tags;
	uint32_t tagCount;

	message.readString(unknownString1, sizeof(unknownString1));
	message.readUInt16(&fileSlot);
	message.readUInt16(&serverID);
	message.readString(serverFileName, sizeof(serverFileName));
	message.readUInt32(&fileSize);
	message.readUInt16(&unknownShort2);
	message.readBlob(metaData);

	message.readArray(10, &tagCount, 0, (void**)&tags);

	tagCount /= 2;

	Trace(__FUNCTION__, "confirm file [%s/%s] %i %i %i %i md %ib", unknownString1, serverFileName, fileSlot, serverID, fileSize, unknownShort2, metaData.size());

	// request building fun
	std::string fileNameS = serverFileName;
	std::wstring fname(fileNameS.begin(), fileNameS.end());

	size_t outlen;
	char* metaEnc = base64_encode((unsigned char*)metaData.c_str(), metaData.size(), &outlen);
	std::string metaStr(metaEnc, outlen);
	free(metaEnc);

	std::wstring metaWStr(metaStr.begin(), metaStr.end());

	json::value request;
	request[L"type"] = json::value::string(U("confirm"));
	request[L"name"] = json::value::string(fname);
	request[L"slot"] = fileSlot;
	request[L"nptype"] = serverID;
	request[L"size"] = (int32_t)fileSize;
	request[L"tags"] = generateTags(tagCount, tags);
	request[L"meta"] = json::value::string(metaWStr);

	json::value result = callToNP(request);

	if (result.is_null())
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	// formulate result, as usual
	bdFileID fileID(result[L"fileid"].as_integer());

	auto reply = server->makeReply(0);
	reply->addResult(&fileID);

	reply->send();
}

void bdService50::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
	case 5:
		writeContentServerFile(server, data);
		break;
	case 6:
		confirmFile(server, data);
		break;
	default:
		Trace("bdService50", "Unhandled call: %i\n", subType);
	}
}

void bdService58::writeContentServerFile(bdServiceServer* server, bdByteBuffer& message)
{
	char fileName[129];
	uint16_t unknownShort1;
	uint32_t unknownInt1;
	uint16_t unknownShort2;
	std::string unknownBlob;

	message.readString(fileName, sizeof(fileName));
	message.readUInt16(&unknownShort1);
	
	uint32_t idCount;
	uint32_t idSize;
	wcsfID* idStuff;

	message.readArray(10, &idCount, &idSize, (void**)&idStuff);

	Trace("bdService58", "'writing' %s", fileName);

	// NP callout
	std::string fnameS = fileName;
	std::wstring fname(fnameS.begin(), fnameS.end());

	json::value request;
	request[L"type"] = json::value::string(U("writeStreamed"));
	request[L"name"] = json::value::string(fname);

	json::value result = callToNP(request);

	if (result.is_null())
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	// formulate an example reply
	bdURL url;
	url.fromJSON(result);

	auto reply = server->makeReply(0);
	reply->addResult(&url);
	
	reply->send();
}

void bdService58::confirmFile(bdServiceServer* server, bdByteBuffer& message)
{
	uint64_t serverNPID;
	uint16_t serverID;
	char serverFileName[129];
	uint32_t fileSize; // unconfirmed

	message.readUInt64(&serverNPID);
	message.readUInt16(&serverID);
	message.readString(serverFileName, sizeof(serverFileName));
	message.readUInt32(&fileSize);

	Trace("bdStreamedContent", "confirm file %s on %i for %llx (size %i)", serverFileName, serverID, serverNPID, fileSize);

	// NP callout
	std::string fnameS = serverFileName;
	std::wstring fname(fnameS.begin(), fnameS.end());

	std::wstringstream ss;
	ss << std::hex << (serverNPID);

	json::value request;
	request[L"type"] = json::value::string(U("confirmStreamed"));
	request[L"name"] = json::value::string(fname);
	request[L"npid"] = json::value::string(ss.str());
	request[L"nptype"] = serverID;
	request[L"size"] = (int32_t)fileSize;

	json::value result = callToNP(request);

	if (result.is_null())
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	// formulate result, as usual
	bdFileID fileID(result[L"fileid"].as_integer());
	
	auto reply = server->makeReply(0);
	reply->addResult(&fileID);

	reply->send();
}

void bdService58::setFileSummary(bdServiceServer* server, bdByteBuffer& message)
{
	uint64_t fileID;
	uint32_t summarySize;
	std::string metaData;

	uint32_t tagCount;
	wcsfID* tags;

	std::string fileChecksum;

	message.readUInt64(&fileID);
	message.readUInt32(&summarySize);
	message.readBlob(metaData);
	
	message.readArray(10, &tagCount, 0, (void**)&tags);

	tagCount /= 2;

	if (message.readBlob(fileChecksum))
	{
		Trace(__FUNCTION__, "checksum present");
	}

	Trace(__FUNCTION__, "writing summary for file %lld (%ib, %ib metadata), %i tags", fileID, summarySize, metaData.size(), tagCount);

	// NP callout
	size_t outlen;
	char* metaEnc = base64_encode((unsigned char*)metaData.c_str(), metaData.size(), &outlen);
	std::string metaStr(metaEnc, outlen);
	free(metaEnc);

	std::wstring metaWStr(metaStr.begin(), metaStr.end());

	json::value request;
	request[L"type"] = json::value::string(U("writeStreamedSummary"));
	request[L"fileid"] = (int32_t)fileID;
	request[L"size"] = (int32_t)summarySize;
	request[L"meta"] = json::value::string(metaWStr);
	
	request[L"tags"] = generateTags(tagCount, tags);

	json::value result = callToNP(request);

	if (result.is_null())
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	// formulate an example reply
	bdURL url;
	url.fromJSON(result);

	auto reply = server->makeReply(0);
	reply->addResult(&url);

	reply->send();
}

void bdService58::confirmFileSummary(bdServiceServer* server, bdByteBuffer& message)
{
	uint64_t fileID;
	uint32_t fileSize;

	message.readUInt64(&fileID);
	message.readUInt32(&fileSize);

	Trace("bdStreamedContent", "confirm file %lli (size %i)", fileID, fileSize);

	json::value request;
	request[L"type"] = json::value::string(U("confirmStreamedSummary"));
	request[L"size"] = (int32_t)fileSize;
	request[L"ownerid"] = (int32_t)fileID;

	json::value result = callToNP(request);

	auto reply = server->makeReply(0);
	reply->send();
}

void bdService58::getFileSummaries(bdServiceServer* server, bdByteBuffer& message)
{
	uint32_t numIDs;
	uint64_t* ids;

	message.readArray(10, &numIDs, 0, (void**)&ids);

	// prepare a json call, again.
	json::value request;
	request[L"type"] = json::value::string(U("getStreamedSummaries"));

	json::value idArray = json::value::array();
	for (int i = 0; i < numIDs; i++)
	{
		idArray[i] = (int32_t)ids[i];
	}

	request[L"ids"] = idArray;

	json::value result = callToNP(request);

	auto reply = server->makeReply(0);

	bdFileMetaData metaDatas[100];
	int i = 0;

	for (auto& row : result)
	{
		json::value& data = row.second;

		std::wstring metaString = data[L"meta"].as_string();
		std::string metaStr(metaString.begin(), metaString.end());

		size_t metaLen;
		unsigned char* meta = base64_decode(metaStr.c_str(), metaStr.size(), &metaLen);

		metaDatas[i].metaData = std::string((char*)meta, metaLen);
		free(meta);

		metaDatas[i].unknownInt1 = data[L"time"].as_integer();
		metaDatas[i].unknownInt2 = data[L"updateTime"].as_integer();
		metaDatas[i].fileSize = data[L"fileSize"].as_integer(); // from an example
		metaDatas[i].summarySize = data[L"summarySize"].as_integer(); // from an example
		metaDatas[i].fileID = data[L"id"].as_integer();
		metaDatas[i].type = 1;
		strcpy(metaDatas[i].unknownString, "HostUser");
		wcstombs(metaDatas[i].serverFileName, data[L"fileName"].as_string().c_str(), sizeof(metaDatas[i].serverFileName));
		metaDatas[i].ddlVersion = 0;//4;
		//wcstombs(metaDatas[i].summaryURL, data[L"summary"].as_string().c_str(), sizeof(metaDatas[i].summaryURL));
		metaDatas[i].summaryURL[0] = '\0';

		std::wstringstream ws;
		ws << data[L"npid"].as_string();
		ws >> std::hex >> metaDatas[i].ownerID;
		
		for (auto& tag : data[L"tags"])
		{
			wcsfID newTag;
			newTag.category = tag.second[0].as_integer();
			
			std::wstringstream wstr;
			wstr << tag.second[1].as_string();
			wstr >> std::hex >> newTag.item;

			metaDatas[i].tags.push_back(newTag);
		}

		reply->addResult(&metaDatas[i]);

		i++;
	}

	reply->send();
}

void bdService58::getFileSummary(bdServiceServer* server, bdByteBuffer& message)
{
	uint64_t fileID;

	message.readUInt64(&fileID);

	Trace("bdStreamedContent", "get summary for file %lli", fileID);

	json::value request;
	request[L"type"] = json::value::string(U("getStreamedSummary"));
	request[L"fileid"] = (int32_t)fileID;

	json::value result = callToNP(request);

	bdSummaryMetaHandler meta;
	wcstombs(meta.url, result[L"url"].as_string().c_str(), sizeof(meta.url));
	meta.unknownInt = 0;

	auto reply = server->makeReply(0);
	reply->addResult(&meta);
	reply->send();
}

void bdService58::downloadFile(bdServiceServer* server, bdByteBuffer& message)
{
	uint64_t fileID;

	message.readUInt64(&fileID);

	Trace("bdStreamedContent", "get file %lli", fileID);

	json::value request;
	request[L"type"] = json::value::string(U("getStreamedFile"));
	request[L"fileid"] = (int32_t)fileID;

	json::value data = callToNP(request);

	bdFileMetaData meta;
	std::wstring metaString = data[L"meta"].as_string();
	std::string metaStr(metaString.begin(), metaString.end());

	size_t metaLen;
	unsigned char* metaD = base64_decode(metaStr.c_str(), metaStr.size(), &metaLen);

	meta.metaData = std::string((char*)metaD, metaLen);
	free(metaD);

	meta.unknownInt1 = data[L"time"].as_integer();
	meta.unknownInt2 = data[L"updateTime"].as_integer();
	meta.fileSize = data[L"fileSize"].as_integer(); // from an example
	meta.summarySize = data[L"summarySize"].as_integer(); // from an example
	meta.fileID = data[L"id"].as_integer();
	meta.type = 1;
	strcpy(meta.unknownString, "HostUser");
	wcstombs(meta.serverFileName, data[L"fileName"].as_string().c_str(), sizeof(meta.serverFileName));
	meta.ddlVersion = 0;//4;
	wcstombs(meta.summaryURL, data[L"url"].as_string().c_str(), sizeof(meta.summaryURL));

	auto reply = server->makeReply(0);
	reply->addResult(&meta);
	reply->send();
}

void bdService58::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
		case 1:
			getFileSummaries(server, data);
			break;
		case 5:
			writeContentServerFile(server, data);
			break;
		case 6:
			confirmFile(server, data);
			break;
		case 9:
			downloadFile(server, data);
			break;
		case 17:
			setFileSummary(server, data);
			break;
		case 18:
			confirmFileSummary(server, data);
			break;
		case 19:
			getFileSummary(server, data);
			break;
		default:
			Trace("bdService58", "Unhandled call: %i\n", subType);
	}
}

void bdContentSearch::searchByTags(bdServiceServer* server, bdByteBuffer& message)
{
	uint32_t type, start, count;
	bool unknownBool;

	uint32_t tagCount;
	wcsfID* tags;

	message.readUInt32(&type);
	message.readUInt32(&start);
	message.readUInt32(&count);

	message.readBoolean(&unknownBool);

	message.readArray(10, &tagCount, 0, (void**)&tags);

	tagCount /= 2;

	// formulate a request
	json::value request;
	request[L"type"] = json::value::string(U("searchByTags"));
	request[L"start"] = (int32_t)start;
	request[L"count"] = (int32_t)count;
	request[L"tags"] = generateTags(tagCount, tags);

	json::value result = callToNP(request);

	if (result.is_null())
	{
		auto reply = server->makeReply(2);
		reply->send();

		return;
	}

	// formulate an example reply
	bdFileID fileIDs[200];
	
	auto reply = server->makeReply(0);
	int i = 0;

	for (auto& fileID : result)
	{
		fileIDs[i].fileID = fileID.second.as_integer();
		reply->addResult(&fileIDs[i]);

		i++;
	}

	reply->send();
}

void bdContentSearch::callService(bdServiceServer* server, std::string& message)
{
	bdByteBuffer data(message);

	char subType = 0;
	data.readByte(&subType);

	switch (subType)
	{
		case 5:
			searchByTags(server, data);
			break;
		default:
			Trace("bdContentSearch", "Unhandled call: %i\n", subType);
	}
}