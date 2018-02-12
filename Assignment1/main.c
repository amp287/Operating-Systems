#include<stdio.h>
#include<stdlib.h>
#include<string.h>

enum Algo {fcfs, sjf, rr};

typedef struct PROCESS{
	char *name;
	int arrival;
	int burst;
} PROCESS;

int process_count;
int run_for;
int algorithm;
int quantum;

PROCESS *queue;

int parse(FILE *in){
	char command[50];
	char name[50];
	int index = 0;
	int arrival;
	int burst;
	while(fscanf(in, "%s", command)){
		if(strcmp(command, "processcount") == 0){
			fscanf(in, "%d", &process_count);
			queue = malloc(sizeof(PROCESS) * process_count);
		} else if(strcmp(command, "runfor") == 0){
			fscanf(in, "%d", &run_for);		
		} else if(strcmp(command, "use") == 0){
			fscanf(in, "%s", command);
			if(strcmp(command, "fcfs") == 0)
				algorithm = fcfs;
			else if(strcmp(command, "sjf") == 0)
				algorithm = sjf;
			else if(strcmp(command, "rr") == 0)
				algorithm = rr;	
		} else if(strcmp(command, "#") == 0){
			//skip comments might change this to make sure we get long comments
			fgets(command, 49, in);  		
		} else if(strcmp(command, "process") == 0){
			
			fscanf(in, "%s %s", command, name);
			queue[index].name = malloc(strlen(name) + 1);
			strcpy(queue[index].name, name);
			fscanf(in, "%s %d", command, &queue[index].arrival);
			fscanf(in, "%s %d", command, &queue[index].burst);
			index++;
		} else if(strcmp(command, "end") == 0){
			break;		
		}
	}
}

int main(int argv, char *argc[]){
	FILE *in;
	int i;
	if(argv < 2){
		printf("Error: Invalid Arguments");
		return -1;	
	}
	
	in = fopen(argc[1], "r");
	
	parse(in);

	printf("Process Count: %d\n", process_count);
	printf("Run for: %d\n", run_for);
	printf("Use: %d\n", algorithm);
	printf("Quantum: %d\n", quantum);
	
	printf("Processes\n");

	for(i = 0; i < process_count; i++){
		printf("Name: %s\n", queue[i].name);
		printf("Arrival: %d\n", queue[i].arrival);
		printf("Burst: %d\n", queue[i].burst);
	}
	
	return 0;

}
