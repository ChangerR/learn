#####一般电调在刚上电时需要使用航模遥控器设置油门行程，或者是设置电调参数时，也需要使用航模遥控器来控制选项 <BR>
#####当然也可以直接使用mega板直接设置电调的高低行程，但我手头现在有一个摇杆所以就试着使用JOYSTICK来模拟航模<BR>
#####遥控器，具体流程见如下图

```sequence
title:JOYSTICK通信Mega
JOYSTICK-->Win32程序:读出Joystick当前位置
note right of Win32程序:使用map映射Joy当前数值
Win32程序-->Mega:通过串口发送电调的PWM值
Mega-->Mega:处理串口数据
Mega-->电调:设置电调的PWM值
```
-------------------
使用Win32 API初始化JOYSTICK如下
``` cpp
UINT pos_x = 0, pos_y = 0,pos_b = 0;
JOYINFO joyinfo;
//JOYCAPS caps;
UINT wNumDevs, wDeviceID;
BOOL bDev1Attached, bDev2Attached;

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
```
设置JOYSTICK后读取其数值并将其map (PS: JOYSTICK读出的数据在（0-65535） 之间中间值为32767 <BR>电调PWM在1000-2000之间）
``` c
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
}
```
接下来就可以通过mega串口读取到当前PWM值然后输出到电调即可

``` cpp
/*
this code is for arduino
*/
#include <Servo.h>
char buf[32];
byte offset = 0;
int num;
Servo ser;
inline int parse_int_dec(const char* p) {
	bool isNeg = false;
	int ret = 0;
	if(*p == '-') {
		isNeg = true;
		p++;
	}
	
	while(*p) {
		if(*p < '0' || *p > '9') {
			ret = 0;
			break;
		}
		ret *= 10;
		ret += *p - '0';
		p++;
	}
	if (isNeg)
		ret *= -1;
	return ret;
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);
	ser.attach(9);
}

void loop() {
	// put your main code here, to run repeatedly:
	char c;
	if(Serial.available()) {
		c = Serial.read();
		if(c >= '0'&& c <= '9') {
			buf[offset++] = c;
		}else if(c == ';') {
			buf[offset++] = 0;
			offset = 0;
			num = parse_int_dec(buf);
			
			if(num >= 1000 & num <= 2000){
			   ser.writeMicroseconds(num);
			}
		}		
	}
}
```
[***github项目地址***](https://github.com/MysteriousChanger/learn/tree/master/joy)