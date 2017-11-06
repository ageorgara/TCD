/*
 *	Technical University of Crete
 *	2014
 *
 */

// Library
	namespace tuc{
		class Draw{
			public:
				// Save dimentions
				int height;
				int width;

				// Frame
				unsigned char *** frame;

				// Color
				unsigned char rgb[3];
				
				// Constructor
				Draw(unsigned char *** frame, int width, int height);
				// Set Object
				Draw* set(unsigned char *** frame);
				// Set Color
				Draw* color(unsigned char r, unsigned char g, unsigned char b);
				// Draw a cross
				Draw* cross(int x, int y);

				// Letters //[7][5]
				static const int letter_0[7][5];
				static const int letter_1[7][5];
				static const int letter_2[7][5];
				static const int letter_3[7][5];
				static const int letter_4[7][5];
				static const int letter_5[7][5];
				static const int letter_6[7][5];
				static const int letter_7[7][5];
				static const int letter_8[7][5];
				static const int letter_9[7][5];
				static const int letter_A[7][5];
				static const int letter_B[7][5];
				static const int letter_C[7][5];
				static const int letter_D[7][5];
				static const int letter_E[7][5];
				static const int letter_F[7][5];
				static const int letter_G[7][5];
				static const int letter_H[7][5];
				static const int letter_I[7][5];
				static const int letter_J[7][5];
				static const int letter_K[7][5];
				static const int letter_L[7][5];
				static const int letter_M[7][5];
				static const int letter_N[7][5];
				static const int letter_O[7][5];
				static const int letter_P[7][5];
				static const int letter_Q[7][5];
				static const int letter_R[7][5];
				static const int letter_S[7][5];
				static const int letter_T[7][5];
				static const int letter_U[7][5];
				static const int letter_V[7][5];
				static const int letter_W[7][5];
				static const int letter_X[7][5];
				static const int letter_Y[7][5];
				static const int letter_Z[7][5];
				static const int letter_a[7][5];
				static const int letter_b[7][5];
				static const int letter_c[7][5];
				static const int letter_d[7][5];
				static const int letter_e[7][5];
				static const int letter_f[7][5];
				static const int letter_g[7][5];
				static const int letter_h[7][5];
				static const int letter_i[7][5];
				static const int letter_j[7][5];
				static const int letter_k[7][5];
				static const int letter_l[7][5];
				static const int letter_m[7][5];
				static const int letter_n[7][5];
				static const int letter_o[7][5];
				static const int letter_p[7][5];
				static const int letter_q[7][5];
				static const int letter_r[7][5];
				static const int letter_s[7][5];
				static const int letter_t[7][5];
				static const int letter_u[7][5];
				static const int letter_v[7][5];
				static const int letter_w[7][5];
				static const int letter_x[7][5];
				static const int letter_y[7][5];
				static const int letter_z[7][5];
				static const int letter_updownpoint[7][5];
				static const int letter_minus[7][5];
				static const int letter_parenthesis_left[7][5];
				static const int letter_parenthesis_right[7][5];

				// Draw a string
				Draw* write(char* string, int x, int y);
				// Draw a array
				Draw* write_arr(const int arr[7][5], int x, int y);

				Draw* rec(int x_1, int y_1, int x_2, int y_2);

		};
	}
