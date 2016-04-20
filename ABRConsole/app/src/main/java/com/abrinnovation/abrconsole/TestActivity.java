package com.abrinnovation.abrconsole;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import com.abrinnovation.utils.HikNetHelper;

import org.MediaPlayer.PlayM4.Player;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by changer on 15-12-31.
 */
public class TestActivity extends AppCompatActivity implements HikNetHelper.DataCallback{

    public SurfaceHolder holder = null;
    public int port = -1;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_layout);
        SurfaceView sview = (SurfaceView)findViewById(R.id.test_surface);
        holder = sview.getHolder();
    }

    public void onTestHik(View view) {
        Log.i("ABRConsole", "Test HIK button");
        if(HikNetHelper.login("192.168.1.66",8000,"admin","ABR123456") == true) {
            Log.i("ABRConsole","login success");

            HikNetHelper.beginRealplay(this);
        }else{
            Log.i("ABRConsole", "login failed");
        }

    }

    public void onTestHik2(View v) {
        HikNetHelper.endRealplay();
        Player.getInstance().closeStream(port);
        Player.getInstance().freePort(port);
        port = -1;
        HikNetHelper.logout();
    }

    @Override
    public void onDataCallback(int dataType, byte[] buffer) {
        Log.i("ABRConsole","Recv data-> datatype=" + dataType + " buffersize=" + buffer.length);
        switch (dataType) {
            case HikNetHelper.NET_DVR_SYSHED:
            {
                port = Player.getInstance().getPort();

                if(buffer.length > 0) {

                    if(!Player.getInstance().setStreamOpenMode(port,Player.STREAM_REALTIME))
                        break;
                    if(!Player.getInstance().openStream(port, buffer, buffer.length, 1024 * 1024))
                        break;
                    if(!Player.getInstance().play(port, TestActivity.this.holder))
                        break;
                }
            }
            break;
            case HikNetHelper.NET_DVR_STREAMDATA:
            {
                if(buffer.length > 0 && port != -1) {
                    if(!Player.getInstance().inputData(port, buffer, buffer.length)) {
                        break;
                    }
                }
            }
            break;
            default:
            {
                if(buffer.length > 0 && port != -1) {
                    if(!Player.getInstance().inputData(port,buffer,buffer.length)) {
                        break;
                    }
                }
            }
            break;
        }
    }
}
