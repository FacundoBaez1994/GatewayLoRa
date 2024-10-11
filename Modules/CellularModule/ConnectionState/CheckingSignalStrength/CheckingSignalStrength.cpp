//=====[Libraries]=============================================================

#include "CheckingSignalStrength.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define LOWER_LIMIT_SIGNAL_LEVEL -100
#define MAX_ATTEMPTS_TIMEOUT_NO_RESPONSE 40
#define MAX_ATTEMPTS_TO_GET_SIGNAL 20
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


/** 
* @brief 
* 
* 
* @returns 
*/
CheckingSignalStrength::~CheckingSignalStrength () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void CheckingSignalStrength::enableConnection () {
    return;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CellularConnectionStatus_t  CheckingSignalStrength::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {

    static char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSend [15] = "AT+CSQ";
    char StringToSendUSB [40] = "CHECKING NETWORK STATE";

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
                char StringToSendUSB [40] = "Cambiando de estado 2";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////        
                if (this->signalLevel > LOWER_LIMIT_SIGNAL_LEVEL) {

                currentCellInformation->signalLevel = this->signalLevel;
                this->mobileNetworkModule->changeConnectionState(new ConsultingIMEI(this->mobileNetworkModule));
                return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
            } else {
                char StringToSendUSB [15] = "Poor signal";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
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
    char StringToCompare[15] = "+CSQ:";
    
    // Verificar si la respuesta comienza con "+CSQ:"
    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        // La respuesta comienza con "+CSQ:"
        // Extraer la parte numérica después de "+CSQ:"
        char* numericPart = response + strlen(StringToCompare);
        
        // Separar los valores de RSSI y BER usando sscanf
        int rssi, ber;
        if (sscanf(numericPart, "%d,%d", &rssi, &ber) == 2) {
            // Convertir RSSI a dBm y almacenar
            if (rssi >= 0 && rssi <= 31) {
                this->signalLevel = -113 + 2 * rssi;
            } else if (rssi >= 100 && rssi <= 199) {
                this->signalLevel = -116 + (rssi - 100);
            } else {
                this->signalLevel = -999; // Valor desconocido
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
    
    // La respuesta no es válida
    return false;
}

