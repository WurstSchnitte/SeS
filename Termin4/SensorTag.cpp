#include "SensorTag.h"

Motion_t CSensorTag::getMotion() {
	return CSensorCommunication::getMotion(*this);
}


char* CSensorTag::getMotionAsByte() {
	return CSensorCommunication::getMotionAsByte(*this);
}

Motion_t CSensorTag::convertMotion(char* rawData){
    return CSensorCommunication::convertMotion(rawData);
}
