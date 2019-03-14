#include "config.h"
//#include "rf24hub_common.h"

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char * CONFIG::trim (char * s) {
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

CONFIG::CONFIG(string prgName, string prgVersion) {
     prgName = prgName;
     prgVersion = prgVersion;
}

CONFIG::~CONFIG() {
    if (  logfile_mode )  logmsg(VERBOSESTARTUP,"Logfile closed");
}

void CONFIG::processParams(int argc, char* argv[]) {
    char config_file[PARAM_MAXLEN_CONFIGFILE];
    strcpy(config_file,"x");
	int c;
    int verboselevel = 0;
    bool forceInteractive_mode = false;
	
    // by default we log to console
     interactive_mode = true;

    // processing argc and argv[]
	while (1) {
		static struct option long_options[] = {	
			{"daemon",  no_argument, 0, 'd'},
			{"interactive",  no_argument, 0, 'i'},
			{"verbose",  required_argument, 0, 'v'},
            {"configfile",    required_argument, 0, 'c'},
			{"scanner", required_argument, 0, 's'},
            {"scannchannel", required_argument, 0, 't'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0} 
		};
        /* getopt_long stores the option index here. */
        int option_index = 0;
        c = getopt_long (argc, argv, "?dhit:s:v:c:",long_options, &option_index);
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
                             startChannelScanner=channel; 
                        } else {
                          printf("Error Channel must be in 0 ... 125\n");
                        }
                      } else {
                        printf("Error Channel required\n");
                      }
                      break;

            case 's':
                      if (optarg[0] && ! optarg[1]) {
                          startScanner = true;
                          setScanLevel = (int) optarg[0]-'0';
                      } else {
                          usage();
                      }
                      break;
            case 'i':
				forceInteractive_mode = true;
            case 'd':
				start_daemon = true;
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
    // Processing config file
    char *s, buff[256];
    // check if config file is readable
    if ( strcmp(config_file,"x") == 0 ) strcpy(config_file,DEFAULT_CONFIG_FILE);
    if (fopen (config_file, "r") == NULL) {
        printf("Config file: \"%s\" not found, terminating\n\n", config_file);
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
        if      (strcmp(name, "db_hostname")==0) strcpy ( parms.db_hostname, value);
        else if (strcmp(name, "db_port")==0)              parms.db_port = atoi(value);
        else if (strcmp(name, "db_schema")==0)   strcpy ( parms.db_schema, value);
        else if (strcmp(name, "db_username")==0) strcpy ( parms.db_username, value);
        else if (strcmp(name, "db_password")==0) strcpy ( parms.db_password, value);
        else if (strcmp(name, "verboselevel")==0) {
            if (verboselevel)  parms.verboselevel = verboselevel;
            else  parms.verboselevel = atoi(value);
        }
    else if (strcmp(name, "telnet_hostname")==0) {
                strcpy ( parms.telnet_hostname, value);
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
    else if (strcmp(name, "logfile")==0) {
                strcpy ( parms.logfilename, value);
                 logfile_mode = true;
    }
    else if (strcmp(name, "pidfile")==0)     strcpy ( parms.pidfilename, value);
    else if (strcmp(name, "rf24network_channel")==0)  parms.rf24network_channel = atoi(value);
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
  if ( logfile_mode ) {
    interactive_mode = false;
  }
  if ( forceInteractive_mode ) {
      start_daemon = false;
      interactive_mode = true;
      logfile_mode = false;
  }
  if ( logfile_mode ) logmsg(VERBOSESTARTUP, "Logfile opened");

}

void CONFIG::printConfig (void) {
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
}

void CONFIG::usage(void) {
    printf("%s version %s\n", PRGNAME, PRGVERSION);
    printf("Usage: %s <option>\n", PRGNAME);
    printf("with options: \n");
    printf("   -h or -? or --help \n");
    printf("           Print help\n");
    printf("   -d or --daemon\n");
    printf("         Starts as daemon\n");
    printf("   -i or --interactive\n");
    printf("         Starts in interactive mode without logfile\n");
    printf("   -c or --configfilename <filename>\n");
    printf("         Set configfilename\n");
    printf("   -v or --verbose <verboselevel>\n");
    printf("         Set verboselevel (1...9)\n");
    printf("   -s or --scanner <scanlevel>\n");
    printf("         Set scannlevel (0...9) and scan all channels\n");
    printf("   -t or --scannchannel <channel>\n");
    printf("         Scanns the single channel <channel>\n");
    printf("For clean exit use \"CTRL-C\" or \"kill -15 <pid>\"\n\n");
}


    // get pid and write it to pidfile
int CONFIG::setPidFile(void) {
    pid_t pid;
    pid=getpid();
    pidfile_ptr = fopen (parms.pidfilename,"w");
    fprintf(pidfile_ptr,"%d",pid);
    fclose(pidfile_ptr);
    return 1;
}

int CONFIG::checkPidFileSet(void) {
    if( access( parms.pidfilename, F_OK ) != -1 ) {
        fprintf(stderr, "PIDFILE: %s exists, terminating\n\n", parms.pidfilename);
        return 1;
    } else {
        return 0;
    }
}

void CONFIG::removePidFile(void) {
        unlink(parms.pidfilename);
}

void CONFIG::logmsg(int mesgloglevel, string mymsg){
	if (mesgloglevel <= parms.verboselevel) {
        if ( logfile_mode ) {
            string line;
            char timestr[20];
			char m0[2], d0[2], mi0[2], s0[2];
            m0[1]='\0'; d0[1]='\0'; mi0[1]='\0'; s0[1]='\0';
            time_t now = time(0);
			tm *ltm = localtime(&now);
            m0[0] = (ltm->tm_mon < 10) ? '0' : '\0';
            d0[0] = (ltm->tm_mday < 10) ? '0' : '\0';
            mi0[0] = (ltm->tm_min < 10) ? '0' : '\0';
            s0[0] = (ltm->tm_sec < 10) ? '0' : '\0';
            sprintf(timestr, "%d.%s%d.%s%d %d:%s%d:%s%d",ltm->tm_year + 1900,m0,ltm->tm_mon+1,d0,ltm->tm_mday,ltm->tm_hour, mi0, ltm->tm_min, s0, ltm->tm_sec);
            line = "[";
            line += timestr;
            line += "] ";
            line += mymsg;
            ofstream out(parms.logfilename, ios_base::app);
            out << line << endl;
            out.close();
        }	
        if ( interactive_mode ) {  // logmode == interactive 
			cout << mymsg << endl; 
		}
    }
}
