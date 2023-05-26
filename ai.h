#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Matrix.h"

#ifndef LEARNING_RATE
#define LEARNING_RATE 0.01f
#endif



typedef struct AI{
	f_Matrix_t* nodes;
	f_Matrix_t* weights;
	int* inputs;
	int* outputs;
	int node;
	int n_inputs;
	int n_outputs;
} AI_t;

AI_t* create_ai(int node,int out,int in,int* inputs,int* outputs);
int AI_Train(int times,f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation);
void feed_forward(int times,AI_t* ai,f_Matrix_t* input);
int destroy_ai(AI_t* ai);

void feed_forward(int times,AI_t* ai,f_Matrix_t* input){
	for(int i =0;i<ai->n_inputs;i++){
		f_Matrix_set(ai->nodes,ai->inputs[i],0,f_Matrix_get(input,i,0));
	}
	for (int i =0;i<times;++i) {
	f_Matrix_t* swap = f_Matrix_constructor(ai->node,1);
	f_Matrix_multiply(ai->nodes,ai->weights,swap);
	//TODO inserer une fonction d'activation ici si tu en veux une.
	ai->nodes = swap;
	}
}

double compute_Error(int times,f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	double Error = 0;
	float expect = 0;
	f_Matrix_t* Local_Error = f_Matrix_constructor(ai->n_outputs,1);
	for (int i = 0;i<ai->n_outputs;i++) {
		expect =f_Matrix_get(expectation,i,0);
		f_Matrix_set(Local_Error,i,0,expect-f_Matrix_get(ai->nodes,ai->outputs[i],0));
	}
	f_Matrix_sign_squared(Local_Error,Local_Error);
	for (int i=0;i<Local_Error->w;i++) {
		Error+=f_Matrix_get(Local_Error,i,0)/Local_Error->w;
	}
	return Error;
}

int print_ai(AI_t* ai){
	f_Matrix_print(ai->nodes);
	printf("\n\n");
	f_Matrix_print(ai->weights);
}


int AI_Train(int times,f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	feed_forward(times,ai,input);
	float* correction = calloc(times*ai->node,sizeof(float));
	for(int i=0;i<ai->n_outputs;i++){
		for(int y=0;y<ai->node;y++){
			correction[y] = f_Matrix_get(ai->nodes,y,0)*f_Matrix_get(ai->nodes,ai->outputs[i],0)*(2.f/ai->n_outputs);
			f_Matrix_set(ai->weights,i,y,f_Matrix_get(ai->weights,i,y)+correction[y]*LEARNING_RATE);
			}
		}
		for (int i=1;i<times;i++) {
			for (int j =0;j<ai->node;j++) {
				for (int k =0;k<ai->node;k++) {
					correction[j+i*ai->node] = correction[k+(i-1)*ai->node]*f_Matrix_get(ai->weights,k,j);
					f_Matrix_set(ai->weights,k,j,f_Matrix_get(ai->weights,k,j)+correction*LEARNING_RATE);
				}
			}
		}
	return 0;
}

f_Matrix_t* create_weights(int node){
	f_Matrix_t* weights = f_Matrix_constructor(node,node);
	for(int i=0;i<node;i++){
		for(int y=0;y<node;y++){
		f_Matrix_set(weights,i,y,((double)rand())/RAND_MAX/node);
		}
	}
	return weights;
}
f_Matrix_t* create_biases(int node){
		f_Matrix_t* biases = f_Matrix_constructor(node*node,1);
		for(int y =0;y<node*node;y++){
			f_Matrix_set(biases,y,0,0.5f);
		}
	return biases;
}
f_Matrix_t* create_nodes(int node){
	f_Matrix_t* nodes = f_Matrix_constructor(node,1);
	return nodes;
}
AI_t* create_ai(int node,int out,int in,int* inputs,int* outputs){
	AI_t* ai =(AI_t*) malloc(sizeof(AI_t));
	ai->node = node;
	ai->nodes=create_nodes(node);
	ai->weights=create_weights(node);
	ai->n_inputs=in;
	ai->n_outputs=out;
	ai->inputs = inputs;
	ai->outputs = outputs;
	return ai;
}

int destroy_ai(AI_t* ai){
	f_Matrix_destructor(ai->nodes);
	f_Matrix_destructor(ai->weights);
	free(ai);
	return 0;
}

