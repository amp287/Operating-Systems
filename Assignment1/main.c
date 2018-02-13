#include<stdio.h>
#include<stdlib.h>
#include<string.h>

enum Algo {fcfs, sjf, rr};

typedef struct PROCESS{
	char *name;
	int arrival;
	int burst;
	struct PROCESS *next;
} PROCESS;

int process_count;
int run_for;
int algorithm;
int quantum;
PROCESS *arrived_queue;
PROCESS *queue;

int parse(FILE *in){
	char command[50];
	char name[50];
	int index = 0;
	int arrival;
	int burst;
	arrived_queue = NULL;
	queue = NULL;

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
		} else if(strcmp(command, "quantum") == 0){
				fscanf(in, "%d", &quantum);
		} else if(strcmp(command, "#") == 0){
			//skip comments might change this to make sure we get long comments
			fgets(command, 49, in);
		} else if(strcmp(command, "process") == 0){
			PROCESS *new_process = malloc(sizeof(PROCESS));
			PROCESS *itr, *prev = NULL;

			new_process->next = NULL;

			fscanf(in, "%s %s", command, name);
			new_process->name = malloc(strlen(name) + 1);
			strcpy(new_process->name, name);
			fscanf(in, "%s %d", command, &new_process->arrival);
			fscanf(in, "%s %d", command, &new_process->burst);

			if(arrived_queue == NULL){
				arrived_queue = new_process;
				continue;
			}

			for(itr = arrived_queue; itr != NULL; itr = itr->next){
				if(itr->arrival > new_process->arrival){
					new_process->next = itr;
					if(prev == NULL)
						arrived_queue = new_process;
					else
						prev->next = new_process;
						break;
				}
				prev = itr;
			}

			if(itr == NULL)
				prev->next = new_process;

		} else if(strcmp(command, "end") == 0){
			break;
		}
	}
}

int main(int argv, char *argc[]){
	FILE *in;
	PROCESS *itr;
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

	for(itr = arrived_queue; itr != NULL; itr = itr->next){
		printf("Name: %s ", itr->name);
		printf("Arrival: %d ", itr->arrival);
		printf("Burst: %d\n", itr->burst);
	}

	return 0;

}
