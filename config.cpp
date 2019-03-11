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
    CONFIG::prgName = prgName;
    CONFIG::prgVersion = prgVersion;
}

CONFIG::~CONFIG() {
    CONFIG::logmsg(1,"Logfile closed");
}

void CONFIG::processParams(int argc, char* argv[]) {
    char config_file[PARAM_MAXLEN_CONFIGFILE];
    strcpy(config_file,"x");
	int c;
    int verboselevel = 0;
	
    // by default we log to console
    CONFIG::interactive_mode = true;

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
                         CONFIG::usage();
                      }
                      exit(0);
                      break;
            case 'i':
				interactive_mode = true;
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
                CONFIG::usage();
                exit (0);
            break;
            default:
                CONFIG::usage ();
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
        CONFIG::trim(buff);
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
        CONFIG::trim (value);
    /* Copy into correct entry in parameters struct */
        if      (strcmp(name, "db_hostname")==0) strcpy (CONFIG::parms.db_hostname, value);
        else if (strcmp(name, "db_port")==0)             CONFIG::parms.db_port = atoi(value);
        else if (strcmp(name, "db_schema")==0)   strcpy (CONFIG::parms.db_schema, value);
        else if (strcmp(name, "db_username")==0) strcpy (CONFIG::parms.db_username, value);
        else if (strcmp(name, "db_password")==0) strcpy (CONFIG::parms.db_password, value);
        else if (strcmp(name, "verboselevel")==0) {
            if (verboselevel) CONFIG::parms.verboselevel = verboselevel;
            else CONFIG::parms.verboselevel = atoi(value);
        }
    else if (strcmp(name, "telnet_hostname")==0) {
                strcpy (CONFIG::parms.telnet_hostname, value);
                CONFIG::tn_host_set=true;
        }
    else if (strcmp(name, "telnet_port")==0) {
                CONFIG::parms.telnet_port = atoi(value);
                CONFIG::tn_port_set=true;
        }
    else if (strcmp(name, "incoming_port")==0) {
                CONFIG::parms.incoming_port = atoi(value);
                CONFIG::in_port_set=true;
        }
    else if (strcmp(name, "logfile")==0) {
                strcpy (CONFIG::parms.logfilename, value);
                CONFIG::logfile_mode = true;
                CONFIG::logmsg(VERBOSESTARTUP, "Logfile opened");
    }
    else if (strcmp(name, "pidfile")==0)     strcpy (CONFIG::parms.pidfilename, value);
    else if (strcmp(name, "rf24network_channel")==0) CONFIG::parms.rf24network_channel = atoi(value);
    else if (strcmp(name, "rf24network_speed")==0) {
                if (strcmp(value, "RF24_2MBPS")==0) {
                        CONFIG::parms.rf24network_speed = RF24_2MBPS;
                }
                else if (strcmp(value, "RF24_250KBPS")==0) {
                        CONFIG::parms.rf24network_speed = RF24_250KBPS;
                }
                else if (strcmp(value, "RF24_1MBPS")==0) {
                        CONFIG::parms.rf24network_speed = RF24_1MBPS;
                }
                else {
                        printf ("%s: Unknown value for %s ! Use RF24_1MBPS \n", value, name);
                        CONFIG::parms.rf24network_speed = RF24_1MBPS;
                }
    }
        else
      printf ("WARNING: %s/%s: Unknown name/value pair!\n", name, value);
  }
  /* Close file */
  fclose (fp);
  CONFIG::verboselevel = CONFIG::parms.verboselevel;
}

void CONFIG::printConfig (void) {
    printf ("Logfile: %s\n", CONFIG::parms.logfilename);
    printf ("PIDfile: %s\n", CONFIG::parms.pidfilename);
    printf ("DB-Hostname: %s\n", CONFIG::parms.db_hostname);
    printf ("DB-Port: %d\n", CONFIG::parms.db_port);
    printf ("DB-Schema: %s\n", CONFIG::parms.db_schema);
    printf ("DB-Username: %s\n", CONFIG::parms.db_username);
    printf ("DB-Password: %s\n", CONFIG::parms.db_password);
    printf ("Telnet-Hostname: %s\n", CONFIG::parms.telnet_hostname);
    printf ("Telnet-Port: %d\n", CONFIG::parms.telnet_port);
    printf ("Verboselevel: %d\n", CONFIG::parms.verboselevel);
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
    pidfile_ptr = fopen (CONFIG::parms.pidfilename,"w");
    fprintf(pidfile_ptr,"%d",pid);
    fclose(pidfile_ptr);
    return 1;
}

int CONFIG::checkPidFileSet(void) {
    if( access( parms.pidfilename, F_OK ) != -1 ) {
        fprintf(stderr, "PIDFILE: %s exists, terminating\n\n", CONFIG::parms.pidfilename);
        return 0;
    } else {
        return 1;
    }
}

void CONFIG::removePidFile(void) {
        unlink(CONFIG::parms.pidfilename);
}

void CONFIG::logmsg(int mesgloglevel, std::string mymsg){
	if (mesgloglevel <= CONFIG::verboselevel) {
        if ( CONFIG::logfile_mode ) {
            std::string line;
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
            std::ofstream out(CONFIG::parms.logfilename, std::ios_base::app);
            out << line << std::endl;
            out.close();
        }	
        if ( CONFIG::interactive_mode ) {  // logmode == interactive 
			std::cout << mymsg << std::endl; 
		}
    }
}
