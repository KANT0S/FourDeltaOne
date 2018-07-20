// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: storage methods
//
// Initial author: NTAuthority
// Started: 2011-07-17
// ==========================================================

#include "StdInc.h"

// buffer details
struct np_get_publisher_file_state_s
{
	NPAsyncImpl<NPGetPublisherFileResult>* asyncResult;
	uint8_t* buffer;
	size_t bufferLength;
};

static void GetPublisherFileCB(NPAsync<INPRPCMessage>* async)
{
	np_get_publisher_file_state_s* state = (np_get_publisher_file_state_s*)async->GetUserData();
	RPCStoragePublisherFileMessage* rpcResult = (RPCStoragePublisherFileMessage*)async->GetResult();

	// print log
	Log_Print("GetPublisherFileCB for %s\n", rpcResult->GetBuffer()->filename().c_str());

	// create and fill out result object
	NPGetPublisherFileResult* result = new NPGetPublisherFileResult();
	result->result = (EGetFileResult)rpcResult->GetBuffer()->result();
	result->buffer = state->buffer;
	result->fileSize = 0;

	// set the buffer to 0
	memset(state->buffer, 0, state->bufferLength);

	// copy file data to the buffer
	if (result->result == GetFileResultOK)
	{
		result->fileSize = rpcResult->GetBuffer()->filedata().length();

		size_t toCopy = (result->fileSize > state->bufferLength) ? state->bufferLength : result->fileSize;
		memcpy(result->buffer, rpcResult->GetBuffer()->filedata().c_str(), toCopy);
		result->fileSize = toCopy;
	}

	// and handle the async result
	state->asyncResult->SetResult(result);
	delete state;
}

LIBNP_API NPAsync<NPGetPublisherFileResult>* LIBNP_CALL NP_GetPublisherFile(const char* fileName, uint8_t* buffer, size_t bufferLength)
{
	RPCStorageGetPublisherFileMessage* request = new RPCStorageGetPublisherFileMessage();
	request->GetBuffer()->set_filename(fileName);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPGetPublisherFileResult>* result = new NPAsyncImpl<NPGetPublisherFileResult>();

	np_get_publisher_file_state_s* state = new np_get_publisher_file_state_s();
	state->asyncResult = result;
	state->buffer = buffer;
	state->bufferLength = bufferLength;

	async->SetCallback(GetPublisherFileCB, state);

	request->Free();

	return result;
}

// buffer details
struct np_get_user_file_state_s
{
	NPAsyncImpl<NPGetUserFileResult>* asyncResult;
	uint8_t* buffer;
	size_t bufferLength;
};

static void GetUserFileCB(NPAsync<INPRPCMessage>* async)
{
	np_get_user_file_state_s* state = (np_get_user_file_state_s*)async->GetUserData();
	RPCStorageUserFileMessage* rpcResult = (RPCStorageUserFileMessage*)async->GetResult();

	// print log
	Log_Print("GetUserFileCB for %s\n", rpcResult->GetBuffer()->filename().c_str());

	// create and fill out result object
	NPGetUserFileResult* result = new NPGetUserFileResult();
	result->result = (EGetFileResult)rpcResult->GetBuffer()->result();
	result->buffer = state->buffer;
	result->fileSize = 0;

	// set the buffer to 0
	memset(state->buffer, 0, state->bufferLength);

	// copy file data to the buffer
	if (result->result == GetFileResultOK)
	{
		result->fileSize = rpcResult->GetBuffer()->filedata().length();

		size_t toCopy = (result->fileSize > state->bufferLength) ? state->bufferLength : result->fileSize;
		memcpy(result->buffer, rpcResult->GetBuffer()->filedata().c_str(), toCopy);
		result->fileSize = toCopy;
	}

	// and handle the async result
	state->asyncResult->SetResult(result);
	delete state;
}

LIBNP_API NPAsync<NPGetUserFileResult>* LIBNP_CALL NP_GetUserFile(const char* fileName, NPID npID, uint8_t* buffer, size_t bufferLength)
{
	RPCStorageGetUserFileMessage* request = new RPCStorageGetUserFileMessage();
	request->GetBuffer()->set_filename(fileName);
	request->GetBuffer()->set_npid(npID);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPGetUserFileResult>* result = new NPAsyncImpl<NPGetUserFileResult>();

	np_get_user_file_state_s* state = new np_get_user_file_state_s();
	state->asyncResult = result;
	state->buffer = buffer;
	state->bufferLength = bufferLength;

	async->SetCallback(GetUserFileCB, state);

	request->Free();

	return result;
}

static void WriteUserFileCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<NPWriteUserFileResult>* asyncResult = (NPAsyncImpl<NPWriteUserFileResult>*)async->GetUserData();
	RPCStorageWriteUserFileResultMessage* result = (RPCStorageWriteUserFileResultMessage*)async->GetResult();

	// handle result object
	NPWriteUserFileResult* writeResult = new NPWriteUserFileResult();
	writeResult->result = (EWriteFileResult)result->GetBuffer()->result();

	// set the auth result
	asyncResult->SetResult(writeResult);
}

LIBNP_API NPAsync<NPWriteUserFileResult>* LIBNP_CALL NP_WriteUserFile(const char* fileName, NPID npID, const uint8_t* buffer, size_t bufferLength)
{
	std::string fileData((char*)buffer, bufferLength);

	RPCStorageWriteUserFileMessage* request = new RPCStorageWriteUserFileMessage();
	request->GetBuffer()->set_filename(fileName);
	request->GetBuffer()->set_npid(npID);
	request->GetBuffer()->set_filedata(fileData);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPWriteUserFileResult>* result = new NPAsyncImpl<NPWriteUserFileResult>();

	async->SetCallback(WriteUserFileCB, result);

	request->Free();

	return result;
}

LIBNP_API void LIBNP_CALL NP_SendRandomString(const char* str)
{
	RPCStorageSendRandomStringMessage* request = new RPCStorageSendRandomStringMessage();
	request->GetBuffer()->set_randomstring(str);

	RPC_SendMessage(request);

	request->Free();
}

struct np_send_random_string_state_s
{
	NPAsyncImpl<NPSendRandomStringResult>* result;
	char* outString;
	size_t outLength;
};

LIBNP_API NPAsync<NPSendRandomStringResult>* LIBNP_CALL NP_SendRandomStringExt(const char* str, char* outStr, size_t outLength)
{
	RPCStorageSendRandomStringMessage* request = new RPCStorageSendRandomStringMessage();
	request->GetBuffer()->set_randomstring(str);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPSendRandomStringResult>* result = new NPAsyncImpl<NPSendRandomStringResult>();

	np_send_random_string_state_s* state = new np_send_random_string_state_s();
	state->result = result;
	state->outString = outStr;
	state->outLength = outLength;

	async->SetCallback([] (NPAsync<INPRPCMessage>* async)
	{
		np_send_random_string_state_s* asyncResult = (np_send_random_string_state_s*)async->GetUserData();
		RPCStorageSendRandomStringMessage* result = (RPCStorageSendRandomStringMessage*)async->GetResult();

		strcpy_s(asyncResult->outString, asyncResult->outLength, result->GetBuffer()->randomstring().c_str());
		asyncResult->outString[asyncResult->outLength - 1] = '\0'; 

		NPSendRandomStringResult* sendResult = new NPSendRandomStringResult();
		sendResult->result = asyncResult->outString;

		asyncResult->result->SetResult(sendResult);
		delete asyncResult;
	}, state);

	request->Free();

	return result;
}