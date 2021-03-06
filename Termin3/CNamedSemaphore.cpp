// File     CNamedSemaphore.cpp
// Version  1.0
// Author   Jens-Peter Akelbein
// Comment  Softwareentwicklung fuer Embedded Systeme - Exercise 3

#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include "CNamedSemaphore.h"

using namespace std;

// construct a new named semaphore
// @name - name of the semaphore being created in /run/shm
// @value - initial value of the semaphore
CNamedSemaphore::CNamedSemaphore(const char *name, int value)
{
    sem_unlink(name);
    semaphore = sem_open(name, O_CREAT, 0, value);
    if(semaphore == SEM_FAILED)
    {
        exitproc("open", errno);
    }
    remember_my_name = name;
    remember_my_creator = getpid();
}

// deconstruct the semaphore
// question:
// How do we delete the semaphore by the last process only?
CNamedSemaphore::~CNamedSemaphore()
{
    if(sem_close(semaphore) == -1)
    {
        exitproc("close", errno);
    }
    if(remember_my_creator == getpid()){
        sem_unlink(remember_my_name);
    }
}


void CNamedSemaphore::increment(void)
{
    if(sem_post(semaphore) == -1)
    {
        exitproc("post", errno);
    }
}


void CNamedSemaphore::decrement(void)
{
    if(sem_wait(semaphore) == -1)
    {
        exitproc("wait", errno);
    }
    /*cout << "decrement id " << getpid() << " state " << state << endl;
    int val = 0;
    sem_getvalue(semaphore, &val);
    cout << val << endl;
    sleep(5);*/
}


int CNamedSemaphore::value(void)
{
    int sval;
    if(sem_getvalue(semaphore, &sval) == -1)
    {
        exitproc("value", errno);
    }
    return sval;
    //return state;
}


// helper function to display errors and terminate our
// process as a very simple error handling mechanism
void CNamedSemaphore::exitproc(const char *text, int err)
{
    cout << text;
    switch (err)
    {
    case EACCES:
        cerr << "EACCES";
        break;
    case EEXIST:
        cerr << "EEXIST";
        break;
    case EINVAL:
        cerr << "EINVAL";
        break;
    case EMFILE:
        cerr << "EMFILE";
        break;
    case ENAMETOOLONG:
        cerr << "TOOLNG";
        break;
    case ENOENT:
        cerr << "ENOENT";
        break;
    case ENOMEM:
        cerr << "ENOMEM";
        break;
    case EOVERFLOW:
        cerr << "EOVERFLOW";
        break;
    default:
        cerr << "UNKNWN";
    }
    cout << endl;
    exit(1);
}
