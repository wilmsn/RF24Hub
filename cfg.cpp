#include "cfg.h"

Cfg::Cfg(string _prgName, string _prgVersion) {
     prgName = _prgName;
     prgVersion = _prgVersion;
}

//Cfg::~Cfg() {
//}

//Cfg::Cfg() {
//}

void Cfg::processParams(const char* prgName, int argc, char* argv[]) {
    configFile = "x";
	int c;
	
    // processing argc and argv[]
	while (1) {
		static struct option long_options[] = {	
			{"daemon",  no_argument, 0, 'd'},
			{"sniffer",  no_argument, 0, 'S'},
            {"configfile",    required_argument, 0, 'c'},
			{"verbose", required_argument, 0, 'v'},
			{"scanner", required_argument, 0, 's'},
            {"scannchannel", required_argument, 0, 't'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0} 
		};
        /* getopt_long stores the option index here. */
        int option_index = 0;
        c = getopt_long (argc, argv, "?dhSt:s:c:v:",long_options, &option_index);
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
                             channelScanner_Channel=channel; 
                        } else {
                          printf("Error Channel must be in 0 ... 125\n");
                        }
                      } else {
                        printf("Error Channel required\n");;
                      }
                      break;
            case 'v':
                      if (optarg[0]) {
                          verboselevel = decodeVerbose(verboselevel, optarg);
                      } else {
                          usage(prgName);
                      }
                      break;
            case 's':
                      if (optarg[0] && ! optarg[1]) {
                          startScanner = true;
                          setScanLevel = (int) optarg[0]-'0';
                      } else {
                          usage(prgName);
                      }
                      break;
            case 'S':
				startSniffer = true;
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
                 usage (prgName);
                 exit (0);
        }
    }
    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf("non-option ARGV-elements: \n");
        while (optind < argc) printf("%s\n",argv[optind++]);
    }
    // END processing argc and argv[]
    // check if config file is readable
    if ( configFile == "x" ) configFile = DEFAULT_CONFIG_FILE;
    // Reading and processing and printing config file
//    printf("%s\n",configFile.c_str());
    FILE *fp = fopen (configFile.c_str(), "r");
    if (fp == NULL) {
        fprintf(stderr,"Configfile: %s nicht gefunden!\n", configFile.c_str() );
        exit(1);
    } else {
		printf("Reading configuration from %s\n", configFile.c_str() );
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
    else if (strcmp(name, "db_port")==0)     dbPortNo = value;
    else if (strcmp(name, "db_schema")==0)   dbSchema = value;
    else if (strcmp(name, "db_username")==0) dbUserName = value;
    else if (strcmp(name, "db_password")==0) dbPassWord = value;
//    else if (strcmp(name, "rf24hub_verboselevel")==0) {
//            if (! verboseLevel) verboseLevel = atoi(value);
//		}
    else if (strcmp(name, "fhem_hostname")==0) {
            fhemHostName = value;
            fhemHostSet=true;
        }
    else if (strcmp(name, "fhem_port")==0) {
            fhemPortNo = value;
            fhemPortSet = true;
        }
    else if (strcmp(name, "hub_tcp_port")==0) {
            hubTcpPortNo = value;
            hubTcpPortSet = true;
        }
    else if (strcmp(name, "gw_tcp_port")==0) {
            gwTcpPortNo = value;
            gwTcpPortSet = true;
        }
    else if (strcmp(name, "hub_logfile")==0) {
            hubLogFileName = value;
        }
    else if (strcmp(name, "hub_pidfile")==0) {
            hubPidFileName = value;
		}
    else if (strcmp(name, "gw_logfile")==0) {
            gwLogFileName = value;
        }
    else if (strcmp(name, "gw_pidfile")==0) {
            gwPidFileName = value;
		}
    else if (strcmp(name, "gw_hubhostname")==0) {
            gwHubHostName = value;
        }
    else if (strcmp(name, "hub_udp_portno")==0) {
            hubUdpPortNo = value;
            hubUdpPortSet = true;
		}
    else if (strcmp(name, "gw_udp_portno")==0) {
            gwUdpPortNo = value;
            gwUdpPortSet = true;
		}
    else if (strcmp(name, "gw_no")==0) {
            gwNo = value;
		}
    else
      printf("WARNING: %s = %s Unknown name=value pair!\n", name, value );
  }
  /* Close file */
	fclose (fp);
/*    if ( startDaemon ) {
        logfile_ptr = fopen (logFileName.c_str(),"a");
        if ( ! logfile_ptr ) {
            fprintf(stderr,"LOGFILE: %s can't open logfile, terminating !!!\n", logFileName.c_str());
            exit(1);
        }
        fclose( logfile_ptr );
    } */
}

void Cfg::printConfig_hub (void) {
    printf("Hub Logfile: %s\n", hubLogFileName.c_str() );
    printf("Hub PIDfile: %s\n", hubPidFileName.c_str() );
    printf("Hub-UDP Port: %s\n", hubUdpPortNo.c_str() );
    printf("Gateway UDP Port: %s\n", gwUdpPortNo.c_str() );
    printf("FHEM-Hostname: %s\n", fhemHostName.c_str() );
    printf("FHEM-Port: %s\n", fhemPortNo.c_str() );
    printf("incoming Telnet Port: %s\n", hubTcpPortNo.c_str() );
}

void Cfg::printConfig_gw (void) {
    printf("Gw Logfile: %s\n", gwLogFileName.c_str() );
    printf("Gw PIDfile: %s\n", gwPidFileName.c_str() );
    printf("Hub-Hostname: %s\n", gwHubHostName.c_str() );
    printf("Hub-UDP Port: %s\n", hubUdpPortNo.c_str() );
    printf("Gateway UDP Port: %s\n", gwUdpPortNo.c_str() );
    printf("Gw ID: %s\n", gwNo.c_str() );
}

void Cfg::printConfig_db (void) {
    printf("DB-Hostname: %s\n", dbHostName.c_str() );
    printf("DB-Port: %s\n", dbPortNo.c_str() );
    printf("DB-Schema: %s\n", dbSchema.c_str() );
    printf("DB-Username: %s\n", dbUserName.c_str() );
    printf("DB-Password: %s\n", dbPassWord.c_str() );
}

void Cfg::usage(const char* prgName) {
    printf("%s Version %d vom %s\n",prgName, SWVERSION, SWDATUM);
    printf("Usage: %s <option>\n",prgName);
    printf("with options: \n");
    printf("   -h or -? or --help\n");
    printf("           Print help\n");
    printf("   -d or --daemon\n");
    printf("         Starts as daemon\n");
    printf("   -v or --verbose +<verboselevel>\n");
    printf("         Sets the verboselevel, can be repeated\n");
    printf("         Verboselevels are: sql, telnet, rf24, order, orderext,\n");
    printf("                            obuffer, obufferext, pointer, other\n");
    printf("   -c or --configfilename <filename>\n");
    printf("         Set configfilename\n");
    printf("   -s or --scanner <scanlevel>\n");
    printf("         Set scannlevel (0...9) and scan all channels\n");
    printf("   -S or --sniffer\n");
    printf("         Starts a sniffer and prints out all catched payloads\n");
    printf("   -t or --scannchannel <channel>\n");
    printf("         Scanns the single channel <channel>\n");
    printf("For clean exit use \"CTRL-C\" or \"kill -15 <pid>\"\n");
}


bool Cfg::setPidFile(string pidFileName) {
    pid_t pid;
    bool retval = false;
    pid=getpid();
	pidfile_ptr = fopen (pidFileName.c_str(),"w");
    if ( pidfile_ptr ) {
        fprintf(pidfile_ptr,"%d",pid);
        fclose(pidfile_ptr);
        retval = true;
    }
    return retval;
}

bool Cfg::checkPidFileSet(string pidFileName) {
	if( access( pidFileName.c_str(), F_OK ) != -1 ) {
		fprintf(stderr,"PIDFILE: %s exists\n", pidFileName.c_str() );
		return true;
	} else {
		return false;
	}
}

void Cfg::removePidFile(string pidFileName) {
    unlink(pidFileName.c_str());
}
