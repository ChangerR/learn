#ifndef __IMAGE_SL
#define __IMAGE_SL
#include "slconfig.h"
#include <stdio.h>

enum IMAGE_TYPE {
	IMAGE_UNKNOW = 0,
	IMAGE_JPEG,
	IMAGE_BMP,
	IMAGE_RGB,
	IMAGE_ARGB,
	IMAGE_YUV422
};

class Image {
public:
	Image() {
		buf = NULL;
		image_length = 0;
		type = IMAGE_UNKNOW;
	}
	
	virtual ~Image() {
		if(buf)
			delete buf;
	}
	
	void fillImage(u8* data,int len,IMAGE_TYPE t) {
		if(buf != 0)
			delete buf;
		
		buf = new u8[len];
		
		memcpy(buf,data,len);
		image_length = len;
		
		type = t;
	}
	
	void writeToFile(const char* path,const char* name) {
		
		stringc location = path;
		stringc filename;
		
		if(path == NULL)
			return;
		
		if(name == NULL) {
			formatRandomString(filename);
		} else {
			filename = name;
		}
		
		location.append("/");
		location.append(filename);
		
		switch(type) {
			case IMAGE_JPEG:
			{
				if(name == NULL) {
					location.append(".jpg");
					
					FILE* pic = fopen(tmp.c_str(),"wb");
					fwrite(buf,len,1,pic);
					fflush(pic);
					fclose(pic);
					
				}
				break;
			}
			default:
				break;
		}
	}
	u8* buf;
	int image_length;
	IMAGE_TYPE type;
};

#endif