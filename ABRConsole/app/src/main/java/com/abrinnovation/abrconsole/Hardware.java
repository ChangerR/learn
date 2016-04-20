package com.abrinnovation.abrconsole;

import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.lang.reflect.Array;
import java.net.Socket;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/**
 * Created by changer on 15-12-17.
 */
public class Hardware extends Thread {

    private String _ip = null;
    private int _port = 8888;
    private Socket _client = null;
    private List _sendList = null;
    private boolean _running = false;
    private BufferedReader _reader = null;
    private OutputStream _outStream = null;
    private Map _status = null;
    private int _power = 20;
    private int deadzone_pos = 50;
    private int deadzone_neg = 50;
    private int smoothingIncriment = 40;
    private int thrust_modifier_port =  1;
    private int thrust_modifier_vertical =  -1;
    private int thrust_modifier_starbord = 1;
    private int thrust_modifier_nport = 2;
    private int thrust_modifier_nvertical = -2;
    private int thrust_modifier_nstarbord =  2;
    private int water_type = 0;
    private int capabilities = 0;
    private boolean _isStart = false;
    private int _thro = 0,_yaw = 0,_lift = 0;
    private int _pitch = 0, _roll = 0;

    public Hardware(String ip,int port) {
        _ip = ip;
        _port = port;
        _sendList = new LinkedList();
        _status = new HashMap();
    }

    public boolean connect() {
        boolean ret = true;
        _running = true;
        try {
            _client = new Socket(_ip,_port);

            _reader = new BufferedReader(new InputStreamReader(_client.getInputStream()));
            _outStream = _client.getOutputStream();

        }catch(Exception e) {
            ret = false;
            _running = false;
            e.printStackTrace();
        }

        return ret;
    }

    public boolean getRunningState() {
        return _running;
    }

    public void close() {

        _running = false;

    }

    public void send(String cmd) {
        if(_running == false)
            return;
        byte crc = CRC8(cmd);
        byte[] sendBuf = new byte[cmd.length() + 1];
        sendBuf[0] = crc;
        System.arraycopy(cmd.getBytes(),0,sendBuf,1,cmd.length());
/*
        String logS = "(" + sendBuf.length + ")[";
        for (byte data:sendBuf) {
            logS += Integer.toHexString(data&0xff) + ",";
        }
        logS += "]";
        Log.i("ABRConsole",cmd);
        Log.i("ABRConsole",logS);
*/
        synchronized (_sendList) {
            _sendList.add(sendBuf);
        }
    }

    public String getStatus(String s) {
        String ret = null;
        synchronized (_status) {
            ret = (String)_status.get(s);
        }
        return ret;
    }

    public void holdHeadingToggle() {
        send("holdHeading_toggle();");
    }

    public void throttle(int t) {
        if(t != _thro) {
            send("thro(" + t + ");");
            _thro = t;
        }
    }

    public void yaw(int y) {
        if(y != _yaw) {
            send("yaw(" + y + ");");
            _yaw = y;
        }
    }

    public void lift(int l) {
        if(l != _lift) {
            send("lift(" + l + ");");
            _lift = l;
        }
    }

    public void pitch(int p) {
        if( p != _pitch) {
            send("pitch(" + p + ");");
            _pitch = p;
        }
    }

    public void roll(int r) {
        send("roll(" + r + ");");
    }

    public int getPower() {
        return _power;
    }

    public void setPower(int p) {
        _power = p;
    }

    public void requestCapabilities() {
        Log.i("ABRConsole", "Sending rcap to arduino");
        send("rcap();");
    }

    public void requestSettings() {
        send("reportSetting();");
        send("rmtrmod();");
        Log.i("ABRConsole", "send rmtrmod");
    }

    public void updateSetting() {

        send("updateSetting(" + smoothingIncriment + "," + deadzone_neg + "," + deadzone_pos
                + "," + water_type + ");");

        send("mtrmod1(" + thrust_modifier_port * 100 + "," + thrust_modifier_vertical * 100 + "," + thrust_modifier_starbord * 100 + ");");

        send("mtrmod2(" + thrust_modifier_nport * 100 + "," + thrust_modifier_nvertical * 100 + "," + thrust_modifier_nstarbord * 100 + ");");

    }

    public void startControl() {
        send("start();");
        _isStart = true;
        Log.i("ABRConsole","StartControl");
    }

    public void stopControl() {
        send("stop()");
        _isStart = false;
        Log.i("ABRConsole", "StopControl");
    }

    public void run() {

        if(connect() == false) {
            Log.i("ABRConsole","Connect to Hardware Error");
            return;
        }

        updateSetting();
        requestSettings();
        requestCapabilities();

        while(_running) {
            String line = null;
            try {
                if ((line = _reader.readLine()) != null) {
                    String[] statuses = line.split(";");
                    for(String str : statuses) {
                        //Log.d("ABRConsole",str);
                        String[] s = str.split(":");
                        synchronized (_status) {
                            if(s.length == 1) {
                                _status.put(s[0],"");
                            }else {
                                _status.put(s[0], s[1]);
                            }
                        }
                        if(s[0].equals("boot")) {
                            updateSetting();
                            requestSettings();
                            requestCapabilities();
                        } else if(s[0].equals("CAPA")) {
                            capabilities = Integer.parseInt(s[1]);
                            if(capabilities != 0&&_isStart == false) {
                                startControl();
                            }
                        }
                    }
                }
            }catch(Exception e) {
                e.printStackTrace();
                _running = false;
            }

            synchronized (_sendList) {
                for (Object bytes : _sendList) {
                    byte[] _send = (byte[]) bytes;
                    try {
                        _outStream.write(_send);
                        /*
                        byte[] tmp = new byte[_send.length - 1];
                        System.arraycopy(_send,1,tmp,0,_send.length - 1 );
                        Log.i("ABRConsole","send cmd=" + new String(tmp));
                        */
                    } catch (IOException e) {
                        Log.i("ABRConsole", "Input Stream Error");
                        _running = false;
                        break;
                    }
                }
                _sendList.clear();
            }
        }

        try {
            _reader.close();
            _outStream.close();
            _client.close();
        }catch (Exception e) {
            e.printStackTrace();
        }
        _status.clear();
        _sendList.clear();
        _client = null;
    }

    public static byte CRC8(String t) {
        byte crc = 0;
        byte data[] =t.getBytes();
        for(byte c : data) {
            for(int index = 0; index < 8;index++) {
                byte sum = (byte)((crc ^ c) & 0x01);
                crc >>>= 1;
                crc &= 0x7f;
                if(sum > 0) {
                    crc ^= 0x8c;
                }
                c >>>= 1;
                c &= 0x7f;
            }
        }

        return crc;
    }
}
