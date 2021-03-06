/*
 * V4L2 Codec decoding example application
 * Kamil Debski <k.debski@samsung.com>
 *
 * MFC operations
 *
 * Copyright 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <linux/videodev2.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "mfc.h"

static char *dbg_type[2] = {"OUTPUT", "CAPTURE"};
static char *dbg_status[2] = {"ON", "OFF"};

int mfc_open(struct instance *i, char *name)
{
	struct v4l2_capability cap;
	int ret;

	i->mfc.fd = open(name, O_RDWR, 0);
	if (i->mfc.fd < 0) {
		err("Failed to open MFC: %s", name);
		return -1;
	}

	memzero(cap);
	ret = ioctl(i->mfc.fd, VIDIOC_QUERYCAP, &cap);
	if (ret != 0) {
		err("Failed to verify capabilities");
		return -1;
	}

	dbg("MFC Info (%s): driver=\"%s\" bus_info=\"%s\" card=\"%s\" fd=0x%x",
			name, cap.driver, cap.bus_info, cap.card, i->mfc.fd);

	if (	!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) ||
		!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE) ||
		!(cap.capabilities & V4L2_CAP_STREAMING)) {
		err("Insufficient capabilities of MFC device (is %s correct?)",
									name);
		return -1;
	}

        return 0;
}



void mfc_close(struct instance *i)
{
	close(i->mfc.fd);
}


int mfc_dec_setup_output(struct instance *i, unsigned long codec,
						unsigned int size, int count)
{
	struct v4l2_format fmt;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_plane planes[MFC_OUT_PLANES];
	int ret;
	int n;

	memzero(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	fmt.fmt.pix_mp.pixelformat = codec;
	fmt.fmt.pix_mp.plane_fmt[0].sizeimage = size;
	fmt.fmt.pix_mp.num_planes = MFC_OUT_PLANES;

	ret = ioctl(i->mfc.fd, VIDIOC_S_FMT, &fmt);
	if (ret != 0) {
		err("Failed to setup OUTPUT for MFC decoding");
		return -1;
	}

	dbg("Setup MFC decoding OUTPUT buffer size=%u (requested=%u)",
		fmt.fmt.pix_mp.plane_fmt[0].sizeimage, size);

	i->mfc.out_buf_size = fmt.fmt.pix_mp.plane_fmt[0].sizeimage;

	memzero(reqbuf);
	reqbuf.count = count;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	reqbuf.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(i->mfc.fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret != 0) {
		err("REQBUFS failed on OUTPUT queue of MFC");
		return -1;
	}
	i->mfc.out_buf_cnt = reqbuf.count;

	dbg("Number of MFC OUTPUT buffers is %d (requested %d)",
					 i->mfc.out_buf_cnt, count);

	for (n = 0; n < i->mfc.out_buf_cnt; n++) {
		memzero(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n;
		buf.m.planes = planes;
		buf.length = MFC_OUT_PLANES;

		ret = ioctl(i->mfc.fd, VIDIOC_QUERYBUF, &buf);
		if (ret != 0) {
			err("QUERYBUF failed on OUTPUT buffer of MFC");
			return -1;
		}

		i->mfc.out_buf_off[n] = buf.m.planes[0].m.mem_offset;

		i->mfc.out_buf_addr[n] = mmap(NULL, buf.m.planes[0].length,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					i->mfc.fd, buf.m.planes[0].m.mem_offset);

		if (i->mfc.out_buf_addr[n] == MAP_FAILED) {
			err("Failed to MMAP MFC OUTPUT buffer");
			return -1;
		}

		i->mfc.out_buf_flag[n] = 0;
	}

	dbg("Succesfully mmapped %d MFC OUTPUT buffers", n);

	return 0;
}

int mfc_dec_queue_buf(struct instance *i, int n, int l1, int l2, int type,
		int nplanes)
{
	struct v4l2_buffer qbuf;
	struct v4l2_plane planes[MFC_MAX_PLANES];
	int ret;

	memzero(qbuf);
	qbuf.type = type;
	qbuf.memory = V4L2_MEMORY_MMAP;
	qbuf.index = n;
	qbuf.m.planes = planes;
	qbuf.length = nplanes;
	qbuf.m.planes[0].bytesused = l1;
	qbuf.m.planes[1].bytesused = l2;

	ret = ioctl(i->mfc.fd, VIDIOC_QBUF, &qbuf);

	if (ret) {
		err("Failed to queue buffer (index=%d) on %s", n,
			dbg_type[type==V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE]);
		return -1;
	}

	dbg("Queued buffer on %s queue with index %d",
		dbg_type[type==V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE], n);

	return 0;
}

int mfc_dec_queue_buf_out(struct instance *i, int n, int length)
{
	if (n >= i->mfc.out_buf_cnt) {
		err("Tried to queue a non exisiting buffer");
		return -1;
	}

	return mfc_dec_queue_buf(i, n, length, 0,
			V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE, MFC_OUT_PLANES);
}

int mfc_dec_queue_buf_cap(struct instance *i, int n)
{
	if (n >= i->mfc.cap_buf_cnt) {
		err("Tried to queue a non exisiting buffer");
		return -1;
	}

	return mfc_dec_queue_buf(i, n, i->mfc.cap_buf_size[0],
		i->mfc.cap_buf_size[1], V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
		MFC_CAP_PLANES);
}

int mfc_dec_dequeue_buf(struct instance *i, struct v4l2_buffer *qbuf)
{
	int ret;

	ret = ioctl(i->mfc.fd, VIDIOC_DQBUF, qbuf);

	if (ret) {
		err("Failed to dequeue buffer");
		return -1;
	}

	dbg("Dequeued buffer with index %d on %s queue", qbuf->index,
		dbg_type[qbuf->type==V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE]);

	return 0;
}

int mfc_stream(struct instance *i, enum v4l2_buf_type type, int status)
{
	int ret;

	ret = ioctl(i->mfc.fd, status, &type);
	if (ret) {
		err("Failed to change streaming on MFC (type=%s, status=%s)",
			dbg_type[type==V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE],
			dbg_status[status==VIDIOC_STREAMOFF]);
		return -1;
	}

	dbg("Stream %s on %s queue", dbg_status[status==VIDIOC_STREAMOFF],
		dbg_type[type==V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE]);

	return 0;
}

int mfc_dec_setup_capture(struct instance *i, int extra_buf)
{
	struct v4l2_format fmt;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_plane planes[MFC_CAP_PLANES];
	struct v4l2_control ctrl;
	struct v4l2_crop crop;
	int ret;
	int n;

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	ret = ioctl(i->mfc.fd, VIDIOC_G_FMT, &fmt);
	if (ret) {
		err("Failed to read format (after parsing header)");
		return -1;
	}
	i->mfc.cap_w = fmt.fmt.pix_mp.width;
	i->mfc.cap_h = fmt.fmt.pix_mp.height;

	i->mfc.cap_buf_size[0] = fmt.fmt.pix_mp.plane_fmt[0].sizeimage;
	i->mfc.cap_buf_size[1] = fmt.fmt.pix_mp.plane_fmt[1].sizeimage;

	ctrl.id = V4L2_CID_MIN_BUFFERS_FOR_CAPTURE;
	ret = ioctl(i->mfc.fd, VIDIOC_G_CTRL, &ctrl);
	if (ret) {
		err("Failed to get the number of buffers required by MFC");
		return -1;
	}

	i->mfc.cap_buf_cnt = ctrl.value + extra_buf;
	i->mfc.cap_buf_cnt_min = ctrl.value;
	i->mfc.cap_buf_queued = 0;

	dbg("MFC buffer parameters: %dx%d plane[0]=%d plane[1]=%d",
		fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height,
		i->mfc.cap_buf_size[0], i->mfc.cap_buf_size[1]);

	memzero(crop);
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	ret = ioctl(i->mfc.fd, VIDIOC_G_CROP, &crop);
	if (ret) {
		err("Failed to get crop information");
		return -1;
	}

	i->mfc.cap_crop_w = crop.c.width;
	i->mfc.cap_crop_h = crop.c.height;
	i->mfc.cap_crop_left = crop.c.left;
	i->mfc.cap_crop_top = crop.c.top;

	dbg("Crop parameters w=%d h=%d l=%d t=%d", crop.c.width, crop.c.height,
		crop.c.left, crop.c.top);

	memzero(reqbuf);
	reqbuf.count = i->mfc.cap_buf_cnt;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	reqbuf.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(i->mfc.fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret != 0) {
		err("REQBUFS failed on CAPTURE queue of MFC");
		return -1;
	}

	dbg("Number of MFC CAPTURE buffers is %d (requested %d, extra %d)",
				reqbuf.count, i->mfc.cap_buf_cnt, extra_buf);

	i->mfc.cap_buf_cnt = reqbuf.count;

	for (n = 0; n < i->mfc.cap_buf_cnt; n++) {
		memzero(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n;
		buf.m.planes = planes;
		buf.length = MFC_CAP_PLANES;

		ret = ioctl(i->mfc.fd, VIDIOC_QUERYBUF, &buf);
		if (ret != 0) {
			err("QUERYBUF failed on CAPTURE buffer of MFC");
			return -1;
		}

		i->mfc.cap_buf_off[n][0] = buf.m.planes[0].m.mem_offset;
		i->mfc.cap_buf_off[n][1] = buf.m.planes[1].m.mem_offset;

		i->mfc.cap_buf_addr[n][0] = mmap(NULL, buf.m.planes[0].length,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					i->mfc.fd, buf.m.planes[0].m.mem_offset);
		i->mfc.cap_buf_addr[n][1] = mmap(NULL, buf.m.planes[1].length,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					i->mfc.fd, buf.m.planes[1].m.mem_offset);

		if (i->mfc.cap_buf_addr[n][0] == MAP_FAILED ||
			i->mfc.cap_buf_addr[n][1] == MAP_FAILED) {
			err("Failed to MMAP MFC CAPTURE buffer");
			return -1;
		}
	}

	dbg("Succesfully mmapped %d MFC CAPTURE buffers", n);

	return 0;
}

