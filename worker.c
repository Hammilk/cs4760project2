#include<stdio.h>
#include<math.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<stdlib.h>

int main(int seconds, int nano){

    //Set up shared memory pointer
    const int sh_key = 2031535;
    int shm_id = shmget(sh_key, sizeof(int) * 10, IPC_CREAT | 0666);
    if(shm_id <= 0){
        fprintf(stderr, "Shared memory get failed\n");
        exit(1);
    }
    int *shm_ptr=shmat(shm_id, 0, 0);
    if(shm_ptr <= 0){
        fprintf(stderr, "Shared memory attach failed\n");
        exit(1);
    }

    //Work
    
    int pid = getpid();
    int ppid = getppid();

    int SysClockS = (*shm_ptr)/(int)(pow(10,9));
    int SysClockNano = (*shm_ptr)%(int)(pow(10,9));
    int timeElapsed = 0;
    


    printf("WORKER PID: %d PPID: %d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n--Just Starting\n"
            , pid, ppid, SysClockS, SysClockNano, seconds, nano);

    int time = (seconds * (pow(10, 9))) + nano;
    while(time>(*shm_ptr)){
        printf("WORKER PID: %d PPID: %d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n--%d seconds have passed since starting\n"
            , pid, ppid, SysClockS, SysClockNano, seconds, nano, timeElapsed++);
    }

    printf("WORKER PID: %d PPID: %d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n--Terminating\n"
            , pid, ppid, SysClockS, SysClockNano, seconds, nano);



    //Unattach shared memory pointer
    shmdt(shm_ptr);
   
    return 0;
}
