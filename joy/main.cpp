#include <windows.h>
#include <stdio.h>
#include "serial.h"
#pragma comment(lib,"winmm.lib")

int map_servo(int pos) {
	
	if (pos < 10)
	{
		return 1000;
	}
	else if (pos > 65525) {
		return 2000;
	}
	else if (pos > 32762 && pos < 32770) {
		return 1500;
	}
	else {
		float scl = (float)(pos) / 65635.f;
		return int(scl * 1000) + 1000;
	}
	
}

#define _CRT_SECURE_NO_WARNINGS

int main() {

	UINT pos_x = 0, pos_y = 0,pos_b = 0;
	JOYINFO joyinfo;
	//JOYCAPS caps;
	UINT wNumDevs, wDeviceID;
	BOOL bDev1Attached, bDev2Attached;
	char buf[1024];
	int servo_y = 0;
	Serial hardware("COM3");

	if ((wNumDevs = joyGetNumDevs()) == 0) {
		printf("no joystick plug in\n");
		return 1;
	}
	/*
	memset(&caps, 0, sizeof(caps));
	for (int i = 0; i < wNumDevs; i++) {
		memset(&caps, 0, sizeof(caps));
		if (joyGetDevCaps(i, &caps, sizeof(caps) == JOYERR_NOERROR)) {
			wprintf(L"joystick %d :product %s reg:%s\n", i, caps.szPname,caps.szRegKey);
		}
	}
	*/
	bDev1Attached = joyGetPos(JOYSTICKID1, &joyinfo) == JOYERR_NOERROR;
	bDev2Attached = wNumDevs > 1 && joyGetPos(JOYSTICKID2, &joyinfo) ==
		JOYERR_NOERROR;

	if (bDev1Attached) {
		wDeviceID = JOYSTICKID1;
		printf("joystick 1 attached\n");
	}
	else if (bDev2Attached) {
		wDeviceID = JOYSTICKID2;
		printf("joystick 2 attached\n");
	}

	else {
		printf("no joystick attck\n");
		return 1;
	}

	if (hardware.begin(_B115200) == false) {
		printf("no hardware\n");
		return 1;
	}

	while (true) {
		if (joyGetPos(JOYSTICKID1, &joyinfo) == JOYERR_NOERROR) {
			
			if (pos_x != joyinfo.wXpos || pos_y != joyinfo.wYpos || pos_b != joyinfo.wButtons)
			{
				pos_x = joyinfo.wXpos;
				pos_y = joyinfo.wYpos;
				pos_b = joyinfo.wButtons;
				if (pos_b & 0x1) {
					printf("exit\n");
					break;
				}
				
				if (map_servo(pos_y) != servo_y) {
					servo_y = map_servo(pos_y);
					sprintf_s(buf, "%d;", map_servo(pos_y));
					hardware.write(buf, sizeof(buf));
				}
			}
		}
		
		Sleep(1);
	}
	hardware.close();

	return 0;
}