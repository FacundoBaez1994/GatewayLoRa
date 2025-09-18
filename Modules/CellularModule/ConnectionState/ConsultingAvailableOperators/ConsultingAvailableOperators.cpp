//=====[Libraries]=============================================================

#include "ConsultingAvailableOperators.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_CONSULT_AVAILABLE_OPERATORS_INFO  "AT+QNWINFO"
#define AT_CMD_CONSULT_AVAILABLE_OPERATORS_INFO_LEN  (sizeof(AT_CMD_CONSULT_AVAILABLE_OPERATORS_INFO ) - 1)

#define AT_CMD_CONSULT_AVAILABLE_OPERATORS_EXPECTED_RESPONSE     "OK"
#define AT_CMD_CONSULT_AVAILABLE_OPERATORS_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CONSULT_AVAILABLE_OPERATORS_EXPECTED_RESPONSE) - 1)

#define BUFFER_LEN 128
#define LOG_MESSAGE "Consulting Available Operators\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================


//=====[Implementations of public methods]===================================

ConsultingAvailableOperators::ConsultingAvailableOperators (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->operatorsInformationRetrived = false;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}

ConsultingAvailableOperators::~ConsultingAvailableOperators () {
    this->mobileNetworkModule = nullptr;
    this->currentOperator = nullptr;
    this->connectionAttempts = 0; 
}


void ConsultingAvailableOperators::enableConnection () {
    return;
}

CellularConnectionStatus_t  ConsultingAvailableOperators::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {

    static char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_CONSULT_AVAILABLE_OPERATORS_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CONSULT_AVAILABLE_OPERATORS_EXPECTED_RESPONSE;
    char StringToSend [AT_CMD_CONSULT_AVAILABLE_OPERATORS_INFO_LEN + 1] = AT_CMD_CONSULT_AVAILABLE_OPERATORS_INFO;
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

    if ( this->operatorsInformationRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             refreshTime->restart();
            if (this->retrivOperatorsInformation (StringToBeRead)) {
                this->operatorsInformationRetrived = true;
           }
        } 
    } 

    if (this->operatorsInformationRetrived  == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                currentCellInformation->mcc = this->mcc;
                currentCellInformation->mnc = this->mnc;
                currentCellInformation->channel = this->channel;
                strcpy (currentCellInformation->band, this->band);             
                this->mobileNetworkModule->changeConnectionState (new RetrievingTimeAndDate (this->mobileNetworkModule));
                return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            this->connectionAttempts = 0; 
            return CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_RETRIV_OPERATORS_INFO;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}

bool ConsultingAvailableOperators::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}


//=====[Implementations of private methods]==================================
bool ConsultingAvailableOperators::retrivOperatorsInformation(char *response) {
    char StringToCompare[10] = "+QNWINFO:";

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        char accessTechnology[20] = {0};
        char operatorCode[10] = {0};
        char band[20] = {0};
        int channel;
        
        // Parsear la respuesta para obtener MCC, MNC, tecnología de acceso, banda y canal
        int n = sscanf(response, "+QNWINFO: \"%19[^\"]\",\"%9[^\"]\",\"%19[^\"]\",%d", accessTechnology, operatorCode, band, &channel);

        if (n == 4) {
            // Extraer MCC y MNC del operatorCode
            char mcc[4] = {0};
            char mnc[4] = {0};

            strncpy(mcc, operatorCode, 3);
            strncpy(mnc, operatorCode + 3, 3);
            strcpy (this->band, band);

            this->mcc = atoi(mcc);
            this->mnc = atoi(mnc);
            this->channel = channel;

            // Enviar la información por UART para verificación
            uartUSB.write("Access Technology: ", strlen("Access Technology: "));
            uartUSB.write(accessTechnology, strlen(accessTechnology));
            uartUSB.write("\r\n", 2);

            uartUSB.write("MCC: ", strlen("MCC: "));
            uartUSB.write(mcc, strlen(mcc));
            uartUSB.write("\r\n", 2);

            uartUSB.write("MNC: ", strlen("MNC: "));
            uartUSB.write(mnc, strlen(mnc));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Band: ", strlen("Band: "));
            uartUSB.write(band, strlen(band));
            uartUSB.write("\r\n", 2);

            char channelStr[10];
            sprintf(channelStr, "%d", channel);
            uartUSB.write("Channel: ", strlen("Channel: "));
            uartUSB.write(channelStr, strlen(channelStr));
            uartUSB.write("\r\n", 2);

            return true;
        }
    }

    return false;
}