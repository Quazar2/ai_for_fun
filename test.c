#include "ai.h"
#include "Matrix.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>


f_Matrix_t* create_random_circle(int s){
	f_Matrix_t* m = f_Matrix_constructor(s*s,1);
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

f_Matrix_t* create_random_square(int s){
	f_Matrix_t* m = f_Matrix_constructor(s*s,1);
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

int train_with_random_shape(AI_t* ai){
	f_Matrix_t* shape;
	f_Matrix_t* expectation;
	int shape_type;
	if(rand()%2==0){
		shape = create_random_circle(100);
		shape_type=0;
	}
	else{
		shape = create_random_square(100);
		shape_type=1;
	}
	expectation = f_Matrix_constructor(2,1);
	f_Matrix_set(expectation,shape_type,0,1);
	AI_Train(shape,ai,expectation);
	f_Matrix_destructor(expectation);
	f_Matrix_destructor(shape);
}

int main(int argc,char** argv){
	//srand(time(NULL));
	int layout[] = {2,2};
	AI_t* ai = create_ai(layout,2);
	int times;
	scanf("%d",&times);
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
	for (int i=0;i<times;i++) {
		for (int y =0;y<4;y++) {
			printf("%lf",compute_Error(inputs[y],ai,outputs[y]));
			AI_Train(inputs[y],ai,outputs[y]);
		}
	}

	return 0;
}
