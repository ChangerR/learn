package com.abrinnovation.utils;

public class HikNetHelper {
	public static final int NET_DVR_SYSHED          = 1;	//系统头数据
	public static final int NET_DVR_STREAMDATA      = 2;	//视频流数据（包括复合流和音视频分开的视频流数据）
	public static final int NET_DVR_AUDIOSTREAMDATA = 3;	//音频流数据
	public static final int NET_DVR_STD_VIDEODATA   = 4;	//标准视频流数据
	public static final int NET_DVR_STD_AUDIODATA   = 5;	//标准音频流数据
	public static final int NET_DVR_SDP             = 6;   //SDP信息(Rstp传输时有效)
	public static final int NET_DVR_CHANGE_FORWARD  = 10;  //码流改变为正放
	public static final int NET_DVR_CHANGE_REVERSE  = 11;  //码流改变为倒放
	public static final int NET_DVR_PRIVATE_DATA    = 112; //私有数据,包括智能信息

	public interface DataCallback {
		void onDataCallback(HikNetHelper helper,int dataType,byte[] buffer);
	}

	public static native boolean login(String ip,int port,String username,String password);

	public static native boolean beginRealplay(DataCallback callback);

	public static native void endRealplay();

	public static native void logout();
}
