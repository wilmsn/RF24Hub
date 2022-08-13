/**
 * @file rf24gw_text.h all text messages for rf24gwd goes here => Easy to translate!
*/
#ifndef _RF24HUB_TEXT_H_   
#define _RF24HUB_TEXT_H_

const char tn_usage01[] = "Usage:"; 
const char tn_usage08[] = "set verbose <+/-><verboseLevel>";
const char tn_usage09[] = "   adds or deletes a verboseLevel. Valid levels are:";
const char tn_usage10[] = "   rf24; other; pointer";
const char tn_usage11[] = "set verbose all: adds all verboseLevel; set verbose none: deletes all";
const char tn_usage18[] = "show radio config";
const char tn_usage19[] = "   prints radio configuration to log";
const char tn_usage20[] = "show verbose";
const char tn_usage21[] = "   list the current verbose level(s)";
const char tn_usage22[] = "truncate logfile";
const char tn_usage23[] = "   truncates the logfile";

/// Dieses Textarray wird als usage im Telnet ausgegeben
const char *tn_usage_txt[] = { tn_usage01, tn_usage08, tn_usage09,
                               tn_usage10, tn_usage11, tn_usage18, tn_usage19, tn_usage20, tn_usage21, tn_usage22, tn_usage23 };

#endif
