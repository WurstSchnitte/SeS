#include <iostream>
#include "SensorTag.h"
#include "SensorConfiguration.h"
#include "SensorCommunication.h"

using namespace std;



int main()
{
    CSensorCommunication scm;
    CSensorConfiguration sc;

    CSensorTag st;

    //st.getMotion();


    cout << "CSensorTag: " << sizeof(st) << " Byte" << endl;

    cout << "CSensorConfiguration: " << sizeof(sc) << " Byte" << endl;
    cout << "CSensorCommunication: " << sizeof(scm) << " Byte" << endl;

    return 0;
}
