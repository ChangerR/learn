#ifndef ABR_SOCKETIO_H
#define ABR_SOCKETIO_H
#include <windows.h>
#include <string>

struct libwebsocket;
struct libwebsocket_context;
struct libwebsocket_protocols;

class WsThreadHelper;
class WsMessage;
class SocketIO;

class SIODelegate {
public:

	virtual ~SIODelegate() {}
	
	virtual void onConnect(SocketIO* client) = 0;
	
	virtual void onMessage(SocketIO* client, const std::string& data) = 0;
	
	virtual void onClose(SocketIO* client) = 0;
	
	virtual void onError(SocketIO* client, const std::string& data) = 0;
};
	
class SocketIO {
public:
	enum SOCKETIO_VERSION {
		SIO_V_0_X,SIO_V_1_X,
	};
	
    enum class State
    {
        CONNECTING,
        OPEN,
        CLOSING,
        CLOSED,
    };
	
	struct Data
    {
        Data():bytes(NULL), len(0), issued(0), isBinary(false){}
        char* bytes;
        ssize_t len, issued;
        bool isBinary;
    };
	
	enum class ErrorCode
    {
        TIME_OUT,
        CONNECTION_FAILURE,
        UNKNOWN,
    };
	
	SocketIO(const char* host,unsigned short port,
				SOCKETIO_VERSION v = SIO_V_0_X,int ssl = 0);
	
	virtual ~SocketIO();
	
	bool open(SIODelegate* del);
	
	void close();
	
protected:
	int onSocketCallback(struct libwebsocket_context *ctx,
                         struct libwebsocket *wsi,
                         int reason,
                         void *user, void *in, ssize_t len);
	
	virtual void onSubThreadStarted();
    virtual int onSubThreadLoop();
    virtual void onSubThreadEnded();
	virtual void onUIThreadReceiveMessage(WsMessage* msg);
	
	bool handShake(const std::string& s);
	
	friend class SocketIOCallbackWrapper;
	friend class WsThreadHelper;
private:
	struct libwebsocket*         _wsInstance;
    struct libwebsocket_context* _wsContext;
	int _SSLConnection;
    struct libwebsocket_protocols* _wsProtocols;
	SIODelegate* _delegate;
	WsThreadHelper* _wsHelper;
	SOCKETIO_VERSION _version;
	State _readyState;
	std::string _host;
	std::string _path;
	int _port;
};

#endif