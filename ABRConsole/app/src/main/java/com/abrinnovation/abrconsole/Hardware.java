package com.abrinnovation.abrconsole;

import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
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
        try {
            join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void send(String cmd) {
        byte crc = CRC8(cmd);
        byte[] sendBuf = new byte[cmd.length() + 1];
        sendBuf[0] = crc;
        System.arraycopy(cmd.getBytes(),0,sendBuf,1,cmd.length());
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

    public void run() {

        while(_running) {
            String line = null;
            try {
                if ((line = _reader.readLine()) != null) {
                    String[] statuses = line.split(";");
                    for(String str : statuses) {
                        String[] s = str.split(":");
                        synchronized (_status) {
                            _status.put(s[0], s[1]);
                        }
                    }
                }
            }catch(Exception e) {
                e.printStackTrace();
                _running = false;
            }

            for(Object bytes:_sendList) {
                byte[] _send = (byte[])bytes;
                try {
                    _outStream.write(_send);
                }catch (IOException e) {
                    Log.i("ABRConsole", "Input Stream Error");
                    _running = false;
                    break;
                }
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
                crc >>= 1;
                if(sum > 0) {
                    crc ^= 0x8c;
                }
                c >>= 1;
            }
        }

        return crc;
    }
}
