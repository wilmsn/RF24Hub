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
     prgIsHub = ( prgName == RF24HUB_PRGNAME );
     prgIsGW = ( prgName == RF24GW_PRGNAME );
}

CONFIG::~CONFIG() {
    if ( prgIsHub ) {
		if ( logFileMode ) {
			char msg[] = "Rf24Hub: Logfile closed";
			logmsg(VERBOSESTARTUP, msg);
		}
	}
}

void CONFIG::processParams(int argc, char* argv[]) {
    configFile = "x";
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
    // check if config file is readable
    if ( configFile == "x" ) configFile = DEFAULT_CONFIG_FILE;
    // Reading and processing and printing config file
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
    else if (strcmp(name, "rf24hub_verboselevel")==0) {
            if (! verboseLevel) verboseLevel = atoi(value);
		}
    else if (strcmp(name, "rf24hub_hostname")==0) {
            rf24HubHostName = value;
        }
    else if (strcmp(name, "fhem_hostname")==0) {
            fhemHostName = value;
            fhemHostSet=true;
        }
    else if (strcmp(name, "fhem_port")==0) {
            fhemPort = value;
            fhemPortSet = true;
        }
    else if (strcmp(name, "rf24hub_tcp_port")==0) {
            rf24HubTcpPort = value;
            rf24HubTcpPortSet = true;
        }
    else if (strcmp(name, "rf24hub_udp_port")==0) {
            rf24HubUdpPort = value;
            rf24HubUdpPortSet = true;
        }
    else if (strcmp(name, "rf24gw_udp_port")==0) {
            rf24GWUdpPort = value;
            rf24GWUdpPortSet = true;
        }
    else if (strcmp(name, "rf24hub_logfile")==0) {
            rf24HubLogFileName = value;
            if ( prgIsHub ) {
				logFileMode = true;
                logFileName = value;
			}
        }
    else if (strcmp(name, "rf24hub_pidfile")==0) {
            rf24HubPidFileName = value;
            if ( prgIsHub ) {
                pidFileName = value;
			}
		}
    else if (strcmp(name, "rf24gw_logfile")==0) {
            rf24GWLogFileName = value;
            if ( prgIsGW ) {
				logFileMode = true;
                logFileName = value;
			}
        }
    else if (strcmp(name, "rf24gw_pidfile")==0) {
            rf24GWPidFileName = value;
            if ( prgIsGW ) {
                pidFileName = value;
			}
		}
    else if (strcmp(name, "rf24_channel")==0) {
		   rf24Channel = value;
	   }
    else if (strcmp(name, "rf24_speed")==0) {
            if (strcmp(value, "RF24_2MBPS")==0 || strcmp(value, "RF24_250KBPS")==0 || strcmp(value, "RF24_1MBPS")==0) {
                rf24Speed = value;
            } else {
                cout << value << ": Unknown value for " << name << "! I use RF24_1MBPS " << endl;
                rf24Speed = "RF24_1MBPS";
            }
        } 
    else
      cout << "WARNING: " << name << "/" << value << ": Unknown name/value pair!" << endl;
  }
  /* Close file */
	fclose (fp);
//  if ( logFileMode ) {
 //   interactiveMode = false;
//  }
	if ( forceInteractiveMode ) {
		startDaemon = false;
		interactiveMode = true;
		logFileMode = false;
	}
	if ( logFileMode ) {
		char msg[] = "Logfile opened";
		logmsg(VERBOSESTARTUP, msg);
	}
/*	if ( prgIsGW ) {
		logFileName = rf24GWLogFileName;
		pidFileName = rf24GWPidFileName;
	}
	if ( prgIsHub ) {
		logFileName = rf24HubLogFileName;
		pidFileName = rf24HubPidFileName;
	} */
	cout << logFileName << endl;
    ofstream out(logFileName, ios_base::app);
	out << "This is the Logfile: " << logFileName << endl;
	out.close();
}

void CONFIG::printConfig (void) {
    cout << "Logfile: "        << logFileName << endl;
    cout << "PIDfile: "        << pidFileName << endl;
    cout << "DB-Hostname:"     << dbHostName << endl;
    cout << "DB-Port:"         << dbPort << endl;
    cout << "DB-Schema:"       << dbSchema << endl;
    cout << "DB-Username:"     << dbUserName << endl;
    cout << "DB-Password:"     << dbPassWord << endl;
    cout << "FHEM-Hostname:"   << fhemHostName << endl;
    cout << "FHEM-Port:"       << fhemPort << endl;
    cout << "Hub telnet Port:" << rf24HubTcpPort << endl;
    cout << "Hub Udp Port:"    << rf24HubUdpPort << endl;
    cout << "GW Udp Port:"     << rf24GWUdpPort << endl;
    if ( rf24HubUdpPortSet ) cout << "rf24HubUdpPortSet = true" << endl; else cout << "rf24HubUdpPortSet = false" << endl;
    cout << "Verboselevel:"    << verboseLevel << endl;
#ifdef DEBUG    
    cout << prgName << " -- " << RF24HUB_PRGNAME << endl;
    cout << prgName << " -- " << RF24GW_PRGNAME << endl;
     if ( prgIsGW ) cout << "Bin GW" << endl;
     if ( prgIsHub ) cout << "Bin Hub" << endl;
#endif     
}

void CONFIG::usage(void) {
    cout << prgName << " version " << prgVersion << endl;
    cout << "Usage: " << prgName << " <option>" << endl;
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
	pidfile_ptr = fopen (pidFileName.c_str(),"w");
    fprintf(pidfile_ptr,"%d",pid);
    fclose(pidfile_ptr);
    return 1;
}

int CONFIG::checkPidFileSet(void) {
	if( access( pidFileName.c_str(), F_OK ) != -1 ) {
		cerr << "PIDFILE: "; cerr << pidFileName; cerr << " exists, terminating" << endl;
		return -1;
	} else {
		return 0;
	}
}

void CONFIG::removePidFile() {
    unlink(pidFileName.c_str());
}

void CONFIG::logmsg(int mesgloglevel, char* mymsg){
/*	string mymsg_str = mymsg;
	CONFIG::logmsg(mesgloglevel, mymsg_str);
}

void CONFIG::logmsg(int mesgloglevel, string mymsg){ */
#ifdef DEBUG	
    if ( logFileMode ) { printf("Logmode: true \n");} else {printf("Logmode: false \n");}
    if ( interactiveMode ) { printf("interactiveMode: true \n"); } else { printf("interactiveMode: false \n"); }     
	printf("MSG: %s\n",mymsg.c_str());
#endif
	if (mesgloglevel <= verboseLevel) {
        if ( logFileMode ) {
            char timestr[20];
			char m0[2], d0[2], mi0[2], s0[2];
            m0[1]='\0'; d0[1]='\0'; mi0[1]='\0'; s0[1]='\0';
            time_t now = time(0);
			tm *ltm = localtime(&now);
            m0[0] = (ltm->tm_mon < 10) ? '0' : '\0';
            d0[0] = (ltm->tm_mday < 10) ? '0' : '\0';
            mi0[0] = (ltm->tm_min < 10) ? '0' : '\0';
            s0[0] = (ltm->tm_sec < 10) ? '0' : '\0';
            sprintf(timestr, "[%d.%s%d.%s%d %d:%s%d:%s%d]",ltm->tm_year + 1900,m0,ltm->tm_mon+1,d0,ltm->tm_mday,ltm->tm_hour, mi0, ltm->tm_min, s0, ltm->tm_sec);
            logfile_ptr = fopen(logFileName.c_str(), "a");
            if ( logfile_ptr ) {
				fprintf(logfile_ptr,"%s %s \n",timestr, mymsg);
				fclose(logfile_ptr);
			} else {
				printf("Error opening Logfile %s\n",logFileName.c_str());
			}
        }	
        if ( interactiveMode ) {  // logmode == interactive 
			printf("%s\n",mymsg);
		}
    }
}

float CONFIG::sensorValue(uint16_t val) {
// highest bit: result is negative if set
// second highest bit: divide by 100 if set
	bool negative = val &  0b1000000000000000;
	bool devide100 = val & 0b0100000000000000;
	float result = val &   0b0011111111111111;
	if ( devide100 ) result = result / 100.0;
	if ( negative ) result = -1.0 * result;
	return result;	
}

uint16_t CONFIG::uint16Value(float val) {
    float _val = val; 
    uint16_t result;
	bool negative = val < 0;
	bool devide100 = val > 999;
	if ( negative ) _val = abs(val);
	if ( devide100 ) _val = _val * 100;
	result = (uint16_t) _val;
	result &= 0b0011111111111111;
	if ( negative ) result |= 0b1000000000000000;
	if ( devide100 ) result |= 0b0100000000000000;
	return (uint16_t) result; 
}


