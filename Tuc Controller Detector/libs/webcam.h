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
	This is a library created in order to access easy and fast
	a camera connected to the system using only the default Linux camera 
	driver Video4Linux version 2 (V4L2) and display the results using 
	Xlib (also known as libX11).

	V4L2
		Documentation				http://www.linuxtv.org/downloads/legacy/video4linux/API/V4L2_API/spec-single/v4l2.html
	Xlib
		Documentation				http://www.x.org/archive/X11R7.5/doc/
		Function documentation		http://www.x.org/archive/X11R7.5/doc/man/man3/
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

/*
 * Library is under the tuc namespace
 * "tuc" stands for "Technical University of Crete"
 */
namespace tuc{
	/*
	 * We named our class WebCam so that we can show
	 * that it is referred to a computer camera for which
	 * we mostly use a WebCam.
	 */
	class WebCam{
		/*
		 * Public section
		 */
		public:
			/*
			 * Camera's File Descriptor
			 */
			int fd;
			/*
			 * Camera's Device Name
			 */
			char* device_name;
			/*
			 * Frame's Resolution
			 */
			int frame_width;
			int frame_height;
			/*
			 * Camera is streaming
			 */
			bool streaming;
			/*
			 * Frame image buffer
			 */
			unsigned char *frame;
			unsigned char ***decoded_frame;
			/*
			 * Frame buffers
			 */
			struct v4l2_buffer buffer;
			/*
			 * Frame memory buffers
			 */
			void *memory[16];

			/*
			 * Constructor
			 *
			 * This function initiate class variables.
			 *
			 * Parameters
			 * 		device_name				the name of the device to open
			 *						
			 */
			WebCam(const char* device_name);

			/*
			 * Open
			 *
			 * This function initiate the process of getting
			 * a camera's file descriptor.
			 * 
			 * returns
			 *		file descriptor on success
			 *		-1 on fail
			 */
			int open();

			/*
			 * Close
			 *
			 * This function close camera and release it.
			 * 
			 * returns
			 *		 1 on success
			 *		-1 on fail
			 */
			int close();

			/*
			 * Change Control
			 *
			 * This function change a camera control.
			 *
			 *
			 * Parameters
			 * 		id				control id value
			 *		custom			true if value is custom
			 *		value			if custom is true
			 *							any int value
			 *						if custom is false
			 *							<0 for minimum value
			 *							>0 for maximum value
			 *							 0 for default value
			 *
			 * List of control ids
			 * 		http://linuxtv.org/downloads/v4l-dvb-apis/control.html
			 *
			 * returns
			 *		1 on success
			 *		-1 on error fail
			 *		-2 on not supported
			 */
			int changeControl(__u32	id, bool custom, int value);
			int loadControl(__u32 id, __s32* value);
			// Check if Control is supported
			struct v4l2_queryctrl* isControlSupported(__u32 id);

			/*
			 * Print Controls
			 *
			 * This function prints all available camera controls.
			 *
			 * List of controls
			 * 		http://linuxtv.org/downloads/v4l-dvb-apis/control.html
			 *
			 * returns
			 *		number of available controls found
			 */
			int printControls();

			/*
			 * Print Formats
			 *
			 * This function prints all available camera formats.
			 *
			 * returns
			 *		number of available formats found
			 */
			int printFormats();

			/*
			 * Supported
			 *
			 * This function check if this camera is supported by this C/C++ code.
			 *
			 * returns
			 *		true	if supported
			 *		false	if not supported (with an error on stderr)
			 */
			bool supported();

			int setDefaultFrameSize();
			int setFrameSize(int width, int height);

			int read();
			int prepare2Read();
			void decode();
			unsigned char *** getFrame();
	};
}