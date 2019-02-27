#include "orderBuffer.h"
#include <thread>
#include <stdint.h>
#include <stdio.h> 
#include <iostream>

void fill_order(orderBuffer *myorder)
{
    for (int i=1; i<101; ++i) myorder->newOrder(i,i,3.234*i,i);
    myorder->listOrder();
    std::cout << "Orders bis 100 geladen." << std::endl;
    for (int i=1; i<91; ++i) myorder->setOrderNo(i,i,i);
    myorder->listOrder();
    std::cout << "Orders bis 90 mit OrderNo versehen." << std::endl;
    for (int i=51; i<91; ++i) myorder->delOrderNo(i);
    myorder->listOrder();
    std::cout << "OrderNo größer 50 gelöscht." << std::endl;
    myorder->delOldOrder(45);
    myorder->listOrder();
    std::cout << "Entrytime unter 45 gelöscht." << std::endl;
    myorder->newOrder(101,101,3,99);
    myorder->newOrder(101,101,4,999);
    myorder->newOrder(101,101,5,998);
    myorder->listOrder();
    std::cout << "Node 101 und Channel 101 sollten nur einmal vorhanden sein." << std::endl;
    
}



int main()
{
    orderBuffer myorderBuffer;

//    std::thread t2(fill_order,&myorderBuffer);
//    t2.detach();
    fill_order(&myorderBuffer);
//    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    
    
    
    return 0;
}
