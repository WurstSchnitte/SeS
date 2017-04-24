#include "SensorCommunication.h"



#define ACC_RANGE_2G      0
#define ACC_RANGE_4G      1
#define ACC_RANGE_8G      2
#define ACC_RANGE_16G     3

Motion_t CSensorCommunication::getMotion(CSensorConfiguration conf)
{

    char *rohdaten = new char[12];
    readMotion(conf, rohdaten);
    return convertMotion(rohdaten);

}

Motion_t CSensorCommunication::zeroMotion(){
    Motion_t out;

    out.gyro.x = 0;
    out.gyro.y = 0;
    out.gyro.z = 0;
    out.acc.x = 0;
    out.acc.y = 0;
    out.acc.z = 0;

    return out;
}

int CSensorCommunication::readMotion(CSensorConfiguration conf, char* buffer)
{
    buffer[0] = char(0x0d);
    buffer[1] = char(0x01);
    buffer[2] = char(0x6c);
    buffer[3] = char(0xfc);
    buffer[4] = char(0xd9);
    buffer[5] = char(0xfc);
    buffer[6] = char(0xf4);
    buffer[7] = char(0xfd);
    buffer[8] = char(0x85);
    buffer[9] = char(0x00);
    buffer[10] = char(0xd2);
    buffer[11] = char(0x0f);
    return 0;
}

Motion_t CSensorCommunication::convertMotion(char* rawData)
{

    Motion_t out;

    int16_t* data16b = (int16_t*)rawData;

    for(int i = 0; i < 3; ++i)
    {
        int16_t data = data16b[i];

        float res = (data * 1.0) / (65536 / 500);
        if(i == 0)
        {
            out.gyro.x = res;
        }
        else if( i == 1)
        {
            out.gyro.y = res;
        }
        else if(i == 2)
        {
            out.gyro.z = res;
        }
    }

    for(int i = 3; i < 6; ++i)
    {
        int16_t data = data16b[i];

        float res = (data * 1.0) / (32768/2);
        if(i == 3)
        {
            out.acc.x = res;
        }
        else if( i == 4)
        {
            out.acc.y = res;
        }
        else if(i == 5)
        {
            out.acc.z = res;
        }
    }
    return out;
}
