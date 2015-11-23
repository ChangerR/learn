#ifndef __IMAGE_SL
#define __IMAGE_SL
/***************************************************************************
*
*  Project libmjpeg
*
* Copyright (C) 2014-2015, Changer, <dingjinchengyx@163.com>.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
***************************************************************************/

#include "slconfig.h"
#include <stdio.h>
#include "stringc.h"

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
		width = 0;
		height = 0;
	}
	
	virtual ~Image() {
		if(buf)
			delete[] buf;
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
					
					FILE* pic = fopen(location.c_str(),"wb");
					fwrite(buf,image_length,1,pic);
					fflush(pic);
					fclose(pic);
					
				}
				break;
			}
			default:
				break;
		}
	}
public:
	u8* buf;
	int image_length;	
	IMAGE_TYPE type;
	int width;
	int height;
	int depth;
};

#endif