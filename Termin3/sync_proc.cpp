// File     sync_proc.h
// Version  1.0
// Author   Jens-Peter Akelbein
// Comment  Softwareentwicklung fuer Embedded Systeme - Exercise 3

#include <iostream>
#include "CNamedSemaphore.h"
#include <stdlib.h>

using namespace std;

// valid states for our two processes, we use the impicit ordering of values
// by an enum starting with the value 1
enum EProc_State {
    STATE_ACTIVE_CHILD = 1,
    STATE_ACTIVE_PARENT,
    STATE_TERMINATE
};

#define NUMBER_OF_LOOPS     10

const char sem_name1[] = "/semaphore";
const char sem_name2[] = "/state";
CNamedSemaphore semaphore(sem_name1, 1);
CNamedSemaphore state(sem_name2, STATE_ACTIVE_CHILD);


// function being executed as parent or as child process to perform ping pong
// between both processes
void pingpong(bool parent) {

}

// main function, here we are just forking into two processes both calling
// pingpong() and indicating with a boolean on who is who
int main() {

    cout << "parent=" << getpid() << endl;

    // now we fork...

    return 0;
}
