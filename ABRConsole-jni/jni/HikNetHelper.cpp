#include "HikNetHelper.h"
#include <string.h>

volatile int HikNetHelper::_referenceLib = 0;

HikNetHelper::HikNetHelper() {
    _hikNetUserId = -1;
    _hikRealPlayHandle = -1;
    _isLogin = false;
    _user =NULL;
    initLibrary();
}

HikNetHelper::~HikNetHelper() {
    freeLibrary();
}

void HikNetHelper::initLibrary() {
    if(_referenceLib <= 0) {
        NET_DVR_Init();
    }
    _referenceLib++;
}

void HikNetHelper::freeLibrary() {
    if(_referenceLib > 0) {
        NET_DVR_Cleanup();
        _referenceLib--;
    }
}

bool HikNetHelper::login(const char* _ip,int _port,const char* _username,const char* _password) {
    char ipaddr[16];
    char username[64];
    char password[64];

    if(_ip == NULL||_username == NULL||_password == NULL) {
        LOGOUT("Function HikNetHelper::login -> Error parameters");
        return false;
    }

    if(strlen(_ip) >=15 ||strlen(_username) >= 63||strlen(_password) >= 63) {
        LOGOUT("Function HikNetHelper::login -> Error parameters len(ip) > 16 len(username) > 64 len(password) > 64");
        return false;
    }

    strcpy(ipaddr,_ip);
    strcpy(username,_username);
    strcpy(password,_password);

    _hikNetUserId = NET_DVR_Login(ipaddr, _port, username, password, &_hikDeviceInfo);

    if(_hikNetUserId < 0) {
        LOGOUT("Function HikNetHelper::login -> Login failed. Error Code=%d",NET_DVR_GetLastError());
        return false;
    }
    _isLogin = true;

    return true;
}

bool HikNetHelper::beginRealplay(HikRealdataCallback callback,void* user) {

    NET_DVR_PREVIEWINFO struPlayInfo = {0};

    if(_isLogin == false) {
        LOGOUT("Function HikNetHelper::beginRealplay -> You should login first!)");
        return false;
    }

    _user = user;
    struPlayInfo.hPlayWnd = 0; //需要 SDK 解码时句柄设为有效值,仅取流不解码时可设为空
    struPlayInfo.lChannel = 1;
    struPlayInfo.dwStreamType = 1; //0-主码流,1-子码流,2-码流 3,3-码流 4,以此类推
    struPlayInfo.dwLinkMode = 0; //0- TCP 方式,1- UDP 方式,2- 多播方式,3- RTP 方式,4-RTP/RTSP,5-RSTP/HTTP
    struPlayInfo.bBlocked = 1; //0- 非阻塞取流,1- 阻塞取流
    _hikRealPlayHandle = NET_DVR_RealPlay_V40(_hikNetUserId, &struPlayInfo, NULL, NULL);
    if (_hikRealPlayHandle < 0)
    {
        LOGOUT("Function HikNetHelper::beginRealplay -> NET_DVR_RealPlay_V40 failed!! Error Code=%d)",NET_DVR_GetLastError());
        logout();
        return false;
    }

    int iRet;
    iRet = NET_DVR_SetRealDataCallBack(_hikRealPlayHandle, callback, (DWORD)this);
    if (!iRet)
    {
       LOGOUT("Function HikNetHelper::beginRealplay -> NET_DVR_RealPlay_V30 failed!! ERROR=%d)",NET_DVR_GetLastError());
       endRealplay();
       logout();
       return false;
    }

    return true;
}

void HikNetHelper::endRealplay() {
    if(_isLogin == true) {
        NET_DVR_StopRealPlay(_hikRealPlayHandle);
    }
}

void HikNetHelper::logout() {
    if(_isLogin == true) {
        NET_DVR_Logout(_hikNetUserId);
        _hikNetUserId = -1;
        _isLogin = false;
    }
}
