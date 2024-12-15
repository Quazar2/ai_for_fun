#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Matrix.h"

#ifndef LEARNING_RATE
#define LEARNING_RATE 0.5f
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

float l_Relu(float v){
	if(v>0){
	return v;
	}else{
	return 0.1*v;
	}
}

float l_Relu_derivative(float v){
	if(v>0){
		return 1;
	}else{
		return 0.1;
	}
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

typedef struct Dataset{
	int length;
	f_Matrix_t** inputs;
	f_Matrix_t** outputs;
} Dataset_t;

typedef struct Convolutive_AI{
	int* layers_layout;
	int layers;
	f_Matrix_t** weights;
	f_Matrix_t** nodes;
	f_Matrix_t** d_nodes;
	float (*activation)(float);
	float (*activationDerivative)(float);
} Convo_AI_t;
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

f_Matrix_t** create_weights(int* layers_layout,int layers){
	f_Matrix_t** weights =(f_Matrix_t**) malloc((layers-1)*sizeof(f_Matrix_t));
	for(int i = 0;i<layers-1;i++){
		weights[i] = f_Matrix_constructor(layers_layout[i+1],layers_layout[i]);
		for(int y =0;y<layers_layout[i];y++){
			for (int j=0;j<layers_layout[i+1];j++) {
			f_Matrix_set(weights[i],j,y,0.5/layers_layout[i+1]);
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
int AI_Train(f_Matrix_t* input,AI_t* ai,f_Matrix_t* expectation){
	AI_t* gradient =(AI_t*) malloc(sizeof(AI_t));
	gradient->layers_layout = ai->layers_layout;
	gradient->layers = ai->layers;
	f_Matrix_t** nodes = create_nodes(ai->layers_layout,ai->layers);
	gradient->nodes = nodes;
	feed_forward(ai,input);
	f_Matrix_sub(ai->nodes[ai->layers-1],expectation,gradient->nodes[gradient->layers-1]);
	f_Matrix_multiply_scalar(gradient->nodes[gradient->layers-1],2.f/gradient->layers_layout[gradient->layers-1]);
	for (int i = ai->layers-1;i>0;i--) {
		for (int y =0;y<gradient->layers_layout[i];y++){
			f_Matrix_set(gradient->nodes[i],y,0,f_Matrix_get(gradient->nodes[i],y,0)*f_Matrix_get(ai->d_nodes[i],y,0));
			for (int z =0;z<gradient->layers_layout[i-1];z++){
				f_Matrix_set(gradient->nodes[i-1],z,0,f_Matrix_get(gradient->nodes[i-1],z,0)+f_Matrix_get(gradient->nodes[i],y,0)*f_Matrix_get(ai->weights[i-1],y,z));
				f_Matrix_set(ai->weights[i-1],y,z,f_Matrix_get(ai->weights[i-1],y,z)-f_Matrix_get(gradient->nodes[i],y,0)*f_Matrix_get(ai->nodes[i-1],z,0)*LEARNING_RATE);
			}
		}

	}
	free(gradient);
	return 0;
}

AI_t* create_ai(int* layers_layout,int layers){
	AI_t* ai =(AI_t*) malloc(sizeof(AI_t));
	(*ai).layers_layout = layers_layout;
	(*ai).layers = layers;
	f_Matrix_t** weights = create_weights(ai->layers_layout,layers);
	(*ai).weights = weights;
	f_Matrix_t** nodes = create_nodes(layers_layout,layers);
	ai->nodes = nodes;
	f_Matrix_t** d_nodes = create_nodes(layers_layout,layers);
	ai->d_nodes = d_nodes;
	ai->activation = *l_Relu;
	ai->activationDerivative = *l_Relu_derivative;
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
	AI_t* ai =(AI_t*) calloc(1,sizeof(AI_t));
	FILE* file = fopen(n,"rb");
	fread(&(ai->layers),sizeof(int),1,file);
	ai->layers_layout =(int *) calloc(ai->layers,sizeof(int));
	fread(ai->layers_layout,sizeof(int),ai->layers,file);
	ai->nodes = create_nodes(ai->layers_layout,ai->layers);
	ai->d_nodes = create_nodes(ai->layers_layout,ai->layers);
	ai->activation = activation;
	ai->activationDerivative = activation_derrivative;
	ai->weights =(f_Matrix_t**) calloc(ai->layers-1,sizeof(f_Matrix_t));
	for(int i =0;i<ai->layers-1;i++){
		ai->weights[i] = f_Matrix_constructor(ai->layers_layout[i+1],ai->layers_layout[i]);
		fread(ai->weights[i]->ptr,sizeof(float),ai->layers_layout[i+1]*ai->layers_layout[i],file);
	}
	fclose(file);
	return ai;
}

Dataset_t* Dataset_constructor(int length,f_Matrix_t** inputs,f_Matrix_t** outputs){
	Dataset_t* data =(Dataset_t*) calloc(1,sizeof(Dataset_t));
	data->length = length;
	data->inputs = inputs;
	data->outputs = outputs;
	return data;
}

int write_dataset(Dataset_t* data,const char* n){
	FILE* file = fopen(n,"wb+");
	fwrite(&(data->length),sizeof(int),1,file);
	unsigned int size[4] = {data->inputs[0]->w,data->inputs[0]->h,data->outputs[0]->w,data->outputs[0]->h};
	fwrite(size,sizeof(int),4,file);
	for(int i=0;i<data->length;i++){
		fwrite(data->inputs[i]->ptr,sizeof(float),data->inputs[i]->w*data->inputs[i]->h,file);
		fwrite(data->outputs[i]->ptr,sizeof(float),data->outputs[i]->w*data->outputs[i]->h,file);
	}
	fclose(file);
	return 1;
}

Dataset_t* read_dataset(const char* n){
	FILE* file = fopen(n,"rb");
	int length = 0;
	fread(&(length),sizeof(int),1,file);
	unsigned int size[4];
	fread(size,sizeof(int),4,file);
	f_Matrix_t** inputs =(f_Matrix_t**) calloc(length,sizeof(f_Matrix_t*));
	f_Matrix_t** outputs =(f_Matrix_t**) calloc(length,sizeof(f_Matrix_t*));
	for(int i=0;i<length;i++){
		inputs[i] = f_Matrix_constructor(size[0],size[1]);
		outputs[i] = f_Matrix_constructor(size[2],size[3]);
		fread(inputs[i]->ptr,sizeof(float),inputs[i]->w*inputs[i]->h,file);
		fread(outputs[i]->ptr,sizeof(float),outputs[i]->w*outputs[i]->h,file);
	}
	fclose(file);
	return Dataset_constructor(length,inputs,outputs);
}
Dataset_t* merge_datasets(int n,Dataset_t** datasets){
	int size = 0;
	for(int i=0;i<n;i++){
		size += datasets[i]->length;
	}
	int current_n =0;
	int y =0;
	f_Matrix_t** inputs = (f_Matrix_t**)calloc(size,sizeof(f_Matrix_t*));
	f_Matrix_t** outputs = (f_Matrix_t**)calloc(size,sizeof(f_Matrix_t*));
	for(int i=0;i<size;i++){
		if(y>=datasets[current_n]->length){
			y-=datasets[current_n]->length;
			current_n+=1;
		}
		inputs[i] = datasets[current_n]->inputs[y];
		outputs[i] = datasets[current_n]->outputs[y];
		y++;
	}
	return Dataset_constructor(size,inputs,outputs);
}
