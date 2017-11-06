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
 *	This is a program that reads the input from a camera
 *	connected and show the result in a window.
 *
 *	Compile using GNU compiler
 *		Command:
 *			g++ video.cpp libs/*.cpp -o video -lX11 -std=c++0x -pthread
 *
 *	Common Errors
 *		Error: fatal error: X11/Xlib.h: No such file or directory
 *		Fix: sudo apt-get install libx11-dev
 */

// C/C++ Libraries
	// For General Purpose
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <iostream>
	#include <thread>
	// Display Library
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	#include <X11/Xutil.h>

// Load Namespaces
	// Stdlib namespace
	using namespace std;

// C++ Custom Libraries
	#include "libs/webcam.h"

// Functions
	void programInfo();
	void parseArguments(int argc, const char* argv[]);
	void camera_stream();
	void windowEventsListener(Display *display, Window window);
	XImage *parseFrame2Image(Display *display, Visual *visual, unsigned char *image, int width, int height, unsigned char * framebuffer);

// Variables
	// WebCam Variable
		tuc::WebCam* cam;
	// Size
		int width = 0;
		int height = 0;
	// Options
		bool reflect = false;
		bool print = false;
		bool default_device = true;
		char device_name[256] = "/dev/video";
		bool ignore_errors = false;
		bool create_gui = true;
	// Loop Code
		bool loop = true;

// Main function
	int main (int argc, const char* argv[]){
		// Check user options
		parseArguments(argc, argv);

		// Init Webcam
		if(default_device){
			cam = new tuc::WebCam(NULL);
		}else{
			cam = new tuc::WebCam(device_name);
		}

		// Open Webcam
		if(cam->open() == -1 && !ignore_errors){
			exit(EXIT_FAILURE);
		}

		// Print camera info and exit
		if(print){
			// Print availiable controls
			cam->printControls();
			// Print availiable formats
			cam->printFormats();
		}else{
			// Check if camera supported
			if(cam->supported()){
				// Initiate camera stream
				camera_stream();
			}
			// Camera not supported
			else{
				exit(EXIT_FAILURE);
			}
			
		}
	}

	// Dsiplay stream from camera 
	void camera_stream(){
		// Frame size decide
		if(width == 0 || height == 0){
			if(cam->setDefaultFrameSize() == -1 && !ignore_errors){
				exit(EXIT_FAILURE);
			}
		}else{
			if(cam->setFrameSize(width, height) == -1 && !ignore_errors){
				exit(EXIT_FAILURE);
			}
		}

		// Window Variables
		Display *display = NULL;
		Visual *visual = NULL;
		Window window;

		// If gui enabled
		if(create_gui){
			// Crete Window
			display = XOpenDisplay(NULL);
			visual = DefaultVisual(display, 0);
			window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, cam->frame_width, cam->frame_height, 1, 0, 0);
			XStoreName(display, window, "Camera Video");

			XSizeHints *window_resize_rule = XAllocSizeHints();
			window_resize_rule->flags = PMinSize | PMaxSize;
			window_resize_rule->min_width = window_resize_rule->max_width = cam->frame_width;
			window_resize_rule->min_height = window_resize_rule->max_height = cam->frame_height;
			XSetWMNormalHints(display, window, window_resize_rule);

			XMapWindow(display, window);
		}

		loop = true;

		// If gui enabled
		std::thread parallel_task;
		if(create_gui){
			// Start a parallel task
			// Listen windows events
			parallel_task = std::thread(windowEventsListener, display, window);
		}

		// Load Image
		cam->read();

		// Start Loop
		XImage *ximage;
		while(loop){
			// If gui enabled
			if(create_gui){
				// Create image
				ximage = parseFrame2Image(display, visual, 0, cam->frame_width, cam->frame_height, cam->frame);
				// Insert image
				XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, cam->frame_width, cam->frame_height);
				XFlush(display);
				// Free up some memory
				ximage->f.destroy_image(ximage);
			}

			// Get frame from camera
			cam->read();
		}
		
		// Report Exit
		cout << "Closing program..." << endl;

		// If gui enabled
		if(create_gui){
			// Wait parallel tark
			parallel_task.join();

			// Close Window
			XDestroyWindow(display, window);
			XCloseDisplay(display);
		}

		// Close Camera
		cam->close();
	}

	void windowEventsListener(Display *display, Window window){
		// Event Variable
		XEvent event;

		// Set up events
		Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(display, window, &wmDelete, 1);
		XSelectInput(display, window, StructureNotifyMask);

		// Event Loop
		while(loop){
			// Wait Event to come
			XNextEvent(display, &event);
			switch(event.type){
				case(ClientMessage):
					// On close window
					if(event.xclient.data.l[0] == wmDelete){
						loop = false;
					}
					break;
			}
		}

	}

	// Print Program's info
	void programInfo(){
		cout << "Technical University of Crete" << endl;
		cout << "[HPY411] Embedded Systems" << endl;
		cout << endl;
		cout << "Students" << endl;
		cout << "    Georgara Athina" << endl;
		cout << "    Grammatopoulos Athanasios Vasileios" << endl;
		cout << endl;
		cout << "Copyright (c) 2015-2016" << endl;
		cout << endl;
		cout << "Display input from camera using default linux driver." << endl;
		cout << endl;
	}

	// Parse Program Arguments
	void parseArguments(int argc, const char* argv[]){
		// Char pointer
		const char * p;

		// Check arguments
		for(int i=1; i<argc; ++i){

			// Print Help
			if(strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0){
				cout << "Usage : video [OPTION]..." << endl;
				cout << "Capture a camera video stream." << endl;
				cout << endl;
				cout << "Optional arguments:" << endl;
				cout << "  -h , --help                  prints this information." << endl;
				cout << "  -d [number]                  camera device number, default is 0." << endl;
				cout << "  -c [string]                  custom camera device name." << endl;
				cout << "  -r [number>0]x[number>0]     set one of the available resolutions." << endl;
				cout << "  -w [on|off]                  window on or off." << endl;
				cout << "  -reflect [on|off]            reverse left right." << endl;
				cout << "  -p                           print device info." << endl;
				cout << "  -i                           ignore errors." << endl;
				cout << "  -about                       about this program." << endl;
				cout << endl;

				exit(EXIT_SUCCESS);
			}

			// Change Camera Device
			else if(strcmp("-d", argv[i]) == 0){
				// Check if it is valid format
				if(i+1 < argc){
					// Next argument
					i++;
					// Custom device
					default_device = false;
					// Change device
					strcat(device_name, argv[i]);
				}
			}

			// Custom Camera Device
			else if(strcmp("-c", argv[i]) == 0){
				// Check if it is valid format
				if(i+1 < argc){
					// Next argument
					i++;
					// Custom device
					default_device = false;
					// Change device
					strcpy(device_name, argv[i]);
				}
			}

			// Change Resolution
			else if(strcmp("-r", argv[i]) == 0){
				// Check if it is valid format
				if(i+1 < argc){
					// Next argument
					i++;
					// Find x
					p = strchr(argv[i], 'x');
					if(p != NULL){
						width = (int) strtol(argv[i], NULL, 10);
						height = (int) strtol(p+1, NULL, 10);
					}
				}
			}

			// Window
			else if(strcmp("-w", argv[i])==0){
				if(i+1 < argc){
					i++;
					if(strcmp("on", argv[i])==0){
						create_gui = true;
					}else if(strcmp("off", argv[i])==0){
						create_gui = false;
					}
				}
			}

			// Reflect
			else if(strcmp("-reflect", argv[i])==0){
				if(i+1 < argc){
					i++;
					if(strcmp("off", argv[i])==0){
						reflect = false;
					} else if(strcmp("on", argv[i])==0){
						reflect = true;
					}
				}
			}

			// Print screen resolutions
			else if(strcmp("-p", argv[i])==0){
				print = true;
			}

			// Ignore errors some errors
			else if(strcmp("-i", argv[i])==0){
				ignore_errors = true;
			}

			// Print about info
			else if(strcmp("-about", argv[i])==0){
				programInfo();
				exit(EXIT_SUCCESS);
			}
		}
	}

	XImage *parseFrame2Image(Display *display, Visual *visual, unsigned char *image, int width, int height, unsigned char * framebuffer){	
		int r1, g1, b1;
		int r2, g2, b2;
		int y1, y2, u, v;

		int i;
		unsigned char *image32 = (unsigned char *)malloc(width*height*4);
		unsigned char *p = image32;

		for(i=0; i<width*height; i+=2){
			if(!reflect){
				y1 = framebuffer[i*2] << 8;
				y2 = framebuffer[i*2+2] << 8;
				u = framebuffer[i*2+1] - 128;
				v = framebuffer[i*2+3] - 128;
			}else{
				y2 = framebuffer[i*2   + (2*(width-2)) - (4*(i%width))] << 8;
				y1 = framebuffer[i*2+2 + (2*(width-2)) - (4*(i%width))] << 8;
				u = framebuffer[i*2+1  + (2*(width-2)) - (4*(i%width))] - 128;
				v = framebuffer[i*2+3  + (2*(width-2)) - (4*(i%width))] - 128;
			}

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


			// 1st pixel
			*p++ = b1; // blue
			*p++ = g1; // green
			*p++ = r1; // red
			p++;

			// 2nd pixel
			*p++ = b2; // blue
			*p++ = g2; // green
			*p++ = r2; // red
			p++;
		}
		
		return XCreateImage(display, visual, 24, ZPixmap, 0, (char *)image32, width, height, 32, 0);
	}

