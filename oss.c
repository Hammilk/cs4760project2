#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHMKEY 2031535 
#define BUFF_SZ sizeof (int)









struct PCB{
    int occupied; //Either true or false
    pid_t pid; //process id of child
    int startSeconds; //time when it was forked
    int startNano; //time when it was forked
};

typedef struct{
    int proc;
    int simul;
    int timelimit;
    int interval;
} options_t;

void print_usage(const char * app){
    fprintf(stderr, "usage: %s [-h] [-n proc] [-s simul] [-t timeLimitForChildren] [-i intervalInMsToLaunchChildren]\n", app);
    fprintf(stderr, "   proc is the total amount of children.\n");
    fprintf(stderr, "   simul is how many children can run simultaneously.\n");
    fprintf(stderr, "   timeLimitForChildren is the bound of time that a child process should be launched for.\n");
    fprintf(stderr, "   intervalInMsToLaunchChildren species how often you should launch a child.\n");
}

void printProcessTable(int PID, int SysClockS, int SysClockNano, struct PCB processTable[20]){
    printf("OSS PID %d SysClockS: %d SysClockNano: %d\n", PID, SysClockS, SysClockNano);
    printf("Process Table:\n");
    printf("Entry     Occupied  PID       StartS    Startn\n"); 
    for(int i = 0; i<20; i++){
        printf("%d         %d         %d         %d         %d\n", i, i, PID, SysClockS, SysClockNano);
    } 
}





int main(int argc, char* argv[]){
    
    //Set up shared memory
    int shmid = shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
    if(shmid == 1){
        fprintf(stderr, "error in shmget\n");
        exit(1);
    }

    char * paddr = (char *)(shmat(shmid, 0, 0));
    int *sh_ptr = (int *)(paddr);

    //Set up structs

    struct PCB processTable[20];
    options_t options;
    options.proc = 1;
    options.simul = 1;
    options.timelimit = 1;
    options.interval = 1;

    const char optstr[] = "hn:s:t:i";

    char opt;
    while((opt = getopt(argc, argv, optstr))!= -1){
        switch(opt){
            case 'h':
                print_usage(argv[0]);
                return(EXIT_SUCCESS);
            case 'n':
                options.proc = atoi(optarg);
                break;
            case 's':
                options.simul = atoi(optarg);
                break;
            case 't':
                options.timelimit = atoi(optarg);
                break;
            case 'i':
                options.interval = atoi(optarg);
                break;
            default:
                printf("Invalid options %c\n", optopt);
                print_usage(argv[0]);
                return(EXIT_FAILURE);
        }
    }

    while(stillChildrenToLaunch){
        incrementClock();

        //Every half a second, output process table
        
        checkIfChildHasTerminated();

        if(childHasTerminated){
            updatePCBOfTerminatedChild;
        }

        //possibly launch new child
        
    }





    

    return 0;
    


}

