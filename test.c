#include "ai.h"
#include "Matrix.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

int main(int argc,char** argv){
	srand(time(NULL));
	int input[] = {0,1};
	int output[] = {6,10};
	AI_t* ai = create_ai(20,2,2,input,output);
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
			printf("%lf\n\n",compute_Error(3,inputs[1],ai,outputs[1]));
			print_ai(ai);
			printf("\n\n");
			AI_Train(3,inputs[1],ai,outputs[1]);
	}

	return 0;
}
