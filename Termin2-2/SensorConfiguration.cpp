#include "SensorConfiguration.h"
#include <string>

void CSensorConfiguration::setAddr(std::string addr){
    this->addr = addr;
}

std::string CSensorConfiguration::getAddr(){
    return this->addr;
}


int CSensorConfiguration::enableNotification(){}

int CSensorConfiguration::disableNotification(){}
