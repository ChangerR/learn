#include "SocketIO.h"
#include "libwebsockets.h"

class WsMessage
{
public:
    WsMessage() : what(0), obj(NULL){}
    unsigned int what; // message type
    void* obj;
};

class WsThreadHelper
{
public:
    WsThreadHelper();
    ~WsThreadHelper();
        
    // Creates a new thread
    bool createThread(const SocketIO& ws);
    // Quits sub-thread (websocket thread).
    void quitSubThread();
    
    // Schedule callback function
    virtual void update();
    
    // Sends message to UI thread. It's needed to be invoked in sub-thread.
    void sendMessageToUIThread(WsMessage *msg);
    
    // Sends message to sub-thread(websocket thread). It's needs to be invoked in UI thread.
    void sendMessageToSubThread(WsMessage *msg);
    
    // Waits the sub-thread (websocket thread) to exit,
    void joinSubThread();
    
    
protected:
    static DWORD WINAPI wsThreadEntryFunc(LPVOID);
    
private:
    std::list<WsMessage*>* _UIWsMessageQueue;
    std::list<WsMessage*>* _subThreadWsMessageQueue;
    CRITICAL_SECTION   _UIWsMessageQueueMutex;
    CRITICAL_SECTION   _subThreadWsMessageQueueMutex;
    HANDLE _subThreadInstance;
	SocketIO* _sio;
    bool _needQuit;
    friend class SocketIO;
};

// Wrapper for converting websocket callback from static function to member function of WebSocket class.
class SocketIOCallbackWrapper {
public:
    
    static int onSocketCallback(struct libwebsocket_context *ctx,
                                struct libwebsocket *wsi,
                                enum libwebsocket_callback_reasons reason,
                                void *user, void *in, size_t len)
    {
        // Gets the user data from context. We know that it's a 'WebSocket' instance.
        SocketIO* sioInstance = (SocketIO*)libwebsocket_context_user(ctx);
        if (sioInstance)
        {
            return sioInstance->onSocketCallback(ctx, wsi, reason, user, in, len);
        }
        return 0;
    }
};

SocketIO::SocketIO(const char* host,unsigned short port,
				SOCKETIO_VERSION v = SIO_V_0_X,int ssl) {
	
}
	
SocketIO::~SocketIO() {
	
}

bool SocketIO::open(SIODelegate* del) {
	
}

void SocketIO::close() {
	
}

DWORD SocketIO::http_handler(LPVOID data) {
	
}
	
bool SocketIO::handShake(const std::string& s) {
	
}