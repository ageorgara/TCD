/*
 *	Technical University of Crete
 *	2014
 *
 */

// Opencv Libraries
	// #include "cv.h"

// Library
	namespace tuc{
		class Frame{
			public:
				// Save dimentions
				int height;
				int width;

				// Reflected
				bool reflect;
				
				// Constructor
				Frame(unsigned char *** frame, bool, int, int);
				// Initiate Object
				Frame* init(unsigned char ***, int, int);
				// Parse Image Object
				Frame* parse(unsigned char ***);
				
				// Pixel define
				typedef struct{
					unsigned char r;
					unsigned char g;
					unsigned char b;
					unsigned char grey;
					bool binary;
				} pixel;
				pixel** pixels;
				pixel* getPixel(int y, int x);
				pixel* setPixel(int y, int x, Frame::pixel value);
				
				// Labeling
				int label_index;
				typedef struct{
					int index;
					int length;
					int perimeter;
					int height;
					int width;
					int pos_top_left[2];
					int pos_top_right[2];
					int pos_bottom_left[2];
					int pos_bottom_right[2];
					int center[2];
				} label_group;
				typedef struct{
					int index;
					label_group* group;
				} label;
				label** groups;
				struct label_list_item{
					label_group* group;
					label_list_item* next;
				};
				typedef struct label_list_item label_list;
				label_list* label_head;

				typedef struct
				{
					int center[2];
					int frames;
					int last_frame;
					int height;
					int width;
					bool on;
				}controler;



				void labels();
				void findLabel(int x, int y);

				//validate Roundness
					//variables
						// Max Found Roudness
						double max_roundness;
						// Max valid diameter
						double max_valid_diameter;
					//method
					double validateRoundness(label_group* group);
					int validateSolidCenter(label_group* group);
					int validColor(label_group* group);
					int validColorHue(label_group* group);
					int checkHistory(Frame::label_group* group, Frame::controler red, Frame::controler blue, Frame::controler green, Frame::controler yellow);
				//Configuration
					typedef struct{
						unsigned char red_max;
						unsigned char red_min;
						unsigned char green_max;
						unsigned char green_min;
						unsigned char blue_max;
						unsigned char blue_min;
						int hue_min;
						int hue_max;
					}color_range;

					typedef struct{
						int x;
						int y;
					}coordinates;

					//variables
						//Threshold used in binary image
						int threshold;
						double hue_saturation_threshold;
						//Maximum valid diameter divisor
						int max_diameter_divisor;
						//Tollerance used in solid center
						unsigned char tol;
						//Solid Center offset coordinates
						coordinates solid_offset;
						//Solid Center width divisor
						int solid_width_divisor;
						//Solid Center height divisor
						int solid_height_divisor;
						//Range for each range
						color_range red_range;
					    color_range green_range;
					    color_range yellow_range;
					    color_range blue_range;
						//History offset coordinates divisor
						coordinates history_offset;
					//method
						void configure();
		};
	}
