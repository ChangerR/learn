#ifndef _ABRCONSOLE_JNI_HIKNETHELPER_
#define _ABRCONSOLE_JNI_HIKNETHELPER_
#include <unistd.h>
#include <android/log.h>
#include "HCNetSDK.h"

#define LOGOUT(str,args...) __android_log_print(ANDROID_LOG_INFO,"ABRConsole",str,##args)

typedef void(CALLBACK *HikRealdataCallback) (int lRealHandle, unsigned int dwDataType, unsigned char *pBuffer,unsigned int dwBufSize,unsigned int dwUser);

class HikNetHelper {
public:
	HikNetHelper();
	virtual ~HikNetHelper();

	static void initLibrary();
	static void freeLibrary();

	bool login(const char*,int,const char*,const char*);

	bool beginRealplay(HikRealdataCallback callback,void* user);

	void endRealplay();

	void logout();

	void* getUser() {
		return _user;
	}
private:
	static volatile int _referenceLib;
	int _hikNetUserId;
	NET_DVR_DEVICEINFO _hikDeviceInfo;
	bool _isLogin;
	long _hikRealPlayHandle;
	void* _user;
};
#endif
