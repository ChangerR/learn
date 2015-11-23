#include "ImageFactory.h"

extern "C" {
	#include <jpeglib.h>
}

#pragma comment(lib,"jpeg")

bool ImageFactory::decodeJpegToRGB(Image* img) {
	
	if (!img||img->buf == NULL||img->type != IMAGE_JPEG)
		return false;
	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;
	unsigned char *point;
	u8* buf = img->buf;
	s32 len = img->image_length;
	
	cinfo.err = jpeg_std_error(&jerr);    //一下为libjpeg函数，具体参看相关文档
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, buf, len);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	img->width = cinfo.output_width;
	img->height = cinfo.output_height;
	img->depth = cinfo.output_components;
	img->image_length = img->width*img->height*img->depth;
	img->buf = new u8[img->image_length];

	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, img->width*img->depth, 1);
	point = img->buf;
	
	while (cinfo.output_scanline < img->height)
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);    //读取一行jpg图像数据到buffer
		memcpy(point, *buffer, img->width*img->depth);    //将buffer中的数据逐行给src_buff
		point += img->width*img->depth;            //一次改变一行
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	if(img->depth == 3)
		img->type = IMAGE_RGB;
	else if(img->depth == 4)
		img->type = IMAGE_ARGB;
	
	delete[] buf;
	return true;
}

bool ImageFactory::decodeImageToRGB(Image* img) {
	return false;
}