#ifndef ABR_SOCKETIO_H
#define ABR_SOCKETIO_H
#include <windows.h>
#include <string>

struct libwebsocket;
struct libwebsocket_context;
struct libwebsocket_protocols;

class SIODelegate {
public:

	virtual ~SIODelegate() {}
	
	virtual void onConnect(SIOClient* client) = 0;
	
	virtual void onMessage(SIOClient* client, const std::string& data) = 0;
	
	virtual void onClose(SIOClient* client) = 0;
	
	virtual void onError(SIOClient* client, const std::string& data) = 0;
};
	
class SocketIO {
public:
	enum SOCKETIO_VERSION {
		SIO_V_0_X,SIO_V_1_X,
	};
	
	SocketIO(const char* host,unsigned short port,
				SOCKETIO_VERSION v = SIO_V_0_X,int ssl);
	
	virtual ~SocketIO();
	
	bool open(SIODelegate* del);
	
	void close();
	
private:
	
	static DWORD WINAPI http_handler(LPVOID data);
	
	bool handShake(const std::string& s);
	
private:
	struct libwebsocket*         _wsInstance;
    struct libwebsocket_context* _wsContext;
	int _SSLConnection;
    struct libwebsocket_protocols* _wsProtocols;
	HANDLE _workThread;
	SIODelegate* _delegate;
	bool _running;
	
	std::string _host;
	int _port;
};

#endif