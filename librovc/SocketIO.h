#ifndef ABR_SOCKETIO_H
#define ABR_SOCKETIO_H
#include <windows.h>
#include <string>

struct libwebsocket;
struct libwebsocket_context;
struct libwebsocket_protocols;

class WsThreadHelper;
class WsMessage;

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
	
protected:
	int onSocketCallback(struct libwebsocket_context *ctx,
                         struct libwebsocket *wsi,
                         int reason,
                         void *user, void *in, ssize_t len);
						 
	bool handShake(const std::string& s);
	
	friend class WebSocketCallbackWrapper;
	
private:
	struct libwebsocket*         _wsInstance;
    struct libwebsocket_context* _wsContext;
	int _SSLConnection;
    struct libwebsocket_protocols* _wsProtocols;
	SIODelegate* _delegate;
	bool _running;
	
	std::string _host;
	int _port;
};

#endif