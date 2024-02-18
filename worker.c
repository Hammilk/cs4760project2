#include<stdio.h>
#include<math.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<stdlib.h>


#define SHMKEY 2031535
#define BUFF_SZ sizeof(int)

int main(int argc, char** argv){
    

    //Set up shared memory pointer
    int shm_id = shmget(SHMKEY, BUFF_SZ, IPC_CREAT | 0777);
    if(shm_id <= 0){
        fprintf(stderr, "Shared memory get failed\n");
        exit(1);
    }
    int *shm_ptr=shmat(shm_id, 0, 0);
    if(shm_ptr <= 0){
        fprintf(stderr, "Shared memory attach failed\n");
        exit(1);
    }

    int seconds = atoi(argv[1]);
    int nano = atoi(argv[2]);
    




    //Work
    
    int pid = getpid();
    int ppid = getppid();

    int SysClockS = (*shm_ptr)/(int)(pow(10,9));
    int SysClockNano = (*shm_ptr)%(int)(pow(10,9));
    int timeElapsed = 0;
    


    printf("WORKER PID: %d PPID: %d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n--Just Starting\n"
            , pid, ppid, SysClockS, SysClockNano, seconds, nano);

    int time = (seconds * (pow(10, 9))) + nano;
    /*
    while(time>(*shm_ptr)){
        printf("WORKER PID: %d PPID: %d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n--%d seconds have passed since starting\n"
            , pid, ppid, SysClockS, SysClockNano, seconds, nano, timeElapsed++);
    }
    */

    printf("WORKER PID: %d PPID: %d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n--Terminating\n"
            , pid, ppid, SysClockS, SysClockNano, seconds, nano);



    //Unattach shared memory pointer
    shmdt(shm_ptr);
  
    exit(0);
}
