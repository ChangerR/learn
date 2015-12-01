#include "websocket.h"
#include <stdio.h>
#include <windows.h>
#ifdef _WIN32
#pragma comment(lib,"websockets.lib")
#endif

class test_delegate : public WebSocket::Delegate {
public:
    virtual ~test_delegate(){}

    virtual void onOpen(WebSocket* ws) {
        printf("Websocket connected\n");
    }

    virtual void onMessage(WebSocket* ws,const WebSocket::Data& data) {
		printf("We have accept data!!!\n");
        if(!data.isBinary)
            printf("RECV:%s\n",data.bytes);
    }

    virtual void onClose(WebSocket* ws) {
        printf("WebSocket Closed\n");
    }

    virtual void onError(WebSocket* ws,const WebSocket::ErrorCode& error) {
        printf("WebSocket Occur Error\n");
    }
};

int main(int argc,char** argv) {
    test_delegate _del;
    WebSocket ws;

	if (ws.init(_del,"ws://changer.site:3000/")) {
		while (1) {
			ws.processMessage();
			Sleep(50);
		}
    }

}
