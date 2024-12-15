#pragma once


#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <pthread.h>
#include <curses.h>
#endif

typedef struct f_Matrix_3{
float* ptr;
unsigned int d;
unsigned int w;
unsigned int h;
} f_Matrix_3_t;

typedef struct f_Matrix{
float* ptr;
unsigned int w;
unsigned int h;
} f_Matrix_t;

typedef struct f_mult_arg{
f_Matrix_t** ptr;
unsigned int m;
unsigned int k;
unsigned int l;
}f_mult_arg_t;


f_Matrix_3_t *f_Matrix_3_constructor(int width,int height,int depth){
	f_Matrix_3_t *Matrix =(f_Matrix_3_t *) malloc(sizeof(f_Matrix_3_t));
	Matrix->w = width;
	Matrix->h = height;
	Matrix->d = depth;
	Matrix->ptr =(float*) calloc(width*height*depth,sizeof(float));
	return Matrix;
}
f_Matrix_t *f_Matrix_constructor(int width,int height){
	f_Matrix_t *Matrix =(f_Matrix_t*) malloc(sizeof(f_Matrix_t));
	Matrix->w = width;
	Matrix->h = height;
	Matrix->ptr =(float *) calloc(width*height,sizeof(float));
	return Matrix;
}
int f_Matrix_3_destructor(f_Matrix_3_t *Matrix){
	free(Matrix->ptr);
	free(Matrix);
	return 0;
}
int f_Matrix_destructor(f_Matrix_t *Matrix){
	free(Matrix->ptr);
	free(Matrix);
	return 0;
}
float f_Matrix_3_get(f_Matrix_3_t *Matrix,int w,int h,int d){
	return *(Matrix->ptr+w+h*Matrix->w+d*Matrix->w*Matrix->h);
}
int f_Matrix_3_set(f_Matrix_3_t *Matrix,int w,int h,int d,float value){
	if((Matrix->w>w)&(Matrix->h>h)&(Matrix->d>d)){
	*(Matrix->ptr+w+h*Matrix->w+d*Matrix->w*Matrix->h) = value;
	return 0;
	}
	else{
	return 1;
	}
}
static inline float f_Matrix_get(f_Matrix_t *Matrix,int w,int h){
	return *(Matrix->ptr+w+h*Matrix->w);
}
static inline int f_Matrix_set(f_Matrix_t *Matrix,int w,int h,float value){
	*(Matrix->ptr+w+h*Matrix->w) = value;
	return 0;
}
#ifndef _WIN32
void* f_Mult_thread(void* ar){
	f_mult_arg_t* arg = (f_mult_arg_t*)ar;
	f_Matrix_set(arg->ptr[0],arg->m,arg->k,0.f);
	for (unsigned int l=0;l<arg->l;l++) {
		f_Matrix_set(arg->ptr[0],arg->m,arg->k,f_Matrix_get(arg->ptr[1],l,arg->k)*f_Matrix_get(arg->ptr[2],arg->m,l)+f_Matrix_get(arg->ptr[0],arg->m,arg->k));
	}
	pthread_exit(NULL);
}
int f_Matrix_multiply(f_Matrix_t *m1,f_Matrix_t *m2,f_Matrix_t* out){
	if ((m1->w==m2->h)&(out->h==m1->h)&(out->w==m2->w)){
		pthread_t threads[m1->h*m2->w];
		f_mult_arg_t* args =(f_mult_arg_t*) malloc(m1->h*m2->w*sizeof(f_mult_arg_t));
		float f =0;
		f_Matrix_t **ptr =(f_Matrix_t**) malloc(3*sizeof(f_Matrix_t*));
		ptr[0]=out;
		ptr[1]=m1;
		ptr[2]=m2;
		for (unsigned int k=0; k<m1->h;k++) {
			for (unsigned int m=0; m<m2->w; m++) {
				args[m+k*m2->w].ptr = ptr;
				args[m+k*m2->w].m = m;
				args[m+k*m2->w].k = k;
				args[m+k*m2->w].l = m1->w;
				pthread_create(&threads[m+k*m2->w],NULL,f_Mult_thread,(void *)&args[m+k*m2->w]);
			}
		}
		for(unsigned int i=0;i<m1->h*m2->w;i++){
			pthread_join(threads[i],NULL);
		}
		free(args);
		free(ptr);
		return 0;
	}
	return 1;
}
#else
//Windows unoptimized code cause they don't have pthreads
int f_Matrix_multiply(f_Matrix_t *m1,f_Matrix_t *m2,f_Matrix_t* out){
	if ((m1->w==m2->h)&(out->h==m1->h)&(out->w==m2->w)){
		for (unsigned int k=0; k<m1->h;k++) {
			for (unsigned int m=0; m<m2->w; m++) {
				for(unsigned int i=0;i<m1->w;i++){
					f_Matrix_set(out,m,k,f_matrix_get(m1,i,k)*f_Matrix_get(m2,m,i));
				}
			}
		}
		return 0;
	}
	return 1;
}
#endif
int f_Matrix_3_multiply(f_Matrix_3_t *m1,f_Matrix_3_t *m2,f_Matrix_3_t* out){
	if ((m1->w==m2->h)&(out->h==m1->h)&(out->w==m2->w)&(m1->d==m2->d)&(out->d==m1->d)){
		float f = 0;
		for (unsigned int k=0; k<m1->h;k++) {
			for (unsigned int m=0; m<m2->w; m++) {
				for (unsigned int d=0; d<m2->d; d++) {
					f_Matrix_3_set(out,m,k,d,0.f);
					for (unsigned int l=0;l<m1->w;l++) {
						f = f_Matrix_3_get(m1,l,k,d)*f_Matrix_3_get(m2,m,l,d);
			  			f_Matrix_3_set(out,m,k,d,f+f_Matrix_3_get(out,m,k,d));
					}
				}
			}
		}
	return 0;
	}
	return 1;
}

int f_Matrix_3_multiply_scalar(f_Matrix_3_t* m,double s){
	for(int i =0;i<m->h;i++){
		for(int j=0;j<m->w;j++){
			for(int k=0;k<m->d;k++){
				f_Matrix_3_set(m,j,i,k,f_Matrix_3_get(m,j,i,k)*s);
			}
		}
	}
	return 0;
}
int f_Matrix_multiply_scalar(f_Matrix_t* m,double s){
	for(int i =0;i<m->h;i++){
		for(int j=0;j<m->w;j++){
			f_Matrix_set(m,i,j,f_Matrix_get(m,j,i)*s);
		}
	}
	return 0;
}
int f_Matrix_3_add(f_Matrix_3_t* m1,f_Matrix_3_t* m2,f_Matrix_3_t* out){
	if ((m1->h == m2->h) & (m2->w==m1->w) & (m2->d == m1->d)){
	for (unsigned int k = 0; k<m1->w;k++) {
		for (unsigned int m = 0; m<m1->h;m++) {
			for (unsigned int l = 0; l<m1->d;l++) {
			f_Matrix_3_set(out,k,m,l,f_Matrix_3_get(m1,k,m,l)+f_Matrix_3_get(m2,k,m,l));
			}
		}
	}
	return 0;
	}
	return 1;
}

int f_Matrix_3_sub(f_Matrix_3_t* m1,f_Matrix_3_t* m2,f_Matrix_3_t* out){
	if ((m1->h == m2->h) & (m2->w==m1->w) & (m2->d == m1->d)){
	for (unsigned int k = 0; k<m1->w;k++) {
		for (unsigned int m = 0; m<m1->h;m++) {
			for (unsigned int l = 0; l<m1->d;l++) {
			f_Matrix_3_set(out,k,m,l,f_Matrix_3_get(m1,k,m,l)-f_Matrix_3_get(m2,k,m,l));
			}
		}
	}
	return 0;
	}
	return 1;
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

int f_Matrix_3_print(f_Matrix_3_t* matrix){
	printf("(");
	for(int z=0;z<matrix->d;z++){
	printf("[\n");
	for(int i=0;i<matrix->h;i++){
		for(int y=0;y<matrix->w;y++){
			printf("%.4f  ",f_Matrix_3_get(matrix,y,i,z));
		}
		printf("\n");
	}
	printf("]\n");
	}
	printf(")");
	return 0;
}

int f_Matrix_3_func_per_val(f_Matrix_3_t* matrix,float (*func)(float)){
	for(int i = 0;i<matrix->w*matrix->h*matrix->d;i++){
		*(matrix->ptr+i) = (*func)(*(matrix->ptr+i));
	}
	return 0;
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
        return 0 ;
}
