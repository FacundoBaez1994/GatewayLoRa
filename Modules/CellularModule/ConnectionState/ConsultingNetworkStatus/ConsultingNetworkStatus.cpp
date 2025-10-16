//=====[Libraries]=============================================================

#include "ConsultingNetworkStatus.h"
#include "CellularModule.h"
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define MODEM_REGISTERED_LOCALY 1
#define MODEM_REGISTERED_ROAMING 5
#define MAXATTEMPTS 20

#define AT_CMD_CONSULT_NETWORK_STATUS_1     "AT+CREG=2"
#define AT_CMD_CONSULT_NETWORK_STATUS_1_LEN  (sizeof(AT_CMD_CONSULT_NETWORK_STATUS_1 ) - 1)

#define AT_CMD_CONSULT_NETWORK_STATUS_EXPECTED_RESPONSE     "OK"
#define AT_CMD_CONSULT_NETWORK_STATUS_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CONSULT_NETWORK_STATUS_EXPECTED_RESPONSE ) - 1)

#define AT_CMD_CONSULT_NETWORK_STATUS_2     "AT+CREG?"
#define AT_CMD_CONSULT_NETWORK_STATUS_2_LEN  (sizeof(AT_CMD_CONSULT_NETWORK_STATUS_2 ) - 1)

#define BUFFER_LEN 128
#define LOG_MESSAGE "Consulting Network Status\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================

ConsultingNetworkStatus::ConsultingNetworkStatus (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->cellDataRetrived = false;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}

ConsultingNetworkStatus::~ConsultingNetworkStatus () {
    this->mobileNetworkModule = nullptr;
    this->connectionAttempts = 0; 
}

void ConsultingNetworkStatus::enableConnection () {
    return;
}


CellularConnectionStatus_t ConsultingNetworkStatus::connect (ATCommandHandler * ATHandler,
 NonBlockingDelay * refreshTime,
 CellInformation * currentCellInformation) {

    static char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_CONSULT_NETWORK_STATUS_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CONSULT_NETWORK_STATUS_EXPECTED_RESPONSE;
    char StringToSend [AT_CMD_CONSULT_NETWORK_STATUS_1_LEN + 1] = AT_CMD_CONSULT_NETWORK_STATUS_1;
    char StringToSend2 [AT_CMD_CONSULT_NETWORK_STATUS_2_LEN + 1] = AT_CMD_CONSULT_NETWORK_STATUS_2;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;

    if (ATHandler == nullptr || refreshTime == nullptr || currentCellInformation == nullptr) {
        return CELLULAR_CONNECTION_STATUS_ERROR_NULL_POINTER;
    }

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ATHandler->sendATCommand(StringToSend2);
        uartUSB.write (StringToSend2  , strlen (StringToSend2  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        refreshTime->restart();
    }

    if ( this->cellDataRetrived == false) {
        if ( ATHandler->readATResponse (StringToBeRead, BUFFER_LEN) == true ) {
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
             refreshTime->restart();
             if (this->retrivIdCellData (StringToBeRead)) {
                this->cellDataRetrived = true;
           }
        }
    } 
    

    if (this->cellDataRetrived == true) {
        if  (ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                currentCellInformation->cellId = this->cellId;
                currentCellInformation->lac = this->lac;
                currentCellInformation->accessTechnology = this->accessTechnology;
                currentCellInformation->registrationStatus = this->registrationStatus;
                if (this->registrationStatus == MODEM_REGISTERED_LOCALY ||
                 this->registrationStatus == MODEM_REGISTERED_ROAMING) {
                    this->connectionAttempts = 0; 
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
            this->connectionAttempts = 0; 
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

//=====[Implementations of private methods]===================================
bool ConsultingNetworkStatus::retrivIdCellData(char *response) {
    char StringToCompare[8] = "+CREG: ";

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        int stat, act;
        char tac[10];
        char ci[20];

        int n = sscanf(response, "+CREG: %*d,%d,\"%9[^\"]\",\"%19[^\"]\",%d", &stat, tac, ci, &act);
        
        if (n == 4) {
            this->registrationStatus = stat;

            this->lac = strtol(tac, nullptr, 16); 
            this->cellId = strtol(ci, nullptr, 16); 
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
