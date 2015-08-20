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
int main(int args,char** argv) {
	int _fd = -1;
	int ret = -1;
	struct v4l2_capability cap;
	struct v4l2_jpegcompression arg;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;

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
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLAN;
	fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_JPEG;
	fmt.fmt.pix_mp.width = 1024;
	fmt.fmt.pix_mp.height = 520;
	fmt.fmt.pix_mp.field = V4L2_FIELD_ANY;
	fmt.fmt.pix_mp.num_planes = 1;
	fmt.fmt.pix_mp.plane_fmt[0].sizeimage = 66201;

	ret = ioctl(_fd,VIDIOC_S_FMT,&fmt);
	if(ret <= 0) {
		printf("OUTPUT VIDIOC_S_FMT failed\n");
		return 1;
	}
	
	memset(&req,0,sizeof(struct v4l2_requestbuffers));
	
	req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLAN;
	req.memory = V4L2_MEMORY_DMABUF;
	req.count = 1;

	ret = ioctl(_fd,VIDIOC_REQBUFS,&req);
	if(ret < 0) {
		printf("OUTPUT VIDIOC_REQBUFS failed\n");
		return ret;
	}

	memset(&fmt,0,sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLAN;
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

	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLAN;
	req.memory = V4L2_MEMORY_DMABUF;
	req.count = 1;

	ret = ioctl(_fd,VIDIOC_REQBUFS,&req);
	if(ret < 0) {
		printf("CAPTURE VIDIOC_REQBUFS failed\n");
		return 1;
	}
	close(_fd);

	return 0;

}
