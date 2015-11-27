#include "SocketIO.h"
#include "libwebsockets.h"

#define WS_WRITE_BUFFER_SIZE 2048

#define CC_SAFE_DELETE_ARRAY(_array) do { \
			if(_array != NULL) \
			{ \
				delete [] _array; \
				_array = NULL; \
			} \
		} while(0)

#define CC_SAFE_DELETE(_data) do { \
			if(_data != NULL) \
			{ \
				delete _data; \
				_data = NULL; \
			} \
		} while(0)

#ifdef _WIN32
#define CC_INIT_MUTEX(l) InitializeCriticalSection(&(l))
#define CC_DESTROY_MUTEX(l) DeleteCriticalSection(&(l))
#define CC_MUTEX_LOCK(l) EnterCriticalSection(&(l))
#define CC_MUTEX_UNLOCK(l) LeaveCriticalSection(&(l))
#elif defined(__linux__)
#define CC_INIT_MUTEX(l) pthread_mutex_init(&(l),NULL)
#define CC_DESTROY_MUTEX(l) pthread_mutex_destroy(&(l))
#define CC_MUTEX_LOCK(l) pthread_mutex_lock(&(l))
#define CC_MUTEX_UNLOCK(l) pthread_mutex_unlock(&(l))
#endif

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

#ifdef _WIN32
    static DWORD WINAPI wsThreadEntryFunc(LPVOID);
#elif defined(__linux__)
	static void* wsThreadEntryFunc(void*);
#endif

private:
    std::list<WsMessage*>* _UIWsMessageQueue;
    std::list<WsMessage*>* _subThreadWsMessageQueue;
#ifdef _WIN32
    CRITICAL_SECTION   _UIWsMessageQueueMutex;
    CRITICAL_SECTION   _subThreadWsMessageQueueMutex;
	HANDLE _subThreadInstance;
#elif defined(__linux__)
	pthread_mutex_t _UIWsMessageQueueMutex;
	pthread_mutex_t _subThreadInstance;
	pthread_t _subThreadInstance;
#endif
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

// Implementation of WsThreadHelper
WsThreadHelper::WsThreadHelper()
:_ws(NULL)
, _needQuit(false)
{
    _UIWsMessageQueue = new std::list<WsMessage*>();
    _subThreadWsMessageQueue = new std::list<WsMessage*>();
#ifdef _WIN32
	_subThreadInstance = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	_s
#endif
	CC_INIT_MUTEX(_UIWsMessageQueue);
	CC_INIT_MUTEX(_subThreadWsMessageQueueMutex);
}

WsThreadHelper::~WsThreadHelper()
{
    joinSubThread();
#ifdef _WIN32
	if(_subThreadInstance != INVALID_HANDLE_VALUE) {
		CloseHandle(_subThreadInstance);
	}
#endif
    delete _UIWsMessageQueue;
    delete _subThreadWsMessageQueue;

	CC_DESTROY_MUTEX(_UIWsMessageQueueMutex);
	CC_DESTROY_MUTEX(_subThreadWsMessageQueueMutex);
}

bool WsThreadHelper::createThread(const SocketIO& ws)
{
	bool ret = false;
    _sio = const_cast<SocketIO*>(&ws);

    // Creates websocket thread
#ifdef _WIN32
    _subThreadInstance = CreateThread(NULL,0,WsThreadHelper::wsThreadEntryFunc,(void*)this,0,NULL);
	ret = _subThreadInstance == INVALID_HANDLE_VALUE ? false : true;
#elif defined(__linux__)
	if(pthread_create(&_subThreadInstance,NULL,WsThreadHelper::wsThreadEntryFunc,(void*)this))
		ret = false;
	else
		ret = true;
#endif
    return ret;
}

void WsThreadHelper::quitSubThread()
{
    _needQuit = true;
}

#ifdef _WIN32
DWORD WsThreadHelper::wsThreadEntryFunc(LPVOID data)
#elif defined(__linux__)
void* WsThreadHelper::wsThreadEntryFunc(void* data)
#endif
{
    WsThreadHelper* _wsHelper = (WsThreadHelper*)data;

	_wsHelper->_sio->onSubThreadStarted();

    while (!_wsHelper->_needQuit)
    {
        if (_wsHelper->_sio->onSubThreadLoop())
        {
            break;
        }
    }

	_wsHelper->_sio->onSubThreadEnded();

	return 0;
}

void WsThreadHelper::sendMessageToUIThread(WsMessage *msg)
{
	CC_MUTEX_LOCK(_UIWsMessageQueueMutex);
    _UIWsMessageQueue->push_back(msg);
	CC_MUTEX_UNLOCK(_UIWsMessageQueueMutex);
}

void WsThreadHelper::sendMessageToSubThread(WsMessage *msg)
{
	CC_MUTEX_LOCK(_subThreadWsMessageQueueMutex);
    _subThreadWsMessageQueue->push_back(msg);
	CC_MUTEX_UNLOCK(_subThreadWsMessageQueueMutex);
}

void WsThreadHelper::joinSubThread()
{
#ifdef _WIN32
    if (_subThreadInstance != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(_subThreadInstance, INFINITE);
    }
#elif defined(__linux__)
	pthread_join(_subThreadInstance,NULL);
#endif
}

void WsThreadHelper::update()
{
    WsMessage *msg = NULL;

    // Returns quickly if no message
	CC_MUTEX_LOCK(_UIWsMessageQueueMutex);
    if (0 == _UIWsMessageQueue->size())
    {
		CC_MUTEX_UNLOCK(_UIWsMessageQueueMutex);
        return;
    }

    // Gets message
    msg = *(_UIWsMessageQueue->begin());
    _UIWsMessageQueue->pop_front();

	CC_MUTEX_LOCK(_UIWsMessageQueueMutex);

    if (_sio)
    {
        _sio->onUIThreadReceiveMessage(msg);
    }

    delete msg;
}

enum WS_MSG {
    WS_MSG_TO_SUBTRHEAD_SENDING_STRING = 0,
    WS_MSG_TO_SUBTRHEAD_SENDING_BINARY,
    WS_MSG_TO_UITHREAD_OPEN,
    WS_MSG_TO_UITHREAD_MESSAGE,
    WS_MSG_TO_UITHREAD_ERROR,
    WS_MSG_TO_UITHREAD_CLOSE
};


SocketIO::SocketIO(const char* host,unsigned short port,
				SOCKETIO_VERSION v,int ssl) {

	_wsInstance = NULL;
    _wsContext = NULL;
	_SSLConnection = ssl;
    _wsProtocols = NULL;
	_delegate = NULL;
	_wsHelper = NULL;
	_version = v;
	_readyState = CLOSED;
	_host = host;
	_path = "";
	_port = port;

}

SocketIO::~SocketIO() {
	close();
}

bool SocketIO::open(SIODelegate* del) {

	if(_readyState != CLOSED || del == NULL)
		return false;

	_delegate = del;
	_wsProtocols = new libwebsocket_protocols[2];
	memset(_wsProtocols, 0, sizeof(libwebsocket_protocols)*2);
	char* name = new char[20];
	strcpy(name, "default-protocol");
	_wsProtocols[0].name = name;
	_wsProtocols[0].callback = SocketIOCallbackWrapper::onSocketCallback;

	_wsHelper = new WsThreadHelper();

	return _wsHelper->createThread(*this);
}

void SocketIO::close() {

	if(_readyState == CLOSED || _readyState == CLOSING)
		return;

	_readyState = CLOSED;
	delete _wsHelper;
	_wsHelper = NULL;
	_delegate->onClose(this);
}

bool SocketIO::handShake(const std::string& s) {

}

int SocketIO::onSocketCallback(struct libwebsocket_context *ctx,
                         struct libwebsocket *wsi,
                         int reason,
                         void *user, void *in, ssize_t len) {

	//printf("socket callback for %d reason", reason);
    assert(_wsContext == NULL || ctx == _wsContext, "Invalid context.");
    assert(_wsInstance == NULL || wsi == NULL || wsi == _wsInstance, "Invaild websocket instance.");

	switch (reason)
    {
        case LWS_CALLBACK_DEL_POLL_FD:
        case LWS_CALLBACK_PROTOCOL_DESTROY:
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            {
                WsMessage* msg = NULL;
                if (reason == LWS_CALLBACK_CLIENT_CONNECTION_ERROR
                    || (reason == LWS_CALLBACK_PROTOCOL_DESTROY && _readyState == State::CONNECTING)
                    || (reason == LWS_CALLBACK_DEL_POLL_FD && _readyState == State::CONNECTING)
                    )
                {
                    msg = new WsMessage();
                    msg->what = WS_MSG_TO_UITHREAD_ERROR;
                    _readyState = State::CLOSING;
                }
                else if (reason == LWS_CALLBACK_PROTOCOL_DESTROY && _readyState == State::CLOSING)
                {
                    msg = new WsMessage();
                    msg->what = WS_MSG_TO_UITHREAD_CLOSE;
                }

                if (msg)
                {
                    _wsHelper->sendMessageToUIThread(msg);
                }
            }
            break;
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            {
                WsMessage* msg = new  WsMessage();
                msg->what = WS_MSG_TO_UITHREAD_OPEN;
                _readyState = State::OPEN;

                /*
                 * start the ball rolling,
                 * LWS_CALLBACK_CLIENT_WRITEABLE will come next service
                 */
                libwebsocket_callback_on_writable(ctx, wsi);
                _wsHelper->sendMessageToUIThread(msg);
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            {

			   CC_MUTEX_LOCK(_wsHelper->_subThreadWsMessageQueueMutex);
                std::list<WsMessage*>::iterator iter = _wsHelper->_subThreadWsMessageQueue->begin();

                int bytesWrite = 0;
                for (; iter != _wsHelper->_subThreadWsMessageQueue->end();)
                {
                    WsMessage* subThreadMsg = *iter;

                    if ( WS_MSG_TO_SUBTRHEAD_SENDING_STRING == subThreadMsg->what
                      || WS_MSG_TO_SUBTRHEAD_SENDING_BINARY == subThreadMsg->what)
                    {
                        Data* data = (Data*)subThreadMsg->obj;

                        const size_t c_bufferSize = WS_WRITE_BUFFER_SIZE;

                        size_t remaining = data->len - data->issued;
                        size_t n = std::min(remaining, c_bufferSize );
                        printf("[websocket:send] total: %d, sent: %d, remaining: %d, buffer size: %d", static_cast<int>(data->len), static_cast<int>(data->issued), static_cast<int>(remaining), static_cast<int>(n));

                        unsigned char* buf = new unsigned char[LWS_SEND_BUFFER_PRE_PADDING + n + LWS_SEND_BUFFER_POST_PADDING];

                        memcpy((char*)&buf[LWS_SEND_BUFFER_PRE_PADDING], data->bytes + data->issued, n);

                        int writeProtocol;

                        if (data->issued == 0) {
							if (WS_MSG_TO_SUBTRHEAD_SENDING_STRING == subThreadMsg->what)
							{
								writeProtocol = LWS_WRITE_TEXT;
							}
							else
							{
								writeProtocol = LWS_WRITE_BINARY;
							}

							// If we have more than 1 fragment
							if (data->len > c_bufferSize)
								writeProtocol |= LWS_WRITE_NO_FIN;
                        } else {
                        	// we are in the middle of fragments
                        	writeProtocol = LWS_WRITE_CONTINUATION;
                        	// and if not in the last fragment
                        	if (remaining != n)
                        		writeProtocol |= LWS_WRITE_NO_FIN;
                        }

                        bytesWrite = libwebsocket_write(wsi,  &buf[LWS_SEND_BUFFER_PRE_PADDING], n, (libwebsocket_write_protocol)writeProtocol);
                        printf("[websocket:send] bytesWrite => %d", bytesWrite);

                        // Buffer overrun?
                        if (bytesWrite < 0)
                        {
                            break;
                        }
                        // Do we have another fragments to send?
                        else if (remaining != n)
                        {
                            data->issued += n;
                            break;
                        }
                        // Safely done!
                        else
                        {
                            CC_SAFE_DELETE_ARRAY(data->bytes);
                            CC_SAFE_DELETE(data);
                            CC_SAFE_DELETE_ARRAY(buf);
                            _wsHelper->_subThreadWsMessageQueue->erase(iter++);
                            CC_SAFE_DELETE(subThreadMsg);
                        }
                    }
                }

				CC_MUTEX_UNLOCK(_wsHelper->_subThreadWsMessageQueueMutex);
                /* get notified as soon as we can write again */

                libwebsocket_callback_on_writable(ctx, wsi);
            }
            break;

        case LWS_CALLBACK_CLOSED:
            {

                printf("%s", "connection closing..");

                _wsHelper->quitSubThread();

                if (_readyState != State::CLOSED)
                {
                    WsMessage* msg = new  WsMessage();
                    _readyState = State::CLOSED;
                    msg->what = WS_MSG_TO_UITHREAD_CLOSE;
                    _wsHelper->sendMessageToUIThread(msg);
                }
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            {
                if (in && len > 0)
                {
                    // Accumulate the data (increasing the buffer as we go)
                    if (_currentDataLen == 0)
                    {
                        _currentData = new char[len];
                        memcpy (_currentData, in, len);
                        _currentDataLen = len;
                    }
                    else
                    {
                        char *new_data = new char [_currentDataLen + len];
                        memcpy (new_data, _currentData, _currentDataLen);
                        memcpy (new_data + _currentDataLen, in, len);
                        CC_SAFE_DELETE_ARRAY(_currentData);
                        _currentData = new_data;
                        _currentDataLen = _currentDataLen + len;
                    }

                    _pendingFrameDataLen = libwebsockets_remaining_packet_payload (wsi);

                    if (_pendingFrameDataLen > 0)
                    {
                        //printf("%ld bytes of pending data to receive, consider increasing the libwebsocket rx_buffer_size value.", _pendingFrameDataLen);
                    }

                    // If no more data pending, send it to the client thread
                    if (_pendingFrameDataLen == 0)
                    {
						WsMessage* msg = new  WsMessage();
						msg->what = WS_MSG_TO_UITHREAD_MESSAGE;

						char* bytes = NULL;
						Data* data = new  Data();

						if (lws_frame_is_binary(wsi))
						{

							bytes = new char[_currentDataLen];
							data->isBinary = true;
						}
						else
						{
							bytes = new char[_currentDataLen+1];
							bytes[_currentDataLen] = '\0';
							data->isBinary = false;
						}

						memcpy(bytes, _currentData, _currentDataLen);

						data->bytes = bytes;
						data->len = _currentDataLen;
						msg->obj = (void*)data;

						CC_SAFE_DELETE_ARRAY(_currentData);
						_currentData = NULL;
						_currentDataLen = 0;

						_wsHelper->sendMessageToUIThread(msg);
                    }
                }
            }
            break;
        default:
            break;

	}

	return 0;
}

void SocketIO::onSubThreadStarted() {
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = _wsProtocols;
#ifndef LWS_NO_EXTENSIONS
	info.extensions = libwebsocket_get_internal_extensions();
#endif
	info.gid = -1;
	info.uid = -1;
    info.user = (void*)this;

	_wsContext = libwebsocket_create_context(&info);

	if(NULL != _wsContext)
    {
        _readyState = State::CONNECTING;
        std::string name;
        for (int i = 0; _wsProtocols[i].callback != NULL; ++i)
        {
            name += (_wsProtocols[i].name);

            if (_wsProtocols[i+1].callback != NULL) name += ", ";
        }
        _wsInstance = libwebsocket_client_connect(_wsContext, _host.c_str(), _port, _SSLConnection,
                                             _path.c_str(), _host.c_str(), _host.c_str(),
                                             name.c_str(), -1);

        if(NULL == _wsInstance) {
            WsMessage* msg = new WsMessage();
            msg->what = WS_MSG_TO_UITHREAD_ERROR;
            _readyState = State::CLOSING;
            _wsHelper->sendMessageToUIThread(msg);
        }

	}
}

int SocketIO::onSubThreadLoop() {
	if (_readyState == State::CLOSED || _readyState == State::CLOSING)
    {
        libwebsocket_context_destroy(_wsContext);
        // return 1 to exit the loop.
        return 1;
    }

    if (_wsContext && _readyState != State::CLOSED && _readyState != State::CLOSING)
    {
        libwebsocket_service(_wsContext, 0);
    }

    // Sleep 50 ms
    Sleep(50);

    // return 0 to continue the loop.
    return 0;
}

void SocketIO::onSubThreadEnded() {
	printf("WEBSOCKET sub thread exit!\n");
}

void SocketIO::onUIThreadReceiveMessage(WsMessage* msg) {
	switch (msg->what) {
        case WS_MSG_TO_UITHREAD_OPEN:
            {
                _delegate->onOpen(this);
            }
            break;
        case WS_MSG_TO_UITHREAD_MESSAGE:
            {
                Data* data = (Data*)msg->obj;
                _delegate->onMessage(this, *data);
                CC_SAFE_DELETE_ARRAY(data->bytes);
                CC_SAFE_DELETE(data);
            }
            break;
        case WS_MSG_TO_UITHREAD_CLOSE:
            {
                //Waiting for the subThread safety exit
                _wsHelper->joinSubThread();
                _delegate->onClose(this);
            }
            break;
        case WS_MSG_TO_UITHREAD_ERROR:
            {
                // FIXME: The exact error needs to be checked.
                SocketIO::ErrorCode err = ErrorCode::CONNECTION_FAILURE;
                _delegate->onError(this, err);
            }
            break;
        default:
            break;
    }
}
