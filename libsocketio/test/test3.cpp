#include "SocketIO.h"
bool running = true;

#ifdef __linux__
#include <unistd.h>
#include <signal.h>
void signal_handler(int sig) {
    running = false;
}
#endif

class TestSIO_Delegate :
    public SocketIO::SIODelegate
{
public:
    virtual void onClose(SIOClient* client) {
        printf("socket io close\n");
    }

    virtual void onError(SIOClient* client, const std::string& data) {
        printf("socket io occur error\n");
    }
};

int main() {
    TestSIO_Delegate _del;

#ifdef __linux__
    if(signal(SIGINT, signal_handler) == SIG_ERR) {
        printf("could not register signal handler\n");
        return 1;
    }
#endif

    SIOClient* client = SocketIO::connect("changer.site:8080",_del);

    if(client != NULL) {
        while (running) {
            SocketIO::getInstance()->dispatchMessage();
        }
        client->disconnect();
    }

    return 0;
}
