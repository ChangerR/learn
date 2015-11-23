#ifndef __SLSERVER_HTTP_CONNECTION_
#define __SLSERVER_HTTP_CONNECTION_
/***************************************************************************
*
*  Project libmjpeg
*
* Copyright (C) 2014-2015, Changer, <dingjinchengyx@163.com>.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
***************************************************************************/

#include "slconfig.h"
#include "curl/curl.h"
#include "HttpStream.h"

class HttpUrlConnection {
	
public:
	HttpUrlConnection(s32 page_size,s32 page_count);
	
	virtual ~HttpUrlConnection();
	
	bool open(const char*);
	
	void close() {
		_running = false;
	}
	
	const char* getResponse();
	
	HttpStream* getHttpOutputStream();

	const bool getRunningState() {
		return _running;
	}
	
protected:	

	static long data_writer_sync(void *data, int size, int nmemb, HttpStream* stream);
	
	static long response_writer(void *data, int size, int nmemb, HttpUrlConnection* _urlconn);
	
private:
	HttpStream* outs;
	char _response[4096];
	int _wpos;
	CURL* handler;
	s32 _page_size, _page_count;
	bool _running;
};
#endif