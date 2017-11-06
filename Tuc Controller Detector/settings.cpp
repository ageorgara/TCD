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
 *	This is a program that reads the controls of a camera
 *	connected and show the available controls on the screen
 *	letting the user to edit them.
 *
 *	Compile using GNU compiler
 *		Command:
 *			g++ settings.cpp libs/*.cpp -o settings -lX11
 *		Example:
 *			./settings
 */

// C/C++ Libraries
	// For General Purpose
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <iostream>
	// Display Library
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	#include <X11/Xutil.h>
	#include <linux/videodev2.h>

// Load Namespaces
	// Stdlib namespace
	using namespace std;

// C++ Custom Libraries
	#include "libs/webcam.h"
	#include "libs/drawlib.h"

// Functions
 	void parseArguments(int argc, const char* argv[]);
 	void store_supported_controls();
 	void settings_window();
 	void editLayout();
 	void checkControl(__u32 id, const char* name, int type);
 	XImage *parseFrame2Image(Display *, Visual *);

// Variables
	// WebCam Variable
		tuc::WebCam* cam;
	// Options
		bool print = false;
		bool default_device = true;
		char device_name[256] = "/dev/video";
		bool ignore_errors = false;
	// Loop Code
		bool loop = true;
	// Size
		int height = 0;
		int width = 0;
	// Frame Buffer
		unsigned char ***frame;
	// Control Struct
	 	struct control_struct{
	 		__u32 id;
			char name[255];
			char type;
			int max;
			int min;
			int def;
			int value;
			struct control_struct* next;
			struct control_struct* previus;
		};
	// Controls List Variables
		struct control_struct* list;
		struct control_struct* active;
		struct control_struct* last;
	// Number of Controls
		int control_length = 0;

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
			// Load Supported Controls
			store_supported_controls();
			// Create Window
			settings_window();
		}
	}

	// Dsiplay Settings Window
	void settings_window(){
		// Window frame
		int x, y;
		frame = new unsigned char **[height];

		for(y = 0; y < height; y++){
			frame[y] = new unsigned char *[width];
			for(x = 0; x < width; x++){
				frame[y][x] = new unsigned char[3];
				// Pixel
				frame[y][x][2] = 20; // blue
				frame[y][x][1] = 20; // green
				frame[y][x][0] = 20; // red
			}
		}

		// Create Window
		XImage *ximage;
		Display *display = XOpenDisplay(NULL);
		Visual *visual = DefaultVisual(display, 0);
		Window window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, width, height, 1, 0, 0x00141414);
		XStoreName(display, window, "Camera Settings");

		XSizeHints *window_resize_rule = XAllocSizeHints();
		window_resize_rule->flags = PMinSize | PMaxSize;
		window_resize_rule->min_width = window_resize_rule->max_width = width;
		window_resize_rule->min_height = window_resize_rule->max_height = height;
		XSetWMNormalHints(display, window, window_resize_rule);

		XMapWindow(display, window);

		// Window Events
		XEvent event;
		Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(display, window, &wmDelete, 1);
		XSelectInput(display, window, KeyPressMask|KeyReleaseMask);

		// Key Events
		KeyCode key_left = XKeysymToKeycode(display, XStringToKeysym("Left"));
		KeyCode key_right = XKeysymToKeycode(display, XStringToKeysym("Right"));
		KeyCode key_up = XKeysymToKeycode(display, XStringToKeysym("Up"));
		KeyCode key_down = XKeysymToKeycode(display, XStringToKeysym("Down"));
		KeyCode key_q = XKeysymToKeycode(display, XStringToKeysym("Q"));
		KeyCode key_r = XKeysymToKeycode(display, XStringToKeysym("R"));

		// Main Loop
		loop = true;
		while(loop){
			// Edit Layout
			editLayout();
			// Create image
			ximage = parseFrame2Image(display, visual);
			// Insert Image
			XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, width, height);
			XFlush(display);
			// Free Up Memory
			ximage->f.destroy_image(ximage);

			// Wait Event
			XNextEvent(display, &event);
			switch(event.type){
					// On button push
					case(KeyRelease):
						// Decrease Selected
						if(event.xkey.keycode == key_left){
							if(active->type == 1){
								if(active->value-1 >= active->min){
									active->value--;
									if(cam->changeControl(active->id, true, active->value) < 0){
										active->value++;
									}
								}
							}
							else if(active->type == 2){
								if(active->value == 0){
									active->value = 1;
									if(cam->changeControl(active->id, true, active->value) < 0){
										active->value = 0;
									}
								}
							}
						}
						// Increase Selected
						else if(event.xkey.keycode == key_right){
							if(active->type == 1){
								if(active->value+1 <= active->max){
									active->value++;
									if(cam->changeControl(active->id, true, active->value) < 0){
										active->value--;
									};
								}
							}
							else if(active->type == 2){
								if(active->value == 1){
									active->value = 0;
									if(cam->changeControl(active->id, true, active->value) < 0){
										active->value = 1;
									}
								}
							}
						}
						// Reset Selected
						else if(event.xkey.keycode == key_r){
							if(cam->changeControl(active->id, true, active->def) > 0){
								active->value = active->def;
							}
						}
						// Go to next
						else if(event.xkey.keycode == key_down){
							active = active->next;
						}
						// Go to previus
						else if(event.xkey.keycode == key_up){
							active = active->previus;
						}
						// Exit
						else if(event.xkey.keycode == key_q){
							loop = false;
						}
						break;

				case(ClientMessage):
					// On close window
					if(event.xclient.data.l[0] == wmDelete){
						loop = false;
					}
					break;
			}
			XFlush(display);

		}
		
		// Report Exit
		cout << "Closing program..." << endl;

		// Close Window
		XDestroyWindow(display, window);
		XCloseDisplay(display);

		// Close Camera
		cam->close();
	}

	// Parse Program Arguments
	void parseArguments(int argc, const char* argv[]){
		// Char pointer
		const char * p;

		// Check arguments
		for(int i=1; i<argc; ++i){

			// Print Help
			if(strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0){
				cout << "Usage : settings [OPTION]..." << endl;
				cout << "Change camera's settings." << endl;
				cout << endl;
				cout << "Optional arguments:" << endl;
				cout << "  -h , --help                  prints this information." << endl;
				cout << "  -d [number]                  camera device number, default is 0." << endl;
				cout << "  -c [string]                  custom camera device name." << endl;
				cout << "  -p                           print device info." << endl;
				cout << "  -i                           ignore errors." << endl;
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

			// Print screen resolutions
			else if(strcmp("-p", argv[i])==0){
				print = true;
			}

			// Ignore errors some errors
			else if(strcmp("-i", argv[i])==0){
				ignore_errors = true;
			}
		}
	}

	void store_supported_controls(){
		// Control
		list = NULL;
		control_length = 0;

		// Types [1:int, 2:bool]
		checkControl(V4L2_CID_BRIGHTNESS, "Brightness", 1);
		checkControl(V4L2_CID_CONTRAST, "Contrast", 1);
		checkControl(V4L2_CID_SATURATION, "Saturation", 1);
		checkControl(V4L2_CID_BLACK_LEVEL, "Black Level", 1);
		checkControl(V4L2_CID_AUTO_WHITE_BALANCE, "Auto White Balance", 2);
		checkControl(V4L2_CID_WHITENESS, "Whiteness", 1);
		checkControl(V4L2_CID_WHITE_BALANCE_TEMPERATURE, "White Balance Temperature", 1);
		checkControl(V4L2_CID_RED_BALANCE, "Red Balance", 1);
		checkControl(V4L2_CID_BLUE_BALANCE, "Blue Balance", 1);
		checkControl(V4L2_CID_AUTOGAIN, "Auto Gain", 2);
		checkControl(V4L2_CID_GAIN, "Gain", 1);
		checkControl(V4L2_CID_HUE_AUTO, "Auto Hue", 2);
		checkControl(V4L2_CID_HUE, "Hue", 1);
		checkControl(V4L2_CID_SHARPNESS, "Sharpness", 1);
		checkControl(V4L2_CID_GAMMA, "Gamma", 1);
		checkControl(V4L2_CID_EXPOSURE, "Exposure", 1);
		checkControl(V4L2_CID_BACKLIGHT_COMPENSATION, "Backlight Compensation", 1);
		checkControl(V4L2_CID_HFLIP, "Horizontal Flip", 2);
		checkControl(V4L2_CID_VFLIP, "Vertical Flip", 2);
		
		list->previus = last;
		active = list;

		height = control_length*30+1+80;
		width = 280;
	}

	void checkControl(__u32 id, const char* name, int type){
		// Define control variable
		struct control_struct* ctrl;
		// Try to get control data
		struct v4l2_queryctrl* control = cam->isControlSupported(id);
		// If control enabled
		if(control != NULL){
			// Create object
			ctrl = new struct control_struct;
			// If list is empty start list
			if(list == NULL){
				list = ctrl;
				last = ctrl;
				ctrl->previus = ctrl;
			}else{
				last->next = ctrl;
				ctrl->previus = last;
				last = ctrl;
			}
			// Next in the list is top of the list
			ctrl->next = list;

			// Save name
			strcpy(ctrl->name, name);
			// Save type
			ctrl->type = type;
			// Save values
			ctrl->id = id;
			ctrl->max = control->maximum;
			ctrl->min = control->minimum;
			ctrl->def = control->default_value;

			if(cam->loadControl(id, &ctrl->value) == -1){
				ctrl->value = control->default_value;
			}

			//cout << ctrl->name << ' ' << ctrl->max << ' ' << ctrl->min << ' ' << ctrl->value << '\n';

			control_length++;
		}
	}

	void editLayout(){
		// Draw lib
		tuc::Draw* draw = new tuc::Draw(frame, width, height);

		// Control Pointer
		struct control_struct* p = list;

		// Integer Variables
		int i, l, m;
		int bar_width;
		int len = 0;

		// String Variables
		char str_value[31];

		// Color Variables
		unsigned char value, color_true, color_false;

		// Layout Words
		char true_str[] = "True";
		char false_str[] = "False";

		// Clear Frame
		for(l=0; l<width; ++l){
			for(m=0; m<height; ++m){
				frame[m][l][2] = 20;
				frame[m][l][1] = 20;
				frame[m][l][0] = 20;
			}	
		}

		// For Each Control
		for(i=0; i<control_length; ++i){
			// Set Colors
			if(p == active){
				value = 255;
			}else{
				value = 100;
			}

			// Draw Up line
			for (l=2; l<=width-4; ++l){
				frame[i*30+2][l][2] = value;
				frame[i*30+2][l][1] = value;
				frame[i*30+2][l][0] = value;
			}
			// Draw Down line
			for (l=2; l<=width-4; ++l){
				frame[i*30+2+26][l][2] = value;
				frame[i*30+2+26][l][1] = value;
				frame[i*30+2+26][l][0] = value;
			}
			// Draw Left line
			for (l=2; l<=30-2; ++l){
				frame[i*30+l][2][2] = value;
				frame[i*30+l][2][1] = value;
				frame[i*30+l][2][0] = value;
			}
			// Draw Right line
			for (l=2; l<=30-2; ++l){
				frame[i*30+l][width-3][2] = value;
				frame[i*30+l][width-3][1] = value;
				frame[i*30+l][width-3][0] = value;
			}

			// Draw Text
			draw->color(value, value, value)->write(p->name, 5, i*30+5);

			// If integer
			if(p->type == 1){
				// Draw a bar
					// Draw Up line
					for (l=5; l<=200+5+2; l++){
						frame[i*30+2+15][l][2] = value;
						frame[i*30+2+15][l][1] = value;
						frame[i*30+2+15][l][0] = value;
					}
					// Draw Down line
					for (l=5; l<=200+5+2; l++){
						frame[i*30+2+22][l][2] = value;
						frame[i*30+2+22][l][1] = value;
						frame[i*30+2+22][l][0] = value;
					}
					// Draw Left line
					for (l=2+15; l<=2+22; l++){
						frame[i*30+l][5][2] = value;
						frame[i*30+l][5][1] = value;
						frame[i*30+l][5][0] = value;
					}
					// Draw Right line
					for (l=2+15; l<=2+22; l++){
						frame[i*30+l][200+5+2][2] = value;
						frame[i*30+l][200+5+2][1] = value;
						frame[i*30+l][200+5+2][0] = value;
					}
				// Draw Bar Content
					bar_width = ((p->value - p->min +1)*200)/(p->max - p->min +1);

					for(l=6; l<=6+bar_width; l++){
						for(m=i*30+2+15+1; m<=i*30+2+22-1; m++){
							frame[m][l][2] = value-60;
							frame[m][l][1] = value-60;
							frame[m][l][0] = value-60;
						}
					}

				// Show Value
					sprintf(str_value, "%d", p->value);
					len = strlen(str_value);
					draw->color(value-60, value-60, value-60)->write(str_value, width-8-(len*6), i*30+15);
			}
			// Else if bool
			else if(p->type == 2){
				// If active False
				if(p->value == 0){
					color_false = value;
					color_true = 40;
				}
				// If active True
				else{
					color_true = value;
					color_false = 40;
				}

				// Show True / False selection
				draw->color(color_true, color_true, color_true)->write(true_str, 10, i*30+17);
				draw->color(color_false, color_false, color_false)->write(false_str, 50, i*30+17);
			}

			// Load next control
			p = p->next;
		}

		char str[255];
		
		// Show hot keys
		strcpy(str, "Q : Exit program");
		draw->color(150, 150, 150)->write(str, 10, i*30+6);
		strcpy(str, "R : Reset active control");
		draw->color(150, 150, 150)->write(str, 10, i*30+16);

		// Show copyright
		strcpy(str, "Copyright (c) 2015-2016");
		draw->color(150, 150, 150)->write(str, 10, i*30+35);
		strcpy(str, "Technical University of Crete");
		draw->color(150, 150, 150)->write(str, 10, i*30+45);
		strcpy(str, "- Georgara Athina");
		draw->color(150, 150, 150)->write(str, 10, i*30+55);
		strcpy(str, "- Grammatopoulos Athanasios Vasileios");
		draw->color(150, 150, 150)->write(str, 10, i*30+65);
	}

	XImage *parseFrame2Image(Display *display, Visual *visual){
		// Variables
		int x, y;
		// Image
		unsigned char *image32 = (unsigned char *)malloc(width*height*4);
		unsigned char *p = image32;
		// Parse Frame
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				// 1st pixel
				*p++ = frame[y][x][2]; // blue
				*p++ = frame[y][x][1]; // green
				*p++ = frame[y][x][0]; // red
				p++;
			}
		}
		// Create Image
		return XCreateImage(display, visual, 24, ZPixmap, 0, (char *)image32, width, height, 32, 0);
	}
