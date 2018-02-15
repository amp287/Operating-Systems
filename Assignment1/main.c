#include<stdio.h>
#include<stdlib.h>
#include<string.h>

enum Algo {fcfs, sjf, rr};

static char *algo_strings[] = {"First Come First Serve", "Shortest Job First", "Round Robin"};

typedef struct PROCESS{
	char *name;
	int arrival;
	// time to completion
	int burst;
	// time a process has ran
	int time_ran;
	//time a process has spent waiting
	int time_waiting;
	struct PROCESS *next;
} PROCESS;

int process_count;
int run_for;
int algorithm;
int quantum;
PROCESS *arrived_queue;
PROCESS *queue;
PROCESS *finished_queue;

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
			new_process->time_ran = 0;
			new_process->time_waiting = 0;

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

int First_Come_First_Serve()
{
	PROCESS *itr;
	FILE *out;
	out = fopen("processes.out", "w");
	int i;
	fprintf(out, "%d processes\n", process_count);
	fprintf(out, "Using: %s\n\n", algo_strings[algorithm]);

	for(i = 0; i < run_for; i++)
	{
		if(arrived_queue != NULL && arrived_queue->arrival == i)
		{
			if(queue == NULL)
			{
				queue = arrived_queue;
				arrived_queue = arrived_queue->next;
				queue->next = NULL;
				fprintf(out, "Time %d: %s arrived\n", i, queue->name);
				fprintf(out, "Time %d: %s selected (burst %d)\n", i, queue->name, queue->burst);
			}

			else
			{
				for(itr = queue; itr->next != NULL; itr = itr->next);
				itr->next = arrived_queue;
				arrived_queue = arrived_queue->next;
				itr->next->next = NULL;
				fprintf(out, "Time %d: %s arrived\n", i, itr->next->name);
			}
		}

		if(queue && queue->burst == 0)
		{
			fprintf(out, "Time %d: %s finished\n", i, queue->name);
			if(!finished_queue)
			{
				finished_queue = queue;
				if(queue)
					queue = queue->next;
				else
					queue = NULL;
			}

			else
			{
				for(itr = finished_queue; itr->next != NULL; itr = itr->next);
				itr->next = queue;
				if(queue)
					queue = queue->next;
				else
					queue = NULL;
				itr->next = NULL;
			}

			if(queue)
				fprintf(out, "Time %d: %s selected (burst %d)\n", i, queue->name, queue->burst);
		}

		if(queue)
		{
			queue->time_ran++;
			queue->burst--;
			for(itr = queue->next; itr != NULL; itr = itr->next)
				itr->time_waiting++;
		}
		else
			fprintf(out, "Time %d: Idle\n", i);
	}

	fprintf(out, "Finished at time %d\n\n", run_for);

	for(itr = finished_queue; itr != NULL; itr = itr->next)
		fprintf(out, "%s wait %d turnaround %d\n", itr->name, itr->time_waiting, itr->time_ran + itr->time_waiting);
	fclose(out);
	return 0;
}
int round_robin(){
	PROCESS *itr;
	int i;
	FILE *out;
	out = fopen("processes.out", "w");

	fprintf(out, "%d processes\n", process_count);
	fprintf(out, "Using: %s\n", algo_strings[algorithm]);
	fprintf(out, "Quantum: %d\n\n", quantum);

	for(i = 0; i < run_for; i++) {
			//Arrival
		if(arrived_queue != NULL && arrived_queue->arrival == i) {
			if(queue == NULL) {
				queue = arrived_queue;
				arrived_queue = arrived_queue->next;
				queue->next = NULL;
				fprintf(out, "Time %d: %s arrived\n", i, queue->name);
				fprintf(out, "Time %d: %s selected (burst %d)\n", i, queue->name, queue->burst);
			} else {
				for(itr = queue; itr->next != NULL; itr = itr->next);
				itr->next = arrived_queue;
				arrived_queue = arrived_queue->next;
				itr->next->next = NULL;
				fprintf(out, "Time %d: %s arrived\n", i, itr->next->name);
			}
		}

		// Process finished
		if(queue && queue->burst == 0){
			fprintf(out, "Time %d: %s finished\n", i, queue->name);
			//add finsihed process to finished queue
			if(!finished_queue) {
				finished_queue = queue;
				if(queue) queue = queue->next;
				else queue = NULL;
			} else {
				for(itr = finished_queue; itr->next != NULL; itr = itr->next);
				itr->next = queue;
				if(queue) queue = queue->next;
				else queue = NULL;
				itr->next = NULL;
			}

			if(queue)
				fprintf(out, "Time %d: %s selected (burst %d)\n", i, queue->name, queue->burst);
		}
		//Interrupt
		else if(queue && queue->time_ran != 0 && !(queue->time_ran % quantum)){
			if(queue->next != NULL){
				for(itr = queue; itr->next != NULL; itr = itr->next);
				itr->next = queue;
				queue = queue->next;
				itr->next->next = NULL;
			}
			fprintf(out, "Time %d: %s selected (burst %d)\n", i, queue->name, queue->burst);
		}

		if(queue) {
			queue->time_ran++;
			queue->burst--;
			for(itr = queue->next; itr != NULL; itr = itr->next)
				itr->time_waiting++;
		}
		else fprintf(out, "Time %d: Idle\n", i);

	}

	fprintf(out, "Finished at time %d\n\n", run_for);

	for(itr = finished_queue; itr != NULL; itr = itr->next)
		fprintf(out, "%s wait %d turnaround %d\n", itr->name, itr->time_waiting, itr->time_ran + itr->time_waiting);
	fclose(out);
	return 0;
}

int main(int argv, char *argc[]){
	FILE *in;
	PROCESS *itr;
	arrived_queue = NULL;
	queue = NULL;

	if(argv < 2){
		printf("Error: Invalid Arguments");
		return -1;
	}

	in = fopen(argc[1], "r");
	parse(in);

	switch(algorithm){
		case 0:
			First_Come_First_Serve();
			break;
		case 1:
			break;
		case 2:
			round_robin();
	}

	return 0;

}
