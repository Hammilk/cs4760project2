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

int interval = 0;

static void myhandler(int s){
    
    interval = 1;
    
}

static void myhandlerExit(int s){
    printf("Got signal, terminated\n");
    exit(1);
}

static int setupinterrupt(void){
    struct sigaction act;
    act.sa_handler = myhandler;
    act.sa_flags = 0;
    return(sigemptyset(&act.sa_mask) || sigaction(SIGINT, &act, NULL) || sigaction(SIGPROF, &act, NULL));
}

static int setupitimer(int s){
    struct itimerval value;
    value.it_interval.tv_sec = s;
    value.it_interval.tv_usec = 0;
    value.it_value = value.it_interval;
    return (setitimer(ITIMER_PROF, &value, NULL));
}

static int setup



//test


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
        //if((processTable[i].occupied) == 1){
            printf("%d         %d         %d         %d         %d\n", i, processTable[i].occupied, processTable[i].pid, processTable[i].startSeconds, processTable[i].startNano);
        //}
        
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
    options.proc = 2; //n
    options.simul = 3; //s
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
    if(setupinterrupt() == -1){
        perror("Failed to set up handler for SIGPROF");
        return 1;
    }
    if(setupitimer(options.interval) == -1){
        perror("Failed to set up the ITIMER_PROF interval timer");
        return 1;
    }
    //Set up variables;
    pid_t pid;
    
    int status = 0;
    int seconds = 0;
    int nano = 0;
    *sharedSeconds = seconds;
    *sharedNano = nano;
    


    //Work
    int terminatedChild = 0;
    int childrenLaunched = 0; 
    int childFinished = 0;
    int simulCount = 0;
    int launchFlag = 0;
    

        //Implement simultaneous children then time interval children then total amount of children:
    //Total children: options.proc
    //simul children: options.simul
    //interval of children: options.interval
   
    int infLoop = 1;

    while(infLoop == 1){
        
        incrementClock(sharedSeconds, sharedNano);
        //Deallocate Array
        terminatedChild = waitpid(-1, &status, WNOHANG);
        if(terminatedChild > 0){
            terminatedChild = 0;
            simulCount--;
            int index = 0;
            int arrayDeleted = 0;
            while(!arrayDeleted){
                if(processTable[index].pid == terminatedChild){
                    arrayDeleted = 1;
                    processTable[index].occupied = 0;
                    processTable[index].pid = 0;
                    processTable[index].startSeconds = 0;
                    processTable[index].startNano = 0;
                }
            }
        }

        //Print Table
        printProcessTable(getpid(), *sharedSeconds, *sharedNano, processTable);

        //Launch Children
        if(launchFlag == 0 && simulCount < options.simul){
            launchFlag = 1;
            interval = 0;
            simulCount++;
            childrenLaunched++;
            printf("Launch\n");
            pid = fork();
        }

        //Launch Executables
        if(pid == 0){
            char terminatedTime[MAXDIGITS];
            sprintf(terminatedTime, "%d", options.timelimit);
            char * args[] = {"./worker", terminatedTime};

            //Run Executable
            execlp(args[0], args[0], args[1],  NULL);
            printf("Exec failed\n");
            exit(1);
        }

        else if (pid > 0 && launchFlag>0 && childrenLaunched < options.proc){
            launchFlag = 0;
           
            
            printf("Insert array\n");
            //Insert child into PCB
            int index = 0;
            int arrayInserted = 0;
            while(!arrayInserted){
                if(processTable[index].occupied == 1){
                    index++;
                }
                else if(processTable[index].occupied == 0){
                    arrayInserted = 1;
                    processTable[index].occupied = 1;
                    processTable[index].pid = pid;
                    processTable[index].startSeconds = *sharedSeconds;
                    processTable[index].startNano = *sharedNano;
                }
                else{
                    printf("ERROR PCB Fail\n");
                    exit(1);
                }
            }
            
        }
        else if (pid > 0){ 
            childFinished = waitpid(-1, NULL, WNOHANG);
            if(childFinished != 0){
                printf("Null flag: %d\n", childFinished);
                break;
            }

        }
        
        /*

        if(childrenLaunched < options.proc && (child_process = fork() == 0)){
            char terminatedTime[MAXDIGITS];
            sprintf(terminatedTime, "%d", options.timelimit);
            char * args[] = {"./worker", terminatedTime};
            //Run Executable
            execlp(args[0], args[0], args[1],  NULL);
            printf("Exec failed\n");
            exit(1);            
        }
        else{
            
            if(child_process>0){
                child_process = 0;
                childrenLaunched++;
            }
            
            childrenLaunched = 1;
            
            
            childFinished = waitpid(-1, NULL, WNOHANG);
            */
        
        //Convert integers to strin
    }

    
    /*
    while(childFinished == 0 && childrenLaunched < options.proc){
        //Increment seconds and nano into shared memory
        incrementClock(sharedSeconds, sharedNano);

        //check to see if child has terminated
        //terminatedChild = checkTermination(status);

        //If child has been terminated, delete child from struct
        if(terminatedChild>0){
            simulCount--;
            int index = 0;
            int arrayDeleted = 0;
            while(!arrayDeleted){
                if(processTable[index].pid == terminatedChild){
                    arrayDeleted = 1;
                    processTable[index].occupied = 0;
                    processTable[index].pid = 0;
                    processTable[index].startSeconds = 0;
                    processTable[index].startNano = 0;
                }
                else{
                    index++;
                }
            }
        }

        if(childrenLaunched < options.proc && simulCount < options.simul){ //Set up so parent can not enter if parent does not intend to launch a fork
            childrenLaunched++;
            simulCount++;
            //Launch children
            pid_t childPid = fork();
            if(childPid == 0){
                //Convert integers to strings
                char terminatedTime[MAXDIGITS];
                sprintf(terminatedTime, "%d", options.timelimit);
                char * args[] = {"./worker", terminatedTime};
            
                //Run Executable
                execlp(args[0], args[0], args[1],  NULL);
                printf("Exec failed\n");
                exit(1);
            }


            //Insert child into PCB
            int index = 0;
            int arrayInserted = 0;
            while(!arrayInserted){
                if(processTable[index].occupied == 0){
                    index++;
                }
                else if(processTable[index].occupied == 1){
                    arrayInserted = 1;
                    processTable[index].occupied = 1;
                    processTable[index].pid = childPid;
                    processTable[index].startSeconds = *sharedSeconds;
                    processTable[index].startNano = *sharedNano;
                }
                else{
                    printf("ERROR PCB Fail\n");
                    exit(1);
                }
            }
        }
        else{
            printf("In Else Clause\n");
            childFinished = waitpid(-1, NULL, WNOHANG);
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


    */
    shmdt(sharedSeconds);
    shmdt(sharedNano);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
    


}









