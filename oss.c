#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<math.h>

#define SHMKEY1 2031535
#define SHMKEY2 2031536
#define SHMKEY3 2031537
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

void incrementClock(int *seconds, int *nano){
    (*nano) += 750000;
    if((*nano) >= (pow(10, 9))){
         (*nano) -= (pow(10, 9));
         (*seconds)++;
    }
}




int main(int argc, char* argv[]){
    
    //Set up shared memory
    int shmid = shmget(SHMKEY1, BUFF_SZ, 0777 | IPC_CREAT);
    if(shmid == -1){
        fprintf(stderr, "error in shmget 1.0\n");
        exit(1);
    }
    int *sharedSeconds = shmat(shmid, 0, 0);
    
    //Attach shared memory to nano
    shmid = shmget(SHMKEY2, BUFF_SZ, 0777 | IPC_CREAT);
    if(shmid == -1){
        fprintf(stderr, "error in shmget 2.0\n");
        exit(1);
    }
    int *sharedNano=shmat(shmid, 0, 0);

    //Set up structs

    struct PCB processTable[20];

    options_t options;
    options.proc = 3; //n
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
    
    //Set up variables;
    int child_process;
  
    int status;
    int seconds = 0;
    int nano = 0;
    *sharedSeconds = seconds;
    *sharedNano = nano;
    


    //Work

    int childrenLaunched = 0; 
    int childFinished = 0;
    

    //Implement simultaneous children then time interval children then total amount of children:
    //Total children: options.proc
    //simul children: options.simul
    //interval of children: options.interval


    while(childFinished == 0){
        incrementClock(sharedSeconds, sharedNano);

        if(childrenLaunched < 1 && (child_process = fork() == 0)){
            

            //Convert integers to strings
            char terminatedTime[MAXDIGITS];
            sprintf(terminatedTime, "%d", options.timelimit);
            char * args[] = {"./worker", terminatedTime};
            
            //Run Executable
            execlp(args[0], args[0], args[1],  NULL);
            printf("Exec failed\n");
            exit(1);            
        }
        else{
            //Trying stuff
            printf("What does fork return: %d\n", child_process);
            


            childrenLaunched = 1;       
            childFinished = waitpid(-1, &status, WNOHANG);
            /*
            if(childrenLaunched < options.proc){
                childFinished = 0;
            }
            */

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
    
    shmdt(sharedSeconds);
    shmdt(sharedNano);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
    


}

