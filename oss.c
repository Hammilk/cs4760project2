#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHMKEY 2031535 
#define BUFF_SZ sizeof (int)
#define MAXDIGITS 3




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

typedef struct{
    int seconds;
    int nano;
} clock;



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

void incrementClock(clock c){
    c.nano += 500;
    if(c.nano >= (pow(10, 9))){
        c.nano -= (pow(10, 9));
        c.seconds++;
    }
}




int main(int argc, char* argv[]){
    
    //Set up shared memory
    int shmid = shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
    if(shmid == -1){
        fprintf(stderr, "error in shmget\n");
        exit(1);
    }

    char * paddr = (char *)(shmat(shmid, 0, 0));
    clock *sh_ptr = (int *)(paddr);

    //Set up structs

    struct PCB processTable[20];

    options_t options;
    options.proc = 1; //n
    options.simul = 1; //s
    options.timelimit = 1; //t
    options.interval = 1; //i

    //Set up user inputs

    const char optstr[] = "hn:s:t:i:";

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
    
    //Set up variables

    int tempLoop = 0;
    int child_process;
    int tempSeconds = 5;
    int tempNano = 3;
    int status;
    
    //Work

    *sh_ptr = 44696494;

    while(tempLoop < 1){
        tempLoop++; //TEMPORARY FOR TESTING
        
        

        if(child_process = fork() == 0){
            printf("Childtest\n");

            char secondsString[MAXDIGITS];
            char nanoString[MAXDIGITS];
            sprintf(secondsString, "%d", 2);
            sprintf(nanoString, "%d", 2);


            char * args[] = {"./worker", secondsString, nanoString, 0};
            
            execlp(args[0], args[0], args[1], args[2], NULL);
            printf("bkjdslfj");            
        }
        else{

            waitpid(-1, &status, 0);
            printf("Test\n");
            tempLoop++;
        }
    }

    
    /*
    while(stillChildrenToLaunch){
        incrementClock();

        //Every half a second, output process table
        
        checkIfChildHasTerminated();

        if(childHasTerminated){
            updatePCBOfTerminatedChild;
        }

        //possibly launch new child
        
    }
    */
    

    return 0;
    


}

