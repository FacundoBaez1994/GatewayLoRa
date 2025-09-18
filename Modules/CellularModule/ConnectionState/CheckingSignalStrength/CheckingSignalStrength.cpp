//=====[Libraries]=============================================================

#include "CheckingSignalStrength.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define LOWER_LIMIT_SIGNAL_LEVEL -100
#define MAX_ATTEMPTS_TIMEOUT_NO_RESPONSE 40
#define MAX_ATTEMPTS_TO_GET_SIGNAL 20

#define AT_CMD_CHECK_SIGNAL_STRENGTH      "AT+CSQ"
#define AT_CMD_CHECK_SIGNAL_STRENGTH_LEN  (sizeof(AT_CMD_CHECK_SIGNAL_STRENGTH ) - 1)

#define AT_CMD_CHECK_SIGNAL_STRENGTH_EXPECTED_RESPONSE     "OK"
#define AT_CMD_CHECK_SIGNAL_STRENGTH_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CHECK_SIGNAL_STRENGTH_EXPECTED_RESPONSE) - 1)

#define AT_CMD_CHECK_SIGNAL_STRENGTH_RESPONSE_HEADER     "+CSQ:"
#define AT_CMD_CHECK_SIGNAL_STRENGTH_RESPONSE_HEADER_LEN  (sizeof(AT_CMD_CHECK_SIGNAL_STRENGTH_RESPONSE_HEADER) - 1)


#define BUFFER_LEN 128
#define LOG_MESSAGE_INITIAL "Checking Signal Strength\r\n"
#define LOG_MESSAGE_INITIAL_LEN  (sizeof(LOG_MESSAGE_INITIAL) - 1)

#define LOG_MESSAGE_WARNING "Poor signal\r\n"
#define LOG_MESSAGE_WARNING_LEN  (sizeof(LOG_MESSAGE_WARNING) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================

CheckingSignalStrength::CheckingSignalStrength (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->signalLevelRetrived = false;
    this->connectionAttemptsATResponse = 0; 
    this->maxConnectionAttemptsATResponse = MAX_ATTEMPTS_TIMEOUT_NO_RESPONSE; 
    this->connectionAttemptsSignal = 0; 
    this->maxConnectionAttemptsSignal =  MAX_ATTEMPTS_TO_GET_SIGNAL;
}

CheckingSignalStrength::~CheckingSignalStrength () {
    this->mobileNetworkModule = nullptr;
    this->connectionAttemptsATResponse = 0; 
}

void CheckingSignalStrength::enableConnection () {
    return;
}

CellularConnectionStatus_t  CheckingSignalStrength::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {

    static char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_CHECK_SIGNAL_STRENGTH_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CHECK_SIGNAL_STRENGTH_EXPECTED_RESPONSE;
    char StringToSend [AT_CMD_CHECK_SIGNAL_STRENGTH_LEN + 1] = AT_CMD_CHECK_SIGNAL_STRENGTH;
    char StringToSendUSB [LOG_MESSAGE_INITIAL_LEN + 1] = LOG_MESSAGE_INITIAL;
    char StringToSendUSB2 [LOG_MESSAGE_WARNING_LEN + 1] = LOG_MESSAGE_WARNING;

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

    if ( this->signalLevelRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
            this->ATFirstResponseRead = true;
             refreshTime->restart();
            if (this->checkExpectedResponse (StringToBeRead)) {
                this->signalLevelRetrived = true;
            }
        } 
     } 
  
    if (this->signalLevelRetrived == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////        
                if (this->signalLevel > LOWER_LIMIT_SIGNAL_LEVEL) {
                    currentCellInformation->signalLevel = this->signalLevel;
                    this->mobileNetworkModule->changeConnectionState(new ConsultingIMEI(this->mobileNetworkModule));
                    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
                } else {
                    uartUSB.write (StringToSendUSB2 , strlen (StringToSendUSB2 ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    connectionAttemptsSignal++;
                    if (this->connectionAttemptsSignal >= this->maxConnectionAttemptsSignal) {
                        return CELLULAR_CONNECTION_STATUS_MODULE_DISCONNECTED;
                    }
                }
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->signalLevelRetrived = false;
        this->connectionAttemptsATResponse++;
        if (this->connectionAttemptsATResponse >= this->maxConnectionAttemptsATResponse) {
            this->connectionAttemptsATResponse = 0; 
            return CELLULAR_CONNECTION_STATUS_MODULE_DISCONNECTED;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}

bool CheckingSignalStrength::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}


//=====[Implementations of private functions]==================================
bool CheckingSignalStrength::checkExpectedResponse(char* response) {
    char StringToCompare[AT_CMD_CHECK_SIGNAL_STRENGTH_RESPONSE_HEADER_LEN + 1] = AT_CMD_CHECK_SIGNAL_STRENGTH_RESPONSE_HEADER;
    
    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        char* numericPart = response + strlen(StringToCompare);
        
        int rssi, ber;
        if (sscanf(numericPart, "%d,%d", &rssi, &ber) == 2) {
            if (rssi >= 0 && rssi <= 31) {
                this->signalLevel = -113 + 2 * rssi;
            } else if (rssi >= 100 && rssi <= 199) {
                this->signalLevel = -116 + (rssi - 100);
            } else {
                this->signalLevel = -999; // Unknown Value
            }
            
            // ber needs an active session to be meassured
            char msgStringSignalQuality [20]= "";
            sprintf (msgStringSignalQuality, "%.2f",  this->signalLevel );  
            char msg []  = "signal level: "; 
            uartUSB.write (msg,  strlen (msg) );  // debug only
            uartUSB.write (msgStringSignalQuality,  strlen (msgStringSignalQuality) );  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only

            return true;
        }
    } 

    return false;
}

