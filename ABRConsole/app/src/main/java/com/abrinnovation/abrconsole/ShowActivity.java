package com.abrinnovation.abrconsole;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.widget.CompoundButton;
import android.widget.ToggleButton;
import com.google.android.gms.common.api.GoogleApiClient;

public class ShowActivity extends AppCompatActivity implements IJoyStickCallback {

    private SettingWindow sw = null;
    private Hardware hardware = null;
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private GoogleApiClient client;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show);
        ToggleButton settingBtn = (ToggleButton) findViewById(R.id.Setting_btn);
        sw = new SettingWindow(ShowActivity.this);

        settingBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    Log.i("ABRConsole", "已开启");
                    sw.showPopupWindow(LayoutInflater.from(ShowActivity.this).inflate(R.layout.dialog_setting, null));
                } else {
                    Log.i("ABRConsole", "已关闭");
                    sw.dismissWindow();
                }
            }
        });

        JoystickView joy = (JoystickView) findViewById(R.id.JoyStick);
        joy.setOnJoystickCallback(ShowActivity.this);


    }


    public Hardware getControler() {
        return hardware;
    }

    @Override
    public void onJoyStickLCallback(float distance, int angle) {
        if (hardware == null) {
            return;
        }

        if (distance < 0.3f) {
            hardware.throttle(0);
            hardware.yaw(0);
            return;
        }

        if (angle >= 45 && angle < 135) {
            hardware.throttle(hardware.getPower());
        } else if (angle >= 135 && angle < 225) {
            hardware.yaw(-hardware.getPower());
        } else if (angle >= 225 && angle < 315) {
            hardware.throttle(-hardware.getPower());
        } else {
            hardware.yaw(-hardware.getPower());
        }
    }

    @Override
    public void onJoyStickRCallback(int value) {
        if (hardware == null) {
            return;
        }

        if (Math.abs(value) > 30) {
            hardware.lift(hardware.getPower() * (value / Math.abs(value)));
        } else {
            hardware.lift(0);
        }

    }

    @Override
    protected void onStart() {
        super.onStart();

        hardware = new Hardware("192.168.1.177", 8888);
        hardware.start();
    }


    @Override
    public void onStop() {
        super.onStop();
        hardware.close();
        hardware = null;
    }
}
