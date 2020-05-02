#include "config.h"

Config::Config(string _prgName, string _prgVersion) {
     prgName = _prgName;
     prgVersion = _prgVersion;
}

Config::~Config() {
}

Config::Config() {
}

void Config::processParams(int argc, char* argv[]) {
    configFile = "x";
	int c;
	
    // processing argc and argv[]
	while (1) {
		static struct option long_options[] = {	
			{"daemon",  no_argument, 0, 'd'},
            {"configfile",    required_argument, 0, 'c'},
			{"scanner", required_argument, 0, 's'},
            {"scannchannel", required_argument, 0, 't'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0} 
		};
        /* getopt_long stores the option index here. */
        int option_index = 0;
        c = getopt_long (argc, argv, "?dht:s:c:",long_options, &option_index);
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
            case 'd':
				startDaemon = true;
            break;
            case 'c':
                //std::string s;
                configFile = optarg;
            break;
            case 'h':
            case '?':
//                 usage();
//                exit (0);
//            break;
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
    // check if config file is readable
    if ( configFile == "x" ) configFile = DEFAULT_CONFIG_FILE;
    // Reading and processing and printing config file
//    printf("%s\n",configFile.c_str());
    FILE *fp = fopen (configFile.c_str(), "r");
    if (fp == NULL) {
        cout << "Configfile " << configFile << " nicht gefunden!" << endl;
        exit(1);
    } else {
		cout << "Reading configuration from " << configFile << endl;
    }
    // Processing config file
    char *s, buff[256];
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
    if      (strcmp(name, "db_hostname")==0) dbHostName = value;
    else if (strcmp(name, "db_port")==0)     dbPort = value;
    else if (strcmp(name, "db_schema")==0)   dbSchema = value;
    else if (strcmp(name, "db_username")==0) dbUserName = value;
    else if (strcmp(name, "db_password")==0) dbPassWord = value;
//    else if (strcmp(name, "rf24hub_verboselevel")==0) {
//            if (! verboseLevel) verboseLevel = atoi(value);
//		}
    else if (strcmp(name, "fhem_hostname")==0) {
            fhemHost = value;
            fhemHostSet=true;
        }
    else if (strcmp(name, "fhem_port")==0) {
            fhemPort = value;
            fhemPortSet = true;
        }
    else if (strcmp(name, "incoming_port")==0) {
            incomingPort = value;
            incomingPortSet = true;
        }
    else if (strcmp(name, "logfile")==0) {
            logFileName = value;
        }
    else if (strcmp(name, "pidfile")==0) {
            pidFileName = value;
		}
    else
      cout << "WARNING: " << name << "=" << value << ": Unknown name=value pair!" << endl;
  }
  /* Close file */
	fclose (fp);
    if ( startDaemon ) {
        logfile_ptr = fopen (logFileName.c_str(),"a");
        if ( ! logfile_ptr ) {
            cerr << "LOGFILE: "<< logFileName<< " can't open logfile, terminating !!!" << endl;
            exit(1);
        }
        fclose( logfile_ptr );
    }
}

void Config::printConfig (void) {
    cout << "Logfile: "        << logFileName << endl;
    cout << "PIDfile: "        << pidFileName << endl;
    cout << "DB-Hostname:"     << dbHostName << endl;
    cout << "DB-Port:"         << dbPort << endl;
    cout << "DB-Schema:"       << dbSchema << endl;
    cout << "DB-Username:"     << dbUserName << endl;
    cout << "DB-Password:"     << dbPassWord << endl;
    cout << "FHEM-Hostname:"   << fhemHost << endl;
    cout << "FHEM-Port:"       << fhemPort << endl;
    cout << "incoming Port:"   << incomingPort << endl;
}

void Config::usage(void) {
    cout << PRGNAME << " version " << PRGVERSION << endl;
    cout << "Usage: " << PRGNAME << " <option>" << endl;
    cout << "with options: " << endl;
    cout << "   -h or -? or --help" << endl;
    cout << "           Print help" << endl;
    cout << "   -d or --daemon" << endl;
    cout << "         Starts as daemon" << endl;
    cout << "   -c or --configfilename <filename>" << endl;
    cout << "         Set configfilename" << endl;
    cout << "   -s or --scanner <scanlevel>" << endl;
    cout << "         Set scannlevel (0...9) and scan all channels" << endl;
    cout << "   -t or --scannchannel <channel>" << endl;
    cout << "         Scanns the single channel <channel>" << endl;
    cout << "For clean exit use \"CTRL-C\" or \"kill -15 <pid>\"" << endl;
}


    // get pid and write it to pidfile
int Config::setPidFile(void) {
    pid_t pid;
    pid=getpid();
	pidfile_ptr = fopen (pidFileName.c_str(),"w");
    fprintf(pidfile_ptr,"%d",pid);
    fclose(pidfile_ptr);
    return 1;
}

int Config::checkPidFileSet(void) {
	if( access( pidFileName.c_str(), F_OK ) != -1 ) {
		cerr << "PIDFILE: "; cerr << pidFileName; cerr << " exists, terminating" << endl;
		return -1;
	} else {
		return 0;
	}
}

void Config::removePidFile() {
    unlink(pidFileName.c_str());
}
