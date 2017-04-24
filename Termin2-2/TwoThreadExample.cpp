#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
using namespace std;


#include "SensorTag.h"
#include "Motion.h"

CSensorTag sensorTag{};
Motion_t motion;


void printMotion(){
    cout << "Gyro -> X: " << motion.gyro.x << " Y: " << motion.gyro.y << " Z: " << motion.gyro.z << endl;
    cout << "Acc -> X: " << motion.acc.x << " Y: " << motion.acc.y << " Z: " << motion.acc.z << endl;
}


void setRealtime(){
    pthread_setschedprio(pthread_self(), 99);
}

void setFifo(){
    pthread_setschedparam(pthread_self(),SCHED_FIFO, sched_getparam());
}
// the function being executed as thread 2
void* main_thread2(void* parameterPtr) {
    setFifo();
    setRealtime();
    // explicit cast of parameterPtr into a pointer to int
    int *xPtr = (int *) parameterPtr;
    // increment x up to 10
    timeval starttime;
    timeval endtime;
    //usleep(1);
    while ( ++(*xPtr) < 10 ) {
        //gettimeofday(&starttime,nullptr);
        motion = sensorTag.zeroMotion();
        cout << "(x=" << *xPtr << ")" << flush;
        printMotion();
        //gettimeofday(&endtime,nullptr);
        //long usec = endtime.tv_usec - starttime.tv_usec;
        //cout << "x: " << usec << endl;
        //usleep(10000-usec);
        //sleep(1); // wait a second
    }
    cout << endl << "reached end of incrementing x" << endl;
    // return NULL as function demands for a return value
    return NULL;
}

int createThread(int x){
    // thread ID for second thread
    pthread_t thread2_id;
    // create second thread executing function thread2_main */
    return pthread_create(&thread2_id, NULL, main_thread2, &x);
}
// main runs thread 1
int main() {
    // variables we modify in two threads
    int x = 0, y = 0;
    // show the initial values of x and y
    cout << "start count x=" << x << ", y=" << y << endl;
    // sleep 2 seconds, then start incrementing y up to 5
    if(createThread(&x)) {
        cerr << "Error: thread not created" << endl;
        return 1;
    }
    //sleep(2);
    //usleep(10000);
    timeval starttime;
    timeval endtime;
    while ( ++y < 10 ) {
        //gettimeofday(&starttime,nullptr);
        motion = sensorTag.getMotion();
        cout << "(y=" << y << ")" << flush;
        printMotion();
        /*gettimeofday(&endtime,nullptr);
        long usec = endtime.tv_usec - starttime.tv_usec;
        cout << "y: " << usec << endl;
        usleep(10000-usec);*/
        //sleep(1); // wait a second
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
