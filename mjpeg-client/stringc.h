#ifndef __ROV_STRINGC__H
#define __ROV_STRINGC__H
#include <string>
#include <stdlib.h>
#include <algorithm>    // std::reverse
#include <time.h>

typedef std::string stringc;

inline void trim(stringc& s) {

	int i = 0;

	while (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n') {
		i++;
	}

	s = s.substr(i);
	i = s.size() - 1;

	while (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n'){
		i--;
	}

	s = s.substr(0, i + 1);
}

inline bool isDigit(const stringc& s) {
	const char *p =  s.c_str();
	int len = s.size();

	if (len == 1 && *p == '0')
		return true;

	if (*p == '-')
		p++;

	if (*p > '9' || *p < '1')
		return false;
	p++;

	while (*p) {
		if (*p > '9' || *p < '0')
			return false;
		p++;
	}
	return true;
}

inline int toDigit(const stringc& s) {
	int i = 0;
	const char* p = s.c_str();

	if (*p == '-')p++;
	while (*p) {
		i *= 10;
		i += *p - '0';
		p++;
	}
	if (s[0] == '-')
		i *= -1;
	return i;
}

inline void formatRandomString(stringc& s) {

	char buf[32]{0};	
	time_t timep;
	struct tm *p;	
		
	time(&timep);	
	p = localtime(&timep);	
	sprintf(buf, "%d%d%d%d%d%d", (1900 + p->tm_year), (1 + p->tm_mon),	
	p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);	
		
	s.clear();	
	s.append(buf);	
}



inline bool isHexDigit(const stringc& s){
	auto p = s.c_str();
	int len = s.size();

	if (len == 1 && *p == '0')
		return true;

	if (!((*p >= '1'&&*p <= '9') || (*p >= 'a'&&*p <= 'f') || (*p >= 'A'&&*p <= 'F')))
		return false;
	p++;

	while (*p) {
		if (!((*p >= '0'&&*p <= '9') || (*p >= 'a'&&*p <= 'f') || (*p >= 'A'&&*p <= 'F')))
			return false;
		p++;
	}

	return true;
}

inline void append_f32(stringc& s,float f) {
	char tmp[32]{ 0 };
	sprintf(tmp, "%.2f", f);
	s.append(tmp);
}

inline bool _check_is_digital(const char* digit) {
	const char *p = digit;
	int len = strlen(p);

	if (len == 1 && *p == '0')
		return true;

	if (*p == '-')
		p++;

	if (*p > '9' || *p < '1')
		return false;
	p++;

	while (*p) {
		if (*p > '9' || *p < '0')
			return false;
		p++;
	}
	return true;
}

inline bool _check_is_ip(const stringc& s) {
	if (s.size() > 15)
		return false;
	char buf[16],*p = buf;
	int dot_num = 4,num;
	memset(buf, '.', 16);
	s.copy(buf, 15);

	while (dot_num)
	{
		char* h = p;
		while (*p != '.')p++;

		if (p - h > 3)
			return false;

		*p = 0;
		if (!_check_is_digital(h))
			return false;
		num = atoi(h);
		if (num < 0 || num > 255)
			return false;

		dot_num--;
		p++;
	}
	
	return true;
}

inline std::string stringc_itoa(int value, int base) {
	std::string buf;
	// check that the base if valid  
	if (base < 2 || base > 16) return buf;
	enum { kMaxDigits = 35 };
	buf.reserve(kMaxDigits); // Pre-allocate enough space.  
	int quotient = value;
	// Translating number to string with base:  
	do {
		buf += "0123456789abcdef"[abs(quotient % base)];
		quotient /= base;
	} while (quotient);

	// Append the negative sign  
	if (value < 0) buf += '-';
	std::reverse(buf.begin(), buf.end());

	return buf;
}


#endif