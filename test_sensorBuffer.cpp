#include "sensorBuffer.h"
#include <thread>
#include <stdint.h>
#include <stdio.h> 
#include <iostream>

void fill_sensor(sensorBuffer *mysensor)
{
    char fhemDev[40]="Test_FHEM_Dev";
    for (int i=1; i<101; ++i) mysensor->newSensor(i,i,i,1,fhemDev,2.234*i);
    std::cout << "Testsensors bis 100 geladen." << std::endl;
    mysensor->listSensor();
}



int main()
{
    sensorBuffer mysensorBuffer;
    std::cout << "SensorBuffer Test" << std::endl;
//    std::thread t2(fill_order,&myorderBuffer);
//    t2.detach();
    fill_sensor(&mysensorBuffer);
//    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    
    
    
    return 0;
}
