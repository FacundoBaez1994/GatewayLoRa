//=====[Libraries]=============================================================

#include "ConnectedState.h"
#include "CellularModule.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAX_CELLS_ON_VECTOR 14
#define MAX_ROWS_TO_READ 30
#define MAX_LENGTH_CELL_INFO_STRING 128

#define MAX_TIME_WATING_RESPONSE_MS 10000
#define MAX_TIMEOUT_RETRIES 6

#define AT_CMD_CELL_INFO_GATHERING      "AT+QOPS"
#define AT_CMD_CELL_INFO_GATHERING_LEN  (sizeof(AT_CMD_CELL_INFO_GATHERING) - 1)

#define AT_CMD_CELL_INFO_GATHERING_EXPECTED_RESPONSE     "OK"
#define AT_CMD_CELL_INFO_GATHERING_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CELL_INFO_GATHERING_EXPECTED_RESPONSE) - 1)

#define BUFFER_LOG_LEN 60
#define LOG_MESSAGE "Retriving Neighboor Cells Data\r\n"
#define LOG_MESSAGE_LEN 40


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================

ConnectedState::ConnectedState () {
    this->mobileNetworkModule = nullptr;
    this->enableTransceiver = false;
}

ConnectedState::ConnectedState (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->enableTransceiver = false;
}


ConnectedState::~ConnectedState () {
    this->mobileNetworkModule = nullptr;
}

void ConnectedState::enableConnection () {
    return;
}

CellularConnectionStatus_t ConnectedState::connect (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, CellInformation * currentCellInformation) {
        return CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK;
    }

bool ConnectedState::retrivNeighborCellsInformation(ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
    
    static bool cellDataRetrived = false;
    static bool readyToSend = true;
    static bool vectorCleared = false;
    
    char StringToBeRead[MAX_LENGTH_CELL_INFO_STRING];
    char ExpectedResponse[AT_CMD_CELL_INFO_GATHERING_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CELL_INFO_GATHERING_EXPECTED_RESPONSE;
    char log [BUFFER_LOG_LEN];
    
    char StringToSend[AT_CMD_CELL_INFO_GATHERING_LEN + 1] = AT_CMD_CELL_INFO_GATHERING;
    char StringToSendUSB[LOG_MESSAGE_LEN] = LOG_MESSAGE;

    static int mnc;
    static int mcc;
    static int counterTimeOut = 0;
    int maxTimeOut = MAX_TIMEOUT_RETRIES;

    int tech;
    int idCell;
    int lac;
    float prx;

    static char readString [MAX_ROWS_TO_READ][MAX_LENGTH_CELL_INFO_STRING];
    static int index = 0; 

    if (!vectorCleared) {
        refreshTime->write(MAX_TIME_WATING_RESPONSE_MS);
        for (auto cell : neighborsCellInformation) {
            delete cell;
        }
        neighborsCellInformation.clear();
        vectorCleared = true;
    }

    if (readyToSend == true && cellDataRetrived == false) {
        uartUSB.write(StringToSendUSB, strlen(StringToSendUSB));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        handler->sendATCommand(StringToSend);
        uartUSB.write(StringToSend, strlen(StringToSend));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        readyToSend = false;
    }
    if (handler->readATResponse(StringToBeRead) == true) {

        if (strcmp (StringToBeRead, ExpectedResponse)  == 0) {
            cellDataRetrived = true;
            for (int i = 0; i < index; i++) {
                if (neighborsCellInformation.size() >= MAX_CELLS_ON_VECTOR) {
                uartUSB.write("\r\nvector cells limit reached\r\n", strlen("\r\nvector cells limitreached\r\n"));
                index = 0;
                cellDataRetrived = false;
                vectorCleared = false;
                counterTimeOut = 0;
                readyToSend = true;
                return true;
                }
                uartUSB.write(readString[i], strlen(readString[i]));  // debug only
                uartUSB.write("\r\n", 3);  // debug only
               if (this->retrivOperatorsCodes(readString[i], &mcc, &mnc) == false) {
                    if (this->retrivCellData(readString[i], &tech, &idCell, &lac, &prx)) {
                        if (neighborsCellInformation.size () < MAX_CELLS_ON_VECTOR  ) {
                            cellDataRetrived = true;
                            CellInformation* cellInfoTemp = new CellInformation;
                            cellInfoTemp->tech = tech;
                            cellInfoTemp->signalLevel = prx;
                            cellInfoTemp->mcc = mcc;
                            cellInfoTemp->mnc = mnc;
                            cellInfoTemp->tech = tech;
                            cellInfoTemp->lac =  lac;
                            cellInfoTemp->cellId = idCell;
                            
                        
                            neighborsCellInformation.push_back(cellInfoTemp);
                            uartUSB.write("Cell added to vector\r\n", strlen("Cell added to vector\r\n"));
                            snprintf(log, sizeof(log), "Vector size: %d\r\n", neighborsCellInformation.size());
                            uartUSB.write(log, strlen(log));
                        }
                    }
                }
            }
            index = 0;
            cellDataRetrived = false;
            vectorCleared = false;
            counterTimeOut = 0;
            readyToSend = true;
            return true;
        }

        if (index < MAX_ROWS_TO_READ) {
            strncpy(readString[index], StringToBeRead, sizeof(readString[index]) - 1);
            readString[index][sizeof(readString[index]) - 1] = '\0';  // Asegúrate de la terminación nula
            index++;
         }
    }


    if (refreshTime->read()) {
        readyToSend = true;
        counterTimeOut ++;
        if ( counterTimeOut > maxTimeOut) {
            index = 0;
            cellDataRetrived = false;
            counterTimeOut = 0;
            vectorCleared = false;
            return true;
        }
        
    }

    return false;
}


//=====[Implementations of private methods]==================================
bool ConnectedState::retrivOperatorsCodes(const char *response, int *mcc, int *mnc) {
    char StringToCompare[7] = "+QOPS:";

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        char operatorCode[10] = {0};
        
        int n = sscanf(response, "+QOPS: \"%*[^\"]\",\"%*[^\"]\",\"%[^\"]\"", operatorCode);

        if (n == 1) {
            char mccStr[4] = {0};
            strncpy(mccStr, operatorCode, 3);
            *mcc = atoi(mccStr);

            char mncStr[4] = {0};
            strncpy(mncStr, operatorCode + 3, strlen(operatorCode) - 3);
            *mnc = atoi(mncStr);

            
        
            char buffer[50]; 
            snprintf(buffer, sizeof(buffer), "MCC: %d, MNC: %d\n", *mcc, *mnc);
            uartUSB.write(buffer, strlen(buffer)); 
            uartUSB.write("\r\n", 3);

            return true;
        }
    }
    return false;
}

bool ConnectedState::retrivCellData(const char *response, int *tech, int *idCell, int *lac, float *prx) {
    char accessTechnologyretrived[7];
    int index = 2;

    if (isdigit(response[0]) == 0) {
        return false;
    }

    sscanf(response, "%d,\"%6[^\"]\",", &index, accessTechnologyretrived);

    if (strcmp(accessTechnologyretrived, "2G") == 0) {

        sscanf(response, "%*d,\"%*[^\"]\",%*d,%x,%x,%*d,%f", lac, idCell, prx);
        *prx = *prx - 110;
        *tech = 2;
    } else if (strcmp(accessTechnologyretrived, "3G") == 0) {

        sscanf(response, "%*d,\"%*[^\"]\",%*d,%*d,%x,%x,%f", lac, idCell, prx);
        *tech = 3;

    } else if (strcmp(accessTechnologyretrived, "4G") == 0) {
        sscanf(response, "%*d,\"%*[^\"]\",%*d,%*d,%x,%x,%f", lac, idCell, prx);
        *tech = 4;
    } else {
        return false;
    }

    char buffer[50]; 
    snprintf(buffer, sizeof(buffer), "tech: %dG, idCell: %X, lac: %X, Prx: %f \n", *tech, *idCell, *lac, *prx );
    uartUSB.write(buffer, strlen(buffer)); 
    uartUSB.write("\r\n", 3); 

    return true;
}

