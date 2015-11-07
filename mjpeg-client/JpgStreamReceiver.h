#ifndef __ROV_RECEIVER_
#define __ROV_RECEIVER_

#include "slconfig.h"
#include "HttpUrlConnection.h"

class JpgStreamReceiver 
{
public:
	typedef DWORD WINAPI (*error_return_handler)(LPVOID);
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
