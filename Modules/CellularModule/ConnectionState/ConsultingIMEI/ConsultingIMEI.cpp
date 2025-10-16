//=====[Libraries]=============================================================

#include "ConsultingIMEI.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_CONSULT_IMEI     "AT+CGSN"
#define AT_CMD_CONSULT_IMEI_LEN  (sizeof(AT_CMD_CONSULT_IMEI) - 1)

#define AT_CMD_CONSULT_IMEI_EXPECTED_RESPONSE     "OK"
#define AT_CMD_CONSULT_IMEI_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CONSULT_IMEI_EXPECTED_RESPONSE ) - 1)

#define BUFFER_LEN 128
#define LOG_MESSAGE "Consulting IMEI\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================


//=====[Implementations of public methods]===================================

ConsultingIMEI::ConsultingIMEI (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->IMEIRetrived = false;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}

ConsultingIMEI::~ConsultingIMEI () {
    this->mobileNetworkModule = nullptr;
    this->connectionAttempts = 0; 
}

void ConsultingIMEI::enableConnection () {
    return;
}

CellularConnectionStatus_t ConsultingIMEI::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {
    static char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_CONSULT_IMEI_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CONSULT_IMEI_EXPECTED_RESPONSE;
     char StringToSend [AT_CMD_CONSULT_IMEI_LEN + 1] = AT_CMD_CONSULT_IMEI ;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;
    
    if (ATHandler == nullptr || refreshTime == nullptr || currentCellInformation == nullptr) {
        return CELLULAR_CONNECTION_STATUS_ERROR_NULL_POINTER;
    }

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

 if ( this->IMEIRetrived == false) {
        if ( ATHandler->readATResponse (StringToBeRead, BUFFER_LEN) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             refreshTime->restart();
            if (this->RetrivIMEI (StringToBeRead,  this->IMEI )) {
                char msgStringSignalQuality [20]= "";
                sprintf (msgStringSignalQuality, "%lld",  this->IMEI );  
                char msg []  = "Module IMEI: "; 
                uartUSB.write (msg,  strlen (msg) );  // debug only
                uartUSB.write (msgStringSignalQuality,  strlen (msgStringSignalQuality) );  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                this->IMEIRetrived= true;
           }
        } 
    } 

    if (this->IMEIRetrived == true) {
        if  (ATHandler->readATResponse (StringToBeRead, BUFFER_LEN) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   //// 
                currentCellInformation->IMEI = this->IMEI;      
                this->connectionAttempts = 0; 
                this->mobileNetworkModule->changeConnectionState (new ConsultingSIMCardStatus (this->mobileNetworkModule) );
                return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->IMEIRetrived= false;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            this->connectionAttempts = 0; 
            return CELLULAR_CONNECTION_STATUS_MODULE_DISCONNECTED;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;

}

bool ConsultingIMEI::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}


//=====[Implementations of private methods]==================================
bool ConsultingIMEI::RetrivIMEI(char *response, long long int &value) {
    for (int i = 0; i < 3; ++i) {
        if (!isdigit(response[i])) {
            return false; 
        }
    }

    char imeiPart[16] = {0}; 
    strncpy(imeiPart, response, 15); 

    if (strlen(imeiPart) != 15) {
        return false; 
    }

    value = std::atoll(imeiPart);
    
    return true; 
}