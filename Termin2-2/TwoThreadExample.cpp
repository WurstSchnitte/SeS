#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;


#include "SensorTag.h"
#include "Motion.h"

CSensorTag sensorTag{};
Motion_t motion;


void printMotion(){
    cout << "Gyro -> X: " << motion.gyro.x << " Y: " << motion.gyro.y << " Z: " << motion.gyro.z << endl;
    cout << "Acc -> X: " << motion.acc.x << " Y: " << motion.acc.y << " Z: " << motion.acc.z << endl;
}


void setFifo(){

    sched_param param;
    param.__sched_priority = 19;
    sched_setscheduler(NULL, SCHED_FIFO, &param);
}
// the function being executed as thread 2
void* main_thread2(void* parameterPtr) {
    setFifo();

    // explicit cast of parameterPtr into a pointer to int
    int *xPtr = (int *) parameterPtr;
    // increment x up to 10
    while ( ++(*xPtr) < 10000 ) {
        motion = sensorTag.zeroMotion();
        cout << "(x=" << *xPtr << ")" << flush;
        printMotion();
        sched_yield();
        //usleep(0);
    }
    cout << endl << "reached end of incrementing x" << endl;
    // return NULL as function demands for a return value
    return NULL;
}

// main runs thread 1
int main() {

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);
    if(sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0 ){
        cout << "FEHLER!!!" << endl;
    }

    // variables we modify in two threads
    int x = 0, y = 0;
    // show the initial values of x and y
    cout << "start count x=" << x << ", y=" << y << endl;
    // sleep 2 seconds, then start incrementing y up to 5
    setFifo();
    pthread_t thread2_id;
    if(pthread_create(&thread2_id, NULL, main_thread2, &x)) {
        cerr << "Error: thread not created" << endl;
        return 1;
    }
    while ( ++y < 10000 ) {
        motion = sensorTag.getMotion();
        cout << "(y=" << y << ")" << flush;
        printMotion();
        sched_yield();
        //usleep(0);
    }
    cout << endl << "reached end of incrementing y" << endl;
    /* wait for the second thread to finish */
    if ( pthread_join(thread2_id, NULL) ) {
        cerr << "Error: thread not joined" << endl;
        return 2;
    }
    /* show the results - x is now 10, thanks to the second thread */
    cout << endl << "end count x=" << x << ", y=" << y << endl;
    return 0;
}
