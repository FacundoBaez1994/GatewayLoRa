//=====[Libraries]=============================================================

#include "RetrievingTimeAndDate.h"
#include "CellularModule.h" 
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_QUERY_TIME_AND_DATE    "AT+QLTS=1"
#define AT_CMD_QUERY_TIME_AND_DATE_LEN  (sizeof(AT_CMD_QUERY_TIME_AND_DATE) - 1)

#define AT_CMD_QUERY_TIME_AND_DATE_EXPECTED_RESPONSE     "OK"
#define AT_CMD_QUERY_TIME_AND_DATE_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_QUERY_TIME_AND_DATE_EXPECTED_RESPONSE ) - 1)

#define LOG_MESSAGE "Retrieving Time And Date\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define BUFFER_LEN 128
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================


//=====[Implementations of public methods]===================================
RetrievingTimeAndDate::RetrievingTimeAndDate (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->timeAndDateRetrived = false;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}

RetrievingTimeAndDate::~RetrievingTimeAndDate () {
    this->mobileNetworkModule = nullptr;
    this->connectionAttempts = 0;
}

void RetrievingTimeAndDate::enableConnection () {
    return;
}

CellularConnectionStatus_t RetrievingTimeAndDate::connect (ATCommandHandler * ATHandler,
 NonBlockingDelay * refreshTime, CellInformation * currentCellInformation) {

    static char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_QUERY_TIME_AND_DATE_EXPECTED_RESPONSE_LEN + 1 ] = AT_CMD_QUERY_TIME_AND_DATE_EXPECTED_RESPONSE;
    char StringToSend [AT_CMD_QUERY_TIME_AND_DATE_LEN + 1] =  AT_CMD_QUERY_TIME_AND_DATE;

    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;

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
                strcpy(currentCellInformation->timestamp, this->date);
                strcat(currentCellInformation->timestamp, this->time);

                set_time(timestampToEpoch (currentCellInformation->timestamp));  // save the epoch into RTC
                
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
            this->connectionAttempts = 0;
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
        strncpy(year, DateAndTimePart + 2, 2);  
        year[2] = '\0';
        strncpy(month, DateAndTimePart + 5, 2); 
        month[2] = '\0';
        strncpy(day, DateAndTimePart + 8, 2);  
        day[2] = '\0';

        snprintf(this->date, 7, "%s%s%s", day, month, year);

        char hour[3], minute[3], second[3];
        strncpy(hour, DateAndTimePart + 11, 2);  
        hour[2] = '\0';
        strncpy(minute, DateAndTimePart + 14, 2); 
        minute[2] = '\0';
        strncpy(second, DateAndTimePart + 17, 2); 
        second[2] = '\0';

        snprintf(this->time, 7, "%s%s%s", hour, minute, second);

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
