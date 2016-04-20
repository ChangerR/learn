#include "HikHelperJni.h"
#include "HikNetHelper.h"
HikNetHelper* _netHelper = NULL;

struct UserData {
    JavaVM* jvm;
    jobject callback;
};

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser) {

    HikNetHelper* helper = (HikNetHelper*)dwUser;
    UserData* data = (UserData*)helper->getUser();
    JNIEnv* env;

    data->jvm->AttachCurrentThread(&env,NULL);

    jclass call_class = env->GetObjectClass(data->callback);

    jmethodID method_Callback = env->GetMethodID(call_class,"onDataCallback","(I[B)V");
    jbyteArray buffer = env->NewByteArray(dwBufSize);

    jbyte* _buf = env->GetByteArrayElements(buffer,0);
    memcpy(_buf,pBuffer,dwBufSize);
    env->ReleaseByteArrayElements(buffer,_buf,0);

    env->CallVoidMethod(data->callback,method_Callback,(jint)dwDataType,buffer);
    env->DeleteLocalRef(buffer);
    data->jvm->DetachCurrentThread();    
}


jboolean JNICALL Java_com_abrinnovation_utils_HikNetHelper_login
  (JNIEnv *env, jclass cls, jstring jip, jint jport, jstring jusername, jstring jpassword) {
      const char* _ip = env->GetStringUTFChars(jip,NULL);
      const char* _username = env->GetStringUTFChars(jusername,NULL);
      const char* _password = env->GetStringUTFChars(jpassword,NULL);

      if(_netHelper == NULL) {
          _netHelper = new HikNetHelper;
      }

      return _netHelper->login(_ip,jport,_username,_password) ? JNI_TRUE : JNI_FALSE;
}

jboolean JNICALL Java_com_abrinnovation_utils_HikNetHelper_beginRealplay
  (JNIEnv *env, jclass cls, jobject callback) {
      if(_netHelper == NULL) {
          return JNI_FALSE;
      }
      UserData* data = new UserData;
      int ret = env->GetJavaVM(&(data->jvm));
      if(ret < 0) {
          LOGOUT("Get JAVAVM Failed");
          return JNI_FALSE;
      }
      data->callback = env->NewGlobalRef(callback);
      return _netHelper->beginRealplay(g_RealDataCallBack_V30,(void*)data) ? JNI_TRUE : JNI_FALSE;
}

void JNICALL Java_com_abrinnovation_utils_HikNetHelper_endRealplay
  (JNIEnv *env, jclass cls) {
      if(_netHelper) {
          UserData* data = (UserData*)_netHelper->getUser();
          _netHelper->endRealplay();
          env->DeleteGlobalRef(data->callback);
          delete data;
      }
}


JNIEXPORT void JNICALL Java_com_abrinnovation_utils_HikNetHelper_logout
(JNIEnv *env, jclass cls) {
    if(_netHelper) {
        _netHelper->logout();
        delete _netHelper;
        _netHelper = NULL;
    }
}
