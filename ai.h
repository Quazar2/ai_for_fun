#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Matrix.h"

#ifndef LEARNING_RATE
#define LEARNING_RATE 0.01f
#endif



typedef struct AI{
	int* layers_layout;
	int layers;
	f_Matrix_t** weights;
	f_Matrix_t** biases;
	f_Matrix_t** nodes;
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
			f_Matrix_add(ai->nodes[i+1],ai->biases[i],ai->nodes[i+1]);
			f_Matrix_leaky_relu(ai->nodes[i+1]);
		}
	}else {
		printf("Error : Size of input inequal to input_size of the ai!");
	}
}

double compute_Error(f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	feed_forward(ai,input);
	double Error = 0;
	f_Matrix_t* Local_Error = f_Matrix_constructor(ai->layers_layout[ai->layers-1],1);
	f_Matrix_sub(ai->nodes[ai->layers-1],expectation,Local_Error);
	f_Matrix_sign_squared(Local_Error,Local_Error);
	for (int i=0;i<ai->layers_layout[ai->layers-1];i++) {
		Error+=f_Matrix_get(Local_Error,i,0)/ai->layers_layout[ai->layers-1];
	}
	return Error;
}

int AI_Train(f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	AI_t* gradients = create_ai(ai->layers_layout,ai->layers);
	feed_forward(ai,input);
	f_Matrix_sub(ai->nodes[ai->layers-1],expectation,gradients->nodes[gradients->layers-1]);
	f_Matrix_multiply_scalar(gradients->nodes[gradients->layers-1],2.f/gradients->layers_layout[gradients->layers-1]);
	for (int i = ai->layers-1;i>0;i--) {
		for (int y = 0;y<ai->layers_layout[i-1];y++) {
			for(int j =0;j<ai->layers_layout[i];j++){
				f_Matrix_set(ai->biases[i-1],y,0,f_Matrix_get(ai->biases[i-1],y,0)-f_Matrix_get(gradients->nodes[i],y,0)*LEARNING_RATE);
				f_Matrix_set(gradients->weights[i-1],j,y,f_Matrix_get(gradients->nodes[i],j,0));
				f_Matrix_set(gradients->nodes[i-1],y,j,f_Matrix_get(ai->weights[i-1],j,y)*leaky_relu_derivative(f_Matrix_get(gradients->nodes[i],j,0)));
				f_Matrix_set(ai->weights[i-1],j,y,f_Matrix_get(ai->weights[i-1],j,y)-f_Matrix_get(gradients->weights[i-1],j,y)*LEARNING_RATE);
			}
		}
	}
	free(gradients);
	//destroy_ai(gradients);
	return 0;
}

f_Matrix_t** create_weights(int* layers_layout,int layers){
	f_Matrix_t** weights =(f_Matrix_t**) malloc((layers-1)*sizeof(f_Matrix_t));
	for(int i = 0;i<layers-1;i++){
		weights[i] = f_Matrix_constructor(layers_layout[i+1],layers_layout[i]);
		for(int y =0;y<layers_layout[i];y++){
			for (int j=0;j<layers_layout[i+1];j++) {
			f_Matrix_set(weights[i],j,y,1.f);
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
	f_Matrix_t** biases = create_biases(layers_layout,layers);
	ai->biases = biases;
	f_Matrix_t** nodes = create_nodes(layers_layout,layers);
	ai->nodes = nodes;
	return ai;
}

int destroy_ai(AI_t* ai){
	for(int i = 0;i<ai->layers-1;i++){
		f_Matrix_destructor(ai->weights[i]);
		f_Matrix_destructor(ai->biases[i]);
	}
	for (int i=0;i<ai->layers;i++) {
		f_Matrix_destructor(ai->nodes[i]);
	}
	free(ai->weights);
	free(ai->biases);
	free(ai->nodes);
	free(ai->layers_layout);
	free(ai);
	return 0;
}

