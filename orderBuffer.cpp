#include "orderBuffer.h"
#include <stdio.h> 
#include <iostream>

/*
 * Constructor
 * Hier wird ein Pointer für den Start der verketteten Liste angelegt.
 */
orderBuffer::orderBuffer(void)
{
    initial_orderBuffer_ptr = NULL;
}

/*
 * Legt einen neuen Eintrag im OrderBuffer an.
 * Alle neuen Einträge bekommen zunächst die OrderNo 0
 * Die OrderNo wird erst gesetzt wenn diese Order in die OrderQueue kommt
 * Ist für die übergebene Kombination aus Node und Channel bereits ein Eintrag vorhanden,
 * wird dieser gelöscht!
 */
int orderBuffer::newOrder(uint16_t node, uint16_t channel, float value, uint64_t entrytime)
{
    orderBuffer::orderBuffer_t *akt_orderBuffer_ptr;
    orderBuffer::delNodeChannel(node, channel);
    if (! orderBuffer::initial_orderBuffer_ptr) 
    {
        orderBuffer::initial_orderBuffer_ptr = new orderBuffer::orderBuffer_t;
        if (! orderBuffer::initial_orderBuffer_ptr) return 0;
        orderBuffer::initial_orderBuffer_ptr->orderno = 0;
        orderBuffer::initial_orderBuffer_ptr->node = node;
        orderBuffer::initial_orderBuffer_ptr->channel = channel;
        orderBuffer::initial_orderBuffer_ptr->value = value;
        orderBuffer::initial_orderBuffer_ptr->entrytime = entrytime;
        orderBuffer::initial_orderBuffer_ptr->next = NULL;
        akt_orderBuffer_ptr = orderBuffer::initial_orderBuffer_ptr;
    } 
    else
    {
        akt_orderBuffer_ptr = orderBuffer::initial_orderBuffer_ptr;
        while (akt_orderBuffer_ptr->next) {
            akt_orderBuffer_ptr = akt_orderBuffer_ptr->next;
        }
        akt_orderBuffer_ptr->next = new orderBuffer::orderBuffer_t;
        if (! akt_orderBuffer_ptr->next) return 0;
        if ( ! orderBuffer::initial_orderBuffer_ptr->next ) orderBuffer::initial_orderBuffer_ptr->next = akt_orderBuffer_ptr->next;
        akt_orderBuffer_ptr = akt_orderBuffer_ptr->next;
        akt_orderBuffer_ptr->orderno = 0;
        akt_orderBuffer_ptr->node = node;
        akt_orderBuffer_ptr->channel = channel;
        akt_orderBuffer_ptr->value = value;
        akt_orderBuffer_ptr->entrytime = entrytime;
        akt_orderBuffer_ptr->next = NULL;
    }
    return 1;
}

/*
 * Löscht alle Einträge, die alter sind als die übergebene EntryTime
 */
int orderBuffer::delOldOrder(uint64_t entrytime)
{
    orderBuffer::orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initial_orderBuffer_ptr;
    while (akt_orderBuffer_ptr) 
    {
        if (akt_orderBuffer_ptr->entrytime < entrytime ) {
            if ( ! orderBuffer::delEntry(akt_orderBuffer_ptr) ) {
                return 0;
            }
        }
        akt_orderBuffer_ptr=akt_orderBuffer_ptr->next;
    }
    return 1;
}

/*
 * Löscht alle Einträge, für die übergebene Kombination aus Node und Channel
 */
int orderBuffer::delNodeChannel(uint16_t node, uint16_t channel)
{
    orderBuffer::orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initial_orderBuffer_ptr;
    while (akt_orderBuffer_ptr) 
    {
        if (akt_orderBuffer_ptr->node == node && akt_orderBuffer_ptr->channel == channel) {
            if ( ! orderBuffer::delEntry(akt_orderBuffer_ptr) ) {
                return 0;
            }
        }
        akt_orderBuffer_ptr=akt_orderBuffer_ptr->next;
    }
    return 1;
}
/*
 * Löscht alle Einträge mit der übergebenen OrderNo
 */
int orderBuffer::delOrderNo(uint16_t orderno)
{
    orderBuffer::orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initial_orderBuffer_ptr;
    while (akt_orderBuffer_ptr) 
    {
        if (akt_orderBuffer_ptr->orderno == orderno ) {
            if ( ! orderBuffer::delEntry(akt_orderBuffer_ptr) ) {
                return 0;
            }
        }
        akt_orderBuffer_ptr=akt_orderBuffer_ptr->next;
    }
    return 1;
}

/*
 * Generische Löschmethode, hier wird der übergebene Pointer aus der Liste entfernt
 * Alle anderen Löschmethoden ermitteln nur den zu löschenden Eintrag
 */
int orderBuffer::delEntry(orderBuffer::orderBuffer_t *orderBufferEntry)
{
    orderBuffer::orderBuffer_t *akt_orderBuffer_ptr, *last_orderBuffer_ptr;
    akt_orderBuffer_ptr = orderBuffer::initial_orderBuffer_ptr;
    int retval=0;
    while (akt_orderBuffer_ptr) 
    {
        if (akt_orderBuffer_ptr != orderBufferEntry )
        {
            last_orderBuffer_ptr = akt_orderBuffer_ptr;
            akt_orderBuffer_ptr=akt_orderBuffer_ptr->next;
        } 
        else
        {
            if (akt_orderBuffer_ptr == initial_orderBuffer_ptr)
            {
                initial_orderBuffer_ptr=akt_orderBuffer_ptr->next;
            }
            else 
            {
                last_orderBuffer_ptr->next=akt_orderBuffer_ptr->next;
            }
            delete orderBufferEntry;
            akt_orderBuffer_ptr=NULL;
            retval=1;
        }
    }
    return retval;
}

/*
 * Setzt die übergeben OrderNo wenn Node und Channel übereinstimmen
 */
int orderBuffer::setOrderNo(uint16_t orderno, uint16_t node, uint16_t channel)
{
    orderBuffer::orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initial_orderBuffer_ptr;
    while (akt_orderBuffer_ptr) {
        if(akt_orderBuffer_ptr->node == node && akt_orderBuffer_ptr->channel == channel) {
            akt_orderBuffer_ptr->orderno = orderno;
        }
        akt_orderBuffer_ptr=akt_orderBuffer_ptr->next;
    }
}

/*
 * Gibt den Bufferinhalt als HTML aus
 */
void orderBuffer::listOrderHTML(void)
{
    
}

/*
 * Gibt den Bufferinhalt als Text aus
 */
void orderBuffer::listOrder(void)
{
    orderBuffer::orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initial_orderBuffer_ptr;
    while (akt_orderBuffer_ptr) {
        printf("OrderNo: %u Node: %u Channel: %u Value: %f Entrytime: %lu \n",akt_orderBuffer_ptr->orderno, akt_orderBuffer_ptr->node, akt_orderBuffer_ptr->channel, akt_orderBuffer_ptr->value, akt_orderBuffer_ptr->entrytime);
        akt_orderBuffer_ptr = akt_orderBuffer_ptr->next;
    }    
}

