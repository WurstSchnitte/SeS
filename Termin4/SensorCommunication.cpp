#include "SensorCommunication.h"
#include "SensorConfiguration.h"
#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
// system
#include <stdlib.h>
// fifo
#include <fcntl.h>
#include <sys/stat.h>
// read
#include <unistd.h>

#define MAX_BUF 12

Motion_t CSensorCommunication::getMotion(CSensorConfiguration conf) {
	char buffer[MAX_BUF];
	memset(buffer, 0, MAX_BUF);
	if (readMotion(conf, buffer) != 0) {
		std::cerr << "can't read motion" << std::endl;
	}
	return convertMotion(buffer);
}

char* CSensorCommunication::getMotionAsByte(CSensorConfiguration conf) {
	char buffer[MAX_BUF];
	memset(buffer, 0, MAX_BUF);
	if (readMotion(conf, buffer) != 0) {
		std::cerr << "can't read motion" << std::endl;
	}
	return buffer;
}

Motion_t CSensorCommunication::convertMotion(char* rawData) {
	Motion_t motion;
	int16_t* rawData16 = (int16_t*) rawData;

	int16_t gx = rawData16[0];
	int16_t gy = rawData16[1];
	int16_t gz = rawData16[2];

	int16_t ax = rawData16[3];
	int16_t ay = rawData16[4];
	int16_t az = rawData16[5];

  	//-- calculate rotation, unit deg/s, range -250, +250
  	motion.gyro.x = (gx * 1.0) / (65536/ 500);
  	motion.gyro.y = (gy * 1.0) / (65536/ 500);
  	motion.gyro.z = (gz * 1.0) / (65536/ 500);

	//-- calculate acceleration, unit G, range -2, +2
    motion.acc.x = (ax * 1.0) / (65536/4);
    motion.acc.y = (ay * 1.0) / (65536/4);
    motion.acc.z = (az * 1.0) / (65536/4);

	return motion;
}

void* readSensor(void* data_ptr) {
	char* addr = (char*) data_ptr;
	char cmd[64];
	sprintf(cmd, "./readSensor.sh %s", addr);
	int ret = system(cmd);
	if (ret != 0) {
		std::cerr << "./readSensor failed with " << ret << std::endl;
	}
	return NULL;
}

//#define NO_PI;

int CSensorCommunication::readMotion(CSensorConfiguration conf, char* buffer) {
#ifdef NO_PI
	buffer[0] = 0x0d;
	buffer[1] = 0x01;
	buffer[2] = 0x6c;
	buffer[3] = 0xfc;
	buffer[4] = 0xd9;
	buffer[5] = 0xfc;
	buffer[6] = 0xf4;
	buffer[7] = 0xfd;
	buffer[8] = 0x85;
	buffer[9] = 0x00;
	buffer[10] = 0xd2;
	buffer[11] = 0x0f;
#else
	int fd;
    char* pipe = (char*) "/tmp/sensorTagPipe";
	pthread_t readSensor_thread;

	mkfifo(pipe, 0666);

	char addr[32];
	strcpy(addr, conf.getAddr().c_str());

	if(pthread_create(&readSensor_thread, NULL, readSensor, (void*) addr)) {
	fprintf(stderr, "Error creating thread\n");
		return -1;
	}

	fd = open(pipe, O_RDONLY);
    read(fd, buffer, MAX_BUF);

    close(fd);
	unlink(pipe);
#endif
	return 0;
}
