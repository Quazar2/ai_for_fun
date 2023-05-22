#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Matrix.h"

#ifndef LEARNING_RATE
#define LEARNING_RATE 0.0001f
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

void feed_forward(AI_t* ai,f_Matrix_t* input){
	if ((*input).w==(*ai).layers_layout[0]) {
		f_Matrix_t** nodes = ai->nodes;
		nodes[0]=input;
		for (int i=0;i<(*ai).layers-1;i++) {
			f_Matrix_multiply(nodes[i],(*ai).weights[i],nodes[i+1]);
			f_Matrix_add(nodes[i+1],ai->biases[i],nodes[i+1]);
			f_Matrix_ceil_positive(nodes[i+1]);
		}
	}else {
		printf("Error : Size of input inequal to input_size of the ai!");
	}
}

int AI_Train(f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	AI_t* gradients = create_ai(ai->layers_layout,ai->layers);
	feed_forward(ai,input);
	double Error = 0;
	f_Matrix_t* Local_Errors = gradients->nodes[gradients->layers-1];
	f_Matrix_sub(ai->nodes[ai->layers-1],expectation,Local_Errors);
	f_Matrix_sign_squared(Local_Errors,Local_Errors);
	for(int i = 0;i<ai->layers_layout[ai->layers-1]-1;i++){
		Error += f_Matrix_get(Local_Errors,i,0)/ai->layers_layout[ai->layers-1];
	}
	for (int i = ai->layers-1;i>=0;i--) {
		for (int y = 0;y<ai->layers_layout[i-1];y++) {
			for(int j =0;j<ai->layers_layout[i];j++){
				f_Matrix_set(gradients->weights[i-1],j,y,f_Matrix_get(ai->nodes[i-1],y,0)*f_Matrix_get(gradients->nodes[i],j,0));
				f_Matrix_set(gradients->nodes[i-1],y,j,f_Matrix_get(ai->weights[i-1],j,y)*f_Matrix_get(gradients->nodes[i],j,0));
				f_Matrix_set(gradients->biases[i],j,0,f_Matrix_get(gradients->nodes[i],j,0));
				f_Matrix_set(ai->weights[i-1],j,y,f_Matrix_get(ai->weights[i-1],j,y)-f_Matrix_get(gradients->weights[i-1],j,y)*LEARNING_RATE);
				f_Matrix_set(ai->biases[i],j,y,f_Matrix_get(ai->biases[i],j,y)-f_Matrix_get(gradients->biases[i],j,0)*LEARNING_RATE);
			}
		}
	}
}

f_Matrix_t** create_weights(int* layers_layout,int layers){
	f_Matrix_t** weights =(f_Matrix_t**) malloc((layers-1)*sizeof(f_Matrix_t));
	for(int i = 0;i<layers-1;i++){
		weights[i] = f_Matrix_constructor(layers_layout[i+1],layers_layout[i]);
		for(int y =0;y<layers_layout[i];y++){
			for (int j=0;j<layers_layout[i+1];j++) {
			f_Matrix_set(weights[i],j,y,(1500-(rand()%2000))/2000);
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
			f_Matrix_set(biases[i],y,0,(10-(rand()%20)));
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
	for(int i = 0;i<(*ai).layers-1;i++){
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

