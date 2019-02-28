#include "orderQueue.h"
#include <thread>
#include <stdint.h>
#include <stdio.h> 
#include <iostream>

void fill_queue(orderQueue *myqueue)
{
    for (int i=1; i<101; ++i) myqueue->newEntry();
 //   std::cout << "Testsensors bis 100 geladen." << std::endl;
 //   mysensor->listSensor();
}



int main()
{
    orderQueue myorderQueue;

//    std::thread t2(fill_order,&myorderBuffer);
//    t2.detach();
    printf("Anzahl offener Orders: %d\n",myorderQueue.ordersWaiting());
    fill_queue(&myorderQueue);
    printf("Anzahl offener Orders: %d\n",myorderQueue.ordersWaiting());
//    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    
    return 0;
}
