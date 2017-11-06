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
 *	connected and reads the controllers on screen so that
 *	we can distinguish each of them and sent position data
 *	on the server game.
 *
 *	Compile using GNU compiler
 *		Command:
 *			g++ main.cpp libs/*.cpp -o main -lX11 -std=c++0x -pthread
 *		Execute
 *			./main -d 1 -w on -reflect on -r 320x240
 *			./main -w on -reflect on -r 320x240 -colordetect on -keephistory on
 */

// C/C++ Libraries
	#include <stdio.h>
	#include <stdlib.h>
	#include <iostream>

// Display Library
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	#include <X11/Xutil.h>

	#include <sys/ioctl.h>
	#include <unistd.h>

 	#include <sys/time.h>

// Network
	#include <string.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>

// Load Namespaces
	// Stdlib namespace
	using namespace std;

// Custom Libraries
	#include "libs/webcam.h"
	#include "libs/framelib.h"
	#include "libs/drawlib.h"

// Global Variables
	// Camera Object
	tuc::WebCam *cam;
	bool default_device = true;
	char device_name[256] = "/dev/video";
	// Camera Options
	int option_width = 0;
	int option_height = 0;
	// Windows Variables
	bool window_open = true;
	bool reflect = false;
	// Net Data
	bool network = false;
	char host[255];
	char port[] = "8080";
	// Loop Code
	bool loop = true;

	// Other options
	bool detect_keep_history = false;
	bool detect_check_color = false;
	bool detect_base_on_hue = true;

// Code Functions
	void programInfo();
	void parseArguments(int, const char* []);
	tuc::Frame::label_list* getValidPoints(tuc::Frame*, tuc::Draw*, tuc::Frame::controler*, tuc::Frame::controler*, tuc::Frame::controler*, tuc::Frame::controler*, tuc::Frame::controler*, int);
	void drawValidPoint(tuc::Frame::label_list*, tuc::Frame::controler*, tuc::Draw*, unsigned char, unsigned char, unsigned char, int);
	XImage *parseFrame2Image(Display*, Visual*, unsigned char ***, int, int);
	XImage *parseFrameObject2Image(Display*, Visual*, tuc::Frame*, int);
	XImage *parseFramePoints2Image(Display*, Visual*, tuc::Frame::controler, tuc::Frame::controler, tuc::Frame::controler, tuc::Frame::controler);
	XImage *parseFramePoints2Image_noColors(Display*, Visual*, tuc::Frame::controler);

// Main function
	int main (int argc, const char* argv[]) {

		host[0] = '\0';
		// Parse Data
		parseArguments(argc, argv);

		/********************************************************************
		 *  Variables
		 ********************************************************************/
		// Frame counter - counts the frames
		int frame_counter = 0;
		int fps_counter = 0;

		// FPS Counters
		struct timeval start, end;
		long mtime, seconds, useconds;
		gettimeofday(&start, NULL);

		char fps_string[3];
		char fps_title[] = "FPS :";
		int fps = 0;
		char frame_title[] = "Frames :";
		char frame_string[20];

		//Initial list of grooups
		tuc::Frame::label_list* group_list = NULL;
		//Temporal list
		tuc::Frame::label_list* temp_list = NULL;
		//Final list of controllers (validated round areas)
		tuc::Frame::label_list* controller = NULL;
		// Window Name
		const string win_name = "Camera Input";

		//Colors
		char red[] = "Red";
		char blue[] = "Blue";
		char green[] = "Green";
		char yellow[] = "Yellow";
		int color = 0;

		//Controlers		
		tuc::Frame::controler red_group;
		red_group.last_frame = -1;
		red_group.frames = 0;
		red_group.on = false;

		tuc::Frame::controler blue_group;
		blue_group.last_frame = -1;
		blue_group.frames = 0;
		blue_group.on = false;

		tuc::Frame::controler green_group;
		green_group.last_frame = -1;
		green_group.frames = 0;
		green_group.on = false;

		tuc::Frame::controler yellow_group;
		yellow_group.last_frame = -1;
		yellow_group.frames = 0;
		yellow_group.on = false;

		tuc::Frame::controler other_group;
		other_group.last_frame = -1;
		other_group.frames = 0;
		other_group.on = false;



		/********************************************************************
		 *  Network
		 ********************************************************************/
		int sockfd, portno, net_error;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		char net_message[1024];

		if(strlen(host) > 0){
			portno = atoi(port);
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if(sockfd < 0){
				fprintf(stderr, "ERROR opening socket\n");
				network = false;
			}else{
				server = gethostbyname(host);
				if(server == NULL){
					fprintf(stderr, "ERROR, no such host\n");
					network = false;
				}else{
					bzero((char *) &serv_addr, sizeof(serv_addr));
					serv_addr.sin_family = AF_INET;
					bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
					serv_addr.sin_port = htons(portno);
					if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
						fprintf(stderr, "ERROR connecting\n");
						network = false;
					}else{
						fprintf(stderr, "Connected on host.\n");
					}
				}
			}
		}

		/********************************************************************
		 *  Set Up camera
		 ********************************************************************/
		// Set a camera input
		if(default_device){
			cam = new tuc::WebCam(NULL);
		}else{
			cam = new tuc::WebCam(device_name);
		}
		// Open any camera
		if(cam->open() == -1){
			exit(EXIT_FAILURE);
		}
		// If not supported exit
		if(!cam->supported()){
			exit(EXIT_FAILURE);
		};

		// Frame size decide
		if(option_width == 0 || option_height == 0){
			if(cam->setDefaultFrameSize() == -1){
				exit(EXIT_FAILURE);
			}
		}else{
			if(cam->setFrameSize(option_width, option_height) == -1){
				exit(EXIT_FAILURE);
			}
		}

		// Get a frame from camera
		cam->read();
		cam->decode();

		/********************************************************************
		 *  Graphic UI - windows create
		 ********************************************************************/

		// Crete Window
		Display *display = NULL;
		Visual *visual = NULL;
		Window  window_camera_layer,
				window_threshold_layer,
				window_colors,
				window_points_layer;

		// Camera image stream
		XImage *ximage;

		// Window
		if(window_open){
			// Get display
			if(!(display = XOpenDisplay(NULL))) {
				fprintf(stderr, "Warning: could not open display.\n");
				window_open = false;
			}
		}
		if(window_open){
			visual = DefaultVisual(display, 0);

			// Create Windows

			// Camera Window
			window_camera_layer = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, cam->frame_width, cam->frame_height, 1, 0, 0);
			XStoreName(display, window_camera_layer, "Camera");

			// Threshold Window
			window_threshold_layer = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, cam->frame_width, cam->frame_height, 1, 0, 0);
			XStoreName(display, window_threshold_layer, "Threshold");

			// Colors Window
			if(detect_check_color){
				window_colors  = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, cam->frame_width*2, cam->frame_height*2, 1, 0, 0);
				XStoreName(display, window_colors, "Colors");
			}

			// Points
			window_points_layer = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, cam->frame_width, cam->frame_height, 1, 0, 0);
			XStoreName(display, window_points_layer, "Controllers");

			XSizeHints *window_resize_rule = XAllocSizeHints();
			window_resize_rule->flags = PMinSize | PMaxSize;
			window_resize_rule->min_width = window_resize_rule->max_width = cam->frame_width;
			window_resize_rule->min_height = window_resize_rule->max_height = cam->frame_height;
			XSetWMNormalHints(display, window_camera_layer, window_resize_rule);
			XSetWMNormalHints(display, window_threshold_layer, window_resize_rule);
			XSetWMNormalHints(display, window_points_layer, window_resize_rule);

			if(detect_check_color){
				window_resize_rule = XAllocSizeHints();
				window_resize_rule->flags = PMinSize | PMaxSize;
				window_resize_rule->min_width = window_resize_rule->max_width = cam->frame_width*2;
				window_resize_rule->min_height = window_resize_rule->max_height = cam->frame_height*2;
				XSetWMNormalHints(display, window_colors, window_resize_rule);
			}

			XMapWindow(display, window_camera_layer);
			XMapWindow(display, window_threshold_layer);
			if(detect_check_color)
				XMapWindow(display, window_colors);
			XMapWindow(display, window_points_layer);

			// Create image
			ximage = parseFrame2Image(display, visual, cam->getFrame(), cam->frame_width, cam->frame_height);
			// Insert image
			XPutImage(display, window_camera_layer, DefaultGC(display, 0), ximage, 0, 0, 0, 0, cam->frame_width, cam->frame_height);
			XFlush(display);
			// Free up some memory
			ximage->f.destroy_image(ximage);
		}
		

		/********************************************************************
		 *  Image process
		 ********************************************************************/

		// Frame
		tuc::Frame* frame = NULL;
		tuc::Draw* draw = NULL;

		// Init Frame
		frame = new tuc::Frame(cam->getFrame(), reflect, cam->frame_width, cam->frame_height);
		draw = new tuc::Draw(cam->getFrame(), cam->frame_width, cam->frame_height);


		/********************************************************************
		 *  Sent data using socket
		 ********************************************************************/

		// Sent net data
		if(network){
			// Size
			sprintf(net_message,"-size: %dx%d", cam->frame_width, cam->frame_height);
			net_error = write(sockfd, net_message, strlen(net_message));
			if(net_error<0){
				printf("ERROR writing to socket");
				network = false;
			}else{
				printf("Size data was sented.\n");
			}

			// Reflection
			sprintf(net_message,"-reflect: %d", reflect?1:0);
			net_error = write(sockfd, net_message, strlen(net_message));
			if(net_error<0){
				printf("ERROR writing to socket");
				network = false;
			}else{
				printf("Reflect option was sented.\n");
			}
		}

		// Find Frames groups
		frame->labels();

		/********************************************************************
		 *  Main Loop
		 ********************************************************************/
		while(loop){
			// Count frames
			frame_counter++;
			fps_counter++;

			// Get an image
			cam->read();
			cam->decode();
			// Parse Frame
			frame->parse(cam->getFrame());
			// Find Frames groups
			frame->labels();

			controller = NULL;
			temp_list = NULL;
			controller = getValidPoints(frame, draw, &red_group, &blue_group, &green_group, &yellow_group, &other_group, frame_counter);
			frame->label_head = NULL;

			if(red_group.last_frame != frame_counter){
				red_group.on = false;
				red_group.frames = 0;
			}
			if(blue_group.last_frame != frame_counter){
				blue_group.on = false;
				blue_group.frames = 0;
			}
			if(green_group.last_frame != frame_counter){
				green_group.on = false;
				green_group.frames = 0;
			}
			if(yellow_group.last_frame != frame_counter){
				yellow_group.on = false;
				yellow_group.frames = 0;
			}
			if(other_group.last_frame != frame_counter){
				other_group.on = false;
				other_group.frames = 0;
			}

		// Sent data
			if(network && (red_group.on || green_group.on || blue_group.on || yellow_group.on || other_group.on)){
				strcpy(net_message, "-data:");
				
				// Red 
				if(red_group.on){
					sprintf(net_message+strlen(net_message),"r:%dx%dx%d", red_group.center[0], red_group.center[1], (red_group.height+red_group.width)/2);
				}

				// Green 
				if(green_group.on){
					sprintf(net_message+strlen(net_message),"g:%dx%dx%d", green_group.center[0], green_group.center[1], (green_group.height+green_group.width)/2);
				}

				// Blue 
				if(blue_group.on){
					sprintf(net_message+strlen(net_message),"b:%dx%dx%d", blue_group.center[0], blue_group.center[1], (blue_group.height+blue_group.width)/2);
				}

				// Yellow 
				if(yellow_group.on){
					sprintf(net_message+strlen(net_message),"y:%dx%dx%d", yellow_group.center[0], yellow_group.center[1], (yellow_group.height+yellow_group.width)/2);
				}
				//Color off
				if(other_group.on){
					sprintf(net_message+strlen(net_message),"w:%dx%dx%d", other_group.center[0], other_group.center[1], (other_group.height+other_group.width)/2);					
				}

				net_error = write(sockfd, net_message, strlen(net_message));
				if(net_error<0){
					printf("ERROR writing to socket");
					network = false;
				}
			}

		//Clear groups
			if(frame->max_roundness > 0){
				while(controller!=NULL){
					temp_list = controller;
					controller = controller->next;


					delete temp_list->group;
					temp_list->group = NULL;
					temp_list->next = NULL;
					delete temp_list;
					temp_list = NULL;
				}
			}

		//Frame Statistics
			gettimeofday(&end, NULL);
			seconds  = end.tv_sec  - start.tv_sec;
			useconds = end.tv_usec - start.tv_usec;
			mtime = (((seconds) * 1000 + useconds/1000.0) + 0.5);
			if(mtime>1000){
				//printf("FPS: %d\n", fps_counter);
				fps = fps_counter;

				fps_counter = 0;
				gettimeofday(&start, NULL);
			}
			
			// Show image
			if(window_open){
				draw->color(0,0,0);
				// FPS Count
				snprintf(fps_string, 3, "%d", fps);
				draw->write(fps_title, 1, 1);
				draw->write(fps_string, 70, 1);
				// Frame Count
				snprintf(frame_string, 20, "%d", frame_counter);
				draw->write(frame_title, 1, 10);
				draw->write(frame_string, 70, 10);

				if(network){
					strcpy(net_message, "-fps:");
					strcat(net_message, fps_string);
					net_error = write(sockfd, net_message, strlen(net_message));
					if(net_error<0){
						printf("ERROR writing to socket\n");
						network = false;
					}
				}

				// Update window
				// Main Frame
					ximage = parseFrame2Image(display, visual, cam->getFrame(), cam->frame_width, cam->frame_height);
					// Insert image
					XPutImage(display, window_camera_layer, DefaultGC(display, 0), ximage, 0, 0, 0, 0, cam->frame_width, cam->frame_height);
					// Free up some memory
					ximage->f.destroy_image(ximage);


				// Threshold Frame
					ximage = parseFrameObject2Image(display, visual, frame, 1);
					// Insert image
					XPutImage(display, window_threshold_layer, DefaultGC(display, 0), ximage, 0, 0, 0, 0, cam->frame_width, cam->frame_height);
					XFlush(display);
					// Free up some memory
					ximage->f.destroy_image(ximage);


				// Points Frame
					if(detect_check_color)
						ximage = parseFramePoints2Image(display, visual, red_group, green_group, blue_group, yellow_group);
					else
						ximage = parseFramePoints2Image_noColors(display, visual, other_group);
					// Insert image
					XPutImage(display, window_points_layer, DefaultGC(display, 0), ximage, 0, 0, 0, 0, cam->frame_width, cam->frame_height);
					XFlush(display);
					// Free up some memory
					ximage->f.destroy_image(ximage);

				if(detect_check_color){
					// Red
					ximage = parseFrameObject2Image(display, visual, frame, 12);
					XPutImage(display, window_colors, DefaultGC(display, 0), ximage, 0, 0, 0, 0, cam->frame_width, cam->frame_height);
					XFlush(display);
					ximage->f.destroy_image(ximage);

					// Green
					ximage = parseFrameObject2Image(display, visual, frame, 11);
					XPutImage(display, window_colors, DefaultGC(display, 0), ximage, 0, 0, cam->frame_width, 0, cam->frame_width, cam->frame_height);
					ximage->f.destroy_image(ximage);

					// Blue
					ximage = parseFrameObject2Image(display, visual, frame, 13);
					XPutImage(display, window_colors, DefaultGC(display, 0), ximage, 0, 0, 0, cam->frame_height, cam->frame_width, cam->frame_height);
					ximage->f.destroy_image(ximage);

					// Yellow
					ximage = parseFrameObject2Image(display, visual, frame, 14);
					XPutImage(display, window_colors, DefaultGC(display, 0), ximage, 0, 0, cam->frame_width, cam->frame_height, cam->frame_width, cam->frame_height);
					ximage->f.destroy_image(ximage);
				}

				XFlush(display);
			}
		}

		// Sent net data
		if(network){
			strcpy(net_message,"-exit");
			net_error = write(sockfd, net_message, strlen(net_message));
			if(net_error<0){
				printf("Failed to close server.");
				network = false;
			}else{
				printf("Server closed.\n");
			}

			close(sockfd);
			network = false;
		}

		if(window_open){
			XDestroyWindow(display, window_camera_layer);
			XCloseDisplay(display);
		}

		cam->close();

		return 0;
	}

	// Get a list of valid points
	tuc::Frame::label_list* getValidPoints(tuc::Frame* frame, tuc::Draw* draw, tuc::Frame::controler* red_group, tuc::Frame::controler* blue_group, tuc::Frame::controler* green_group, tuc::Frame::controler* yellow_group, tuc::Frame::controler* other_group, int frame_counter){
		// Get the list of points
		tuc::Frame::label_list* group_list = frame->label_head;
		// Temp list
		tuc::Frame::label_list* temp_list = NULL;
		// Temp vars
		int color = 0;
		tuc::Frame::label_list* controller = NULL;

		// For every point found
		while(group_list!=NULL){
			// Save next temporary
			temp_list = group_list->next;

			color = 0;
			// Validate group roundness
			if(
				// Check roundness
				frame->validateRoundness(group_list->group)!=0 && (
				// Check history
				(detect_keep_history && (color = frame->checkHistory(group_list->group, *red_group, *blue_group, *green_group, *yellow_group)) != 0) ||
				// Check for solid Center
				(frame->validateSolidCenter(group_list->group)!=0 && 
				// Check for a valid color
				//(!detect_check_color || (color = frame->validColor(group_list->group))!=0) ))
				(!detect_check_color || (color = ((detect_base_on_hue)?frame->validColorHue(group_list->group):frame->validColor(group_list->group)))!=0) ))
			){

				// Find controler color
				// RED
				if(color == 1){
					if(!(red_group->on && red_group->last_frame == frame_counter)){
						drawValidPoint(group_list, red_group, draw, 255,0,0, frame_counter);
					}
				}
				// BLUE
				else if(color == 2){
					if(!(blue_group->on && blue_group->last_frame == frame_counter)){
						drawValidPoint(group_list, blue_group, draw, 0,0,255, frame_counter);
					}
				}
				// GREEN
				else if(color == 3){
					if(!(green_group->on && green_group->last_frame == frame_counter)){
						drawValidPoint(group_list, green_group, draw, 0,255,0, frame_counter);
					}
				}
				// YELLOW
				else if(color == 4){
					if(!(yellow_group->on && yellow_group->last_frame == frame_counter)){
						drawValidPoint(group_list, yellow_group, draw, 255,165,0, frame_counter);
					}
				}
				// OTHER
				else{
					if(!(other_group->on && other_group->last_frame == frame_counter)){
						drawValidPoint(group_list, other_group, draw, 255,255,255, frame_counter);
					}
				}

				group_list->next = controller;
				controller = group_list;
			}

			// Delete point
			else {
				delete group_list->group;
				group_list->group = NULL;
				group_list->next = NULL;
				delete group_list;
				group_list = NULL;
			}

			// Restore list
			group_list = temp_list;
			temp_list = NULL;
		}

		return group_list;
	}

	// Draw valid point
	void drawValidPoint(tuc::Frame::label_list* group_list, tuc::Frame::controler* color_group, tuc::Draw* draw, unsigned char r, unsigned char g, unsigned char b, int frame_counter){
		// Color group set point
		color_group->on = true;
		color_group->center[0] = group_list->group->center[0];
		color_group->center[1] = group_list->group->center[1];
		color_group->height = group_list->group->height;
		color_group->width = group_list->group->width;
		color_group->frames++;
		color_group->last_frame = frame_counter;
		// Draw point
		draw->color(0,0,0)->cross(group_list->group->center[0],group_list->group->center[1]);
		//draw->color(r,g,b)->write(red, group_list->group->pos_top_left[0]+2,group_list->group->pos_top_left[1]+2);
		draw->color(r,g,b)->rec(group_list->group->pos_top_left[0],group_list->group->pos_top_left[1], group_list->group->pos_bottom_right[0], group_list->group->pos_bottom_right[1]);		
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
		cout << "Detect light balls controlers from camera input." << endl;
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
				cout << "Usage : tucmove [OPTION]..." << endl;
				cout << "Capture webcamera and detect light spots LEDs." << endl;
				cout << endl;
				cout << "Optional arguments:" << endl;
				cout << "  -h , --help                  prints this information." << endl;
				cout << "  -d [number]                  camera device number, default is 0." << endl;
				cout << "  -c [string]                  custom camera device name." << endl;
				cout << "  -r [number>0]x[number>0]     set one of the available resolutions." << endl;
				cout << "  -w [on|off]                  window on or off." << endl;
				cout << "  -reflect [on|off]            reverse left right." << endl;
				cout << "  -colordetect [on|off]        detect controller's color." << endl;
				cout << "  -useHUE [on|off]             use HUE for color detection." << endl;
				cout << "  -keephistory [on|off]        relate controller with previous positions." << endl;
				cout << "  -host [hostname]             sent data to this host name." << endl;
				cout << "  -port [number]               port to sent data." << endl;
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
						option_width = (int) strtol(argv[i], NULL, 10);
						option_height = (int) strtol(p+1, NULL, 10);
					}
				}
			}

			// Window
			else if(strcmp("-w", argv[i])==0){
				if(i+1 < argc){
					i++;
					if(strcmp("on", argv[i])==0){
						window_open = true;
					}else if(strcmp("off", argv[i])==0){
						window_open = false;
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

			// Host
			else if(strcmp("-host", argv[i])==0){
				if(i+1 < argc){
					i++;
					strcpy(host, argv[i]);
					network = true;
				}
			}

			// Port
			else if(strcmp("-port", argv[i])==0){
				if(i+1 < argc){
					i++;
					strcpy(port, argv[i]);
				}
			}

			// Print about info
			else if(strcmp("-about", argv[i])==0){
				programInfo();
				exit(EXIT_SUCCESS);
			}

			// Detect Color
			else if(strcmp("-colordetect", argv[i])==0){
				if(i+1 < argc){
					i++;
					if(strcmp("off", argv[i])==0){
						detect_check_color = false;
					} else if(strcmp("on", argv[i])==0){
						detect_check_color = true;
					}
				}
			}

			// Detect Color
			else if(strcmp("-keephistory", argv[i])==0){
				if(i+1 < argc){
					i++;
					if(strcmp("off", argv[i])==0){
						detect_keep_history = false;
					} else if(strcmp("on", argv[i])==0){
						detect_keep_history = true;
					}
				}
			}

			// Use HUE
			else if(strcmp("-useHUE", argv[i])==0){
				if(i+1 < argc){
					i++;
					if(strcmp("off", argv[i])==0){
						detect_base_on_hue = false;
					} else if(strcmp("on", argv[i])==0){
						detect_base_on_hue = true;
					}
				}
			}

		}
	}

	XImage *parseFrame2Image(Display *display, Visual *visual, unsigned char ***frame, int width, int height){
		int i,j;
		unsigned char *image32 = (unsigned char *)malloc(width*height*4);
		unsigned char *p = image32;

		for(i=0; i<height; i++){
			for (j = 0; j < width; j++){
				*p++ = frame[i][j][0];
				*p++ = frame[i][j][1];
				*p++ = frame[i][j][2];
				p++;
			}
		}
		
		return XCreateImage(display, visual, 24, ZPixmap, 0, (char *)image32, width, height, 32, 0);
	}

	XImage *parseFrameObject2Image(Display *display, Visual *visual, tuc::Frame* frame, int type){	
		int i,j;
		unsigned char r, g, b;
		unsigned char *image32 = (unsigned char *)malloc(frame->width*frame->height*4);
		unsigned char *p = image32;

		for(i=0; i<frame->height; i++){
			for (j=0; j<frame->width; j++){
				/*
				if(type==1){
					*p++ = frame->pixels[j][i].binary*255;
					*p++ = frame->pixels[j][i].binary*255;
					*p++ = frame->pixels[j][i].binary*255;
					p++;
				}else if(type==2 || type==11 || type==12 || type==13 || type==14){
					r = 0;
					g = 0;
					b = 0;
					// GREEN
					if(	(type==11 || type==2) &&
						frame->pixels[j][i].g>=frame->green_range.green_min && frame->pixels[j][i].g<frame->green_range.green_max &&
						frame->pixels[j][i].r>=frame->green_range.red_min && frame->pixels[j][i].r<frame->green_range.red_max && 
						frame->pixels[j][i].b>=frame->green_range.blue_min && frame->pixels[j][i].b<frame->green_range.blue_max
					){
						g = 255;
					}

					// RED
					if(	(type==12 || type==2) &&
						frame->pixels[j][i].r>=frame->red_range.red_min && frame->pixels[j][i].r<frame->red_range.red_max &&
						frame->pixels[j][i].g>=frame->red_range.green_min && frame->pixels[j][i].g<frame->red_range.green_max &&
						frame->pixels[j][i].b>=frame->red_range.blue_min && frame->pixels[j][i].b<frame->red_range.blue_max
					){
						r = 255;
					}

					// BLUE
					if( (type==13 || type==2) &&
						frame->pixels[j][i].b>=frame->blue_range.blue_min && frame->pixels[j][i].b<frame->blue_range.blue_max &&
						frame->pixels[j][i].g>=frame->blue_range.green_min && frame->pixels[j][i].g<frame->blue_range.green_max &&
						frame->pixels[j][i].r>=frame->blue_range.red_min && frame->pixels[j][i].r<frame->blue_range.red_max
					){
						b = 255;
					}

					// YELLOW
					if( (type==14 || type==2) &&
						frame->pixels[j][i].r>=frame->yellow_range.red_min && frame->pixels[j][i].r<frame->yellow_range.red_max &&
						frame->pixels[j][i].g>=frame->yellow_range.green_min && frame->pixels[j][i].g<frame->yellow_range.green_max &&
						frame->pixels[j][i].b>=frame->yellow_range.blue_min && frame->pixels[j][i].b<frame->yellow_range.blue_max
					){
						r = 255;
						g = 255;
					}


					*p++ = b;
					*p++ = g;
					*p++ = r;
					p++;
				}else{
					*p++ = 0;
					*p++ = 0;
					*p++ = 0;
					p++;
				}
				*/

				if(type==1){
					*p++ = frame->pixels[j][i].binary*255;
					*p++ = frame->pixels[j][i].binary*255;
					*p++ = frame->pixels[j][i].binary*255;
					p++;
				}else if(type==2 || type==11 || type==12 || type==13 || type==14){
					r = 0;
					g = 0;
					b = 0;
					// GREEN
					if(type==11 || type==2){
						g = frame->pixels[j][i].g;
					}

					// RED
					if(type==12 || type==2){
						r = frame->pixels[j][i].r;
					}

					// BLUE
					if(type==13 || type==2){
						b = frame->pixels[j][i].b;
					}

					// YELLOW
					if(type==14 || type==2){
						r = frame->pixels[j][i].r;
						g = frame->pixels[j][i].g;
					}


					*p++ = b;
					*p++ = g;
					*p++ = r;
					p++;
				}else{
					*p++ = 0;
					*p++ = 0;
					*p++ = 0;
					p++;
				}

			}
		}
		
		return XCreateImage(display, visual, 24, ZPixmap, 0, (char *)image32, frame->width, frame->height, 32, 0);
	}

	XImage *parseFramePoints2Image(Display* display, Visual* visual, tuc::Frame::controler red, tuc::Frame::controler green, tuc::Frame::controler blue, tuc::Frame::controler yellow){	
		int i,j;
		unsigned char r, g, b;
		unsigned char *image32 = (unsigned char *)malloc(cam->frame_width*cam->frame_height*4);
		unsigned char *p = image32;

		for(i=0; i<cam->frame_height; i++){
			for (j=0; j<cam->frame_width; j++){
				if(green.on && (((i-green.center[1])*(i-green.center[1])) + ((j-green.center[0])*(j-green.center[0]))) < (((green.height+green.width)/4)*((green.height+green.width)/4))){
					*p++ = 0;
					*p++ = 255;
					*p++ = 0;
					p++;
				}else if(blue.on && (((i-blue.center[1])*(i-blue.center[1])) + ((j-blue.center[0])*(j-blue.center[0]))) < (((blue.height+blue.width)/4)*((blue.height+blue.width)/4))){
					*p++ = 255;
					*p++ = 0;
					*p++ = 0;
					p++;
				}else if(red.on && (((i-red.center[1])*(i-red.center[1])) + ((j-red.center[0])*(j-red.center[0]))) < (((red.height+red.width)/4)*((red.height+red.width)/4))){
					*p++ = 0;
					*p++ = 0;
					*p++ = 255;
					p++;
				}else if(yellow.on && (((i-yellow.center[1])*(i-yellow.center[1])) + ((j-yellow.center[0])*(j-yellow.center[0]))) < (((yellow.height+yellow.width)/4)*((yellow.height+yellow.width)/4))){
					*p++ = 0;
					*p++ = 255;
					*p++ = 255;
					p++;
				}else{
					*p++ = 0;
					*p++ = 0;
					*p++ = 0;
					p++;
				}
			}
		}
		
		return XCreateImage(display, visual, 24, ZPixmap, 0, (char *)image32, cam->frame_width, cam->frame_height, 32, 0);
	}

	XImage *parseFramePoints2Image_noColors(Display* display, Visual* visual, tuc::Frame::controler group){	
		int i,j;
		unsigned char r, g, b;
		unsigned char *image32 = (unsigned char *)malloc(cam->frame_width*cam->frame_height*4);
		unsigned char *p = image32;

		if(group.on){
			for(i=0; i<cam->frame_height; i++){
				for (j=0; j<cam->frame_width; j++){
					if((((i-group.center[1])*(i-group.center[1])) + ((j-group.center[0])*(j-group.center[0]))) < (((group.height+group.width)/4)*((group.height+group.width)/4))){
						*p++ = 255;
						*p++ = 255;
						*p++ = 255;
						p++;
					}else{
						*p++ = 0;
						*p++ = 0;
						*p++ = 0;
						p++;
					}
				}
			}
		}else{
			for(i=0; i<cam->frame_height; i++){
				for (j=0; j<cam->frame_width; j++){
					*p++ = 0;
					*p++ = 0;
					*p++ = 0;
					p++;
				}
			}
		}
		
		return XCreateImage(display, visual, 24, ZPixmap, 0, (char *)image32, cam->frame_width, cam->frame_height, 32, 0);
	}
