#include "MpegtsParser.h"
#include <android/log.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
}

#include <vector>

#define ROVCPP_TAG "Rovcpp"

#define LOGOUT(str,args...) __android_log_print(ANDROID_LOG_INFO,ROVCPP_TAG,str,##args)

struct MpegtsLocalContext {
	bool isRunning;
	AVFormatContext* formatCtx;
	AVPacket* packet;
	int videoIndex;
};

static int g_ffmpeg_inited = 0;

static std::vector<MpegtsLocalContext*> g_localContexts;

static void setMpegtsStreamInfo(MpegtsLocalContext* _context,JNIEnv* env,jobject info) {
	jclass _cls = env->GetObjectClass(info);
	jfieldID _fid_width = env->GetFieldID(_cls,"video_width","I");
	jfieldID _fid_height = env->GetFieldID(_cls,"video_height","I");
	jfieldID _fid_bitrate = env->GetFieldID(_cls,"video_bitrate","L");
	if(_fid_width == NULL){LOGOUT("get info width occur error");return;}
	if(_fid_height == NULL){LOGOUT("get info height occur error");return;}
	if(_fid_bitrate == NULL){LOGOUT("get info bitrate occur error");return;}
	
	env->SetIntField(info,_fid_width,jint(_context->formatCtx->streams[_context->videoIndex]->codec->width));
	env->SetIntField(info,_fid_height,jint(_context->formatCtx->streams[_context->videoIndex]->codec->height));
	env->SetLongField(info,_fid_bitrate,jlong(_context->formatCtx->streams[_context->videoIndex]->codec->bit_rate));
}

static int setMpegtsPacket(AVPacket* packet,JNIEnv* env,jobject jpac) {
	jclass _cls = env->GetObjectClass(jpac);
	jfieldID _fid_data = env->GetFieldID(_cls,"data","[B");
	jfieldID _fid_pts = env->GetFieldID(_cls,"pts","L");
	jfieldID _fid_dts = env->GetFieldID(_cls,"dts","L");
	jfieldID _fid_size = env->GetFieldID(_cls,"size","I");
	jfieldID _fid_duration = env->GetFieldID(_cls,"duration","L");
	jfieldID _fid_alloc_size = env->GetFieldID(_cls,"alloc_size","I");
	jbyteArray arr;
	jbyte* _buf = NULL;
	jint alloc_size;
	
	if(_fid_data == NULL){LOGOUT("get packet data occur error");return -2;}
	if(_fid_pts == NULL){LOGOUT("get packet pts occur error");return -2;}
	if(_fid_dts == NULL){LOGOUT("get packet dts occur error");return -2;}
	if(_fid_size == NULL){LOGOUT("get packet size occur error");return -2;}
	if(_fid_duration == NULL){LOGOUT("get packet duration occur error");return -2;}
	if(_fid_alloc_size == NULL){LOGOUT("get packet alloc size occur error");return -2;}
	
	alloc_size = env->GetIntField(jpac,_fid_alloc_size);
	
	if(alloc_size < packet->size) {
		LOGOUT("java packet size little than avpacket size...");
		return -1;
	}
	
	arr = (jbyteArray)env->GetObjectField(jpac,_fid_data);
	
	_buf = env->GetByteArrayElements(arr,0);
	memcpy(_buf,packet->data,packet->size);
	env->ReleaseByteArrayElements(arr,_buf,0);
	
	env->SetIntField(jpac,_fid_size,jint(packet->size));
	env->SetLongField(jpac,_fid_pts,jlong(packet->pts));
	env->SetLongField(jpac,_fid_dts,jlong(packet->dts));
	env->SetLongField(jpac,_fid_duration,jlong(packet->duration));
	
	return 0;
}

JNIEXPORT jboolean JNICALL Java_com_rovhome_changer_util_MpegtsParser_open
  (JNIEnv * env,jobject thiz, jstring str, jobject info) {
	jboolean _ret = JNI_FALSE;
	const char* _str = env->GetStringUTFChars(str,NULL);
	int i = 0;
	AVInputFormat* l_ifmt;
	AVDictionary* l_opts;
	jfieldID _fid_ctx;
	jclass _mpegts_class;
	jint _locaCtxNum;
	MpegtsLocalContext* _context = NULL;
	
	if(g_ffmpeg_inited <= 0) {
		g_ffmpeg_inited++;
		av_register_all();
		avformat_network_init();
	}
	
	do {
		
		_mpegts_class = env->GetObjectClass(thiz);
		_fid_ctx = env->GetFieldID(_mpegts_class,"m_pLocalCtx","I");
		
		if(_fid_ctx == NULL) {
			LOGOUT("when get local ctx number occur error");
			break;
		}
		
		_locaCtxNum = env->GetIntField(thiz,_fid_ctx);
		
		if(_locaCtxNum < 0) {
			_context = new MpegtsLocalContext;
			
			_context->isRunning = false;
			_context->formatCtx = NULL;
			_context->packet = NULL;
			_context->videoIndex = -1;
	
		} else 
			_context = g_localContexts[_locaCtxNum];
		
		if(_context->isRunning == true)
			break;
			
		l_ifmt = av_find_input_format("mpegts");
		_context->formatCtx = avformat_alloc_context();
		
		LOGOUT("ffmpeg try to open %s",_str);
		
		av_dict_set(&l_opts,"timeout","1000",0);
		
		if(avformat_open_input(&(_context->formatCtx),_str,l_ifmt,&l_opts) < 0) {
			LOGOUT("couldn't open input stream:%s",_str);
			break;
		}
		
		if(avformat_find_stream_info(_context->formatCtx,NULL) < 0) {
			LOGOUT("could not detect stream information");
			break;
		}
		
		for(i = 0; i < _context->formatCtx->nb_streams; i++) {
			if(_context->formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
				_context->videoIndex = i;
				break;
			}
		}
		
		if(_context->videoIndex == -1) {
			LOGOUT("could not find a video stream");
			break;
		}
		
		setMpegtsStreamInfo(_context,env,info);
		
		_context->packet = (AVPacket*)av_malloc(sizeof(AVPacket));
		
		_context->isRunning = true;
		_ret = JNI_TRUE;
	}while(0);
	
	if(_locaCtxNum < 0 && _ret == JNI_TRUE) {
		
		i = -1;
		
		for(int n = 0;n < g_localContexts.size();n++) {
			if(g_localContexts[n] == NULL)
				i = n;
		}
		
		if(i == -1) {
			_locaCtxNum = g_localContexts.size();			
			g_localContexts.push_back(_context);
		} else {
			_locaCtxNum = i;
			g_localContexts[_locaCtxNum] = _context;
		}
		
		env->SetIntField(thiz,_fid_ctx,_locaCtxNum);
		LOGOUT("MpegtsParse open set context number:%d",_locaCtxNum);
	}
	
	env->ReleaseStringUTFChars(str,_str);
	
	return _ret;
}

JNIEXPORT jint JNICALL Java_com_rovhome_changer_util_MpegtsParser_getStreamElement
  (JNIEnv * env,jobject thiz, jobject pac) {
	jint ret = -1;
	MpegtsLocalContext* _context = NULL;
	jfieldID _fid_ctx;
	jclass _mpegts_class;
	jint _locaCtxNum;
	
	_mpegts_class = env->GetObjectClass(thiz);
	_fid_ctx = env->GetFieldID(_mpegts_class,"m_pLocalCtx","I");
	
	if(_fid_ctx == NULL) {
		LOGOUT("when get local ctx number occur error");
		return -1;	
	}
	
	_locaCtxNum = env->GetIntField(thiz,_fid_ctx);
	
	if(_locaCtxNum < 0)
		return -3;
	
	_context = g_localContexts[_locaCtxNum];
	
	if(_context->isRunning == false) {
		LOGOUT("use Mpegts without open");
		return -4;
	}
		
	if( (ret = av_read_frame(_context->formatCtx,_context->packet)) >= 0) {
		
		if(_context->packet->stream_index == _context->videoIndex ) {
			
			ret = setMpegtsPacket(_context->packet,env,pac);

			if(ret == 0)
				ret = 1;
		}
		
		av_free_packet(_context->packet);
	} 

	return ret;
}

JNIEXPORT void JNICALL Java_com_rovhome_changer_util_MpegtsParser_close
  (JNIEnv * env,jobject thiz) {
	
	MpegtsLocalContext* _context = NULL;
	jfieldID _fid_ctx;
	jclass _mpegts_class;
	jint _locaCtxNum;
	
	_mpegts_class = env->GetObjectClass(thiz);
	_fid_ctx = env->GetFieldID(_mpegts_class,"m_pLocalCtx","I");
	
	if(_fid_ctx == NULL) {
		LOGOUT("when get local ctx number occur error");
		return;
	}
	
	_locaCtxNum = env->GetIntField(thiz,_fid_ctx);
	
	if(_locaCtxNum < 0)
		return;
	
	_context = g_localContexts[_locaCtxNum];
	
	g_localContexts[_locaCtxNum] = NULL;
	
	if(_context && _context->isRunning == true) {
		
		av_free(_context->packet);
		avformat_close_input(&_context->formatCtx);
		
		//_context->isRunning = false;
		
		delete _context;
	}
	
	env->SetIntField(thiz,_fid_ctx,-1);
}
