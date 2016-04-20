package com.abrinnovation.abrconsole;

/**
 * Created by changer on 15-12-21.
 */
public interface IJoyStickCallback {
    public void onJoyStickLCallback(float distance,int angle);

    public void onJoyStickRCallback(int value);
}
