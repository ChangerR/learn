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
import android.widget.PopupWindow;

/**
 * Created by changer on 15-12-18.
 */
public class SettingWindow  extends PopupWindow{
    private Context _context = null;

    public SettingWindow(Context context) {
        View lView = LayoutInflater.from(context).inflate(R.layout.dialog_setting,null);
        setContentView(lView);
        Point size = new Point();
        ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getSize(size);

        setWidth(size.x / 2 + 20);
        setHeight(size.y / 2);

        setFocusable(false);
        setOutsideTouchable(true);
        setTouchable(true);

        update();

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
