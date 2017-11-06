/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Technical University of Crete,
 *                    Georgara Athina,
 *                    Grammatopoulos Athanasios Vasileios
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
	Read webcam.h for informations about this library
 */

// C/C++ Libraries
	// For General Purpose
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <iostream>
	// V4L2
	#include <linux/videodev2.h>
	#include <unistd.h>
	#include <sys/mman.h>
	#include <sys/ioctl.h>
	// Error Handlers
	#include <fcntl.h>
	#include <errno.h>

// Library Head
	#include "webcam.h"

namespace tuc{
	// Constructor
	WebCam::WebCam(const char* device_name){
		// Default device name
		if(device_name == NULL){
			// Save device name
			this->device_name = strdup("/dev/video0");
		}
		// Custom device name
		else{
			// Save device name
			this->device_name = strdup(device_name);
		}

		// Initiate frame resolution
		this->frame_width = 0;
		this->frame_height = 0;
		// Camera not streaming
		this->streaming = false;
		// Frame Buffer
		this->frame = NULL;
	};

	// Open
	int WebCam::open(){
		// Variables
		struct v4l2_input input;

		// Open device
		this->fd = ::open(this->device_name, O_RDWR);
		// Check if error on device opened
		if(this->fd == -1){
			perror ("[tuc:WebCam:open] VIDEO_OPEN");
			return -1;
		}

		// Initiate v4l2_input
		memset(&input, 0, sizeof(input));
		// Check index
		if(-1 == ioctl(this->fd, VIDIOC_G_INPUT, &input.index)){
			perror ("[tuc:WebCam:open] VIDIOC_G_INPUT");
			return -1;
		}
		// Check input
		if(-1 == ioctl(this->fd, VIDIOC_ENUMINPUT, &input)){
			perror("[tuc:WebCam:open] VIDIOC_ENUM_INPUT");
			return -1;
		}

		// Return file descriptor
		return this->fd;
	};

	// Close
	int WebCam::close(){
		// Try to close Camera
		if(::close(this->fd) == -1){
			perror("[tuc:WebCam:close] VIDEO_CLOSE");
			return -1;
		};
		
		return 1;
	};

	// Read
	int WebCam::read(){
		// If camera not streaming
		if(!this->streaming){
			this->prepare2Read();
		}

		memset(&this->buffer, 0, sizeof (struct v4l2_buffer));
		this->buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		this->buffer.memory = V4L2_MEMORY_MMAP;
		if(ioctl (this->fd, VIDIOC_DQBUF, &this->buffer) == -1){
			perror("[tuc:WebCam:read] BUFFER_ERROR");
			return -1;
		}

		if (this->buffer.bytesused >= (1280*720<<1)){
			memcpy(this->frame, this->memory[this->buffer.index], (size_t) 1280*720<<1);
		}else{
			memcpy(this->frame, this->memory[this->buffer.index], (size_t) this->buffer.bytesused);
		}

		if(ioctl(this->fd, VIDIOC_QBUF, &this->buffer) == -1){
			perror("[tuc:WebCam:read] BUFFER_REQUEUE_ERROR");
			return -1;
		}

		return 1;
	};

	// Prepare to Read
	int WebCam::prepare2Read(){
		// If camera is streaming
		if(this->streaming){
			return 1;
		}

		// Set camera state as streaming
		this->streaming = true;

		// Check Camera's Capability
		struct v4l2_capability cap;
		memset(&cap, 0, sizeof (struct v4l2_capability));
		if(ioctl (this->fd, VIDIOC_QUERYCAP, &cap) == -1){
			perror("[tuc:WebCam:prepare2Read] CAP_ERROR");
			return -1;
		}

		// Set Camera's format
		struct v4l2_format fmt;
		memset (&fmt, 0, sizeof (struct v4l2_format));
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = this->frame_width;
		fmt.fmt.pix.height = this->frame_height;
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		fmt.fmt.pix.field = V4L2_FIELD_ANY;
		if(ioctl(this->fd, VIDIOC_S_FMT, &fmt) == -1){
			perror("[tuc:WebCam:prepare2Read] FORMAT_ERROR");
			return -1;
		}

		// Prepare buffers
		struct v4l2_requestbuffers rb;
		memset(&rb, 0, sizeof (struct v4l2_requestbuffers));
		rb.count = 16;
		rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		rb.memory = V4L2_MEMORY_MMAP;
		if(ioctl(this->fd, VIDIOC_REQBUFS, &rb) == -1){
			perror("[tuc:WebCam:prepare2Read] ALLOCATE_BUFFER_ERROR");
			return -1;
		}

		// Map the Buffers
		int i;
		for (i = 0; i < 16; i++){
			memset(&this->buffer, 0, sizeof (struct v4l2_buffer));
			this->buffer.index = i;
			this->buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			this->buffer.memory = V4L2_MEMORY_MMAP;
			if(ioctl(this->fd, VIDIOC_QUERYBUF, &this->buffer) == -1){
				perror("[tuc:WebCam:prepare2Read] BUFFER_MAP_ERROR");
				return -1;
			}

			this->memory[i] = mmap(0, this->buffer.length, PROT_READ, MAP_SHARED, fd, this->buffer.m.offset);
			if(this->memory[i] == MAP_FAILED) {
				perror("[tuc:WebCam:prepare2Read] BUFFER_MAP_FAILED");
				return -1;
			}
		}

		// Queue the Buffers
		for(i = 0; i < 16; ++i){
			memset(&this->buffer, 0, sizeof (struct v4l2_buffer));
			this->buffer.index = i;
			this->buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			this->buffer.memory = V4L2_MEMORY_MMAP;
			if(ioctl(this->fd, VIDIOC_QBUF, &this->buffer) == -1){
				perror("[tuc:WebCam:prepare2Read] BUFFER_QUEUE_ERROR");
				return -1;
			}
		}

		// Free last frame buffer
		free(this->frame);
		// Allocate new frame buffer 
		this->frame = (unsigned char *) calloc(1, (size_t) this->frame_width*this->frame_height<<1);

		// Stream Capture
		int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(ioctl(this->fd, VIDIOC_STREAMON, &type) == -1){
			perror("[tuc:WebCam:prepare2Read] STREAM_ERROR");
			return -1;
		}

		this->decoded_frame = new unsigned char **[this->frame_height];

		int y, x;
		for(y = 0; y < this->frame_height; y++){
			this->decoded_frame[y] = new unsigned char *[this->frame_width];
			for(x = 0; x < this->frame_width; x++){
				this->decoded_frame[y][x] = new unsigned char[3];
				// Pixel
				this->decoded_frame[y][x][2] = 20; // blue
				this->decoded_frame[y][x][1] = 20; // green
				this->decoded_frame[y][x][0] = 20; // red
			}
		}

		return 1;
	}

	// Change Control
	int WebCam::changeControl(__u32	id, bool custom, int value){
		// Variables
		struct v4l2_control control;
		struct v4l2_queryctrl queryctrl;

		// Initiate Query Control
		memset(&queryctrl, 0, sizeof(queryctrl));

		// Set query control id
		queryctrl.id = id;

		// Check Query Control
		if(-1 == ioctl(this->fd, VIDIOC_QUERYCTRL, &queryctrl)){
			if(errno != EINVAL){
				perror("[tuc:WebCam:open] VIDIOC_QUERYCTRL");
				return -1;
			}else{
				// Not supported
				return -2;
			}
		}else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED){
			// Not supported
			return -2;
		}

		// Initiate Control
		memset(&control, 0, sizeof (control));
		control.id = id;

		// Set Custom Value
		if(custom){
			control.value = value;
		}
		// Set Fixed Value
		else{
			// Minimum
			if(value < 0){
				control.value = queryctrl.minimum;
			}
			// Maximum
			else if(value > 0){
				control.value = queryctrl.maximum;
			}
			// Default
			else{
				control.value = queryctrl.default_value;
			}
		}

		// Check set value
		if (-1 == ioctl(this->fd, VIDIOC_S_CTRL, &control)){
			perror("[tuc:WebCam:open] VIDIOC_S_CTRL");
			return -1;
		}

		return 1;
	}

	// Load Control Value
	int WebCam::loadControl(__u32 id, __s32* value){
		// Variables
		struct v4l2_control control;

		// Initiate Control
		memset(&control, 0, sizeof (control));
		control.id = id;

		// Check Query Control
		if(-1 == ioctl(this->fd, VIDIOC_G_CTRL, &control)){
			perror("[tuc:WebCam:open] VIDIOC_G_CTRL");
			return -1;
		}
		
		// Save Value
		*value = control.value;

		return 1;
	}

	// Check if Control is supported
	struct v4l2_queryctrl* WebCam::isControlSupported(__u32 id){
		// Variables
		struct v4l2_queryctrl* queryctrl = new struct v4l2_queryctrl;

		// Initiate Query Control
		memset(queryctrl, 0, sizeof(struct v4l2_queryctrl));

		// Set query control id
		queryctrl->id = id;

		// Check Query Control
		if(-1 == ioctl(this->fd, VIDIOC_QUERYCTRL, queryctrl)){
			if(errno != EINVAL){
				perror("[tuc:WebCam:open] VIDIOC_QUERYCTRL");
				return NULL;
			}else{
				// Not supported
				return NULL;
			}
		}else if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED){
			// Not supported
			return NULL;
		}

		return queryctrl;
	}

	// Print Available Controls
	int WebCam::printControls(){
		// Variables
		struct v4l2_queryctrl queryctrl;
		struct v4l2_querymenu querymenu;
		int count = 0;

		// Initiate Query Control
		memset(&queryctrl, 0, sizeof(queryctrl));
		queryctrl.id = V4L2_CTRL_CLASS_USER | V4L2_CTRL_FLAG_NEXT_CTRL;

		// Print format
		std::cout << "[ Camera's \"" << this->device_name << "\" available controls ]" << std::endl;
		std::cout << "\t<control_name> [<value>] [<min>|<default>|<max>]" << std::endl;

		while(0 == ioctl(this->fd, VIDIOC_QUERYCTRL, &queryctrl)){
			// End condition
			if(V4L2_CTRL_ID2CLASS(queryctrl.id) != V4L2_CTRL_CLASS_USER){
				break;
			}
			// Slip Condition
			if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED){
				continue;
			}

			// If control has menu
			if(queryctrl.type == V4L2_CTRL_TYPE_MENU){
				// Print Control Name
				std::cout << '\t' << queryctrl.name << " [" << queryctrl.default_value << ']' << std::endl;

				// Initiate Query Menu
				memset(&querymenu, 0, sizeof(querymenu));
				querymenu.id = queryctrl.id;

				for(querymenu.index = queryctrl.minimum; querymenu.index <= queryctrl.maximum; querymenu.index++){
					if(0 == ioctl(this->fd, VIDIOC_QUERYMENU, &querymenu)){
						std::cout << "\t\t" << querymenu.name << std::endl;
					}
				}
			}
			// If no menu
			else{
				// Print Control Name
				std::cout << '\t' << queryctrl.name << " [" << queryctrl.minimum << '|' << queryctrl.default_value << '|' << queryctrl.maximum << ']' << std::endl;
			}

			// Count results
			count++;

			queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		}

		// Print number of results
		std::cout << "[ Found " << count << " available control(s). ]" << std::endl << std::endl;

		return count;
	}

	// Print Available Formats
	int WebCam::printFormats(){
		// Variables
		struct v4l2_fmtdesc vid_fmtdesc;
		struct v4l2_frmsizeenum frmsize;
		int count = 0;

		int factor = 0;

		// Initiate Frame Description
		memset(&vid_fmtdesc, 0, sizeof(vid_fmtdesc));
		vid_fmtdesc.index = 0;

		// Print format
		std::cout << "[ Camera's \"" << this->device_name << "\" available frame formats ]" << std::endl;
		std::cout << "\t<format_description> [<index>]" << std::endl;

		for (vid_fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; vid_fmtdesc.type < V4L2_BUF_TYPE_VIDEO_OVERLAY; vid_fmtdesc.type++){
			while(ioctl(this->fd, VIDIOC_ENUM_FMT, &vid_fmtdesc) == 0){
				std::cout << '\t' << vid_fmtdesc.description << " [" << vid_fmtdesc.index << ']' << std::endl;
				std::cout << "\t   type         : " << vid_fmtdesc.type << std::endl;
				std::cout << "\t   compressed   : " << vid_fmtdesc.flags << std::endl;
				std::cout << "\t   pixelformat  : " << (char) (vid_fmtdesc.pixelformat & 0xFF) << (char) ((vid_fmtdesc.pixelformat >> 8) & 0xFF) << (char) ((vid_fmtdesc.pixelformat >> 16) & 0xFF) << (char) ((vid_fmtdesc.pixelformat >> 24) & 0xFF) << std::endl;

				frmsize.pixel_format = vid_fmtdesc.pixelformat;
				frmsize.index = 0;
				while(ioctl(this->fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0){
					if(frmsize.index == 0)
						std::cout << "\t   sizes        :" << std::endl;

					if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE){
						std::cout << "\t      " << frmsize.discrete.width << 'x' << frmsize.discrete.height << " [discrete]" << std::endl;
					} else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE){
						std::cout << "\t      " << frmsize.stepwise.max_width << 'x' << frmsize.stepwise.max_height << " [stepwise]" << std::endl;
					}
					frmsize.index++;
				}
				vid_fmtdesc.index++;
				
				count++;
			}
		}

		// Print number of results
		std::cout << "[ Found " << count << " available frame format(s). ]" << std::endl << std::endl;

		return count;
	}

	bool WebCam::supported(){
		// Variables
		struct v4l2_frmsizeenum frmsize;

		// Check for V4L2_PIX_FMT_YUYV support
		frmsize.pixel_format = V4L2_PIX_FMT_YUYV;
		frmsize.index = 0;
		if(ioctl(this->fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0){
			return true;
		}

		std::cout << "Camera not supported." << std::endl;

		return false;
	}

	int WebCam::setDefaultFrameSize(){
		// Variables
		struct v4l2_frmsizeenum frmsize;
		// Reset Frame Resolution
		this->frame_width = 0;
		this->frame_height = 0;

		// Check the V4L2_PIX_FMT_YUYV format
		frmsize.pixel_format = V4L2_PIX_FMT_YUYV;
		frmsize.index = 0;
		while(ioctl(this->fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0){
			if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE && (this->frame_width<frmsize.discrete.width || this->frame_height<frmsize.discrete.height)){
				this->frame_width = frmsize.discrete.width;
				this->frame_height = frmsize.discrete.height;
			}
			frmsize.index++;
		}

		if(this->frame_width == 0 || this->frame_height == 0){
			std::cout << "Can not find a camera resolution." << std::endl;
			return -1;
		}

		return 1;
	}

	int WebCam::setFrameSize(int width, int height){
		// Variables
		struct v4l2_frmsizeenum frmsize;

		// Check the V4L2_PIX_FMT_YUYV format
		frmsize.pixel_format = V4L2_PIX_FMT_YUYV;
		frmsize.index = 0;
		while(ioctl(this->fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0){
			if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE && width == frmsize.discrete.width && height == frmsize.discrete.height){
				this->frame_width = width;
				this->frame_height = height;

				return 1;
			}
			frmsize.index++;
		}

		std::cout << "Camera do not support " << width << 'x' << height << " resolution." << std::endl;
		return -1;
	}

	void WebCam::decode(){
		int r1, g1, b1;
		int r2, g2, b2;
		int y1, y2, u, v;

		int i;
		int row = 0;
		int col = 0;
	
		for(i=0; i<this->frame_width*this->frame_height; i+=2){
			y1 = this->frame[i*2] << 8;
			y2 = this->frame[i*2+2] << 8;
			u = this->frame[i*2+1] - 128;
			v = this->frame[i*2+3] - 128;


			r1 = (y1 + (359 * v)) >> 8;
			if(r1>255) r1 = 255;
			else if(r1<0) r1 = 0;

			g1 = (y1 - (88 * u) - (183 * v)) >> 8;
			if(g1>255) g1 = 255;
			else if(g1<0) g1 = 0;

			b1 = (y1 + (454 * u)) >> 8;
			if(b1>255) b1 = 255;
			else if(b1<0) b1 = 0;

			r2 = (y2 + (359 * v)) >> 8;
			if(r2>255) r2 = 255;
			else if(r2<0) r2 = 0;

			g2 = (y2 - (88 * u) - (183 * v)) >> 8;
			if(g2>255) g2 = 255;
			else if(g2<0) g2 = 0;

			b2 = (y2 + (454 * u)) >> 8;
			if(b2>255) b2 = 255;
			else if(b2<0) b2 = 0;

			row = i/this->frame_width;
			col = i%this->frame_width;

			// 1st pixel
			this->decoded_frame[row][col][0] = b1; // blue
			this->decoded_frame[row][col][1] = g1; // green
			this->decoded_frame[row][col][2] = r1; // red

			// 2nd pixel
			this->decoded_frame[row][col+1][0] = b2; // blue
			this->decoded_frame[row][col+1][1] = g2; // green
			this->decoded_frame[row][col+1][2] = r2; // red
		}
	}

	unsigned char *** WebCam::getFrame(){
		return this->decoded_frame;
	}
}