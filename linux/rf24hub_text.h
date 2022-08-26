/**
 * @file rf24hub_text.h all text messages for rf24hubd goes here => Easy to translate!
*/
#ifndef _RF24HUB_TEXT_H_   
#define _RF24HUB_TEXT_H_

const char tn_usage01[]  = "Usage:"; 
const char tn_usage02[]  = "set sensor <sensornumber> <sensorvalue>";
const char tn_usage03[]  = "   Sets the sensor <sensornumber> to the value <sensorvalue>";
const char tn_usage04[]  = "push <node> <channel> <sensorvalue>";
const char tn_usage05[]  = "   Pushes a value direct to a channel inside a node";
const char tn_usage06[]  = "set verbose <+/-><verboseLevel>";
const char tn_usage07[]  = "   adds or deletes a verboseLevel. Valid levels are:";
const char tn_usage08[]  = "   config; telnet; rf24; sql; sensor; order; orderext; obuffer; obufferext; other; pointer";
const char tn_usage09[]  = "   set verbose all: adds all verboseLevel; set verbose none: deletes all";
const char tn_usage10[]  = "set node <node_no> <mastered/unmastered>";
const char tn_usage11[]  = "   sets a node to mastered (gets commands from this Hub) or unmastered";
const char tn_usage12[]  = "set gateway <gateway_no> <on/off>";
const char tn_usage13[]  = "   sets a gateway active (on) or inactive (off)";
const char tn_usage14[]  = "add gateway <gatewayname> <gateway_no>";
const char tn_usage15[]  = "   adds a active gateway to the system";
const char tn_usage16[]  = "delete gateway <gateway_no>";
const char tn_usage17[]  = "   deletes a gateway from the system";
const char tn_usage18[]  = "init";
const char tn_usage19[]  = "   Reinitialisation of rf24hub";
const char tn_usage20[]  = "sync";
const char tn_usage21[]  = "   Syncs all in Memory tables to the persistent tables";
const char tn_usage22[]  = "show order";
const char tn_usage23[]  = "   lists the content of the order queue";
const char tn_usage24[]  = "show sensor";
const char tn_usage25[]  = "   lists the nodes and sensors in system";
const char tn_usage26[]  = "show gateway";
const char tn_usage27[]  = "   lists the gateways in system";
const char tn_usage28[]  = "show verbose";
const char tn_usage29[]  = "   list the current verbose level(s)";
const char tn_usage30[]  = "truncate logfile";
const char tn_usage31[]  = "   truncates the logfile";

const char *tn_usage_txt[] = { tn_usage01, tn_usage02, tn_usage03, tn_usage04, tn_usage05, tn_usage06, tn_usage07, tn_usage08,
                               tn_usage09,
                               tn_usage10, tn_usage11, tn_usage12, tn_usage13, tn_usage14, tn_usage15, tn_usage16, tn_usage17, tn_usage18, 
                               tn_usage19, tn_usage20, tn_usage21, tn_usage22, tn_usage23, tn_usage24, tn_usage25, tn_usage26, tn_usage27,
                               tn_usage28, tn_usage29, tn_usage30, tn_usage31 };
                               
unsigned int tn_usage_size = 31;                               

#endif
