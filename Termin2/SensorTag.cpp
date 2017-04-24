#include "SensorTag.h"

Motion_t CSensorTag::getMotion(){
    return CSensorCommunication::getMotion(*this);
}
