#include "config.h"
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

CONFIG::CONFIG(string _prgName, string _prgVersion) {
     prgName = _prgName;
     prgVersion = _prgVersion;
}

CONFIG::~CONFIG() {
    if ( logfileMode )  logmsg(VERBOSESTARTUP,"Logfile closed");
}

void CONFIG::processParams(int argc, char* argv[]) {
    char config_file[PARAM_MAXLEN_CONFIGFILE];
    strcpy(config_file,"x");
	int c;
    int verboselevel = 0;
    bool forceInteractiveMode = false;
	
    // by default we log to console
     interactiveMode = true;

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
                          cout << "Error Channel must be in 0 ... 125" << endl;
                        }
                      } else {
                        cout << "Error Channel required\n" << endl;
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
				forceInteractiveMode = true;
            case 'd':
				startDaemon = true;
				interactiveMode = false;
            break;
			case 'v':
                verboseLevel = (optarg[0] - '0') * 1;
                parms.verboseLevel = verboseLevel;
            break;
            case 'c':
                //std::string s;
                configFile = optarg;
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
        cout << "non-option ARGV-elements: " << endl;
        while (optind < argc) cout << argv[optind++] << endl;
    }
    // END processing argc and argv[]
    // Processing config file
    char *s, buff[256];
    // check if config file is readable
    if ( strcmp(config_file,"x") == 0 ) strcpy(config_file,DEFAULT_CONFIG_FILE);
    if (fopen (config_file, "r") == NULL) {
        cout << "Config file: " << configFile << " not found, terminating" << endl;
        exit(1);
    }
    // Reading and processing and printing config file
    cout << "Reading configuration from " << config_file << endl;
//    printf ("Reading config file...\n");
    FILE *fp = fopen (config_file, "r");
    if (fp == NULL) {
        cout << "Configfile " << configFile << " nicht gefunden!" << endl;
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
    if      (strcmp(name, "db_hostname")==0) parms.dbHostname = value;
    else if (strcmp(name, "db_port")==0)     parms.dbPort = atoi(value);
    else if (strcmp(name, "db_schema")==0)   parms.dbSchema = value;
    else if (strcmp(name, "db_username")==0) parms.dbUsername = value;
    else if (strcmp(name, "db_password")==0) parms.dbPassword = value;
    else if (strcmp(name, "verboselevel")==0) {
            if (verboseLevel)  parms.verboseLevel = verboseLevel;
            else  parms.verboseLevel = atoi(value);
        }
    else if (strcmp(name, "fhem_hostname")==0) {
            parms.fhemHostname = value;
            fhemHostSet=true;
        }
    else if (strcmp(name, "fhem_port")==0) {
            parms.fhemPort = atoi(value);
            fhemPortSet=true;
        }
    else if (strcmp(name, "telnet_port")==0) {
            parms.telnetPort = value;
            telnetPortSet=true;
        }
    else if (strcmp(name, "udp_port")==0) {
            parms.udpPort = value;
            udpPortSet=true;
        }
    else if (strcmp(name, "logfile")==0) {
            parms.logFilename = value;
            logfileMode = true;
        }
    else if (strcmp(name, "pidfile")==0)                parms.pidFilename = value;
    else if (strcmp(name, "rf24network_channel")==0)    parms.rf24NetworkChannel = atoi(value);
    else if (strcmp(name, "rf24network_speed")==0) {
            if (strcmp(value, "RF24_2MBPS")==0 || strcmp(value, "RF24_250KBPS")==0 || strcmp(value, "RF24_1MBPS")==0) {
                parms.rf24NetworkSpeed = value;
            } else {
                cout << value << ": Unknown value for " << name << "! I use RF24_1MBPS " << endl;
                parms.rf24NetworkSpeed = "RF24_1MBPS";
            }
        } 
    else
      cout << "WARNING: " << name << "/" << value << ": Unknown name/value pair!" << endl;
  }
  /* Close file */
  fclose (fp);
  if ( logfileMode ) {
 //   interactiveMode = false;
  }
  if ( forceInteractiveMode ) {
      startDaemon = false;
      interactiveMode = true;
      logfileMode = false;
  }
  if ( logfileMode ) logmsg(VERBOSESTARTUP, "Logfile opened");

}

void CONFIG::printConfig (void) {
    cout << "Logfile: "        << parms.logFilename << endl;
    cout << "PIDfile: "        << parms.pidFilename << endl;
    cout << "DB-Hostname:"     << parms.dbHostname << endl;
    cout << "DB-Port:"         << parms.dbPort << endl;
    cout << "DB-Schema:"       << parms.dbSchema << endl;
    cout << "DB-Username:"     << parms.dbUsername << endl;
    cout << "DB-Password:"     << parms.dbPassword << endl;
    cout << "FHEM-Hostname:"   << parms.fhemHostname << endl;
    cout << "FHEM-Port:"       << parms.fhemPort << endl;
    cout << "telnet Port:"     << parms.telnetPort << endl;
    cout << "udp Port:"        << parms.udpPort << endl;
    cout << "Verboselevel:"    << parms.verboseLevel << endl;
}

void CONFIG::usage(void) {
    cout << PRGNAME << " version " << PRGVERSION << endl;
    cout << "Usage: " << PRGNAME << " <option>" << endl;
    cout << "with options: " << endl;
    cout << "   -h or -? or --help" << endl;
    cout << "           Print help" << endl;
    cout << "   -d or --daemon" << endl;
    cout << "         Starts as daemon" << endl;
    cout << "   -i or --interactive" << endl;
    cout << "         Starts in interactive mode without logfile" << endl;
    cout << "   -c or --configfilename <filename>" << endl;
    cout << "         Set configfilename" << endl;
    cout << "   -v or --verbose <verboselevel>" << endl;
    cout << "         Set verboselevel (1...9)" << endl;
    cout << "   -s or --scanner <scanlevel>" << endl;
    cout << "         Set scannlevel (0...9) and scan all channels" << endl;
    cout << "   -t or --scannchannel <channel>" << endl;
    cout << "         Scanns the single channel <channel>" << endl;
    cout << "For clean exit use \"CTRL-C\" or \"kill -15 <pid>\"" << endl;
}


    // get pid and write it to pidfile
int CONFIG::setPidFile(void) {
    pid_t pid;
    pid=getpid();
    pidfile_ptr = fopen (parms.pidFilename.c_str(),"w");
    fprintf(pidfile_ptr,"%d",pid);
    fclose(pidfile_ptr);
    return 1;
}

int CONFIG::checkPidFileSet(void) {
    if( access( parms.pidFilename.c_str(), F_OK ) != -1 ) {
        cerr << "PIDFILE: "; cerr << parms.pidFilename; cerr << " exists, terminating" << endl;
        return 1;
    } else {
        return 0;
    }
}

void CONFIG::removePidFile(void) {
        unlink(parms.pidFilename.c_str());
}

void CONFIG::logmsg(int mesgloglevel, string mymsg){
	if (mesgloglevel <= parms.verboseLevel) {
        if ( logfileMode ) {
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
            ofstream out(parms.logFilename, ios_base::app);
            out << line << endl;
            out.close();
        }	
        if ( interactiveMode ) {  // logmode == interactive 
			cout << mymsg << endl; 
		}
    }
}
