#include "orderbuffer.h"
#include <stdio.h> 
#include <iostream>

#define LISTSIZE 10000

int main()
{
    OrderBuffer myorder;
    OrderBuffer::orderbuffer_t* order_ptr;
    for (int i=1; i<=LISTSIZE; ++i) {
        OrderBuffer::orderbuffer_t* neworder_ptr = new OrderBuffer::orderbuffer_t;
        neworder_ptr->orderno = i;
        myorder.new_entry(neworder_ptr);
    }
    order_ptr=myorder.initial_ptr;
    while ( order_ptr ) {
      printf("%d\n",order_ptr->orderno);
      order_ptr=order_ptr->next;
    }
    myorder.del_orderno(98);
    myorder.del_orderno(97);
    myorder.del_orderno(96);
    myorder.del_orderno(95);
    myorder.del_orderno(93);
    myorder.del_orderno(92);
    myorder.del_orderno(98);
    myorder.del_orderno(80);
    myorder.del_orderno(79);
    myorder.del_orderno(50);
    order_ptr=myorder.initial_ptr;
    while ( order_ptr ) {
      printf("%u\n",order_ptr->orderno);
      order_ptr=order_ptr->next;
    }
    
}
