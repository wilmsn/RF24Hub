/*
rf24hub_text.h ==> all text goes here => Easy to translate!

*/
#ifndef _RF24HUB_TEXT_H_   
#define _RF24HUB_TEXT_H_

const char tn_usage01[] = "Usage:"; 
const char tn_usage02[] = "set[last] sensor <sensornumber> <sensorvalue>";
const char tn_usage03[] = "   Sets the sensor <sensornumber> to the value <sensorvalue>";
const char tn_usage04[] = "   'set' stores only; 'setlast' executes all settings for none HeartBeat Node";
const char tn_usage05[] = "push <node> <channel> <sensorvalue>";
const char tn_usage06[] = "   Pushes a value direct to a channel inside a node";
const char tn_usage08[] = "set verbose <+/-><verboselevel>";
const char tn_usage09[] = "   adds or deletes a verboselevel. Valid levels are:";
const char tn_usage10[] = "   config; telnet; rf24; sql; sensor; order; orderext; obuffer; obufferext; other; pointer";
const char tn_usage11[] = "set verbose all: adds all verboselevel; set verbose none: deletes all";
const char tn_usage12[] = "init";
const char tn_usage13[] = "   Reinitialisation of rf24hub (all open ordes will be deleted)";
const char tn_usage12a[] = "sync";
const char tn_usage13a[] = "   Syncs all in Memory tables to the persistent tables";
const char tn_usage14[] = "show order";
const char tn_usage15[] = "   lists the content of the order queue";
const char tn_usage16[] = "show sensor";
const char tn_usage17[] = "   lists the nodes and sensors in system";
const char tn_usage18[] = "show radio config";
const char tn_usage19[] = "   prints radio configuration to log";
const char tn_usage20[] = "show verbose";
const char tn_usage21[] = "   list the current verbose level(s)";

const char *tn_usage_txt[] = { tn_usage01, tn_usage02, tn_usage03, tn_usage04, tn_usage05, tn_usage06, tn_usage08, tn_usage09,
                               tn_usage10, tn_usage11, tn_usage12, tn_usage13, tn_usage12a, tn_usage13a, tn_usage14, tn_usage15, tn_usage16, tn_usage17, tn_usage18, tn_usage19, tn_usage20, tn_usage21 };

#endif
