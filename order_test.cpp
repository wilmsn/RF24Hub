#include "order.h"
#include <stdint.h>
#include <stdio.h> 
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"


int main()
{
    Order order;
    order.addOrder(1,1,true,1,0);
    order.addOrder(2,1,true,1,0);
    order.addOrder(3,1,true,1,0);
    order.addOrder(4,1,true,1,0);
    order.addOrder(5,1,true,1,0);
    order.addOrder(6,1,true,1,0);
    order.addOrder(7,1,true,1,0);
    order.addOrder(8,1,true,1,0);
    order.addOrder(9,1,true,1,0);
    order.addOrder(10,1,true,1,0);
    order.addOrder(11,1,true,1,0);
    order.addOrder(12,1,true,1,0);
    order.addOrder(13,1,true,1,0);
    order.addOrder(14,1,true,1,0);
    order.addOrder(15,1,true,1,0);
    order.addOrder(16,1,true,1,0);
    order.printBuffer();
    printf("---------\nDelete by Node: 1,2,6,10,11,15,16\n--------------\n");
    order.delByNode(2);
    order.delByNode(16);
    order.delByNode(6);
    order.delByNode(1);
    order.delByNode(10);
    order.delByNode(11);
    order.delByNode(15);    
    order.printBuffer();
    printf("---------\nDelete by OrderNo: 5,6,9,10,14,15\n--------------\n");
    order.delByOrderNo(15);
    order.delByOrderNo(10);
    order.delByOrderNo(14);
    order.delByOrderNo(9);
    order.delByOrderNo(5);
    order.delByOrderNo(6);
    order.printBuffer();    
}
