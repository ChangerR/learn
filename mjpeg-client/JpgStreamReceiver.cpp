#include "JpgStreamReceiver.h"
#include "stringc.h"
#include "HttpUrlConnection.h"

DWORD WINAPI JpgStreamReceiver::data_handler(LPVOID data) {

	JpgStreamReceiver* pointer = (JpgStreamReceiver*)data;
	
	while(pointer->_running) {
		pointer->push();
	}

	pointer->_conn->close();
	
	if(pointer->_error_minitor&&pointer->_triger)
		CreateThread(NULL,0,LPTHREAD_START_ROUTINE(pointer->_triger),pointer->_handler_data,0,NULL);

	LOGOUT("data handler return\n");
	return 1;
}

DWORD WINAPI JpgStreamReceiver::http_handler(LPVOID data) {
	
	JpgStreamReceiver* pointer = (JpgStreamReceiver*)data;
	
	pointer->_conn->open(pointer->_url.c_str());
	
	LOGOUT("http handler return\n");
	
	return 1;
}

JpgStreamReceiver::JpgStreamReceiver(int _imgbufCount,int max_):_maxSize(max_)
{
	_conn = new HttpUrlConnection(1024*64,16);
	_buf = new u8[max_];
	_dataHandle = INVALID_HANDLE_VALUE;
	_httpHandle = INVALID_HANDLE_VALUE;
	_running = false;
	_maxImagesCount = _imgbufCount;
	_error_minitor = false;
	_triger = NULL;
	_handler_data = NULL;
	InitializeCriticalSection(&_lock);
}

JpgStreamReceiver::~JpgStreamReceiver( void )
{
	close();

	delete[] _buf;
	if (_conn) {
		delete _conn;
	}
	DeleteCriticalSection(&_lock);
}

void JpgStreamReceiver::lock() {
	EnterCriticalSection(&_lock);
}
	
void JpgStreamReceiver::unlock() {
	LeaveCriticalSection(&_lock);
}

static char buffer[256] = {0};

bool JpgStreamReceiver::readImage(int* len)
{
	int state = 0;
	int recv_len;
	stringc jsHeader;
	u8* buf = _buf;

	HttpStream* outs = _conn->getHttpOutputStream();		

	do 
	{
		memset(buffer,0,256);

		recv_len = outs->readline((u8*)buffer,256);

		if (recv_len == -2) {
			LOGOUT("error read\n");
			return false;
		}else if (recv_len == -1)
		{
			Sleep(1);
		}else
			jsHeader.append(buffer);
		
	} while (!outs->isEOF()
		&&(recv_len > 2||recv_len <= 0||jsHeader.size()<=2));

	//printf("->(\n%s,%d)\n",jsHeader.c_str(),jsHeader.size());
	
	if (outs->isEOF())
		return false;

	int pos = jsHeader.find("Content-Length:",0);
	
	if (pos == std::string::npos) {
		LOGOUT("cannot find length\n");
		return false;
	}

	int pos2 = jsHeader.find("\n",pos);
	if (pos2 == std::string::npos)
		return false;

	pos += 15;
	
	stringc digital = jsHeader.substr(pos,pos2 - pos);
	
	trim(digital);
	
	if (!isDigit(digital)) {
		LOGOUT("(is not digital:%s)\n",digital.c_str());
		return false;
	}

	recv_len = toDigit(digital);
	//LOGOUT("image length=%d\n", recv_len);
	int write_len = 0;
	if (recv_len > *len)
	{
		LOGOUT("We do not have enough memory to fill the jpg!\n");
		*len = -1;		
		while(recv_len) {
			write_len = outs->skip(recv_len);
			recv_len -= write_len;
		}
		return true;
	}
	
	*len = recv_len;
	while(recv_len) {

		write_len = outs->read(buf,recv_len);
		recv_len -= write_len;
		buf += write_len;

	}

	return true;
}

bool JpgStreamReceiver::open(const char* url,error_return_handler handler,LPVOID data) {
	
	if(_dataHandle != INVALID_HANDLE_VALUE || _httpHandle != INVALID_HANDLE_VALUE) {
		LOGOUT("you cannot start Stream receiver twice!!\n");
		return false;
	}
	_running = true;
	_error_minitor = true;
	
	_url = url;
	_triger = handler;
	_handler_data = data;
	
	_httpHandle = CreateThread(NULL,0,JpgStreamReceiver::http_handler,(void*)this,0,NULL);
	
	_dataHandle = CreateThread(NULL,0,JpgStreamReceiver::data_handler,(void*)this,0,NULL);
	
	return true;
}
	
void JpgStreamReceiver::close() {
	
	_error_minitor = false;
	_running = false;
	
	if(_httpHandle != INVALID_HANDLE_VALUE) {
		_conn->close();
		WaitForSingleObject(_httpHandle, INFINITE);  
		CloseHandle(_httpHandle);
		_httpHandle = INVALID_HANDLE_VALUE;
	}
	
	if(_dataHandle != INVALID_HANDLE_VALUE) {

		WaitForSingleObject(_dataHandle, INFINITE);  
		CloseHandle(_dataHandle);
		_dataHandle = INVALID_HANDLE_VALUE;
	}
	
	if(!_images.empty()) {
		lock();
		
		for(list<Image*>::node* p = _images.begin(); p != _images.end();p = p->next) {
			delete p->element;
		}
		unlock();
	}
	
}

void JpgStreamReceiver::push()
{
	int len = _maxSize;
	Image* img = new Image;
	
	if (!readImage(&len)) {
		LOGOUT("get image data false\n");
		_running = false;
		return;
	}

	img->fillImage(_buf, len,IMAGE_JPEG);
	
	lock();

	if (img)
		_images.push_back(img);

	if (_images.getSize() > _maxImagesCount)
	{
		//LOGOUT("images count bigger than maxImagesCount in buffer\n");
		img = _images.begin()->element;
		_images.erase(_images.begin());

		if(img)
			delete img;
	}

	unlock();
}

Image* JpgStreamReceiver::pop()
{
	Image* img;

	lock();

	if (_images.empty())
		img = NULL;
	else {
		img = _images.begin()->element;
		_images.erase(_images.begin());
	}
		
	unlock();
	return img;
}

bool JpgStreamReceiver::restart()
{
	close();
	
	return open(_url.c_str(),_triger,_handler_data);
}


