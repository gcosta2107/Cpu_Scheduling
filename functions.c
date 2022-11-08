#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

#define MAX_LINE 80

#define INACTIVE 0
#define ACTIVE 1

//recebe cada linha e trata os " "
void parse_input(char *process, char **parsed_process)
{
    char *separator;
    int arg_count = 0;

    int size =  sizeof process / sizeof process[0];

    separator = strtok(process, " ");
    
    while( separator != NULL) {
        parsed_process[arg_count] = malloc(size * sizeof(char));
		parsed_process[arg_count] = strdup(separator);
        separator = strtok(NULL, " ");
        arg_count++;
    }
}

int file_read(char *filename, char **arguments)
{
    char* readline = NULL;
    int cont = 0;
    size_t len = 0;

    FILE *fp = fopen (filename, "r");

    if(!fp){
        printf("Arquivo não existe\n");
        exit(1);
    }

    while ((getline(&readline, &len, fp)) != -1) {
        
        char *separator = strtok(readline, "\n");
        arguments[cont] = strdup(separator);
        
        cont++;
    }
    
    fclose(fp);

    return cont;
}

Process create_process(char** struct_fields)
{
    Process process;
    
    process.name = struct_fields[0];
    process.period = atoi(struct_fields[1]);
    process.exec_time = atoi(struct_fields[2]);

    process.status = ACTIVE;
    process.priority = 0;
    process.remain_exec_time = 0;
    process.executed_units = 0;
    process.remain_from_hold = 0;
    process.in_hold = 0;

    process.data.lost_deadlines = 0;
    process.data.times_executed = 0;
    process.data.times_killed = 0;

    return process;
}

void priority_assignment(Process all_processes[], int num_processes){

    for (int i = 0; i < num_processes; i++){
        all_processes[i].priority = num_processes;
    }

    for (int i = 0; i < num_processes; i++) {
        for (int j = i+1; j < num_processes; j++) {
            if (all_processes[i].period > all_processes[j].period) {
                all_processes[i].priority--;
            }
            else if (all_processes[i].period == all_processes[j].period){
                all_processes[j].priority = all_processes[i].priority;
            }
            else {
                all_processes[j].priority--;
            }
        }
    }
}

void printList(Process processes[], int size)
{
    for (int i = 0; i < size; i++){
        printf("Nome: %s ", processes[i].name);
        printf("Periodo: %d ", processes[i].period);
        printf("Tempo de execucao: %d ", processes[i].exec_time);
    }
    printf("\n");
}

void printPriority(Process processes[], int size){
    for(int i = 0; i < size; i++){
        printf("%s: ", processes[i].name);
        printf("Prioridade: %d ", processes[i].priority);
    }
    printf("\n");
}