#include <linux/videodev2.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#define MFC_DECODE "/dev/video6"
#define MFC_ENCODE "/dev/video7"
#define MAX_JPEG_PLAN_CNT 3

#define BMP_ID 0x4d42
#pragma pack( push, packing )
#pragma pack(1)
typedef struct  
{  
    unsigned short   bfType;   
    unsigned int    bfSize;  
    unsigned short   bfReserved1;  
    unsigned short   bfReserved2;  
    unsigned int    bfOffBits;  
} bmp_file_header;  

typedef struct  
{  
    unsigned int  biSize;   
    int   biWidth;   
    int   biHeight;   
    unsigned short   biPlanes;   
    unsigned short   biBitCount;  
    unsigned int  biCompression;   
    unsigned int  biSizeImage;   
    int   biXPelsPerMeter;   
    int   biYPelsPerMeter;   
    unsigned int   biClrUsed;   
    unsigned int   biClrImportant;   
} bmp_info_header;  
#pragma pack( pop, packing )

int write_bmp(const char* name,int width,int height,int bpp,char* buf) {
	FILE* f;
	bmp_file_header _file_header;
	bmp_info_header _info_header;
	int pitch;
	int i;
	
	memset(&_file_header,0,sizeof(bmp_file_header));
	memset(&_info_header,0,sizeof(bmp_info_header));
	pitch = (width * bpp + 3) & ~3;
	_file_header.bfType = BMP_ID;
	_file_header.bfOffBits = sizeof(bmp_file_header) + sizeof(bmp_info_header);
	_file_header.bfSize = _file_header.bfOffBits + pitch*height;

	_info_header.biSize = sizeof(bmp_info_header);
	_info_header.biWidth = width;
	_info_header.biHeight = height;
	_info_header.biPlanes = 1;
	_info_header.biBitCount = bpp * 8;
	_info_header.biCompression = 0;
	_info_header.biSizeImage = pitch * height;
	_info_header.biXPelsPerMeter = 0;
	_info_header.biYPelsPerMeter = 0;
	
	f = fopen(name,"wb");
	
	if(f == NULL)
		return -1;
	
	fwrite(&_file_header,sizeof(bmp_file_header),1,f);
	fwrite(&_info_header,sizeof(bmp_info_header),1,f);
	
	for(i = 0;i < height; i++) {
		fwrite(buf + i * width*bpp,width*bpp,1,f);
		if(pitch - width*bpp > 0)
			fwrite(&i,pitch - width*bpp,1,f);
	}
	
	fclose(f);
	return 0;	
}

int file_open(const char* name,char** pbuf) {
	struct stat _stat;
	int size;
	int fd = open(name,O_RDONLY);
	if(fd <= 0) {
		printf("open file %s failed\n",name);
		return -1;
	}
	fstat(fd,&_stat);
	size = _stat.st_size;
	*pbuf = (char*)malloc(size);
	read(fd,*pbuf,size);
	
	return size;
}

int main(int args,char** argv) {
	int _fd = -1;
	int ret = -1;
	struct v4l2_capability cap;
	struct v4l2_jpegcompression arg;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct v4l2_plane planes[MAX_JPEG_PLAN_CNT];
	int n;
	int out_buf_offset[MAX_JPEG_PLAN_CNT];
	char* out_buf_address[MAX_JPEG_PLAN_CNT];
	int capture_buf_offset[MAX_JPEG_PLAN_CNT];
	char* capture_buf_address[MAX_JPEG_PLAN_CNT];
	char* jpeg_buf;
	int jpeg_size,rgb_size;
	enum v4l2_buf_type _type;
	_fd = open(MFC_DECODE,O_RDWR,0);
	if(_fd < 0) {
		printf("Open decode error\n");
		return 1;
	}

	ret = ioctl(_fd,VIDIOC_QUERYCAP,&cap);
	if(ret != 0) {
		printf("failed verify capabilities\n");
		return 1;
	}

	printf("MFC Info (%s):driver=%s bus_info=%s card=%s\n",MFC_DECODE,
		cap.driver,cap.bus_info,cap.card);
	
	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) ||
	!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE) ||
	!(cap.capabilities & V4L2_CAP_STREAMING)) {
		printf("Insufficient capabilities of MFC device\n");
		return 1;
	}
	
	arg.quality = 0;
	ret = ioctl(_fd,VIDIOC_S_JPEGCOMP,&arg);
	if(ret < 0) {
		printf("VIDIOC_S_JPEGCOMP failed\n");
		return 1;
	}
	
	memset(&fmt,0 ,sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_JPEG;
	fmt.fmt.pix_mp.width = 1024;
	fmt.fmt.pix_mp.height = 520;
	fmt.fmt.pix_mp.field = V4L2_FIELD_ANY;
	fmt.fmt.pix_mp.num_planes = 1;
	fmt.fmt.pix_mp.plane_fmt[0].sizeimage = 1024 * 520 * 4;

	ret = ioctl(_fd,VIDIOC_S_FMT,&fmt);
	if(ret <= 0) {
		printf("OUTPUT VIDIOC_S_FMT failed\n");
		return 1;
	}
	
	memset(&req,0,sizeof(struct v4l2_requestbuffers));
	
	req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	req.memory = V4L2_MEMORY_MMAP;
	req.count = 1;

	ret = ioctl(_fd,VIDIOC_REQBUFS,&req);
	if(ret < 0) {
		printf("OUTPUT VIDIOC_REQBUFS failed\n");
		return ret;
	}
	
	for(n = 0; n < req.count;n++) {
		memset(&buf,0,sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n;
		buf.m.planes = planes;
		buf.length = 1;
		
		ret = ioctl(_fd,VIDIOC_QUERYBUF,&buf);
		if(ret != 0) {
			printf("OUTPUT query buffer error\n");
			return 1;
		}
		out_buf_offset[n] = buf.m.planes[0].m.mem_offset;
		out_buf_address[n] = mmap(NULL,buf.m.planes[0].length,
			PORT_READ|PORT_WRITE,MAP_SHARED,
			_fd,buf.m.planes[0].m.mem_offset);
			
		if(out_buf_address[n] == MAP_FAILED) {
			printf("Failed to MMAP MFC OUTPUT\n");
			return 1;
		}
	}
	memset(&fmt,0,sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_RGB24;
	fmt.fmt.pix_mp.width = 1024;
	fmt.fmt.pix_mp.height = 520;
	fmt.fmt.pix_mp.field = V4L2_FIELD_ANY;
	fmt.fmt.pix_mp.num_planes = 1;
	
	ret = ioctl(_fd,VIDIOC_S_FMT,&fmt);
	if(ret <= 0) {
		printf("CAPTURE VIDIOC_S_FMT failed\n");
		return ret;
	}

	memset(&req,0,sizeof(struct v4l2_requestbuffers));

	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	req.memory = V4L2_MEMORY_MMAP;
	req.count = 1;


	ret = ioctl(_fd,VIDIOC_REQBUFS,&req);
	if(ret < 0) {
		printf("CAPTURE VIDIOC_REQBUFS failed\n");
		return 1;
	}
	
	for(n = 0; n < req.count;n++) {
		memset(&buf,0,sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n;
		buf.m.planes = planes;
		buf.length = 1;
		
		ret = ioctl(_fd,VIDIOC_QUERYBUF,&buf);
		if(ret != 0) {
			printf("OUTPUT query buffer error\n");
			return 1;
		}
		capture_buf_offset[n] = buf.m.planes[0].m.mem_offset;
		capture_buf_address[n] = mmap(NULL,buf.m.planes[0].length,
			PORT_READ|PORT_WRITE,MAP_SHARED,
			_fd,buf.m.planes[0].m.mem_offset);
			
		if(capture_buf_address[n] == MAP_FAILED) {
			printf("Failed to MMAP MFC OUTPUT\n");
			return 1;
		}
	}
	
	jpeg_size = file_open("a.jpg",&jpeg_buf);
	
	if(jpeg_size <= 0) {
		printf("read jpeg failed\n");
		return 1;
	}
	memcpy(out_buf_address,jpeg_buf,jpeg_size);
	
	memset(&buf,0,sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;
	buf.m.planes = planes;
	buf.length = 1;
	buf.m.planes[0].bytesused = jpeg_size;
	
	ret = ioctl(_fd,VIDIOC_QBUF,&buf);
	if(ret) {
		printf("failed queue output buf\n");
		return 1;
	}
	
	memset(&buf,0,sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;
	buf.m.planes = planes;
	buf.length = 1;
	
	ret = ioctl(_fd,VIDIOC_QBUF,&buf);
	if(ret) {
		printf("failed queue capture buf\n");
		return 1;
	}
	
	_type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	ret = ioctl(_fd,VIDIOC_STREAMON,&_type);
	if(ret) {
		printf("failed to open stream OUTPUT\n");
		return 1;
	}
	
	_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	ret = ioctl(_fd,VIDIOC_STREAMON,&_type);
	if(ret) {
		printf("failed to open steram CAPTURE\n");
		return 1;
	}
	
	memset(&buf,0,sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.m.planes = planes;
	buf.length = 1;
	
	ret = ioctl(_fd,VIDIOC_DQBUF,&buf);
	if(ret) {
		printf("Output failed to dqbufer\n");
		return 1;
	}
	
	memset(&buf,0,sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.m.planes = planes;
	buf.length = 1;
	
	ret = ioctl(_fd,VIDIOC_DQBUF,&buf);
	if(ret) {
		printf("Output failed to dqbufer\n");
		return 1;
	}
	
	rgb_size = buf.m.planes[0].bytesused;
	write_bmp("a.bmp",1024,520,3,capture_buf_address[0]);
	close(_fd);

	return 0;

}
