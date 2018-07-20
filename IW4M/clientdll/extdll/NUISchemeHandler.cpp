// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: CEF scheme handler for nui://game/ (rawfiles)
//
// Initial author: NTAuthority
// Started: 2013-01-22
// ==========================================================

#include "StdInc.h"
#include "NUI.h"

#ifdef WE_DO_WANT_NUI
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

FS_FOpenFileRead_t FS_FOpenFileReadTc = (FS_FOpenFileRead_t)0x643270;

typedef bool (__cdecl * Sys_IsDatabaseReady_t)();
static Sys_IsDatabaseReady_t Sys_IsDatabaseReady = (Sys_IsDatabaseReady_t)0x4CA4A0;

class NUIResourceHandler : public CefResourceHandler
{
private:
	char* data_;
	int dataLength_;

	std::string mimeType_;

	bool dataManaged_;

	int read_;

	char* filename_;
public:
	virtual ~NUIResourceHandler()
	{
		if (dataManaged_)
		{
			delete[] data_;
		}
	}

	virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
	{
		REQUIRE_IO_THREAD();

		std::string url = request->GetURL();
		filename_ = _strdup(va("nui/%s", url.substr(11).c_str()));

		// remove # parts
		char* hash = strchr(filename_, '#');

		if (hash)
		{
			hash[0] = '\0';
		}

		int fh;
		
		// FIXME: might be bad regarding threading of file handles?
		int fsLength;

		read_ = 0;

		// open file for thread 5; which is our custom FS thread
		fsLength = FS_FOpenFileReadTc(filename_, &fh, 5);

		if (fsLength >= 0)
		{
			data_ = new char[fsLength];
			FS_Read(data_, fsLength, fh);
			FS_FCloseFile(fh);

			dataLength_ = fsLength;
			dataManaged_ = true;
		}
		else
		{
			/*RawFile* file = (RawFile*)DB_FindXAssetHeader(ASSET_TYPE_RAWFILE, filename_);

			if (file->compressedSize != 0)
			{
				data_ = new char[file->size];
				dataLength_ = file->size;
				dataManaged_ = true;

				mz_stream stream;
				memset(&stream, 0, sizeof(stream));
				stream.avail_in = file->compressedSize;
				stream.next_in = (unsigned char*)file->data;
				stream.avail_out = file->size;
				stream.next_out = (unsigned char*)data_;

				mz_inflateInit(&stream);
				mz_inflate(&stream, MZ_FINISH);
				mz_inflateEnd(&stream);
			}
			else
			{
				data_ = file->data;
				dataLength_ = file->size;
				dataManaged_ = false;
			}*/

			data_ = "";
			dataLength_ = 0;
			dataManaged_ = false;
		}

		// set mime type
		std::string ext = url.substr(url.rfind('.') + 1);

		mimeType_ = "text/html";

		if (ext == "png")
		{
			mimeType_ = "image/png";
		}
		else if (ext == "js")
		{
			mimeType_ = "application/javascript";
		}
		else if (ext == "css")
		{
			mimeType_ = "text/css";
		}

		callback->Continue();

		free(filename_);
		return true;
	}

	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
	{
		REQUIRE_IO_THREAD();

		response->SetMimeType(mimeType_);
		response->SetStatus(200);

		CefResponse::HeaderMap map;
		response->GetHeaderMap(map);

		map.insert(std::make_pair("cache-control", "no-cache, must-revalidate"));
		response->SetHeaderMap(map);

		response_length = dataLength_;
	}

	virtual void Cancel()
	{
		REQUIRE_IO_THREAD();
	}

	virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
	{
		REQUIRE_IO_THREAD();

		bytes_read = 0;

		if (read_ < dataLength_)
		{
			int toRead = min(bytes_to_read, (dataLength_ - read_));

			memcpy(data_out, &data_[read_], toRead);
			read_ += toRead;

			bytes_read = toRead;

			return true;
		}

		return false;
	}

	IMPLEMENT_REFCOUNTING(NUIResourceHandler);
};

CefRefPtr<CefResourceHandler> NUISchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
{
	REQUIRE_IO_THREAD();

	return new NUIResourceHandler();
}
#endif