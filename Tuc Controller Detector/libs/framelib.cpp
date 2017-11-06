/*
 *	Technical University of Crete
 *	2014
 *
 */

 // framelib.cpp

#include "framelib.h"
#include <math.h>
#include <fstream>
#include <string.h>
 
#define min_f(a, b, c)  (fminf(a, fminf(b, c)))
#define max_f(a, b, c)  (fmaxf(a, fmaxf(b, c)))
using namespace std;

namespace tuc{
	Frame::Frame(unsigned char *** frame, bool reflect, int width, int height){
		// Set frame orientation
		this->reflect = reflect;
		// Initiate frame
		this->init(frame, width, height);
	}
	Frame* Frame::init(unsigned char *** frame, int width, int height){
		//Read from configuration file
		this->configure();
		// Save dimensions
		this->height = height;
		this->width = width;

		// Init index
		label_index = 0;
		label_head = NULL;

		// Allocate Pixel memory
		this->pixels = new Frame::pixel*[this->width];
		this->groups = new Frame::label*[this->width];
		for(int x=0; x<this->width; ++x){
			this->pixels[x] = new Frame::pixel[this->height];
			this->groups[x] = new Frame::label[this->height];
		}

		// Return object
		return this;
	}

	Frame* Frame::parse(unsigned char *** frame){
		// Define pixel
		unsigned char *rgb;
		unsigned char *rgb2;
		
		int _x = 0;
		// Parse Pixels info
		// Faster
		for(int y=0; y<this->height; y++){
			for(int x=0; x<this->width; x++){
				rgb = frame[y][x];
				// Reverse image
				_x = (this->reflect)? this->width-1-x : x;
				// Red
				this->pixels[_x][y].r = rgb[2];
				// Green
				this->pixels[_x][y].g = rgb[1];
				// Blue
				this->pixels[_x][y].b = rgb[0];
				// Grey
				this->pixels[_x][y].grey = (unsigned char) (0.2126 * this->pixels[_x][y].r + 0.7152 * this->pixels[_x][y].g + 0.0722 * this->pixels[_x][y].b);
				// Binary (binary = grayscale > 0.961*255)  // 245
				this->pixels[_x][y].binary = ( this->pixels[_x][y].grey <= this->threshold ) ? 0 : 1;
				// init groups
				this->groups[_x][y].index = 0;
				this->groups[_x][y].group = NULL;

				if(this->reflect && x+1>this->width/2){
					rgb2 = frame[y][_x];
					rgb[0] = rgb2[0];
					rgb[1] = rgb2[1];
					rgb[2] = rgb2[2];
					rgb2[0] = this->pixels[_x][y].b;
					rgb2[1] = this->pixels[_x][y].g;
					rgb2[2] = this->pixels[_x][y].r;
				}
			}
		}

		return this;
	}

	void Frame::labels(){
		for(int y=0; y<this->height; y++){
			for(int x=0; x<this->width; x++){
				// Grey
				if(this->groups[x][y].index==0){
					this->label_index ++;

					this->groups[x][y].group = new Frame::label_group;

					Frame::label_list* group = new Frame::label_list;
					group->group = this->groups[x][y].group;
					group->next = this->label_head;
					this->label_head = group;

					this->groups[x][y].group->index = label_index;
					this->groups[x][y].group->length = 0;
					this->groups[x][y].group->perimeter = 0;

					this->groups[x][y].group->pos_top_left[0] = this->width-1;
					this->groups[x][y].group->pos_top_left[1] = this->height-1;
					this->groups[x][y].group->pos_top_right[0] = 0;
					this->groups[x][y].group->pos_top_right[1] = this->height-1;
					this->groups[x][y].group->pos_bottom_left[0] = this->width-1;
					this->groups[x][y].group->pos_bottom_left[1] = 0;
					this->groups[x][y].group->pos_bottom_right[0] = 0;
					this->groups[x][y].group->pos_bottom_right[1] = 0;
					
					this->groups[x][y].group->center[0] = 0;
					this->groups[x][y].group->center[1] = 0;

					this->findLabel(x, y);

					this->groups[x][y].group->center[0] /= this->groups[x][y].group->length;
					this->groups[x][y].group->center[1] /= this->groups[x][y].group->length;

					this->groups[x][y].group->height = this->groups[x][y].group->pos_bottom_right[1] - this->groups[x][y].group->pos_top_left[1] + 1;
					this->groups[x][y].group->width = this->groups[x][y].group->pos_bottom_right[0] - this->groups[x][y].group->pos_top_left[0] + 1;
				}
				
			}
		}
		//Max roundness
		this->max_roundness = 0;
		// Max object to be found
		this->max_valid_diameter = std::min(this->height, this->width)/this->max_diameter_divisor;
	}

	void Frame::findLabel(int x, int y){
		// Set pixel on the group
		this->groups[x][y].index = this->label_index;
		this->groups[x][y].group->length++;

		this->groups[x][y].group->center[0] += x;
		this->groups[x][y].group->center[1] += y;

		if(
			(x > 0 && this->pixels[x-1][y].binary != this->pixels[x][y].binary) ||
			(y > 0 && this->pixels[x][y-1].binary != this->pixels[x][y].binary) ||
			(x < this->width-1 && this->pixels[x+1][y].binary != this->pixels[x][y].binary) ||
			(y < this->height-1 && this->pixels[x][y+1].binary != this->pixels[x][y].binary)
		){
			this->groups[x][y].group->perimeter++;
		}
		if(x < this->groups[x][y].group->pos_top_left[0]){
			this->groups[x][y].group->pos_top_left[0] = x;
		}
		if(y < this->groups[x][y].group->pos_top_left[1]){
			this->groups[x][y].group->pos_top_left[1] = y;
		}
		if(x > this->groups[x][y].group->pos_top_right[0]){
			this->groups[x][y].group->pos_top_right[0] = x;
		}
		if(y < this->groups[x][y].group->pos_top_right[1]){
			this->groups[x][y].group->pos_top_right[1] = y;
		}
		if(x < this->groups[x][y].group->pos_bottom_left[0]){
			this->groups[x][y].group->pos_bottom_left[0] = x;
		}
		if(y > this->groups[x][y].group->pos_bottom_left[1]){
			this->groups[x][y].group->pos_bottom_left[1] = y;
		}
		if(x > this->groups[x][y].group->pos_bottom_right[0]){
			this->groups[x][y].group->pos_bottom_right[0] = x;
		}
		if(y > this->groups[x][y].group->pos_bottom_right[1]){
			this->groups[x][y].group->pos_bottom_right[1] = y;
		}
		if(x > 0 && this->groups[x-1][y].index == 0 && this->pixels[x-1][y].binary == this->pixels[x][y].binary){
			this->groups[x-1][y].group = this->groups[x][y].group;
			this->findLabel(x-1, y);
		}
		if(y > 0 && this->groups[x][y-1].index == 0 && this->pixels[x][y-1].binary == this->pixels[x][y].binary){
			this->groups[x][y-1].group = this->groups[x][y].group;
			this->findLabel(x, y-1);
		}
		if(x < this->width-1 && this->groups[x+1][y].index == 0 && this->pixels[x+1][y].binary == this->pixels[x][y].binary){
			this->groups[x+1][y].group = this->groups[x][y].group;
			this->findLabel(x+1, y);
		}
		if(y < this->height-1 && this->groups[x][y+1].index == 0 && this->pixels[x][y+1].binary == this->pixels[x][y].binary){
			this->groups[x][y+1].group = this->groups[x][y].group;
			this->findLabel(x, y+1);
		}

	}

	
	Frame::pixel* Frame::getPixel(int y, int x){
		if(y < this->height && x < this->width){
			return &(this->pixels[x][y]);
		} else {
			Frame::pixel *retval = new Frame::pixel;
			retval->r=0;
			retval->g=0;
			retval->b=0;
			retval->grey=0;
			retval->binary=0;
			return retval;
		}
	}

	Frame::pixel* Frame::setPixel(int y, int x, Frame::pixel value){
		if(y >= 0 && y < this->height && x >= 0 && x < this->width){
			this->pixels[x][y] = value;
			return &(this->pixels[x][y]);
		} else {
			return NULL;
		}
	}

	double Frame::validateRoundness(Frame::label_group* group){
		// Roundness
		double roundness = 0;

		if(
			group->height > 4 &&
			group->width > 4 &&
			this->max_valid_diameter > std::max(group->height, group->width) &&
			std::max(group->height/group->width, group->width/group->height)<2
		){

			roundness = (4 * 3.14 * group->length)/(group->perimeter*group->perimeter);
			if(roundness > this->max_roundness)
				this->max_roundness = roundness;
		}

		return roundness;
	}

	int Frame::validateSolidCenter(Frame::label_group* group){
		if(group->height<this->height/64 || group->width<this->width/64){
			return 0;
		}

		// Data
		int left = group->pos_top_left[0] + group->width/this->solid_offset.x;
		int top = group->pos_top_left[1] + group->height/this->solid_offset.y;
		int width = group->width/this->solid_width_divisor;
		int height = group->height/this->solid_height_divisor;


		unsigned char r_up = (this->pixels[left][top].r+tol<255)?this->pixels[left][top].r+tol:255;
		unsigned char r_down = (this->pixels[left][top].r-tol>0)?this->pixels[left][top].r-tol:0;
		unsigned char g_up = (this->pixels[left][top].g+tol<255)?this->pixels[left][top].g+tol:255;
		unsigned char g_down = (this->pixels[left][top].g-tol>0)?this->pixels[left][top].g-tol:0;
		unsigned char b_up = (this->pixels[left][top].b+tol<255)?this->pixels[left][top].b+tol:255;
		unsigned char b_down = (this->pixels[left][top].b-tol>0)?this->pixels[left][top].b-tol:0;

		for (int i=0; i<width; ++i){
			for (int j=0; j<height; ++j){
				if(
					this->pixels[left+i][top+j].r > r_up ||
					this->pixels[left+i][top+j].r < r_down ||
					this->pixels[left+i][top+j].g > g_up ||
					this->pixels[left+i][top+j].g < g_down ||
					this->pixels[left+i][top+j].b > b_up ||
					this->pixels[left+i][top+j].b < b_down
				)
					return 0;
			}
		}

		return 1;
	}


	int Frame::validColor(Frame::label_group* group){
		int blue = 0;
		int red = 0;
		int green = 0;
		int yellow = 0;

		int offset = group->height/4;

		for (int x = group->pos_top_left[0]-offset; x<group->pos_bottom_right[0]+offset; ++x){
			for (int y = group->pos_top_left[1]-offset; y<group->pos_bottom_right[1]+offset; ++y){
				if(x>=0 && x<this->width && y>=0 && y<this->height){
					// YELLOW
					if(
						this->pixels[x][y].r>=yellow_range.red_min && this->pixels[x][y].r<yellow_range.red_max &&
						this->pixels[x][y].g>=yellow_range.green_min && this->pixels[x][y].g<yellow_range.green_max &&
						this->pixels[x][y].b>=yellow_range.blue_min && this->pixels[x][y].b<yellow_range.blue_max
					){
						yellow++;
					}

					// GREEN
					if(
						this->pixels[x][y].g>=green_range.green_min && this->pixels[x][y].g<green_range.green_max &&
						this->pixels[x][y].r>=green_range.red_min && this->pixels[x][y].r<green_range.red_max && 
						this->pixels[x][y].b>=green_range.blue_min && this->pixels[x][y].b<green_range.blue_max
					){
						green++;
					}

					// RED
					if(
						this->pixels[x][y].r>=red_range.red_min && this->pixels[x][y].r<red_range.red_max &&
						this->pixels[x][y].g>=red_range.green_min && this->pixels[x][y].g<red_range.green_max &&
						this->pixels[x][y].b>=red_range.blue_min && this->pixels[x][y].b<red_range.blue_max
					){
						red++;
					}

					// BLUE
					if(
						this->pixels[x][y].b>=blue_range.blue_min && this->pixels[x][y].b<blue_range.blue_max &&
						this->pixels[x][y].g>=blue_range.green_min && this->pixels[x][y].g<blue_range.green_max &&
						this->pixels[x][y].r>=blue_range.red_min && this->pixels[x][y].r<blue_range.red_max
					){
						blue++;
					}
				}
			}
		}

		// 'RED'
		if(red>blue && red>green && red>yellow){
			return 1;
		}
		// 'BLUE'
		else if(blue>red && blue>green && blue>yellow){
			return 2;
		}
		// 'GREEN'
		else if(green>red && green>blue && green>yellow){
			return 3;
		}
		// 'YELLOW'
		else if(yellow>red && yellow>blue && yellow>green){
			return 4;
		}
		// NO COLOR
		else{
			return 0;
		}
	}

	int Frame::validColorHue(Frame::label_group* group){
		int blue = 0;
		int red = 0;
		int green = 0;
		int yellow = 0;

		int offset = group->height/4;

		double min, max, delta;
		double h, s, v;

		double h_sum = 0;
		int len = 0;
		for (int x = group->pos_top_left[0]-offset; x<group->pos_bottom_right[0]+offset; ++x){
			for (int y = group->pos_top_left[1]-offset; y<group->pos_bottom_right[1]+offset; ++y){
				if(x>=0 && x<this->width && y>=0 && y<this->height){

					min = this->pixels[x][y].r < this->pixels[x][y].g ? this->pixels[x][y].r : this->pixels[x][y].g;
					min = min  < this->pixels[x][y].b ? min  : this->pixels[x][y].b;

					max = this->pixels[x][y].r > this->pixels[x][y].g ? this->pixels[x][y].r : this->pixels[x][y].g;
					max = max  > this->pixels[x][y].b ? max  : this->pixels[x][y].b;

					v = max;// v
					delta = max - min;
					if (delta < 0.00001){
						s = 0;
						h = 0; // undefined, maybe nan?
					}
					else{
						if( max < 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
							// if max is 0, then r = g = b = 0              
								// s = 0, v is undefined
							s = 0.0;
							h = NAN;// its now undefined
						} else {
							s = (delta / max);// s
							if( this->pixels[x][y].r >= max )// > is bogus, just keeps compilor happy
								h = ( this->pixels[x][y].g - this->pixels[x][y].b ) / delta;// between yellow & magenta
							else if( this->pixels[x][y].g >= max )
								h = 2.0 + ( this->pixels[x][y].b - this->pixels[x][y].r ) / delta;  // between cyan & yellow
							else
								h = 4.0 + ( this->pixels[x][y].r - this->pixels[x][y].g ) / delta;  // between magenta & cyan

							h *= 60.0;// degrees

							if(h < 0.0) h += 360.0;
						}
					}

					if(s > this->hue_saturation_threshold){
						len++;
						h_sum += h;
					}

				}
			}
		}

		if(len == 0){
			//printf("NaN\n");
			return 0;
		}
		else{
			//printf("%f\n", h_sum/len);
			h_sum /= len;
		}

		// BLUE
		if(h_sum>this->blue_range.hue_min && h_sum<this->blue_range.hue_max) return 2;
		// RED
		else if(h_sum<this->red_range.hue_min || h_sum>this->red_range.hue_max) return 1;
		// GREEN
		else if(h_sum>this->green_range.hue_min && h_sum<this->green_range.hue_max) return 3;
		// YELLOW
		else if(h_sum>this->yellow_range.hue_min && h_sum<this->yellow_range.hue_max) return 4;
		else return 0;
	}

	int Frame::checkHistory(Frame::label_group* group, Frame::controler red, Frame::controler blue, Frame::controler green, Frame::controler yellow){
		int offset_y = this->height/this->history_offset.y;
		int offset_x = this->width/this->history_offset.x;

		int y_up = group->center[1]+offset_y;
		int y_down = group->center[1]-offset_y;
		int x_up = group->center[0]+offset_x;
		int x_down = group->center[0]-offset_x;

		if(red.on){
			
			if(
				x_up>=red.center[0] && red.center[0]>=x_down &&
				y_up>=red.center[1] && red.center[1]>=y_down
			){
				return 1;
			}
		}
		if(blue.on){
			
			if(
				x_up>=blue.center[0] && blue.center[0]>=x_down &&
				y_up>=blue.center[1] && blue.center[1]>=y_down
			){
				return 2;
			}
		}
		if(green.on){
			
			if(
				x_up>=green.center[0] && green.center[0]>=x_down &&
				y_up>=green.center[1] && green.center[1]>=y_down
			){
				return 3;
			}
		}
		if(yellow.on){
			
			if(
				x_up>=yellow.center[0] && yellow.center[0]>=x_down &&
				y_up>=yellow.center[1] && yellow.center[1]>=y_down
			){
				return 4;
			}
		}
		return 0;
	}
	void Frame::configure(){
		//Open File
		ifstream configurationFile;
		configurationFile.open("config.txt");

		string line = "";

		if(configurationFile.good()){
		//Reading from configuration file
			//parse : threshold
				getline(configurationFile,line);
				sscanf(line.c_str(),"threshold %d",&this->threshold);
			//parse : max_valid_diameter divisor
				getline(configurationFile,line);
				sscanf(line.c_str(),"max_valid_diameter %d",&this->max_diameter_divisor);
			
			//validateSolidCenter : diff
				getline(configurationFile,line);
				sscanf(line.c_str(),"solid_center_diff %d", (int*)&this->tol);
			//validateSolidCenter : left
				getline(configurationFile,line);
				sscanf(line.c_str(),"solid_center_offset_left %d",&this->solid_offset.x);
			//validateSolidCenter : top
				getline(configurationFile,line);
				sscanf(line.c_str(),"solid_center_offset_top %d",&this->solid_offset.y);
			//validateSolidCenter : width divisor
				getline(configurationFile,line);
				sscanf(line.c_str(),"solid_center_width %d",&this->solid_width_divisor);
			//validateSolidCenter : height divisor
				getline(configurationFile,line);
				sscanf(line.c_str(),"solid_center_height %d",&this->solid_height_divisor);
			
			// HUE saturation threshold
				getline(configurationFile,line);
				sscanf(line.c_str(),"valid_color_hue_saturation_threshold %lf", &this->hue_saturation_threshold);

			//validColor : color RGB range and HUE range
				//yellow
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_yellow %d %d %d %d %d %d", 
						(int*)&this->yellow_range.red_min, 
						(int*)&this->yellow_range.red_max, 
						(int*)&this->yellow_range.green_min, 
						(int*)&this->yellow_range.green_max, 
						(int*)&this->yellow_range.blue_min, 
						(int*)&this->yellow_range.blue_max
					);
				// yellow hue
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_yellow_hue %d %d", 
						&this->yellow_range.hue_min, 
						&this->yellow_range.hue_max
					);

				//green
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_green %d %d %d %d %d %d", 
						(int*)&this->green_range.red_min, 
						(int*)&this->green_range.red_max, 
						(int*)&this->green_range.green_min, 
						(int*)&this->green_range.green_max, 
						(int*)&this->green_range.blue_min, 
						(int*)&this->green_range.blue_max
					);
				// green hue
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_green_hue %d %d", 
						&this->green_range.hue_min, 
						&this->green_range.hue_max
					);

				//red
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_red %d %d %d %d %d %d", 
						(int*)&this->red_range.red_min, 
						(int*)&this->red_range.red_max, 
						(int*)&this->red_range.green_min, 
						(int*)&this->red_range.green_max, 
						(int*)&this->red_range.blue_min, 
						(int*)&this->red_range.blue_max
					);
				// red hue
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_red_hue %d %d", 
						&this->red_range.hue_min, 
						&this->red_range.hue_max
					);

				//blue
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_blue %d %d %d %d %d %d", 
						(int*)&this->blue_range.red_min, 
						(int*)&this->blue_range.red_max, 
						(int*)&this->blue_range.green_min, 
						(int*)&this->blue_range.green_max, 
						(int*)&this->blue_range.blue_min, 
						(int*)&this->blue_range.blue_max
					);
				// blue hue
					getline(configurationFile,line);
					sscanf(line.c_str(),"valid_color_range_blue_hue %d %d", 
						&this->blue_range.hue_min, 
						&this->blue_range.hue_max
					);

			//checkHistory : y offset
				getline(configurationFile,line);
				sscanf(line.c_str(),"history_offset_y %d",&this->history_offset.y);
			//checkHistory : x offset
				getline(configurationFile,line);
				sscanf(line.c_str(),"history_offset_x %d",&this->history_offset.x);

			// Close File
			configurationFile.close();
		}
		else{
			// Report no file
			printf("Configuration file could not open.\n");
			
			// Options
			this->threshold = 230;
			this->max_diameter_divisor = 3;
			this->tol = 5;
			this->solid_offset.x = 3;
			this->solid_offset.y = 3;
			this->solid_width_divisor = 3;
			this->solid_height_divisor = 3;

			this->yellow_range.red_max = 230;
			this->yellow_range.red_min = 255;
			this->yellow_range.green_max = 110;
			this->yellow_range.green_min = 140;
			this->yellow_range.blue_max = 60;
			this->yellow_range.blue_min = 120;
			
			this->green_range.red_max = 255;
			this->green_range.red_min = 200;
			this->green_range.green_max = 130;
			this->green_range.green_min = 80;
			this->green_range.blue_max = 240;
			this->green_range.blue_min = 200;
			
			this->red_range.red_max = 255;
			this->red_range.red_min = 255;
			this->red_range.green_max = 120;
			this->red_range.green_min = 060;
			this->red_range.blue_max = 110;
			this->red_range.blue_min = 30;
			
			this->blue_range.red_max = 255;
			this->blue_range.red_min = 0;
			this->blue_range.green_max = 255;
			this->blue_range.green_min = 255;
			this->blue_range.blue_max = 120;
			this->blue_range.blue_min = 60;

			history_offset.y = 64;
			history_offset.x = 64;
		}
	}
}
