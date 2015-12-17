package com.abrinnovation.abrconsole;

import java.io.BufferedReader;
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

    public void close() {
        _running = true;
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

    public void run() {

        while(_running) {
            String line = null;
            try {
                if ((line = _reader.readLine()) != null) {
                    String[] statuses = line.split(";");
                    for(String str : statuses) {
                        String[] s = str.split(":");
                        _status.put(s[0],s[1]);
                    }
                }
            }catch(Exception e) {
                e.printStackTrace();
            }

        }

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
