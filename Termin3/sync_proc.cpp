// File     sync_proc.h
// Version  1.0
// Author   Jens-Peter Akelbein
// Comment  Softwareentwicklung fuer Embedded Systeme - Exercise 3

#include <iostream>
#include "CNamedSemaphore.h"
#include "SensorTag.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

// valid states for our two processes, we use the impicit ordering of values
// by an enum starting with the value 1
enum EProc_State
{
    STATE_ACTIVE_CHILD = 1,
    STATE_ACTIVE_PARENT,
    STATE_TERMINATE
};

#define NUMBER_OF_LOOPS     10


const char sem_name1[] = "/semaphore";
const char sem_name2[] = "/state";
CNamedSemaphore semaphore(sem_name1, 1);
CNamedSemaphore state(sem_name2, STATE_ACTIVE_CHILD);
CSensorTag stag;


void printMotion(Motion_t motion){
    cout << "Gyro -> X: " << motion.gyro.x << " Y: " << motion.gyro.y << " Z: " << motion.gyro.z << endl;
    cout << "Acc -> X: " << motion.acc.x << " Y: " << motion.acc.y << " Z: " << motion.acc.z << endl;
}

// function being executed as parent or as child process to perform ping pong
// between both processes
void pingpong(bool parent)
{
    for(int i = 0; i < NUMBER_OF_LOOPS; ++i)
    {
        if(parent)
        {
            while(true)
            {
                semaphore.decrement();
                if(state.value() == STATE_ACTIVE_PARENT)
                {
                    cout << "Parent" << endl;
                    printMotion(stag.getMotion());
                    state.decrement();
                    semaphore.increment();
                    break;
                }
                semaphore.increment();
            }
        }
        else
        {
            while(true)
            {
                semaphore.decrement();
                if(state.value() == STATE_ACTIVE_CHILD)
                {
                    cout << "Child" << endl;
                    printMotion(stag.getMotion());
                    state.increment();
                    semaphore.increment();
                    break;
                }
                semaphore.increment();
            }
        }
    }
    cout << "!!!!" << endl;
    if(!parent)
    {
        semaphore.decrement();
        if(state.value() == STATE_ACTIVE_CHILD)
        {
            state.increment();
            state.increment();
            cout << "Child beendet" << endl;
            semaphore.increment();
            exit(1);
        }
    }
    else
    {
        while(true)
        {
            semaphore.decrement();
            if(state.value() == STATE_TERMINATE)
            {
                cout << "Parent beendet" << endl;
                semaphore.increment();
                break;
            }
            semaphore.increment();
        }
    }
}

// main function, here we are just forking into two processes both calling
// pingpong() and indicating with a boolean on who is who
int main()
{
    cout << "parent=" << getpid() << endl;

    stag.setAddr("24:71:89:E8:6A:07");

    pid_t parent = getpid();

    pid_t child = fork();
    if(parent == getpid())
    {
        pingpong(true);
    }
    else
    {
        pingpong(false);
    }

    return 0;
}
