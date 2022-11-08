#define MAX_LINE 80

#define INACTIVE 0
#define ACTIVE 1

struct Data
{
    int times_executed;
    int lost_deadlines;
    int times_killed;
};
typedef struct Data Data;


struct Process{
    char* name;
    int period;
    int exec_time;

    int status;
    int priority;
    int executed_units;
    int remain_exec_time;
    int in_hold; //simula um bool, serve pra nao printar o hold mais de uma vez quando outras tarefas chegam no mesmo periodo
    int remain_from_hold; //armazena as unidades restantes em caso de hold

    Data data;
};
typedef struct Process Process;

void parse_input(char *process, char **parsed_process);

int file_read(char *filename, char **arguments);

Process create_process(char** struct_fields);

void priority_assignment(Process all_processes[], int num_processes);

void printList(Process processes[], int size);

void printPriority(Process processes[], int size);