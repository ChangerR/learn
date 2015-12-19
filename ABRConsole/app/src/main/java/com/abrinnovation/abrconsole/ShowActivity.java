package com.abrinnovation.abrconsole;

import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.widget.CompoundButton;
import android.widget.ToggleButton;

public class ShowActivity extends AppCompatActivity {

    private SettingWindow sw = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show);
        ToggleButton settingBtn = (ToggleButton)findViewById(R.id.Setting_btn);
        sw = new SettingWindow(ShowActivity.this);

        settingBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    Log.i("ABRCOnsole", "已开启");
                    sw.showPopupWindow(LayoutInflater.from(ShowActivity.this).inflate(R.layout.dialog_setting,null));
                }else{
                    Log.i("ABRCOnsole", "已关闭");
                    sw.dismissWindow();
                }
            }
        });

    }


}
