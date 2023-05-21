#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct f_Matrix{
float* ptr;
unsigned int w;
unsigned int h;
} f_Matrix_t;

f_Matrix_t *f_Matrix_constructor(int width,int height){
	f_Matrix_t *Matrix = (f_Matrix_t*)malloc(sizeof(f_Matrix_t));
	(*Matrix).w = width;
	(*Matrix).h = height;
	(*Matrix).ptr = (float *)malloc(width*height*sizeof(float));
	return Matrix;
}
int f_Matrix_destructor(f_Matrix_t *Matrix){
	free((*Matrix).ptr);
	free(Matrix);
	return 0;
}
float f_Matrix_get(f_Matrix_t *Matrix,int w,int h){
	return *((*Matrix).ptr+w+h*(*Matrix).w);
}
int f_Matrix_set(f_Matrix_t *pMatrix,int w,int h,float value){
	f_Matrix_t Matrix =*pMatrix;
	if((Matrix.w>w)&(Matrix.h>h)){
	*(Matrix.ptr+w+h*Matrix.w) = value;
	return 0;
	}
	else{
	return 1;
	}
}
int f_Matrix_multiply(f_Matrix_t *pm1,f_Matrix_t *pm2,f_Matrix_t* out){
	f_Matrix_t m1 = *pm1;
	f_Matrix_t m2 = *pm2;
	if ((m1.w==m2.h)&((*out).h==m1.h)&((*out).w==m2.w)){
		float f = 0;
		for (unsigned int k=0; k<m1.h;k++) {
			for (unsigned int m=0; m<m2.w; m++) {
				for (unsigned int l=0;l<m1.w;l++) {
					f = f_Matrix_get(pm1,l,k)*f_Matrix_get(pm2,m,l);
			  	f_Matrix_set(out,m,k,f+f_Matrix_get(out,m,k));
				}
			}
		}
		return 0;
	}
	else{
	return 1;
	}
}
f_Matrix_t* f_Matrix_rotate_right(f_Matrix_t* pm1){
	f_Matrix_t m1 = *pm1;
	if (m1.w==1){
		int swap = m1.w;
		m1.w = m1.h;
		m1.h = swap;
		return pm1;
	}else {
		//TODO Finish this
		printf("Error : Unimplemented");
	}
}

f_Matrix_t* f_Matrix_rotate_left(f_Matrix_t* pm1){
	f_Matrix_t m1 = *pm1;
	if (m1.h==1){
		int swap = m1.w;
		m1.w = m1.h;
		m1.h = swap;
		return pm1;
	}else {
		//TODO Finish this
		printf("Error : Unimplemented");
	}
}

int f_Matrix_ceil_positive(f_Matrix_t* matrix){
	for (int i =0;i<(*matrix).h;i++) {
		for (int y=0;y<(*matrix).w;y++) {
			if (f_Matrix_get(matrix,y,i)<0) {
				f_Matrix_set(matrix,y,i,0);
			}
		}
	}
	return 0;
}

int f_Matrix_multiply_scalar(f_Matrix_t* m,double s){
	int width = (*m).w;
	int height = (*m).h;
	for(int i =0;i<height;i++){
		for(int j=0;j<width;j++){
			f_Matrix_set(m,i,j,f_Matrix_get(m,j,i)*s);
		}
	}
	return 0;
}
int f_Matrix_add(f_Matrix_t* pm1,f_Matrix_t* pm2,f_Matrix_t* out){
	f_Matrix_t m1 = *pm1;
	f_Matrix_t m2 = *pm2;
	if ((m1.h == m2.h) & (m2.w==m1.w)){
	for (unsigned int k = 0; k<m1.w;k++) {
		for (unsigned int m = 0; m<m1.h;m++) {
			f_Matrix_set(out,k,m,f_Matrix_get(pm1,k,m)+f_Matrix_get(pm2,k,m));
		}
	}
	return 0;
	}
}

int f_Matrix_sub(f_Matrix_t* pm1,f_Matrix_t* pm2,f_Matrix_t* out){
	f_Matrix_t m1 = *pm1;
	f_Matrix_t m2 = *pm2;
	if ((m1.h == m2.h) & (m2.w==m1.w)){
	for (unsigned int k = 0; k<m1.w;k++) {
		for (unsigned int m = 0; m<m1.h;m++) {
			f_Matrix_set(out,k,m,f_Matrix_get(pm1,k,m)-f_Matrix_get(pm2,k,m));
		}
	}
	return 0;
	}
}
int f_Matrix_sign_squared(f_Matrix_t* pm1,f_Matrix_t* out){
	f_Matrix_t m1 = *pm1;
	int val;
	for (unsigned int k = 0; k<m1.w;k++) {
		for (unsigned int m = 0; m<m1.h;m++) {
			val = f_Matrix_get(pm1,k,m);
			f_Matrix_set(out,k,m,val*val);
		}
	}
	return 0;
}
int f_Matrix_print(f_Matrix_t* matrix){
	printf("(");
	for(int i=0;i<(*matrix).h;i++){
		for(int y=0;y<(*matrix).w;y++){
			printf("%.4f  ",f_Matrix_get(matrix,y,i));
		}
		printf("\n");
	}
	printf(")");
	return 0;
}


