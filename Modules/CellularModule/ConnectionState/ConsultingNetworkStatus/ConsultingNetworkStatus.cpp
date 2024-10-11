//=====[Libraries]=============================================================

#include "ConsultingNetworkStatus.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define MODEM_REGISTERED_LOCALY 1
#define MODEM_REGISTERED_ROAMING 5
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
ConsultingNetworkStatus::ConsultingNetworkStatus (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->cellDataRetrived = false;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConsultingNetworkStatus::~ConsultingNetworkStatus () {
    this->mobileNetworkModule = NULL;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void ConsultingNetworkStatus::enableConnection () {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
CellularConnectionStatus_t ConsultingNetworkStatus::connect (ATCommandHandler * ATHandler,
 NonBlockingDelay * refreshTime,
 CellInformation * currentCellInformation) {

    static char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSend [15] = "AT+CREG=2";
    char StringToSend2 [15] = "AT+CREG?";
    char StringToSendUSB [40] = "CONSULTING NETWORK STATUS";

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ATHandler->sendATCommand(StringToSend2);
        uartUSB.write (StringToSend2  , strlen (StringToSend2  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        refreshTime->restart();
        ////   ////   ////   ////   ////   ////   
    }

    if ( this->cellDataRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             refreshTime->restart();
             if (this->retrivIdCellData (StringToBeRead)) {
                this->cellDataRetrived = true;
           }
        }
    } 
    

    if (this->cellDataRetrived == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////     
                char StringToSendUSB [40] = "Cambiando de estado 4";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
                currentCellInformation->cellId = this->cellId;
                currentCellInformation->lac = this->lac;
                currentCellInformation->accessTechnology = this->accessTechnology;
                currentCellInformation->registrationStatus = this->registrationStatus;
                if (this->registrationStatus == MODEM_REGISTERED_LOCALY ||
                 this->registrationStatus == MODEM_REGISTERED_ROAMING) {
                    this->mobileNetworkModule->changeConnectionState
                     (new ConsultingAvailableOperators (this->mobileNetworkModule) );
                    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
                 }         
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->cellDataRetrived = false;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            return CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_REGISTER;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool ConsultingNetworkStatus::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}



//=====[Implementations of private functions]==================================
bool ConsultingNetworkStatus::retrivIdCellData(char *response) {
    char StringToCompare[8] = "+CREG: ";

    // Verificar si la respuesta comienza con "+CREG: "
    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        // Variables para almacenar los campos parseados
        int stat, act;
        char tac[10];
        char ci[20];
        
        // Parsear la respuesta
        int n = sscanf(response, "+CREG: %*d,%d,\"%9[^\"]\",\"%19[^\"]\",%d", &stat, tac, ci, &act);
        
        // Verificar que se hayan parseado correctamente los 4 valores
        if (n == 4) {
            this->registrationStatus = stat;

            // Convertir LAC y Cell ID de hexadecimal a entero
            this->lac = strtol(tac, nullptr, 16);  // LAC como entero
            this->cellId = strtol(ci, nullptr, 16);  // Cell ID como entero
            this->accessTechnology = act;

            // Debugging
            char lacStr[10];
            char idCellStr[10];
            char regStatusStr[10];
            char accessTechStr[10];

            sprintf(lacStr, "%X", this->lac);
            sprintf(idCellStr, "%X", this->cellId);
            sprintf(regStatusStr, "%d", this->registrationStatus);
            sprintf(accessTechStr, "%d", this->accessTechnology);

            uartUSB.write("LAC: ", strlen("LAC: "));
            uartUSB.write(lacStr, strlen(lacStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Cell ID: ", strlen("Cell ID: "));
            uartUSB.write(idCellStr, strlen(idCellStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Registration Status: ", strlen("Registration Status: "));
            uartUSB.write(regStatusStr, strlen(regStatusStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Access Technology: ", strlen("Access Technology: "));
            uartUSB.write(accessTechStr, strlen(accessTechStr));
            uartUSB.write("\r\n", 2);

            return true;
        }
    }
    
    return false;
}
