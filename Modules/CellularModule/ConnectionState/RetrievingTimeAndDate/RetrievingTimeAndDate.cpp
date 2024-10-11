//=====[Libraries]=============================================================

#include "RetrievingTimeAndDate.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
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
RetrievingTimeAndDate::RetrievingTimeAndDate (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->timeAndDateRetrived = false;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}


/** 
* @brief 
* 
* 
* @returns 
*/
RetrievingTimeAndDate::~RetrievingTimeAndDate () {
    this->mobileNetworkModule = NULL;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void RetrievingTimeAndDate::enableConnection () {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
CellularConnectionStatus_t RetrievingTimeAndDate::connect (ATCommandHandler * ATHandler,
 NonBlockingDelay * refreshTime, CellInformation * currentCellInformation) {

    static char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSend [15] =  "AT+QLTS=1";

    char StringToSendUSB [40] = "RETRIVING TIME AND DATE ";

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        refreshTime->restart();
        ////   ////   ////   ////   ////   //// 
    }

    if ( this->timeAndDateRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             refreshTime->restart();
           if (this->retrieveNetworkTime (StringToBeRead)) {
                this->timeAndDateRetrived = true;
            }
        } 
    } 

    if (this->timeAndDateRetrived  == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////     
                char StringToSendUSB [40] = "Cambiando de estado 6";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////            
                strcpy (currentCellInformation->date, this->date);
                strcpy (currentCellInformation->time, this->time);
                this->mobileNetworkModule->changeConnectionState 
                (new AttachingToPacketService (this->mobileNetworkModule) );
                return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            return CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_RETRIV_DATETIME;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}

bool RetrievingTimeAndDate::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}

//=====[Implementations of private functions]==================================
bool RetrievingTimeAndDate::retrieveNetworkTime(char *response) {
    char StringToCompare[8] = "+QLTS: ";

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        char *DateAndTimePart = response + strlen(StringToCompare) + 1;
        
        // Extraer la fecha
        char year[3], month[3], day[3];
        strncpy(year, DateAndTimePart + 2, 2);   // "24" (últimos dos dígitos del año)
        year[2] = '\0';
        strncpy(month, DateAndTimePart + 5, 2);  // "08" (mes)
        month[2] = '\0';
        strncpy(day, DateAndTimePart + 8, 2);    // "13" (día)
        day[2] = '\0';

        snprintf(this->date, 7, "%s%s%s", day, month, year);

        // Extraer la hora en formato HHMMSS
        char hour[3], minute[3], second[3];
        strncpy(hour, DateAndTimePart + 11, 2);   // "19" (hora)
        hour[2] = '\0';
        strncpy(minute, DateAndTimePart + 14, 2); // "56" (minuto)
        minute[2] = '\0';
        strncpy(second, DateAndTimePart + 17, 2); // "05" (segundo)
        second[2] = '\0';

        snprintf(this->time, 7, "%s%s%s", hour, minute, second);

        // Mostrar la fecha y hora en el formato deseado (debug)
        uartUSB.write("Date: ", strlen("Date: ")); 
        uartUSB.write(this->date, strlen(this->date)); 
        uartUSB.write("\r\n", 2);

        uartUSB.write("Time: ", strlen("Time: ")); 
        uartUSB.write(this->time, strlen(this->time)); 
        uartUSB.write("\r\n", 2);

        return true;
    }

    return false;
}
