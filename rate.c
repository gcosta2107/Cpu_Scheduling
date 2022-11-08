#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

void parse_input(char *process, char **parsed_process);

int file_read(char *filename, char **arguments);

Process create_process(char** struct_fields);

void priority_assignment(Process all_processes[], int num_processes);

void printList(Process processes[], int size);

void printPriority(Process processes[], int size);

int main(int argc, char *argv[]){

    if(argc == 2){
        int total_time;

        char *all_lines[MAX_LINE]; //armazena as linhas do arquivo em uma lista
        char *struct_fields[3]; //inputs de cada processo do arquivo

        if (argv[1][strlen(argv[1]) - 1] != 't' || argv[1][strlen(argv[1]) - 2] != 'x' || argv[1][strlen(argv[1]) - 3] != 't' || argv[1][strlen(argv[1]) - 4] != '.') {
            printf("Não é um arquivo válido\n");
            exit(1);
        }

        int file_totalLines = file_read(argv[1], all_lines);
        int num_processes = file_totalLines - 1;

        Process all_processes[num_processes];

        for(int i = 0; i < file_totalLines; i++){
            if(i == 0){ //primeira linha = tempo total
                total_time = atoi(all_lines[0]);
            }
            else{
                parse_input(all_lines[i], struct_fields); //recebe as linhas e trata os espaços em branco
                Process process = create_process(struct_fields);
                all_processes[i-1] = process;
            }
        }

        // printf("Tempo total: %d\n", total_time);
        // printList(all_processes, num_processes);

        priority_assignment(all_processes, num_processes);

        // printPriority(all_processes, num_processes);

        int idle_time = 0;
        int current_process_copy = 0;

        FILE *output_file = fopen("rate_gcs.out","w");

        fprintf(output_file, "EXECUTION BY RATE\n");
        for(int t = 0; t < total_time; t++)
        {   
            int current_process = 0;

            //percorre a lista de processsos a cada unidade de tempo
            for(int i = 0; i < num_processes; i++){ 
                //verifica a ocorrencia do periodo
                if( t % all_processes[i].period == 0){ 
                    //lost deadlines
                    if ((all_processes[i].remain_exec_time > 0) && all_processes[i].priority == all_processes[current_process_copy].priority) {
                        if(all_processes[i].in_hold == 0){
                            int units_lost = all_processes[i].exec_time - all_processes[i].remain_exec_time;
                            all_processes[i].executed_units = all_processes[i].exec_time - units_lost;
                            fprintf(output_file, "[%s] for %i units - L\n", all_processes[i].name, all_processes[i].executed_units);
                            all_processes[i].data.lost_deadlines++;
                            all_processes[i].in_hold = 0;
                            all_processes[i].remain_exec_time = all_processes[i].exec_time;
                        }
                        else if(all_processes[i].in_hold == 1){
                            int units_lost = all_processes[i].remain_from_hold - all_processes[i].remain_exec_time;
                            all_processes[i].executed_units = all_processes[i].remain_from_hold - all_processes[i].remain_exec_time;
                            if(all_processes[i].remain_from_hold > 0){
                                fprintf(output_file, "[%s] for %d units - L\n", all_processes[i].name, all_processes[i].executed_units);
                            }
                            all_processes[i].in_hold = 0;
                            all_processes[i].remain_exec_time = all_processes[i].exec_time;
                        }
                        
                    }
                    //hold
                    else if(all_processes[current_process_copy].remain_exec_time > 0 && all_processes[i].priority > all_processes[current_process_copy].priority){
                        if(current_process_copy < num_processes){
                            if( t > 0){ 
                                if(all_processes[current_process_copy].in_hold == 0){
                                    all_processes[current_process_copy].executed_units = all_processes[current_process_copy].exec_time - all_processes[current_process_copy].remain_exec_time;
                                    if(all_processes[current_process_copy].executed_units > 0){
                                        fprintf(output_file, "[%s] for %d units - H\n", all_processes[current_process_copy].name, all_processes[current_process_copy].executed_units);
                                    }
                                    all_processes[current_process_copy].remain_from_hold = all_processes[current_process_copy].remain_exec_time;
                                    all_processes[current_process_copy].in_hold = 1;
                                }
                                else if(all_processes[current_process_copy].in_hold == 1){
                                    all_processes[current_process_copy].executed_units = all_processes[current_process_copy].remain_from_hold - all_processes[current_process_copy].remain_exec_time;
                                    if(all_processes[current_process_copy].remain_from_hold > 0){
                                        fprintf(output_file, "[%s] for %d units - H\n", all_processes[current_process_copy].name, all_processes[current_process_copy].executed_units);
                                    }
                                    all_processes[current_process_copy].remain_from_hold = all_processes[current_process_copy].remain_exec_time;
                                }
                            }
                        }
                    }

                    //debug
                    // printf("%d ", t);
                    // printf("%s ", all_processes[i].name);
                    // printf("%d ", all_processes[i].status);
                    // printf("%d ", all_processes[i].exec_time);
                    // printf("%d\n", all_processes[i].remain_exec_time);
                    
                    //valida caso um processo seja finalizado sem mesmo ter iniciado (ex: C encontra um lost deadline em p2.txt)
                    if(all_processes[i].status == ACTIVE && all_processes[i].remain_exec_time == all_processes[i].exec_time){
                        //ativa novo processo na ocorrencia do periodo, independentemente dele ser executado na mesma hora ou não
                        all_processes[i].status = ACTIVE;
                        all_processes[i].remain_exec_time = all_processes[i].exec_time;
                        all_processes[i].data.lost_deadlines++;
                    }
                    else{
                        all_processes[i].status = ACTIVE;
                        all_processes[i].remain_exec_time = all_processes[i].exec_time;
                    }

                    // debug
                    // printf("%d ", t);
                    // printf("%s ", all_processes[i].name);
                    // printf("%d ", all_processes[i].status);
                    // printf("%d ", all_processes[i].exec_time);
                    // printf("%d\n", all_processes[i].remain_exec_time);

                }

                //checa o processo ativo com maior prioridade naquela unidade de tempo
                if (all_processes[current_process].status) {
                    if (all_processes[i].status) {
                        if (all_processes[i].priority > all_processes[current_process].priority) {
                            current_process = i;
                        }
                    }
                    current_process_copy = current_process; 
                }
                else {
                    current_process++;
                    current_process_copy = current_process;
                }

            }
            //fim dos loops

            //inicio da manipulação do processo
            if ((current_process < num_processes)) {
                //idle
                if( idle_time > 0){
                    fprintf(output_file, "idle for %d units\n", idle_time);
                    idle_time = 0;
                }

                all_processes[current_process].remain_exec_time--;

                if (all_processes[current_process].remain_exec_time > 0) {
                    all_processes[current_process].status = ACTIVE;
                }
                else {
                    all_processes[current_process].status = INACTIVE;
                }
                
                //debug
                // printf("%d ", t);
                // printf("%s\n", all_processes[current_process].name);

                if(all_processes[current_process].remain_exec_time == 0){
                    if(all_processes[current_process].remain_from_hold > 0){
                        all_processes[current_process].executed_units = all_processes[current_process].remain_from_hold;
                        fprintf(output_file, "[%s] for %d units - F\n", all_processes[current_process].name, all_processes[current_process].executed_units);
                        all_processes[current_process].data.times_executed++;
                        all_processes[current_process].in_hold = 0;
                    }
                    else{
                        all_processes[current_process].executed_units = all_processes[current_process].exec_time;
                        fprintf(output_file, "[%s] for %d units - F\n", all_processes[current_process].name, all_processes[current_process].executed_units);
                        all_processes[current_process].data.times_executed++;
                    }
                }
                
                
            }
            else {
                idle_time++;
                //debug
                // printf("idle\n");
            }

            //como o for so chega ao tempo total - 1, checa se algum processo chega na ultima unidade de tempo
            if( t == total_time - 1){
                t = total_time;
                for(int j = 0; j < num_processes; j++){
                    if(t % all_processes[j].period == 0){
                        all_processes[j].data.times_killed = 1;
                    }
                }
            }
        }
        
        //print da contagem do idle após todos os processos finalizados e tempo ainda não finalizado
        if( idle_time > 0){
            fprintf(output_file, "idle for %d units\n", idle_time);
            idle_time = 0;
        }

        for(int i = 0; i < num_processes; i++){
            if(all_processes[i].remain_exec_time <= all_processes[i].exec_time && all_processes[i].status == ACTIVE){
                all_processes[i].data.times_killed = 1;
            }
        }

        for(int i = 0; i < num_processes; i++){
            if(all_processes[i].remain_exec_time < all_processes[i].exec_time && all_processes[i].remain_exec_time > 0){
                all_processes[i].executed_units = all_processes[i].exec_time - all_processes[i].remain_exec_time;
                fprintf(output_file, "[%s] for %d units - K\n", all_processes[i].name, all_processes[i].executed_units);
                break;
            }
        }

        fprintf(output_file,"\n");
        fprintf(output_file, "LOST DEADLINES\n");
        for(int p = 0; p < num_processes; p++){
            fprintf(output_file, "[%s] %d\n", all_processes[p].name, all_processes[p].data.lost_deadlines);
        }
        fprintf(output_file,"\n");
        fprintf(output_file, "COMPLETE EXECUTION\n");
        for(int p = 0; p < num_processes; p++){
            fprintf(output_file, "[%s] %d\n", all_processes[p].name, all_processes[p].data.times_executed);
        }
        fprintf(output_file,"\n");
        fprintf(output_file, "KILLED\n");
        for(int p = 0; p < num_processes; p++){
            fprintf(output_file, "[%s] %d\n", all_processes[p].name, all_processes[p].data.times_killed);
        }
        fclose(output_file);
        
    }
    else{
        printf("Numero de argumentos invalido\n");
    }

}