//=====[Libraries]=============================================================

#include "ObtainingPositionInformation.h" 
#include "GNSSModule.h" //debido a declaracion adelantadas
#include "Debugger.h" // due to global usbUart
#include "TurningOffGNSS.h"

//=====[Declaration of private defines]========================================
#define MAXRETRIES  30

#define LOG_MESSAGE "Obtaining GNSS Position\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define LOG_MESSAGE_NO_GNSS_WARNING "\r\n**GNSS UNAVAILABLE!**\r\n"
#define LOG_MESSAGE_NO_GNSS_WARNING_LEN (sizeof(LOG_MESSAGE_NO_GNSS_WARNING ) - 1)

#define LOG_MESSAGE_GNSS_NOT_ACTIVE_WARNING "GNSS NOT active\r\n"
#define LOG_MESSAGE_GNSS_NOT_ACTIVE_WARNING_LEN (sizeof(LOG_MESSAGE_GNSS_NOT_ACTIVE_WARNING) - 1)

#define LOG_MESSAGE_POSITION_ACQUIRED "\r\n**GNSS Acquired - Changing State!**\r\n"
#define LOG_MESSAGE_POSITION_ACQUIRED_LEN (sizeof(LOG_MESSAGE_POSITION_ACQUIRED) - 1)

#define AT_CMD_GNSS_POSITION_ACQUISITION "AT+QGPSLOC?"
#define AT_CMD_GNSS_POSITION_ACQUISITION_LEN  (sizeof(AT_CMD_GNSS_POSITION_ACQUISITION) - 1)

#define AT_CMD_GNSS_CONFIGURE_FORMAT "AT+QGPSLOC=2"
#define AT_CMD_GNSS_CONFIGURE_FORMAT_LEN  (sizeof(AT_CMD_GNSS_CONFIGURE_FORMAT) - 1)

#define AT_CMD_GNSS_EXPECTED_RESPONSE "OK"
#define AT_CMD_GNSS_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_GNSS_EXPECTED_RESPONSE) - 1)

#define AT_CMD_GNSS_POSITION_ACQUISITION_EXPECTED_RESPONSE "+QGPSLOC:"
#define AT_CMD_GNSS_POSITION_ACQUISITION_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_GNSS_POSITION_ACQUISITION_EXPECTED_RESPONSE) - 1)

#define AT_CMD_GNSS_ERROR_NO_ACTIVE_SESSION "+CME ERROR: 505"
#define AT_CMD_GNSS_ERROR_NO_ACTIVE_SESSION_LEN  (sizeof(AT_CMD_GNSS_ERROR_NO_ACTIVE_SESSION) - 1)


#define BUFFER_LEN 128
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ObtainingPositionInformation::ObtainingPositionInformation () {
    this->currentGNSSModule = nullptr;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAXRETRIES;
    
}

ObtainingPositionInformation::ObtainingPositionInformation  (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAXRETRIES;
}

ObtainingPositionInformation::~ObtainingPositionInformation  () {
    this->currentGNSSModule = nullptr;
}

GNSSState_t ObtainingPositionInformation::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
 
    char StringToSend [AT_CMD_GNSS_POSITION_ACQUISITION_LEN + 1] = AT_CMD_GNSS_POSITION_ACQUISITION ;
    char ChangeLatLongFormat [AT_CMD_GNSS_CONFIGURE_FORMAT_LEN + 1] = AT_CMD_GNSS_CONFIGURE_FORMAT;
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_GNSS_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_GNSS_EXPECTED_RESPONSE;
    char sessionNoTActive [AT_CMD_GNSS_ERROR_NO_ACTIVE_SESSION_LEN + 1] = AT_CMD_GNSS_ERROR_NO_ACTIVE_SESSION;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] =  LOG_MESSAGE;

    if (Geodata == nullptr || ATHandler == nullptr || refreshTime == nullptr) {
        return GNSS_STATE_ERROR_NULL_POINTER;
    }

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(ChangeLatLongFormat);
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (ChangeLatLongFormat , strlen (ChangeLatLongFormat ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ///
        if (this->retrivPositionInfo (StringToBeRead) == true ) {
            ////   ////   ////   ////   ////   ////
            uartUSB.write (LOG_MESSAGE_POSITION_ACQUIRED , strlen (LOG_MESSAGE_POSITION_ACQUIRED));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////  
            
            Geodata->latitude = this->latitude;
            Geodata->longitude = this->longitude;
            Geodata->hdop = this->hdop;
            Geodata->altitude = this->altitude;
            Geodata->fix = this->fix;
            Geodata->cog = this->cog;
            Geodata->spkm = this->spkm;
            Geodata->spkn = this->spkn;
            strcpy(Geodata->timestamp, this->date);
            strcat(Geodata->timestamp, this->utc);

            set_time(timestampToEpoch (Geodata->timestamp));  // save the epoch into RTC

            Geodata->nsat = this->nsat; 
            this->currentGNSSModule->changeGeopositioningState (new TurningOffGNSS (this->currentGNSSModule));
            return GNSS_STATE_CONNECTION_OBTAIN;
        }
        if (strcmp (StringToBeRead, sessionNoTActive) == 0 ) {
            uartUSB.write (LOG_MESSAGE_GNSS_NOT_ACTIVE_WARNING , strlen (LOG_MESSAGE_GNSS_NOT_ACTIVE_WARNING ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only         
            this->currentGNSSModule->changeGeopositioningState (new TurningOnGNSS (this->currentGNSSModule));
             return GNSS_STATE_TRYING_TO_CONNECT;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;    
        this->numberOfTries ++;
    
        if (this->numberOfTries >= this->maxTries) {
            uartUSB.write (LOG_MESSAGE_NO_GNSS_WARNING , strlen (LOG_MESSAGE_NO_GNSS_WARNING ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            this->numberOfTries = 0;
            this->currentGNSSModule->changeGeopositioningState (new TurningOffGNSS (this->currentGNSSModule));
            return GNSS_STATE_CONNECTION_UNAVAILABLE;
        }
       
    }

    return GNSS_STATE_TRYING_TO_CONNECT;
}


void ObtainingPositionInformation::enableGNSS ()  {
    return;
}


bool ObtainingPositionInformation::retrivPositionInfo(char *response) {
    char StringToCompare[AT_CMD_GNSS_POSITION_ACQUISITION_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_GNSS_POSITION_ACQUISITION_EXPECTED_RESPONSE;

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        char utc[UTC_LEN];          // <UTC>
        float latitude;        // <latitude> 
        float longitude;       // <longitude> 
        float hdop;            // <hdop>
        float altitude;        // <altitude>
        int fix;               // <fix>
        float cog;             // <cog>
        float spkm;            // <spkm>
        float spkn;            // <spkn>
        char date[DATE_LEN];          // <date> en formato DDMMYY
        int nsat;              // <nsat>

        int n = sscanf(response, "+QGPSLOC: %9[^,],%f,%f,%f,%f,%d,%f,%f,%f,%6[^,],%d",
                       utc, &latitude, &longitude, &hdop, &altitude, &fix, &cog, &spkm, &spkn, date, &nsat);
        
        if (n == 11) {

            utc[6] = '\0';
            strncpy(this->utc, utc, sizeof(this->utc));
            this->latitude = latitude;
            this->longitude = longitude;
            this->hdop = hdop;
            this->altitude = altitude;
            this->fix = fix;
            this->cog = cog;
            this->spkm = spkm;
            this->spkn = spkn;
            strncpy(this->date, date, sizeof(this->date));
            this->nsat = nsat;

            // DEBUGING //
            uartUSB.write("UTC: ", strlen("UTC: "));
            uartUSB.write(this->utc, strlen(this->utc));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Latitude: ", strlen("Latitude: "));
            char latitudeStr[15];
            sprintf(latitudeStr, "%.5f", this->latitude);
            uartUSB.write(latitudeStr, strlen(latitudeStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Longitude: ", strlen("Longitude: "));
            char longitudeStr[15];
            sprintf(longitudeStr, "%.5f", this->longitude);
            uartUSB.write(longitudeStr, strlen(longitudeStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("HDOP: ", strlen("HDOP: "));
            char hdopStr[10];
            sprintf(hdopStr, "%.1f", this->hdop);
            uartUSB.write(hdopStr, strlen(hdopStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Altitude: ", strlen("Altitude: "));
            char altitudeStr[10];
            sprintf(altitudeStr, "%.1f", this->altitude);
            uartUSB.write(altitudeStr, strlen(altitudeStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Fix: ", strlen("Fix: "));
            char fixStr[5];
            sprintf(fixStr, "%d", this->fix);
            uartUSB.write(fixStr, strlen(fixStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("COG: ", strlen("COG: "));
            char cogStr[10];
            sprintf(cogStr, "%.2f", this->cog);
            uartUSB.write(cogStr, strlen(cogStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Speed km/h: ", strlen("Speed km/h: "));
            char spkmStr[10];
            sprintf(spkmStr, "%.1f", this->spkm);
            uartUSB.write(spkmStr, strlen(spkmStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Speed kn: ", strlen("Speed kn: "));
            char spknStr[10];
            sprintf(spknStr, "%.1f", this->spkn);
            uartUSB.write(spknStr, strlen(spknStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Date: ", strlen("Date: "));
            uartUSB.write(this->date, strlen(this->date));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Number of Satellites: ", strlen("Number of Satellites: "));
            char nsatStr[5];
            sprintf(nsatStr, "%d", this->nsat);
            uartUSB.write(nsatStr, strlen(nsatStr));
            uartUSB.write("\r\n", 2);
            // DEBUGING //

            return true;
        }
    }
    
    return false;
}

//=====[Implementations of private functions]==================================
