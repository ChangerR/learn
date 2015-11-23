#include "HttpUrlConnection.h"
#include "curl/curl.h"
#include "HttpStream.h"
#include <stdio.h>
#include <string.h>

#ifdef SLSERVER_WIN32
#pragma comment(lib,"libcurl.lib")
#endif

long HttpUrlConnection::data_writer_sync(void *data, int size, int nmemb, HttpStream* stream) { 
	return stream->write((u8*)data,size*nmemb,false);
}

long HttpUrlConnection::response_writer(void *data, int size, int nmemb, HttpUrlConnection* _urlconn) {
	long sizes = size * nmemb;
	if(_urlconn->_wpos + sizes >= 4096)
		return sizes;
	memcpy(_urlconn->_response + _urlconn->_wpos, (char*)data, sizes);
	_urlconn->_wpos += sizes;
	return sizes;
}

HttpUrlConnection::HttpUrlConnection(s32 page_size,s32 page_count){	
	
	_wpos = 0;
	_page_size = page_size;
	_page_count = page_count;
	handler = curl_easy_init();
	_running = false;
	outs = new HttpStream(_page_size, _page_count);
}

HttpUrlConnection::~HttpUrlConnection() {
	
	curl_easy_cleanup(handler);
	if (outs)
		delete outs;
}

static int progress_callback(void *clientp,double dltotal,double dlnow,double ultotal,double ulnow) {

	HttpUrlConnection* _conn = (HttpUrlConnection*)clientp;
	
	if (_conn->getRunningState() != true) {
		printf("progress_callback call for close\n");
		return CURLE_ABORTED_BY_CALLBACK;
	}
	
	return 0;
}

bool HttpUrlConnection::open(const char* url) {
	
	_wpos = 0;
	_running = true;
	
	memset(_response,0,4096);
	
	outs->clear();
	
	curl_easy_setopt(handler, CURLOPT_URL, url);
	curl_easy_setopt(handler, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(handler, CURLOPT_FOLLOWLOCATION,1);

	curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, HttpUrlConnection::data_writer_sync);
	curl_easy_setopt(handler, CURLOPT_WRITEDATA, outs);

	//curl_easy_setopt(handler, CURLOPT_TIMEOUT_MS, 5000L);

	curl_easy_setopt(handler, CURLOPT_HEADERFUNCTION, HttpUrlConnection::response_writer);
	curl_easy_setopt(handler, CURLOPT_WRITEHEADER, this);
	
	
	CURLcode res = CURLE_OK;
	
	int retcode = 0;
	res = curl_easy_perform(handler);
	res = curl_easy_getinfo(handler, CURLINFO_RESPONSE_CODE , &retcode);
	
	outs->setEOF(true);
	
	if ( (res == CURLE_OK) && retcode == 200 ) {		
		return true;
	}
	else
		return false;
}

HttpStream* HttpUrlConnection::getHttpOutputStream(){
	return outs;
}

const char* HttpUrlConnection::getResponse() {
	return _response;
}
