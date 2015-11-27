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
