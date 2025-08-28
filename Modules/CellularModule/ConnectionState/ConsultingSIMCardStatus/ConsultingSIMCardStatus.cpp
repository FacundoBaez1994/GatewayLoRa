//=====[Libraries]=============================================================

#include "ConsultingSIMCardStatus.h"
#include "CellularModule.h"
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_CONSULT_SIMCARD_STATUS     "AT+CPIN?"
#define AT_CMD_CONSULT_SIMCARD_STATUS_LEN  (sizeof(AT_CMD_CONSULT_SIMCARD_STATUS) - 1)

#define AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE     "OK"
#define AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE ) - 1)

#define AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE_SIM_READY    "+CPIN: READY"
#define AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE_SIM_READY_LEN  (sizeof(AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE_SIM_READY ) - 1)

#define AT_CMD_CONSULT_SIMCARD_STATUS_NO_SIMCARD_ERROR    "+CME ERROR: 10"
#define AT_CMD_CONSULT_SIMCARD_STATUS_NO_SIMCARD_ERROR_LEN  (sizeof(AT_CMD_CONSULT_SIMCARD_STATUS_NO_SIMCARD_ERROR ) - 1)


#define LOG_MESSAGE_1 "Consulting SimCard Status\r\n"
#define LOG_MESSAGE_1_LEN (sizeof(LOG_MESSAGE_1) - 1)

#define LOG_MESSAGE_2 "Switching SimCard\r\n"
#define LOG_MESSAGE_2_LEN (sizeof(LOG_MESSAGE_2) - 1)


#define BUFFER_LEN 128
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

ConsultingSIMCardStatus::ConsultingSIMCardStatus (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->simCardDetected = false;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}

ConsultingSIMCardStatus::~ConsultingSIMCardStatus () {
    this->mobileNetworkModule = nullptr;
}

CellularConnectionStatus_t ConsultingSIMCardStatus::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {
    static char StringToBeRead [BUFFER_LEN];
    char expectedResponse [AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE;
    char noSimCardError [AT_CMD_CONSULT_SIMCARD_STATUS_NO_SIMCARD_ERROR_LEN + 1] = AT_CMD_CONSULT_SIMCARD_STATUS_NO_SIMCARD_ERROR;
    char simCardReady [AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE_SIM_READY_LEN + 1] = AT_CMD_CONSULT_SIMCARD_STATUS_EXPECTED_RESPONSE_SIM_READY;
    char StringToSend [AT_CMD_CONSULT_SIMCARD_STATUS_LEN + 1] = AT_CMD_CONSULT_SIMCARD_STATUS;
    char StringToSendUSB [LOG_MESSAGE_1_LEN + 1] = LOG_MESSAGE_1;
    char StringToSendUSB2 [LOG_MESSAGE_2_LEN + 1] = LOG_MESSAGE_2;
   

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

    if ( this->simCardDetected == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
            
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             if (strcmp (StringToBeRead, noSimCardError) == 0 || strcmp (StringToBeRead, noSimCardError) == 0) {
                this->mobileNetworkModule->switchSIMCARD();
                uartUSB.write (StringToSendUSB2 , strlen (StringToSendUSB2 ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
            }
            if (strcmp (StringToBeRead, simCardReady ) == 0) {
                this->simCardDetected = true;
            }
            refreshTime->restart();
        }
    } 
     

    if (this->simCardDetected == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, expectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only     

                ////   ////   ////   ////   ////   ////            
                this->mobileNetworkModule->changeConnectionState (new ConsultingNetworkStatus (this->mobileNetworkModule) );
                return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->simCardDetected= false;
        this->connectionAttempts ++;
        if ( this->connectionAttempts >= this->maxConnectionAttempts) {
            this->mobileNetworkModule->reboot();
            return CELLULAR_CONNECTION_STATUS_INVALID_SIM;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}

bool ConsultingSIMCardStatus::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}


void ConsultingSIMCardStatus::enableConnection () {
    return;
}