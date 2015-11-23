#ifndef _IMAGE_FACTORY_
#define _IMAGE_FACTORY_
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
 
#include "Image.h"

class ImageFactory {
public:
	static bool decodeImageToRGB(Image* img);
	
	static bool decodeJpegToRGB(Image* img);
};

#endif