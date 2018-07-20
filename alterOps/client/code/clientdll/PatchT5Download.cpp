// ==========================================================
// secretSchemes
// 
// Component: steam_api
// Sub-component: clientdll
// Purpose: Simple downloader (implement properly for
//          downloading of stats and what not later on.
//
// Initial author: Terminator
// Started: 2012-05-15
// ==========================================================

#include "StdInc.h"
#include <stdio.h>

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

static cmd_function_t download;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void Download_t() 
{
	Com_Printf(1, "-------------------------------------------\n");
	Com_Printf(1, "Initiating download process...\n");
	CURL *curl;
    FILE *fp;
    CURLcode res;
	char *url = "http://secretschemes.net/index.php";
    Com_Printf(1, "Attempting to download file: %s\n", url);
	char outfilename[FILENAME_MAX] = "C:\\index.php";
	Com_Printf(1, "Trying to download file to: %s\n", outfilename);
	curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        CURLcode code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
        if (code == CURLE_OK)
		{
			Com_Printf(1, "File downloaded!\n");
			Com_Printf(1, "-------------------------------------------\n");
		}
		else
		{
			Com_Printf(1, "Your file could not be downloaded!\nError code from CURL: %x.\n", code);
			Com_Printf(1, "-------------------------------------------\n");
		}
		fclose(fp);
    }
	return;
}

void PatchT5_Download()
{
	Cmd_AddCommand("downloadFile", Download_t, &download);
}