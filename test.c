#include "ai.h"
#include "Matrix.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#define shape_size 20

f_Matrix_t* create_random_circle(int s,f_Matrix_t* m){
	int r = (1 + rand())%(s-2)/2;
	int x = r + rand()%(s-2*r);
	int y = r + rand()%(s-2*r);
	for(int i=0;i<s;i++){
		for(int j=0;j<s;j++){
			if(sqrt((x-j)*(x-j)+(y-i)*(y-i))<=r){
				f_Matrix_set(m,i+y*s,0,1);
			}
		}
	}
	return m;
}

f_Matrix_t* create_random_square(int s,f_Matrix_t* m){
	int r = (1 + rand())%(s-2)/2;
	int x = r + rand()%(s-2*r);
	int y = r + rand()%(s-2*r);
	for(int i=0;i<s;i++){
		for(int j=0;j<s;j++){
			if(i>=(x-r)&&i<=(x+r)&&j>=(y-r)&&j<=(y+r)){
				f_Matrix_set(m,i+y*s,0,1);
			}
		}
	}
	return m;
}

double train_with_random_shape(AI_t* ai,unsigned int times,unsigned int shapes){
	srand(time(NULL));
	f_Matrix_t** m = malloc(shapes*sizeof(f_Matrix_t*));
	f_Matrix_t* expectation;
	int* shape_type= malloc(shapes*sizeof(int));
	double error =0;
	double average_error = 0.;
	for(int i=0;i<shapes;i++){
		m[i]=f_Matrix_constructor(shape_size*shape_size,1);
		if(rand()%2==0){
			m[i] = create_random_circle(shape_size,m[i]);
			shape_type[i]=0;
		}
		else{
			m[i] = create_random_square(shape_size,m[i]);
			shape_type[i]=1;
		}
	}
	for(int i=0;i<times;i++){
		error =0;
		for(int j=0;j<shapes;j++){
			expectation = f_Matrix_constructor(2,1);
			f_Matrix_set(expectation,shape_type[j],0,1);
			//printf("\n%d , %d\n",m[j]->w,j);
			AI_Train(m[j],ai,expectation);
			error += compute_Error(m[j],ai,expectation);
			f_Matrix_destructor(expectation);
		}
		printf("%.12lf\n",error/shapes);
	}
	for(int i=0;i<shapes;i++){
		f_Matrix_destructor(m[i]);
	}
	free(m);
	free(shape_type);
	return average_error/times;
}

int main(int argc,char** argv){
	srand(time(NULL));
	int* layout = malloc(4*sizeof(int));
	layout[0] = shape_size*shape_size;
	layout[1] = 15;
	layout[2] = 10;
	layout[3] = 2;
	printf("Do you want to load your ai.bin (y/n): ");
	char r;
	AI_t* ai;
	scanf(" %c",&r);
	if(r=='y'){
		ai = read_ai("ai.bin",*l_Relu,*l_Relu_derivative);
	}else{
	ai = create_ai(layout,4);
	}
	int times = 1;
	for(int i=0;i<times;i++){
		train_with_random_shape(ai,10,20);
	}
	/*
	f_Matrix_t** inputs = malloc(4*sizeof(f_Matrix_t*));
	f_Matrix_t** outputs = malloc(4*sizeof(f_Matrix_t*));
	for (int i=0;i<4;i++) {
		inputs[i] =f_Matrix_constructor(2,1);
		outputs[i] =f_Matrix_constructor(2,1);
	}
	f_Matrix_set(inputs[0],0,0,0);
	f_Matrix_set(inputs[0],1,0,0);
	f_Matrix_set(inputs[1],0,0,1);
	f_Matrix_set(inputs[1],1,0,0);
	f_Matrix_set(inputs[2],0,0,0);
	f_Matrix_set(inputs[2],1,0,1);
	f_Matrix_set(inputs[3],0,0,1);
	f_Matrix_set(inputs[3],1,0,1);
	f_Matrix_set(outputs[0],0,0,0);
	f_Matrix_set(outputs[0],1,0,0);
	f_Matrix_set(outputs[1],0,0,1);
	f_Matrix_set(outputs[1],1,0,0);
	f_Matrix_set(outputs[2],0,0,1);
	f_Matrix_set(outputs[2],1,0,0);
	f_Matrix_set(outputs[3],0,0,0);
	f_Matrix_set(outputs[3],1,0,1);
	double overall_Error;
	for (int i=0;i<times;i++) {
		overall_Error =0;
		for (int y =0;y<4;y++) {
			overall_Error+=compute_Error(inputs[y],ai,outputs[y]);
			AI_Train(inputs[y],ai,outputs[y]);
		}
			f_Matrix_print(ai->weights[0]);
			f_Matrix_print(ai->weights[1]);
			printf("%.12lf\n",overall_Error/4);
			printf("\n");
	}
	*/
	printf("Do you want to save your save to ai.bin (y/n): ");
	scanf(" %c",&r);
	if(r=='y'){
		write_ai(ai,"ai.bin");
	}
	/*
	for(int i = 0;i<4;i++){
		f_Matrix_destructor(inputs[i]);
		f_Matrix_destructor(outputs[i]);
	}
	*/
	destroy_ai(ai);
	return 0;
}
