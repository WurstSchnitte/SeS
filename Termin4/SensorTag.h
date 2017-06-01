#ifndef __SENSOR_TAG_H__
#define __SENSOR_TAG_H__

#include "SensorConfiguration.h"
#include "SensorCommunication.h"

class CSensorTag: public CSensorConfiguration, private CSensorCommunication {
    public:
        /*
            Reads motion sensor from SensorTag
            @return Return Motion_t with convertet values
        */
        Motion_t getMotion();
};

#endif /* __SENSOR_TAG_H__ */
