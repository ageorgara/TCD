/*
 *	Technical University of Crete
 *	2014
 *
 */

 // drawlib.cpp 

#include "drawlib.h"

namespace tuc{
	// Constructor
	Draw::Draw(unsigned char *** frame, int width, int height){
		// Color Black
		this->rgb[0] = 0;
		this->rgb[1] = 0;
		this->rgb[2] = 0;
		// Save dimentions
		this->height = height;
		this->width = width;

		this->set(frame);
	}

	// Set frame object
	Draw* Draw::set(unsigned char *** frame){
		// Set frame
		this->frame = frame;
		// Return object
		return this;
	}

	// Set frame object
	Draw* Draw::color(unsigned char r, unsigned char g, unsigned char b){
		// Color Set
		this->rgb[0] = r;
		this->rgb[1] = g;
		this->rgb[2] = b;

		// Return object
		return this;
	}

	// Cross
	Draw* Draw::cross(int x, int y){
		//Draw a cross
		for(int i=-3; i<=3; ++i){
			if(x+i<this->width && x+i>=0){
				if(y-i<this->height && y-i>=0){
					this->frame[y-i][x+i][0] = this->rgb[2];
					this->frame[y-i][x+i][1] = this->rgb[1];
					this->frame[y-i][x+i][2] = this->rgb[0];
				}
				if(y+i<this->height && y+i>=0){
					this->frame[y+i][x+i][0] = this->rgb[2];
					this->frame[y+i][x+i][1] = this->rgb[1];
					this->frame[y+i][x+i][2] = this->rgb[0];
				}
			}
		}
		
		// Return object
		return this;
	}

	const int Draw::letter_0[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{1,0,0,1,1},{1,0,1,0,1},{1,1,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_1[7][5] = {{0,0,1,0,0},{0,1,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{1,1,1,1,1}};
	const int Draw::letter_2[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{0,0,0,0,1},{0,0,1,1,0},{0,1,0,0,0},{1,0,0,0,0},{1,1,1,1,1}};
	const int Draw::letter_3[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{0,0,0,0,1},{0,0,1,1,0},{0,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_4[7][5] = {{0,0,0,1,1},{0,0,1,0,1},{0,1,0,0,1},{1,0,0,0,1},{1,1,1,1,1},{0,0,0,0,1},{0,0,0,0,1}};
	const int Draw::letter_5[7][5] = {{1,1,1,1,1},{1,0,0,0,0},{1,1,1,1,0},{0,0,0,0,1},{0,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_6[7][5] = {{0,0,1,1,0},{0,1,0,0,0},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_7[7][5] = {{1,1,1,1,1},{1,0,0,0,1},{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0}};
	const int Draw::letter_8[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_9[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,1},{0,0,0,0,1},{0,0,0,1,0},{0,1,1,0,0}};
	const int Draw::letter_A[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_B[7][5] = {{1,1,1,1,0},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0}};
	const int Draw::letter_C[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_D[7][5] = {{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0}};
	const int Draw::letter_E[7][5] = {{1,1,1,1,1},{1,0,0,0,0},{1,1,1,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,1},{1,1,1,1,1}};
	const int Draw::letter_F[7][5] = {{1,1,1,1,1},{1,0,0,0,0},{1,1,1,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0}};
	const int Draw::letter_G[7][5] = {{0,1,1,1,1},{1,0,0,0,0},{1,0,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_H[7][5] = {{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_I[7][5] = {{0,1,1,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,1,1,0}};
	const int Draw::letter_J[7][5] = {{0,0,0,0,1},{0,0,0,0,1},{0,0,0,0,1},{0,0,0,0,1},{0,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_K[7][5] = {{1,0,0,0,1},{1,0,0,1,0},{1,1,1,0,0},{1,0,0,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_L[7][5] = {{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,1}};
	const int Draw::letter_M[7][5] = {{1,0,0,0,1},{1,1,0,1,1},{1,0,1,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_N[7][5] = {{1,0,0,0,1},{1,1,0,0,1},{1,0,1,0,1},{1,0,0,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_O[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_P[7][5] = {{1,1,1,1,0},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0}};
	const int Draw::letter_Q[7][5] = {{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,1,0},{0,1,1,0,1}};
	const int Draw::letter_R[7][5] = {{1,1,1,1,0},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_S[7][5] = {{0,1,1,1,1},{1,0,0,0,0},{0,1,1,1,0},{0,0,0,0,1},{0,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_T[7][5] = {{1,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0}};
	const int Draw::letter_U[7][5] = {{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_V[7][5] = {{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,0,1,0},{0,1,0,1,0},{0,0,1,0,0}};
	const int Draw::letter_W[7][5] = {{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,1,0,1},{1,1,0,1,1},{1,0,0,0,1}};
	const int Draw::letter_X[7][5] = {{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,1,0,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_Y[7][5] = {{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0}};
	const int Draw::letter_Z[7][5] = {{1,1,1,1,1},{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,0,0,0,0},{1,1,1,1,1}};
	const int Draw::letter_a[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,1,1,1,0},{0,0,0,0,1},{0,1,1,1,1},{1,0,0,0,1},{0,1,1,1,1}};
	const int Draw::letter_b[7][5] = {{1,0,0,0,0},{1,0,0,0,0},{1,0,1,1,0},{1,1,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0}};
	const int Draw::letter_c[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,0},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_d[7][5] = {{0,0,0,0,1},{0,0,0,0,1},{0,1,1,0,1},{1,0,0,1,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,1}};
	const int Draw::letter_e[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,1,1,1,0},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,0},{0,1,1,1,1}};
	const int Draw::letter_f[7][5] = {{0,0,0,1,1},{0,0,1,0,0},{0,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0}};
	const int Draw::letter_g[7][5] = {{0,0,0,0,0},{0,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,1},{0,0,0,0,1},{1,1,1,1,0}};
	const int Draw::letter_h[7][5] = {{1,0,0,0,0},{1,0,0,0,0},{1,0,1,1,0},{1,1,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_i[7][5] = {{0,0,1,0,0},{0,0,0,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0}};
	const int Draw::letter_j[7][5] = {{0,0,0,0,1},{0,0,0,0,0},{0,0,0,0,1},{0,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_k[7][5] = {{1,0,0,0,0},{1,0,0,0,0},{1,0,0,1,0},{1,0,1,0,0},{1,1,0,0,0},{1,0,1,0,0},{1,0,0,1,0}};
	const int Draw::letter_l[7][5] = {{0,1,0,0,0},{0,1,0,0,0},{0,1,0,0,0},{0,1,0,0,0},{0,1,0,0,0},{0,1,0,0,0},{0,0,1,0,0}};
	const int Draw::letter_m[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,1,0,1,0},{1,0,1,0,1},{1,0,1,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_n[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1}};
	const int Draw::letter_o[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}};
	const int Draw::letter_p[7][5] = {{0,0,0,0,0},{1,0,1,1,0},{1,1,0,0,1},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0}};
	const int Draw::letter_q[7][5] = {{0,0,0,0,0},{0,1,1,0,1},{1,0,0,1,1},{1,0,0,0,1},{0,1,1,1,1},{0,0,0,0,1},{0,0,0,0,1}};
	const int Draw::letter_r[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,0,1,1,0},{1,1,0,0,1},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0}};
	const int Draw::letter_s[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,1,1,1,1},{1,0,0,0,0},{0,1,1,1,0},{0,0,0,0,1},{1,1,1,1,0}};
	const int Draw::letter_t[7][5] = {{0,0,1,0,0},{0,1,1,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,0,1,0}};
	const int Draw::letter_u[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,1}};
	const int Draw::letter_v[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0}};
	const int Draw::letter_w[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,1,0,1},{1,0,1,0,1},{0,1,1,1,1}};
	const int Draw::letter_x[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,1,0,1,0},{1,0,0,0,1}};
	const int Draw::letter_y[7][5] = {{0,0,0,0,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,1},{0,0,0,0,1},{1,1,1,1,0}};
	const int Draw::letter_z[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{1,1,1,1,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,1,1,1,1}};
	const int Draw::letter_updownpoint[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,1,0,0},{0,0,0,0,0}};
	const int Draw::letter_minus[7][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};

	const int Draw::letter_parenthesis_left[7][5] = {{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,0,1,0},{0,0,0,0,1}};
	const int Draw::letter_parenthesis_right[7][5] = {{1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,0,0,0},{1,0,0,0,0}};

	// Draw Word
	Draw* Draw::write(char* string, int x, int y){
		char* c = string;
		while(c[0]!='\0' && x+6<this->width && x>=0 && y+8<this->height && y>=0){

			switch(c[0]){
				case '0':this->write_arr(this->letter_0, x, y);break;
				case '1':this->write_arr(this->letter_1, x, y);break;
				case '2':this->write_arr(this->letter_2, x, y);break;
				case '3':this->write_arr(this->letter_3, x, y);break;
				case '4':this->write_arr(this->letter_4, x, y);break;
				case '5':this->write_arr(this->letter_5, x, y);break;
				case '6':this->write_arr(this->letter_6, x, y);break;
				case '7':this->write_arr(this->letter_7, x, y);break;
				case '8':this->write_arr(this->letter_8, x, y);break;
				case '9':this->write_arr(this->letter_9, x, y);break;

				case 'A':this->write_arr(this->letter_A, x, y);break;
				case 'B':this->write_arr(this->letter_B, x, y);break;
				case 'C':this->write_arr(this->letter_C, x, y);break;
				case 'D':this->write_arr(this->letter_D, x, y);break;
				case 'E':this->write_arr(this->letter_E, x, y);break;
				case 'F':this->write_arr(this->letter_F, x, y);break;
				case 'G':this->write_arr(this->letter_G, x, y);break;
				case 'H':this->write_arr(this->letter_H, x, y);break;
				case 'I':this->write_arr(this->letter_I, x, y);break;
				case 'J':this->write_arr(this->letter_J, x, y);break;
				case 'K':this->write_arr(this->letter_K, x, y);break;
				case 'L':this->write_arr(this->letter_L, x, y);break;
				case 'M':this->write_arr(this->letter_M, x, y);break;
				case 'N':this->write_arr(this->letter_N, x, y);break;
				case 'O':this->write_arr(this->letter_O, x, y);break;
				case 'P':this->write_arr(this->letter_P, x, y);break;
				case 'Q':this->write_arr(this->letter_Q, x, y);break;
				case 'R':this->write_arr(this->letter_R, x, y);break;
				case 'S':this->write_arr(this->letter_S, x, y);break;
				case 'T':this->write_arr(this->letter_T, x, y);break;
				case 'U':this->write_arr(this->letter_U, x, y);break;
				case 'V':this->write_arr(this->letter_V, x, y);break;
				case 'W':this->write_arr(this->letter_W, x, y);break;
				case 'X':this->write_arr(this->letter_X, x, y);break;
				case 'Y':this->write_arr(this->letter_Y, x, y);break;
				case 'Z':this->write_arr(this->letter_Z, x, y);break;

				case 'a':this->write_arr(this->letter_a, x, y);break;
				case 'b':this->write_arr(this->letter_b, x, y);break;
				case 'c':this->write_arr(this->letter_c, x, y);break;
				case 'd':this->write_arr(this->letter_d, x, y);break;
				case 'e':this->write_arr(this->letter_e, x, y);break;
				case 'f':this->write_arr(this->letter_f, x, y);break;
				case 'g':this->write_arr(this->letter_g, x, y);break;
				case 'h':this->write_arr(this->letter_h, x, y);break;
				case 'i':this->write_arr(this->letter_i, x, y);break;
				case 'j':this->write_arr(this->letter_j, x, y);break;
				case 'k':this->write_arr(this->letter_k, x, y);break;
				case 'l':this->write_arr(this->letter_l, x, y);break;
				case 'm':this->write_arr(this->letter_m, x, y);break;
				case 'n':this->write_arr(this->letter_n, x, y);break;
				case 'o':this->write_arr(this->letter_o, x, y);break;
				case 'p':this->write_arr(this->letter_p, x, y);break;
				case 'q':this->write_arr(this->letter_q, x, y);break;
				case 'r':this->write_arr(this->letter_r, x, y);break;
				case 's':this->write_arr(this->letter_s, x, y);break;
				case 't':this->write_arr(this->letter_t, x, y);break;
				case 'u':this->write_arr(this->letter_u, x, y);break;
				case 'v':this->write_arr(this->letter_v, x, y);break;
				case 'w':this->write_arr(this->letter_w, x, y);break;
				case 'x':this->write_arr(this->letter_x, x, y);break;
				case 'y':this->write_arr(this->letter_y, x, y);break;
				case 'z':this->write_arr(this->letter_z, x, y);break;

				case ':':this->write_arr(this->letter_updownpoint, x, y);break;
				case '-':this->write_arr(this->letter_minus, x, y);break;
				
				case '(':this->write_arr(this->letter_parenthesis_left, x, y);break;
				case ')':this->write_arr(this->letter_parenthesis_right, x, y);break;
			}
			
			// move cursor
			x += 6;
			c ++;
		}
		
		
		// Return object
		return this;
	}

	// Draw Array
	Draw* Draw::write_arr(const int arr[7][5], int x, int y){
		
		for (int i=0; i<7; ++i){
			for (int j=0; j<5; ++j){
				if(arr[i][j]!=0){
					this->frame[y+i][x+j][0] = this->rgb[2];
					this->frame[y+i][x+j][1] = this->rgb[1];
					this->frame[y+i][x+j][2] = this->rgb[0];
				}
			}
		}
		
		
		// Return object
		return this;
	}

	Draw* Draw::rec(int x_1, int y_1, int x_2, int y_2){
		int i;

		for (i = x_1; i <= x_2; i++){
			this->frame[y_1][i][0] = this->rgb[2];
			this->frame[y_1][i][1] = this->rgb[1];
			this->frame[y_1][i][2] = this->rgb[0];

			this->frame[y_2][i][0] = this->rgb[2];
			this->frame[y_2][i][1] = this->rgb[1];
			this->frame[y_2][i][2] = this->rgb[0];
		}
		for (i = y_1; i <= y_2; i++){
			this->frame[i][x_1][0] = this->rgb[2];
			this->frame[i][x_1][1] = this->rgb[1];
			this->frame[i][x_1][2] = this->rgb[0];

			this->frame[i][x_2][0] = this->rgb[2];
			this->frame[i][x_2][1] = this->rgb[1];
			this->frame[i][x_2][2] = this->rgb[0];
		}
		return this;
	}
}
