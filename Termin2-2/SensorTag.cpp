#include "SensorTag.h"

Motion_t CSensorTag::getMotion(){
    return CSensorCommunication::getMotion(*this);
}

Motion_t CSensorTag::zeroMotion(){
    return CSensorCommunication::zeroMotion();
}
