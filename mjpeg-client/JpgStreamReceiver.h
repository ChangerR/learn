#ifndef __ROV_RECEIVER_
#define __ROV_RECEIVER_
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
#include "Image.h"
#include "list.h"

class HttpUrlConnection;

class JpgStreamReceiver 
{
public:
	typedef DWORD (WINAPI *error_return_handler)(LPVOID);
public:

	JpgStreamReceiver(int _imgbufCount,int max_ = 1024*512);

	virtual ~JpgStreamReceiver(void);

	Image* pop();

	void lock();
	
	void unlock();
	
	bool getRunningState(){
		return _running;
	}
	
	bool restart();

	bool open(const char* url,error_return_handler handler,LPVOID data);
	
	void close();
	
private:
	static DWORD WINAPI data_handler(LPVOID data);
	
	static DWORD WINAPI http_handler(LPVOID data);
	
private:
	bool readImage(int *len);
	
	void push();
	
private:

	HttpUrlConnection* _conn;
	list<Image*> _images;
	u8* _buf;
	int _maxSize;
	HANDLE _dataHandle;
	HANDLE _httpHandle;
	CRITICAL_SECTION _lock;
	bool _running;
	stringc _url;
	int _maxImagesCount;
	bool _error_minitor;
	error_return_handler _triger;
	void* _handler_data;
};
#endif
