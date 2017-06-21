#include "SensorConfiguration.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void CSensorConfiguration::setAddr(std::string addr) {
	this->addr = addr;
}

std::string CSensorConfiguration::getAddr() {
	return this->addr;
}

int CSensorConfiguration::enableNotification() {
	char cmd[128];
	sprintf(cmd, "gatttool -b %s --char-write-req --handle=0x3d --value=0100", this->addr.c_str());
	int ret = system(cmd);
	return ret;
}

int CSensorConfiguration::disableNotification() {
	char cmd[128];
	sprintf(cmd, "gatttool -b %s --char-write-req --handle=0x3d --value=0000", this->addr.c_str());
	int ret = system(cmd);
	return ret;
}
