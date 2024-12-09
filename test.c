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

Dataset_t* create_Shapes(int shapes){
	f_Matrix_t** m = malloc(shapes*sizeof(f_Matrix_t*));
	f_Matrix_t** expectations = malloc(shapes*sizeof(f_Matrix_t*));
	double error =0;
	double average_error = 0.;
	for(int i=0;i<shapes;i++){
		m[i]=f_Matrix_constructor(shape_size*shape_size,1);
		if(rand()%2==0){
			m[i] = create_random_circle(shape_size,m[i]);
			expectations[i]=f_Matrix_constructor(2,1);
			f_Matrix_set(expectations[i],0,0,1);
		}
		else{
			m[i] = create_random_square(shape_size,m[i]);
			expectations[i]=f_Matrix_constructor(2,1);
			f_Matrix_set(expectations[i],1,0,1);
		}
	}
	return Dataset_constructor(shapes,m,expectations);
}

double train_with_random_shape(AI_t* ai,Dataset_t* data,unsigned int times){
	srand(time(NULL));
	f_Matrix_t** m = data->inputs;
	f_Matrix_t** expectation = data->outputs;
	int shapes = data->length;
	double error =0;
	double average_error = 0.;
	for(int i=0;i<times;i++){
		error =0;
		for(int j=0;j<shapes;j++){
			AI_Train(m[j],ai,expectation[j]);
			error += compute_Error(m[j],ai,expectation[j]);
		}
		printf("%.12lf\n",error/shapes);
	}
	return average_error/times;
}

int main(int argc,char** argv){
	srand(time(NULL));
	int* layout = malloc(4*sizeof(int));
	layout[0] = shape_size*shape_size;
	layout[1] = 20;
	layout[2] = 15;
	layout[3] = 2;
	char r;
	char good = 0;
	Dataset_t* data = NULL;
	AI_t* ai = NULL;
	int response = 0;
	char* s_ai = calloc(64,sizeof(char));
	char* s_data = calloc(64,sizeof(char));
	int nb_data =0;
	Dataset_t** datas;
	while(good==0){
		printf("\n- 1 load an ai\n- 2 load a dataset\n- 3 merge datasets\n- 4 Run and create unloaded things\n");
		scanf("%d",&response);
		switch(response){
			case 1:
				scanf("%d",&response);
				sprintf(s_ai,"%d.ai",response);
				ai = read_ai(s_ai,*l_Relu,*l_Relu_derivative);
				break;
			case 2:
				scanf("%d",&response);
				sprintf(s_data,"%d.data",response);
				data = read_dataset(s_data);
				break;
			case 3:
				printf("nb datasets : ");
				scanf("%d",&nb_data);
				datas =(Dataset_t**) calloc(nb_data,sizeof(Dataset_t*));
				for(int i =0;i<nb_data;i++){
					printf("dataset num ");
					scanf("%d",&response);
					sprintf(s_data,"%d.data",response);
					datas[i] = read_dataset(s_data);
				}
				data = merge_datasets(nb_data,datas);
				printf("vers : ");
				scanf("%d",&response);
				sprintf(s_data,"%d.data",response);
				write_dataset(data,s_data);
				data= NULL;
				break;
			case 4:
				if(data==NULL){
					data = create_Shapes(1000);
				}
				if(ai==NULL){
					ai = create_ai(layout,4);
				}
				good=1;
				break;
		}
	}
	clock_t begin = clock();
	if(argc>1){
		int times = 0;
		sscanf(argv[1],"%d",&times);
		train_with_random_shape(ai,data,times);
	}else{
		train_with_random_shape(ai,data,10);
	}
	double time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
	printf("Ran in %dh - %dm - %ds\n",((int)time_spent/3600)%3600,((int)time_spent/60)%60,((int)time_spent)%60);
	good =0;
	while(good==0){
		printf("- 1 save an ai\n- 2 save a dataset\n- 3 merge datasets(can't save after that)\n- 4 End\n");
		scanf("%d",&response);
		switch(response){
			case 1:
				scanf("%d",&response);
				sprintf(s_ai,"%d.ai",response);
				write_ai(ai,s_ai);
				break;
			case 2:
				scanf("%d",&response);
				sprintf(s_data,"%d.data",response);
				write_dataset(data,s_data);
				break;
			case 3:
				if(data!=NULL){
					for(int i=0;i<data->length;i++){
						f_Matrix_destructor(data->inputs[i]);
						f_Matrix_destructor(data->outputs[i]);
					}
				}
				printf("nb datasets : ");
				scanf("%d",&nb_data);
				datas =(Dataset_t**) calloc(nb_data,sizeof(Dataset_t*));
				for(int i =0;i<nb_data;i++){
					printf("dataset num ");
					scanf("%d",&response);
					sprintf(s_data,"%d.data",response);
					datas[i] = read_dataset(s_data);
				}
				data = merge_datasets(nb_data,datas);
				printf("vers : ");
				scanf("%d",&response);
				sprintf(s_data,"%d.data",response);
				write_dataset(data,s_data);
				data= NULL;
				break;
			case 4:
				if(data!=NULL){
					for(int i=0;i<data->length;i++){
						f_Matrix_destructor(data->inputs[i]);
						f_Matrix_destructor(data->outputs[i]);
					}
				}
				good=1;
				destroy_ai(ai);
				free(s_ai);
				free(s_data);
				free(data);
				return 0;
				break;
		}
	}
	return 0;
}
