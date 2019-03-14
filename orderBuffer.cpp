#include "orderBuffer.h"
#include <stdio.h> 
#include <iostream>


extern uint64_t mymillis(void);
/*
 * Constructor
 * Hier wird ein Pointer für den Start der verketteten Liste angelegt.
 */
ORDERBUFFER::ORDERBUFFER() {
    initialBuffer_ptr = NULL;
}

void * ORDERBUFFER::getInitialBuffer_ptr(void) {
    return (void *) initialBuffer_ptr;
}

/*
 * Legt einen neuen Eintrag im OrderBuffer an.
 * Alle neuen Einträge bekommen zunächst die OrderNo 0
 * Die OrderNo wird erst gesetzt wenn diese Order in die OrderQueue kommt
 * Ist für die übergebene Kombination aus Node und Channel bereits ein Eintrag vorhanden,
 * wird dieser gelöscht!
 */
int ORDERBUFFER::newOrder(uint16_t node, uint16_t channel, float value)
{
    orderBuffer_t *aktBuffer_ptr, *newBuffer_ptr;
    delNodeChannel(node, channel);
    newBuffer_ptr = new orderBuffer_t;
    if (! initialBuffer_ptr) {
        initialBuffer_ptr = newBuffer_ptr;
        aktBuffer_ptr = newBuffer_ptr;
    } else {
        aktBuffer_ptr = initialBuffer_ptr;
        while ( aktBuffer_ptr->next ) aktBuffer_ptr = aktBuffer_ptr->next;
        aktBuffer_ptr->next = newBuffer_ptr;
        aktBuffer_ptr = newBuffer_ptr;
    }
    aktBuffer_ptr->orderno = 0;
    aktBuffer_ptr->node = node;
    aktBuffer_ptr->channel = channel;
    aktBuffer_ptr->value = value;
    aktBuffer_ptr->entrytime = mymillis();
    aktBuffer_ptr->next = NULL;

    printf ("################ %p, \n",initialBuffer_ptr);    
    aktBuffer_ptr=initialBuffer_ptr;
    int i=0;
    while (aktBuffer_ptr) {
        printf("Line %d: Node: %u Channel: %u \n",i,aktBuffer_ptr->node, aktBuffer_ptr->channel);
        aktBuffer_ptr=aktBuffer_ptr->next;
    }
    return 1;
}

/*
 * Löscht alle Einträge, die alter sind als die übergebene EntryTime
 */
int ORDERBUFFER::delOldOrder(uint64_t entrytime)
{
    orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initialBuffer_ptr;
    while (akt_orderBuffer_ptr) 
    {
        if (akt_orderBuffer_ptr->entrytime < entrytime ) {
            if ( ! delEntry(akt_orderBuffer_ptr) ) {
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
int ORDERBUFFER::delNodeChannel(uint16_t node, uint16_t channel)
{
    orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initialBuffer_ptr;
    while (akt_orderBuffer_ptr) 
    {
        if (akt_orderBuffer_ptr->node == node && akt_orderBuffer_ptr->channel == channel) {
            if ( ! delEntry(akt_orderBuffer_ptr) ) {
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
int ORDERBUFFER::delOrderNo(uint16_t orderno)
{
    orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initialBuffer_ptr;
    while (akt_orderBuffer_ptr) 
    {
        if (akt_orderBuffer_ptr->orderno == orderno ) {
            if ( ! delEntry(akt_orderBuffer_ptr) ) {
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
int ORDERBUFFER::delEntry(orderBuffer_t *orderBufferEntry)
{
    orderBuffer_t *akt_orderBuffer_ptr, *last_orderBuffer_ptr;
    akt_orderBuffer_ptr = initialBuffer_ptr;
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
            if (akt_orderBuffer_ptr == initialBuffer_ptr)
            {
                initialBuffer_ptr=akt_orderBuffer_ptr->next;
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
int ORDERBUFFER::setOrderNo(uint16_t orderno, uint16_t node, uint16_t channel)
{
    orderBuffer_t *akt_orderBuffer_ptr;
    akt_orderBuffer_ptr = initialBuffer_ptr;
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
void ORDERBUFFER::listOrderHTML(void)
{
    
}

/*
 * Gibt den Bufferinhalt als Text aus
 */
void* ORDERBUFFER::listOrder(void * pos, char * msg) {
    orderBuffer_t *aktBuffer_ptr, *listBuffer_ptr;
//    aktBuffer_ptr = initialBuffer_ptr;
//    while (aktBuffer_ptr) {
//printf(">>0<< aktBuffer_ptr: %p\n",aktBuffer_ptr);
//      aktBuffer_ptr=aktBuffer_ptr->next;
//    }
        
    aktBuffer_ptr = initialBuffer_ptr;
//printf(">>1<< pos: %p\n",pos);    
    while (aktBuffer_ptr) {
//printf(">>1a<< akt_orderBuffer_ptr: %p\n",aktBuffer_ptr);    
        if (aktBuffer_ptr == (orderBuffer_t *) pos) {
            listBuffer_ptr = aktBuffer_ptr;
//printf(">>1b<< listBuffer_ptr: %p\n",listBuffer_ptr);    
        }            
        aktBuffer_ptr = aktBuffer_ptr->next;
    }
//printf(">>2<< listBuffer_ptr: %p\n", listBuffer_ptr);    
    if (listBuffer_ptr) {
//printf(">>2a<< listBuffer_ptr: %p listBuffer_ptr->next: %p\n", listBuffer_ptr, listBuffer_ptr->next);    
        sprintf(msg, "OrderNo: %u  Node: %u  Channel: %u Value: %f Entrytime: %lu \n", listBuffer_ptr->orderno, listBuffer_ptr->node, listBuffer_ptr->channel, listBuffer_ptr->value, listBuffer_ptr->entrytime);
//printf(">>>> %s\n",msg);
    }
//printf(">>3<<\n");    
    return (void*) listBuffer_ptr->next;
}

