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
	f_Matrix_set(expectation,shape_type,1,1);
	AI_Train(shape,ai,expectation);
	f_Matrix_destructor(expectation);
	f_Matrix_destructor(shape);
}

int main(int argc,char** argv){
	srand(time(NULL));
	return 0;
}
