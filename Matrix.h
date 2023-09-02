#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct f_Matrix{
float* ptr;
unsigned int w;
unsigned int h;
} f_Matrix_t;

f_Matrix_t *f_Matrix_constructor(int width,int height){
	f_Matrix_t *Matrix = malloc(sizeof(f_Matrix_t));
	Matrix->w = width;
	Matrix->h = height;
	Matrix->ptr = calloc(width*height,sizeof(float));
	return Matrix;
}
int f_Matrix_destructor(f_Matrix_t *Matrix){
	free(Matrix->ptr);
	free(Matrix);
	return 0;
}
float f_Matrix_get(f_Matrix_t *Matrix,int w,int h){
	return *(Matrix->ptr+w+h*Matrix->w);
}
int f_Matrix_set(f_Matrix_t *Matrix,int w,int h,float value){
	if((Matrix->w>w)&(Matrix->h>h)){
	*(Matrix->ptr+w+h*Matrix->w) = value;
	return 0;
	}
	else{
	return 1;
	}
}
int f_Matrix_multiply(f_Matrix_t *m1,f_Matrix_t *m2,f_Matrix_t* out){
	if ((m1->w==m2->h)&(out->h==m1->h)&(out->w==m2->w)){
		float f = 0;
		for (unsigned int k=0; k<m1->h;k++) {
			for (unsigned int m=0; m<m2->w; m++) {
				f_Matrix_set(out,m,k,0.f);
				for (unsigned int l=0;l<m1->w;l++) {
					f = f_Matrix_get(m1,l,k)*f_Matrix_get(m2,m,l);
			  	f_Matrix_set(out,m,k,f+f_Matrix_get(out,m,k));
				}
			}
		}
		return 0;
	}
	return 1;
}

int f_Matrix_multiply_scalar(f_Matrix_t* m,double s){
	for(int i =0;i<m->h;i++){
		for(int j=0;j<m->w;j++){
			f_Matrix_set(m,i,j,f_Matrix_get(m,j,i)*s);
		}
	}
	return 0;
}
int f_Matrix_add(f_Matrix_t* m1,f_Matrix_t* m2,f_Matrix_t* out){
	if ((m1->h == m2->h) & (m2->w==m1->w)){
	for (unsigned int k = 0; k<m1->w;k++) {
		for (unsigned int m = 0; m<m1->h;m++) {
			f_Matrix_set(out,k,m,f_Matrix_get(m1,k,m)+f_Matrix_get(m2,k,m));
		}
	}
	return 0;
	}
	return 1;
}

int f_Matrix_sub(f_Matrix_t* m1,f_Matrix_t* m2,f_Matrix_t* out){
	if ((m1->h == m2->h) & (m2->w==m1->w)){
	for (unsigned int k = 0; k<m1->w;k++) {
		for (unsigned int m = 0; m<m1->h;m++) {
			f_Matrix_set(out,k,m,f_Matrix_get(m1,k,m)-f_Matrix_get(m2,k,m));
		}
	}
	return 0;
	}
	return 1;
}

int f_Matrix_print(f_Matrix_t* matrix){
	printf("(");
	for(int i=0;i<matrix->h;i++){
		for(int y=0;y<(*matrix).w;y++){
			printf("%.4f  ",f_Matrix_get(matrix,y,i));
		}
		printf("\n");
	}
	printf(")");
	return 0;
}

int f_Matrix_func_per_val(f_Matrix_t* matrix,float (*func)(float)){
	for(int i = 0;i<matrix->w*matrix->h;i++){
		*(matrix->ptr+i) = (*func)(*(matrix->ptr+i));
	}
	return 0;
}
