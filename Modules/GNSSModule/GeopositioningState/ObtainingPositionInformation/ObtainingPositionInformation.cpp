//=====[Libraries]=============================================================

#include "ObtainingPositionInformation.h" 
#include "GNSSModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "TurningOffGNSS.h"

//=====[Declaration of private defines]========================================
#define MAXRETRIES  5

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
ObtainingPositionInformation::ObtainingPositionInformation () {
    this->currentGNSSModule = NULL;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAXRETRIES;
    
}


/** 
* @brief
* 
* @param 
*/
ObtainingPositionInformation::ObtainingPositionInformation  (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAXRETRIES;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ObtainingPositionInformation::~ObtainingPositionInformation  () {
    this->currentGNSSModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
GNSSState_t ObtainingPositionInformation::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
 
    char StringToSend [15] = "AT+QGPSLOC?";
    char ChangeLatLongFormat [15] = "AT+QGPSLOC=2";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char sessionNoTActive [20] = "+CME ERROR: 505";
    char StringToSendUSB [40] = "OBTAINING POSITION INFO";

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

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ///
        if (this->retrivPositionInfo (StringToBeRead) == true ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS Acquired - Changing State!";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////  
            strncpy(Geodata->utc, this->utc, sizeof(Geodata->utc));
            Geodata->latitude = this->latitude;
            Geodata->longitude = this->longitude;
            Geodata->hdop = this->hdop;
            Geodata->altitude = this->altitude;
            Geodata->fix = this->fix;
            Geodata->cog = this->cog;
            Geodata->spkm = this->spkm;
            Geodata->spkn = this->spkn;
            strncpy( Geodata->date, this->date, sizeof(Geodata->date));
            Geodata->nsat = this->nsat; 
            this->currentGNSSModule->changeGeopositioningState (new TurningOffGNSS (this->currentGNSSModule));
            return GNSS_STATE_CONNECTION_OBTAIN;
        }
        if (strcmp (StringToBeRead, sessionNoTActive) == 0 ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS NOT active";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->currentGNSSModule->changeGeopositioningState (new TurningOnGNSS (this->currentGNSSModule));
             return GNSS_STATE_TRYING_TO_CONNECT;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;    
        this->numberOfTries ++;
    
        char StringToSendUSB [40] = "+1 counter retry";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (this->numberOfTries >= this->maxTries) {
             ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS UNAVAILABLE, TURNING OFF";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////    
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
    char StringToCompare[10] = "+QGPSLOC:";
    
    // Verificar si la respuesta comienza con "+QGPSLOC: "
    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        // Variables para almacenar los campos parseados
        char utc[10];          // <UTC> en formato HHMMSS.SS
        float latitude;        // <latitude> en formato decimal
        float longitude;       // <longitude> en formato decimal
        float hdop;            // <hdop>
        float altitude;        // <altitude>
        int fix;               // <fix>
        float cog;             // <cog>
        float spkm;            // <spkm>
        float spkn;            // <spkn>
        char date[7];          // <date> en formato DDMMYY
        int nsat;              // <nsat>

        // Parsear la respuesta
        int n = sscanf(response, "+QGPSLOC: %9[^,],%f,%f,%f,%f,%d,%f,%f,%f,%6[^,],%d",
                       utc, &latitude, &longitude, &hdop, &altitude, &fix, &cog, &spkm, &spkn, date, &nsat);
        
        // Verificar que se hayan parseado correctamente los 11 valores
        if (n == 11) {
            // Asignar los valores a los atributos del objeto
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
