#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Matrix.h"

#ifndef LEARNING_RATE
#define LEARNING_RATE 0.1f
#endif

float squared(float v){
	return v*v;
}

float sigmoid (float v){
	return (1/(1+exp(-v)));
}

float sigmoid_derivative(float v){
	double val = sigmoid(v);
	return val*(1-val);
}

typedef struct AI{
	int* layers_layout;
	int layers;
	f_Matrix_t** weights;
	f_Matrix_t** nodes;
	f_Matrix_t** d_nodes;
	float (*activation)(float);
	float (*activationDerivative)(float);
} AI_t;

AI_t* create_ai(int* layers_layout,int layers);
int AI_Train(f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation);
void feed_forward(AI_t* ai,f_Matrix_t* input);
int destroy_ai(AI_t* ai);

void feed_forward(AI_t* ai,f_Matrix_t* input){
	if (input->w == ai->layers_layout[0]) {
		for (int i=0;i<ai->layers_layout[0];i++) {
			f_Matrix_set(ai->nodes[0],i,0,f_Matrix_get(input,i,0));
		}
		for (int i=0;i<ai->layers-1;i++) {
			f_Matrix_multiply(ai->nodes[i],ai->weights[i],ai->nodes[i+1]);
			for(int y=0;y<ai->layers_layout[i+1];y++){
				f_Matrix_set(ai->d_nodes[i+1],y,0,ai->activationDerivative(f_Matrix_get(ai->nodes[i+1],y,0)));
			}
			f_Matrix_func_per_val(ai->nodes[i+1],ai->activation);
		}
	}else {
		printf("Error : Size of input inequal to input_size of the ai, %d !\n",input->w);
	}
}

double compute_Error(f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	feed_forward(ai,input);
	double Error = 0;
	f_Matrix_t* Local_Error = f_Matrix_constructor(ai->layers_layout[ai->layers-1],1);
	f_Matrix_sub(ai->nodes[ai->layers-1],expectation,Local_Error);
	f_Matrix_func_per_val(Local_Error,*squared);
	for (int i=0;i<ai->layers_layout[ai->layers-1];i++) {
		Error+=f_Matrix_get(Local_Error,i,0)/ai->layers_layout[ai->layers-1];
	}
	f_Matrix_destructor(Local_Error);
	return Error;
}

int AI_Train(f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	AI_t* gradients = create_ai(ai->layers_layout,ai->layers);
	gradients->layers_layout = malloc(gradients->layers*sizeof(int));
	for(int i =0 ;i<ai->layers;i++){
		gradients->layers_layout[i] = ai->layers_layout[i];
	}
	feed_forward(ai,input);
	f_Matrix_sub(ai->nodes[ai->layers-1],expectation,gradients->nodes[gradients->layers-1]);
	f_Matrix_multiply_scalar(gradients->nodes[gradients->layers-1],2.f/gradients->layers_layout[gradients->layers-1]);
	for (int i = ai->layers-1;i>0;i--) {
		for (int y =0;y<gradients->layers_layout[i];y++){
			f_Matrix_set(gradients->nodes[i],y,0,f_Matrix_get(gradients->nodes[i],y,0)*f_Matrix_get(ai->d_nodes[i],y,0));
			for (int z =0;z<gradients->layers_layout[i-1];z++){
				f_Matrix_set(gradients->nodes[i-1],z,0,f_Matrix_get(gradients->nodes[i-1],z,0)+f_Matrix_get(gradients->nodes[i],y,0)*f_Matrix_get(ai->weights[i-1],y,z));
				f_Matrix_set(ai->weights[i-1],y,z,f_Matrix_get(ai->weights[i-1],y,z)-f_Matrix_get(gradients->nodes[i],y,0)*f_Matrix_get(ai->nodes[i-1],z,0)*LEARNING_RATE);
			}
		}

	}
	destroy_ai(gradients);
	return 0;
}

f_Matrix_t** create_weights(int* layers_layout,int layers){
	f_Matrix_t** weights =(f_Matrix_t**) malloc((layers-1)*sizeof(f_Matrix_t));
	for(int i = 0;i<layers-1;i++){
		weights[i] = f_Matrix_constructor(layers_layout[i+1],layers_layout[i]);
		for(int y =0;y<layers_layout[i];y++){
			for (int j=0;j<layers_layout[i+1];j++) {
			f_Matrix_set(weights[i],j,y,(float)rand()/RAND_MAX);
			}
		}
	}
	return weights;
}
f_Matrix_t** create_biases(int* layers_layout,int layers){
	f_Matrix_t** biases =(f_Matrix_t**) malloc((layers-1)*sizeof(f_Matrix_t));
	for(int i = 0;i<layers-1;i++){
		biases[i] = f_Matrix_constructor(layers_layout[i],1);
		for(int y =0;y<layers_layout[i];y++){
			f_Matrix_set(biases[i],y,0,1);
		}
	}
	return biases;
}
f_Matrix_t** create_nodes(int* layers_layout,int layers){
	f_Matrix_t** nodes =(f_Matrix_t**) malloc((layers)*sizeof(f_Matrix_t));
	for(int i = 0;i<layers;i++){
		nodes[i] = f_Matrix_constructor(layers_layout[i],1);
		}
	return nodes;
}
AI_t* create_ai(int* layers_layout,int layers){
	AI_t* ai =(AI_t*) malloc(sizeof(AI_t));
	(*ai).layers_layout = layers_layout;
	(*ai).layers = layers;
	f_Matrix_t** weights = create_weights(layers_layout,layers);
	(*ai).weights = weights;
	f_Matrix_t** nodes = create_nodes(layers_layout,layers);
	ai->nodes = nodes;
	f_Matrix_t** d_nodes = create_nodes(layers_layout,layers);
	ai->d_nodes = d_nodes;
	ai->activation = *sigmoid;
	ai->activationDerivative = *sigmoid_derivative;
	return ai;
}

int destroy_ai(AI_t* ai){
	for(int i = 0;i<ai->layers-1;i++){
		f_Matrix_destructor(ai->weights[i]);
	}
	for (int i=0;i<ai->layers;i++) {
		f_Matrix_destructor(ai->nodes[i]);
		f_Matrix_destructor(ai->d_nodes[i]);
	}
	free(ai->weights);
	free(ai->d_nodes);
	free(ai->nodes);
	free(ai->layers_layout);
	free(ai);
	return 0;
}

int write_ai(AI_t* ai,const char* n){
	FILE* file = fopen(n,"wb+");
	fwrite(&(ai->layers),sizeof(int),1,file);
	fwrite(ai->layers_layout,sizeof(int),ai->layers,file);
	for(int i=0;i<ai->layers-1;i++){
		fwrite(ai->weights[i]->ptr,sizeof(float),ai->layers_layout[i]*ai->layers_layout[i+1],file);
	}
	fclose(file);
	return 1;
}

AI_t* read_ai(const char* n,float (*activation)(float),float (*activation_derrivative)(float)){
	AI_t* ai = calloc(1,sizeof(AI_t));
	FILE* file = fopen(n,"rb");
	fread(&(ai->layers),sizeof(int),1,file);
	ai->layers_layout = calloc(ai->layers,sizeof(int));
	fread(ai->layers_layout,sizeof(int),ai->layers,file);
	ai->nodes = create_nodes(ai->layers_layout,ai->layers);
	ai->d_nodes = create_nodes(ai->layers_layout,ai->layers);
	ai->activation = activation;
	ai->activationDerivative = activation_derrivative;
	ai->weights = calloc(ai->layers-1,sizeof(f_Matrix_t));
	for(int i =0;i<ai->layers-1;i++){
		ai->weights[i] = f_Matrix_constructor(ai->layers_layout[i+1],ai->layers_layout[i]);
		fread(ai->weights[i]->ptr,sizeof(float),ai->layers_layout[i+1]*ai->layers_layout[i],file);
	}
	fclose(file);
	return ai;
}
