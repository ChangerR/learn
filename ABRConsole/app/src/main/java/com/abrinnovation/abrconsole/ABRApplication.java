package com.abrinnovation.abrconsole;

import android.app.Application;

/**
 * Created by changer on 15-12-30.
 */
public class ABRApplication extends Application {
    private static ABRApplication _ins ;

    @Override
    public void onCreate() {
        super.onCreate();
        _ins = this;
    }

    public static ABRApplication getIns() {
        return _ins;
    }

}
