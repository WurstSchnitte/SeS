#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "CCommQueue.h"
#include "SensorTag.h"

using namespace std;

#define SHM_NAME        "/estSHM"
#define QUEUE_SIZE      16
#define NUM_MESSAGES    100

struct PackedData
{
    Motion_t motion;
    UInt64 time;
};
typedef struct PackedData PackedData_t;

pid_t parentId;
CSensorTag cTag = CSensorTag();



int open_memory(mode_t mode)
{
    return shm_open(SHM_NAME, O_CREAT | mode, NULL);
}

void close_memory(int filedescriptor)
{
    close(filedescriptor);
    if(getpid() == parentId)
    {
        if(shm_unlink(SHM_NAME) == 0)
        {
            cout << "SHM Gelöscht!" << endl;
        };
    }
}

size_t getSizeForMMap()
{
    size_t out = sizeof(CBinarySemaphore);
    out += sizeof(CCommQueue);
    out += QUEUE_SIZE*sizeof(CMessage);
    return out;
}

void* map_memory(int descr)
{
    void* startMap = mmap(0, getSizeForMMap(), PROT_WRITE | PROT_READ ,MAP_SHARED, descr,0);
    if(getpid() == parentId)
    {
        new (startMap)CBinarySemaphore(true,true);
        new (startMap + sizeof(CBinarySemaphore))CCommQueue(QUEUE_SIZE,*(CBinarySemaphore*)startMap);
    }
    return startMap;
}

void printMotion(Motion_t motion)
{
    cout << "Gyro -> X: " << motion.gyro.x << " Y: " << motion.gyro.y << " Z: " << motion.gyro.z << endl;
    cout << "Acc -> X: " << motion.acc.x << " Y: " << motion.acc.y << " Z: " << motion.acc.z << endl << endl;
}

void readMessage(CCommQueue* commQueue, CBinarySemaphore* sem, int newsockfd)
{
    if(commQueue->getNumOfMessages() < 1)
    {
        sem->take();
    }
    CMessage msg;
    commQueue->getMessage(msg);
    timeval stop;
    gettimeofday(&stop, NULL);
    timeval sub;
    timeval start = msg.getStructMostMessage().time;
    timersub(&stop,&start,&sub);
    cout << "Zeit: " << sub.tv_sec << " Sekunden "
         << sub.tv_usec << " Mikrosekunden" << endl;

    PackedData_t data;
    data.motion = cTag.convertMotion((char*)(msg.getStructMostMessage().data.bytes));
    data.time = stop.tv_sec;
    int n = write(newsockfd,&data,sizeof(PackedData_t));
    if(n <= 0){
        cout << "Fehler beim senden der Daten" << endl;
    }
    //printMotion(cTag.convertMotion((char*)(msg.getStructMostMessage().data.bytes)));

}

void writeMessage(CCommQueue* commQueue)
{
    MostMessage mostMsg;
    mostMsg.mType = CMessage::Softical_Most_Type;
    char* motion = cTag.getMotionAsByte();
    for(int i = 0; i < 12; ++i)
    {
        mostMsg.data.bytes[i] = motion[i];
    }
    gettimeofday(&mostMsg.time, NULL);
    CMessage msg = CMessage(mostMsg);
    commQueue->add(msg);
}


void child(int port, int descr)
{

    cout << "CHILD:" << endl;
    void* start = map_memory(descr);
    CBinarySemaphore* sem = (CBinarySemaphore*)start;
    CCommQueue* commQueue = (CCommQueue*)(start+sizeof(CBinarySemaphore));
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        cout << "Socket konnte nicht erzeugt werden" << endl;
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0){
        cout << "Socket konnte nicht gebunden werden" << endl;
        exit(0);
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0){
        cout << "Verbindung konnte nicht angenommen werden" << endl;
        exit(0);
    }
    cout << "Verbunden" <<endl;

    for(int i = 0; i < NUM_MESSAGES; ++i)
    {
        cout << "ReadMessage: " << i << endl;
        readMessage(commQueue, sem, newsockfd);
    }

    close(newsockfd);
    close(sockfd);
    munmap(start,getSizeForMMap());
    close_memory(descr);

}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    shm_unlink(SHM_NAME);

    cout << "Creating a child process ..." << endl;
    parentId = getpid();
    int descr;
    descr = open_memory(O_RDWR);
    pid_t pid = fork();
    if (0 == pid)
    {
        child(atof(argv[1]), descr);
    }
    else if (pid > 0)
    {
        cout << "PARENT:" << endl;
        //descr = open_memory(O_RDWR);
        if(descr < 0)
        {
            return descr;
        }
        int a = ftruncate(descr, getSizeForMMap());
        if(a < 0){
            cout << "Shared Memory konnte nidcht allokiert werden" << endl;
            exit(0);
        }
        void* start = map_memory(descr);
        CBinarySemaphore* sem = (CBinarySemaphore*)start;
        CCommQueue* commQueue = (CCommQueue*)(start+sizeof(CBinarySemaphore));

        for(int i = 0; i < NUM_MESSAGES; ++i)
        {
            cout << "WriteMessage: " << i << endl;
            writeMessage(commQueue);
        }
        sem->give();
        munmap(start,getSizeForMMap());
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
