package com.abrinnovation.abrconsole;

import android.app.ActionBar;
import android.content.Context;
import android.graphics.Point;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.ShapeDrawable;
import android.os.Bundle;
import android.text.Layout;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.PopupWindow;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.ToggleButton;

/**
 * Created by changer on 15-12-18.
 */
public class SettingWindow  extends PopupWindow{
    private ShowActivity _parent = null;

    public SettingWindow(ShowActivity parent) {

        _parent = parent;
        final View lView = LayoutInflater.from(_parent).inflate(R.layout.dialog_setting,null);
        setContentView(lView);
        Point size = new Point();
        ((WindowManager)_parent.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getSize(size);

        setWidth(size.x / 4 * 3);
        setHeight(size.y / 4 * 3);

        setFocusable(false);
        setOutsideTouchable(true);
        setTouchable(true);

        update();

        SeekBar powerBar = (SeekBar)lView.findViewById(R.id.powerBar);

        powerBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                TextView t = (TextView)lView.findViewById(R.id.powerText);
                t.setText("功率:"+ progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        SeekBar lightBar = (SeekBar)lView.findViewById(R.id.lightBar);

        lightBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                TextView t = (TextView) lView.findViewById(R.id.lightText);
                t.setText("灯光:" + progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        Button btnCameraUp = (Button)lView.findViewById(R.id.camera_up);
        btnCameraUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        Button btnCameraRecover = (Button)lView.findViewById(R.id.camera_recover);
        btnCameraRecover.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        Button btnCameraDown = (Button)lView.findViewById(R.id.camera_down);
        btnCameraDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        ToggleButton btnHoldDeep = (ToggleButton)lView.findViewById(R.id.toggleDeep);
        btnHoldDeep.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {

                }else{

                }
            }
        });

        ToggleButton btnHoldForward = (ToggleButton)lView.findViewById(R.id.toggleForward);
        btnHoldForward.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {

                }else{

                }
            }
        });
    }

    public void showPopupWindow(View parent) {
        this.showAtLocation(parent, Gravity.CENTER,0,0);
    }

    public void dismissWindow() {
        if(this.isShowing()) {
            this.dismiss();
        }
    }
}
