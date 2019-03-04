#include "config.h"
//#include "rf24hub_common.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>


void processParams(int argc, char* argv[]) {
//    char config_file[PARAM_MAXLEN_CONFIGFILE];
    strcpy(config_file,"x");
	int c;
//    char debug[DEBUGSTRINGSIZE];
	
    // processing argc and argv[]
	while (1) {
		static struct option long_options[] = {	
			{"daemon",  no_argument, 0, 'd'},
			{"verbose",  required_argument, 0, 'v'},
            		{"configfile",    required_argument, 0, 'c'},
			{"scanner", required_argument, 0, 's'},
                        {"scannchannel", required_argument, 0, 't'},
            		{"help", no_argument, 0, 'h'},
            		{0, 0, 0, 0} 
		};
        /* getopt_long stores the option index here. */
        int option_index = 0;
        c = getopt_long (argc, argv, "?dht:s:v:c:",long_options, &option_index);
        /* Detect the end of the options. */
        if (c == -1) break;
        switch (c) {
            case 't':
                      uint8_t channel;
                      if ( optarg[0] ) {
                        channel=optarg[0]-'0';
                        if ( optarg[1] ) {
                          channel=channel*10+optarg[1]-'0';
                          if ( optarg[2] ) {
                            channel=channel*10+optarg[2]-'0';
                          }
                        }
                        if (channel < 126) {
 //                         channelscanner(channel); 
                        } else {
                          printf("Error Channel must be in 0 ... 125\n");
                        }
                      } else {
                        printf("Error Channel required\n");
                      }
                      exit(0);
                      break;

            case 's':
                      if (optarg[0] && ! optarg[1]) {
//                         scanner(optarg[0]);
                      } else {
                         usage();
                      }
                      exit(0);
                      break;
            case 'd':
				start_daemon = true;
				logmode = logfile;
            break;
			case 'v':
                verboselevel = (optarg[0] - '0') * 1;
            break;
            case 'c':
                strcpy(config_file, optarg);
            break;
            case 'h':
            case '?':
                usage();
                exit (0);
            break;
            default:
                usage ();
                exit (0);
        }
    }
    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
    // END processing argc and argv[]
    strcpy(config_file,"x");
	
    // processing argc and argv[]
	while (1) {
		static struct option long_options[] = {	
			{"daemon",  no_argument, 0, 'd'},
			{"verbose",  required_argument, 0, 'v'},
            		{"configfile",    required_argument, 0, 'c'},
			{"scanner", required_argument, 0, 's'},
                        {"scannchannel", required_argument, 0, 't'},
            		{"help", no_argument, 0, 'h'},
            		{0, 0, 0, 0} 
		};
        /* getopt_long stores the option index here. */
        int option_index = 0;
        c = getopt_long (argc, argv, "?dht:s:v:c:",long_options, &option_index);
        /* Detect the end of the options. */
        if (c == -1) break;
        switch (c) {
            case 't':
                      uint8_t channel;
                      if ( optarg[0] ) {
                        channel=optarg[0]-'0';
                        if ( optarg[1] ) {
                          channel=channel*10+optarg[1]-'0';
                          if ( optarg[2] ) {
                            channel=channel*10+optarg[2]-'0';
                          }
                        }
                        if (channel < 126) {
 //                         channelscanner(channel); 
                        } else {
                          printf("Error Channel must be in 0 ... 125\n");
                        }
                      } else {
                        printf("Error Channel required\n");
                      }
                      exit(0);
                      break;

            case 's':
                      if (optarg[0] && ! optarg[1]) {
//                         scanner(optarg[0]);
                      } else {
                         usage();
                      }
                      exit(0);
                      break;
            case 'd':
				start_daemon = true;
				logmode = logfile;
            break;
			case 'v':
                verboselevel = (optarg[0] - '0') * 1;
            break;
            case 'c':
                strcpy(config_file, optarg);
            break;
            case 'h':
            case '?':
                usage();
                exit (0);
            break;
            default:
                usage ();
                exit (0);
        }
    }
    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
    // END processing argc and argv[]    
}

void parseConfigFile (char * config_file ) {
    char *s, buff[256];
    // check if config file is readable
    if ( strcmp(config_file,"x") == 0 ) strcpy(config_file,DEFAULT_CONFIG_FILE);
    if (fopen (config_file, "r") == NULL) {
        fprintf(stdout, "Config file: \"%s\" not found, terminating\n\n", config_file);
        exit(1);
    }
    // Reading and processing and printing config file
    printf ("Reading configuration from %s\n",config_file);
//    printf ("Reading config file...\n");
    FILE *fp = fopen (config_file, "r");
    if (fp == NULL) {
        printf( "Configfile %s nicht gefunden!\n", config_file );
        exit(1);
    }
    /* Read lines */
    while ((s = fgets (buff, sizeof buff, fp)) != NULL) {
        /* Skip blank lines and comments */
        trim(buff);
        if (buff[0] == '\n' || buff[0] == '#')
            continue;
        /* Parse name/value pair from line */
        char name[PARAM_MAXLEN], value[PARAM_MAXLEN];
        s = strtok (buff, "=");
        if (s==NULL)
            continue;
        else
            strncpy (name, s, PARAM_MAXLEN);
        s = strtok (NULL, "=");
        if (s==NULL)
            continue;
        else
            strncpy (value, s, PARAM_MAXLEN);
        trim (value);
    /* Copy into correct entry in parameters struct */
        if      (strcmp(name, "db_hostname")==0) strcpy (parms.db_hostname, value);
        else if (strcmp(name, "db_port")==0)              parms.db_port = atoi(value);
        else if (strcmp(name, "db_schema")==0)   strcpy (parms.db_schema, value);
        else if (strcmp(name, "db_username")==0) strcpy (parms.db_username, value);
        else if (strcmp(name, "db_password")==0) strcpy (parms.db_password, value);
        else if (strcmp(name, "verboselevel")==0) parms.verboselevel = atoi(value);
    else if (strcmp(name, "telnet_hostname")==0) {
                strcpy (parms.telnet_hostname, value);
                tn_host_set=true;
        }
    else if (strcmp(name, "telnet_port")==0) {
                parms.telnet_port = atoi(value);
                tn_port_set=true;
        }
    else if (strcmp(name, "incoming_port")==0) {
                parms.incoming_port = atoi(value);
                in_port_set=true;
        }
    else if (strcmp(name, "logfile")==0)     strcpy (parms.logfilename, value);
    else if (strcmp(name, "pidfile")==0)     strcpy (parms.pidfilename, value);
    else if (strcmp(name, "rf24network_channel")==0) parms.rf24network_channel = atoi(value);
    else if (strcmp(name, "rf24network_speed")==0) {
                if (strcmp(value, "RF24_2MBPS")==0) {
                        parms.rf24network_speed = RF24_2MBPS;
                }
                else if (strcmp(value, "RF24_250KBPS")==0) {
                        parms.rf24network_speed = RF24_250KBPS;
                }
                else if (strcmp(value, "RF24_1MBPS")==0) {
                        parms.rf24network_speed = RF24_1MBPS;
                }
                else {
                        printf ("%s: Unknown value for %s ! Use RF24_1MBPS \n", value, name);
                        parms.rf24network_speed = RF24_1MBPS;
                }
    }
        else
      printf ("WARNING: %s/%s: Unknown name/value pair!\n", name, value);
  }
  /* Close file */
  fclose (fp);
}

void printConfig (void) {
    printf ("Logfile: %s\n", parms.logfilename);
    printf ("PIDfile: %s\n", parms.pidfilename);
    printf ("DB-Hostname: %s\n", parms.db_hostname);
    printf ("DB-Port: %d\n", parms.db_port);
    printf ("DB-Schema: %s\n", parms.db_schema);
    printf ("DB-Username: %s\n", parms.db_username);
    printf ("DB-Password: %s\n", parms.db_password);
    printf ("Telnet-Hostname: %s\n", parms.telnet_hostname);
    printf ("Telnet-Port: %d\n", parms.telnet_port);
    printf ("Verboselevel: %d\n", parms.verboselevel);
//    printf ("Ende print_config\n");
}

void usage(void) {
    fprintf(stdout, "%s version %s\n", PRGNAME, PRGVERSION);
    fprintf(stdout, "Usage: %s <option>\n", PRGNAME);
    fprintf(stdout, "with options: \n");
    fprintf(stdout, "   -h or -? or --help \n");
    fprintf(stdout, "           Print help\n");
    fprintf(stdout, "   -d or --daemon\n");
    fprintf(stdout, "         Start as daemon\n");
    fprintf(stdout, "   -c or --configfilename <filename>\n");
    fprintf(stdout, "         Set configfilename\n");
    fprintf(stdout, "   -v or --verbose <verboselevel>\n");
    fprintf(stdout, "         Set verboselevel (1...9)\n");
    fprintf(stdout, "   -s or --scanner <scanlevel>\n");
    fprintf(stdout, "         Set scannlevel (0...9) and scan all channels\n");
    fprintf(stdout, "   -t or --scannchannel <channel>\n");
    fprintf(stdout, "         Scanns the single channel <channel>\n");
    fprintf(stdout, "For clean exit use \"CTRL-C\" or \"kill -15 <pid>\"\n\n");
}


    // get pid and write it to pidfile
int setPidfile(void) {
    pid_t pid;
    pid=getpid();
    char debug[DEBUGSTRINGSIZE];
    pidfile_ptr = fopen (parms.pidfilename,"w");
    if ( logfile_ptr == NULL ) {
        logmode = interactive;
        fprintf(stdout,"Could not open %s for writing\n Printing logs to console\n", parms.logfilename );
        return 0;
    } else {
        fclose(logfile_ptr);
        logmode = logfile;
		sprintf(debug, "Start logging to %s", parms.logfilename);
        logmsg(VERBOSESTARTUP, debug);
    }
    sprintf(debug, "%s running with PID: %d", PRGNAME, pid);
    logmsg(VERBOSESTARTUP, debug);
    return 1;
}

void removePidfile(void) {
        unlink(parms.pidfilename);
}
/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char * trim (char * s) {
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];
  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';
  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;
  /* Copy finished string */
  strcpy (s, s1);
  return s;
}
