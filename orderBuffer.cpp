#include "orderBuffer.h"
#include <stdio.h> 
#include <iostream>
#include <sys/socket.h>


extern uint64_t mymillis(void);
/*
 * Constructor
 * Hier wird ein Pointer für den Start der verketteten Liste angelegt.
 */
ORDERBUFFER::ORDERBUFFER() {
    iniOB_ptr = NULL;
    iniSB_ptr = NULL;
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
    orderBuffer_t *aktOB_ptr, *newOB_ptr;
    delNodeChannel(node, channel);
    newOB_ptr = new orderBuffer_t;
    if (iniOB_ptr) {
        aktBuffer_ptr = iniOB_ptr;
        while ( aktOB_ptr->next ) aktOB_ptr = aktOB_ptr->next;
        aktOB_ptr->next = newOB_ptr;
        aktOB_ptr = newOB_ptr;
    } else {
        iniOB_ptr = newOB_ptr;
        aktOB_ptr = newOB_ptr;
    }
    aktOB_ptr->orderno = 0;
    aktOB_ptr->node = node;
    aktOB_ptr->channel = channel;
    aktOB_ptr->value = value;
    aktOB_ptr->entrytime = mymillis();
    aktOB_ptr->next = NULL;

    printf ("################ %p, \n",iniOB_ptr);    
    aktOB_ptr=iniOB_ptr;
    int i=0;
    while (aktOB_ptr) {
        printf("Line %d: Node: %u Channel: %u \n",i,aktOB_ptr->node, aktOB_ptr->channel);
        aktOB_ptr=aktOB_ptr->next;
    }
    return 1;
}

/*
 * Löscht alle Einträge, die alter sind als die übergebene EntryTime
 */
int ORDERBUFFER::delOldOrder(uint64_t entrytime)
{
    orderBuffer_t *aktOB_ptr;
    aktOB_ptr = iniOB_ptr;
    while (aktOB_ptr) 
    {
        if (aktOB_ptr->entrytime < entrytime ) {
            if ( ! delEntry(aktOB_ptr) ) {
                return 0;
            }
        }
        aktOB_ptr=aktOB_ptr->next;
    }
    return 1;
}

/*
 * Löscht alle Einträge, für die übergebene Kombination aus Node und Channel
 */
int ORDERBUFFER::delNodeChannel(uint16_t node, uint16_t channel)
{
    orderBuffer_t *aktOB_ptr;
    aktOB_ptr = iniOB_ptr;
    while (aktOB_ptr) 
    {
        if (aktOB_ptr->node == node && aktOB_ptr->channel == channel) {
            if ( ! delEntry(aktOB_ptr) ) {
                return 0;
            }
        }
        aktOB_ptr=aktOB_ptr->next;
    }
    return 1;
}
/*
 * Löscht alle Einträge mit der übergebenen OrderNo
 */
int ORDERBUFFER::delOrderNo(uint16_t orderno)
{
    orderBuffer_t *aktOB_ptr;
    aktOB_ptr = iniOB_ptr;
    while (aktOB_ptr) 
    {
        if (aktOB_ptr->orderno == orderno ) {
            if ( ! delEntry(aktOB_ptr) ) {
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
void ORDERBUFFER::listOrder(int telnet_ptr, char msg_type) {
    orderBuffer_t *listOB_ptr;
    listOB_ptr = iniOB_ptr;
    char *msg =  (char*) malloc (1024);
    if (listOB_ptr) {
        switch (msg_type) {
            case 'h':
                sprintf(msg, "<tr><td>%u</td><td>%u</td><td>%u</td><td>%f</td><td>%lu</td></tr>\n", listBuffer_ptr->orderno, listBuffer_ptr->node, listBuffer_ptr->channel, listBuffer_ptr->value, listBuffer_ptr->entrytime);
            break;
            case 't':
            default:
                sprintf(msg, "OrderNo: %u  Node: %u  Channel: %u Value: %f Entrytime: %lu \n", listBuffer_ptr->orderno, listBuffer_ptr->node, listBuffer_ptr->channel, listBuffer_ptr->value, listBuffer_ptr->entrytime);
        }
        write(telnet_ptr , msg , strlen(msg));
        listOB_ptr=listOB_ptr->next;
    }
    free(msg);
}

