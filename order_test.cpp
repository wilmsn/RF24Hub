#include "order.h"
#include <thread>
#include <stdint.h>
#include <stdio.h> 
#include <iostream>


int main()
{
    Order myorder;
    struct Order::order_t neworder;
    Order::order_t* order_ptr;
    for (int i=1; i<101; ++i) {
        neworder.orderno = i;
        myorder.new_entry(&neworder);
    }
    order_ptr=myorder.initial_order_ptr;
    while ( order_ptr ) {
      printf("%d\n",order_ptr->orderno);
      order_ptr=order_ptr->next;
    }
    myorder.del_entry(98);
    myorder.del_entry(97);
    myorder.del_entry(96);
    myorder.del_entry(95);
    myorder.del_entry(93);
    myorder.del_entry(92);
    myorder.del_entry(98);
    myorder.del_entry(80);
    myorder.del_entry(79);
    myorder.del_entry(50);
    order_ptr=myorder.initial_order_ptr;
    while ( order_ptr ) {
      printf("%d\n",order_ptr->orderno);
      order_ptr=order_ptr->next;
    }
    
}
