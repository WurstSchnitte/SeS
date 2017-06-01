#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#include "CCommQueue.h"
#include "SensorTag.h"

using namespace std;

#define SHM_NAME        "/estSHM"
#define QUEUE_SIZE      1
#define NUM_MESSAGES    10

struct PackedData {
	Motion_t motion;
	UInt64 time;
};
typedef struct PackedData PackedData_t;

pid_t parentId;
int8_t memory[185];

int open_memory(mode_t mode){
    return shm_open(SHM_NAME, O_CREAT, mode);
}

void close_memory(int filedescriptor){
    close(filedescriptor);
    if(getpid() == parentId){
        if(shm_unlink(SHM_NAME) == 0){
            cout << "Gelöscht!" << endl;
        };
    }
}

void readMessage(){

}

void writeMessage(){

}

int getSizeForMMap(){
    //int out = sizeof(CBinarySemaphore);
    int out = sizeof(CCommQueue);
    out += QUEUE_SIZE-1*sizeof(CMessage);
    return out;
}

int main()
{
    int descr;

    cout << "Creating a child process ..." << endl;
    parentId = getpid();
    pid_t pid = fork();
    if (0 == pid)
    {
        cout << "CHILD:" << endl;
        descr = open_memory(O_RDONLY);
        CCommQueue* commQueue = (CCommQueue*)mmap(NULL, getSizeForMMap(), PROT_READ|PROT_WRITE,MAP_SHARED, descr,0);
        // Child process - Reads all Messages from the Queue and outputs them with auxiliary data
        close_memory(descr);
    }
    else if (pid > 0)
    {
        cout << getSizeForMMap() << endl;
        cout << "PARENT:" << endl;
        descr = open_memory(O_RDWR);
        CCommQueue* commQueue = (CCommQueue*)mmap(NULL, getSizeForMMap(), PROT_READ|PROT_WRITE,MAP_SHARED, descr,0);
        cout << commQueue << endl;
        cout << errno << endl;
        CBinarySemaphore sem = CBinarySemaphore(true,true);
        new (commQueue)CCommQueue(QUEUE_SIZE,sem);
        cout << commQueue->getNumOfMessages() << endl;
        // Parent process - Writes all Messages into the Queue
        close_memory(descr);

    }
    else
    {
        // Error
        cerr << "Couldn't create a child process. Exiting" << endl;
        abort();
    }

    return 0;
}
