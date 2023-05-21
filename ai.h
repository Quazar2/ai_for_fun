#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Matrix.h"

typedef struct AI{
	int* layers_layout;
	int layers;
	f_Matrix_t** weights;
	f_Matrix_t** biases;
	f_Matrix_t** nodes;
} AI_t;

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
	feed_forward(ai,input);
	int Error = 0;
	f_Matrix_t* Local_Errors = f_Matrix_constructor(ai->layers_layout[ai->layers-1],1);
	f_Matrix_sign_squared(ai->nodes[ai->layers-1],Local_Errors);
	for(int i = 0;i<ai->layers_layout[ai->layers-1]-1;i++){
		Error+=f_Matrix_get(Local_Errors,i,0);
	}
	double Partial_derivative_to_error = 0;
	for (int i = ai->layers-1;i>0;i++) {
		for (int y = 0;y<ai->layers_layout[i];i++) {
			//UNDER CONSTRUCTION
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

