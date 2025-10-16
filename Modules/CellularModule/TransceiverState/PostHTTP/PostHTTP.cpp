//=====[Libraries]=============================================================

#include "PostHTTP.h"
#include "CellularModule.h" 
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 5

#define LOG_MESSAGE_1 "\r\nHTTP Posting\r\n"
#define LOG_MESSAGE_1_LEN (sizeof(LOG_MESSAGE_1) - 1)

#define LOG_MESSAGE_2 "POST Message:\r\n"
#define LOG_MESSAGE_3 "POST Message SUCCESS!\r\n"
#define LOG_MESSAGE_4 "Reading POST response\r\n"

#define LOG_ERROR_MESSAGE "Error decoding JWT\r\n"

#define AT_CMD_HTTP_POST_SET_URL "AT+QHTTPURL="
#define AT_CMD_HTTP_POST_SET_URL_LEN  (sizeof(AT_CMD_HTTP_POST_SET_URL) - 1)

#define AT_CMD_HTTP_POST_EXPECTED_RESPONSE_1 "OK"
#define AT_CMD_HTTP_POST_EXPECTED_RESPONSE_1_LEN  (sizeof(AT_CMD_HTTP_POST_EXPECTED_RESPONSE_1) - 1)

#define AT_CMD_HTTP_POST_EXPECTED_RESPONSE_2 "CONNECT"
#define AT_CMD_HTTP_POST_EXPECTED_RESPONSE_2_LEN  (sizeof(AT_CMD_HTTP_POST_EXPECTED_RESPONSE_2) - 1)

#define URL "https://intent-lion-loudly.ngrok-free.app/api/canal/envio"
#define URL_LEN  (sizeof(URL) - 1)

#define AT_CMD_HTTP_POST "AT+QHTTPPOST="
#define AT_CMD_HTTP_POST_LEN  (sizeof(AT_CMD_HTTP_POST) - 1)

#define AT_CMD_HTTP_POST_READ "AT+QHTTPREAD="
#define AT_CMD_HTTP_POST_READ_LEN  (sizeof(AT_CMD_HTTP_POST_READ) - 1)


#define BUFFER_LEN 256

#define INPUT_TIMEOUT 80
#define RESPONSE_TIMEOUT 80

#define REFRESH_TIMEOUT_1_MS 10000
#define REFRESH_TIMEOUT_2_MS 5000
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of public methods]===================================
PostHTTP::PostHTTP () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_URL;
    //this->jwt = new JWTManager ();
}


PostHTTP::PostHTTP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_URL;
    //this->jwt = new JWTManager ();
}

PostHTTP::~PostHTTP () {
    this->Attempts = 0; 
    this->mobileNetworkModule = nullptr;
    //delete this->jwt;
    //this->jwt = nullptr;
}

void PostHTTP::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t PostHTTP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    static char StringToBeRead [BUFFER_LEN];
    static int dataLen;
    char StringToSendUSB [LOG_MESSAGE_1_LEN + 1] = LOG_MESSAGE_1 ;
    char ExpectedResponse1 [AT_CMD_HTTP_POST_EXPECTED_RESPONSE_1_LEN + 1] = AT_CMD_HTTP_POST_EXPECTED_RESPONSE_1;
    char ExpectedResponse2 [AT_CMD_HTTP_POST_EXPECTED_RESPONSE_2_LEN + 1] = AT_CMD_HTTP_POST_EXPECTED_RESPONSE_2;
    char PartialStringToSend1 [AT_CMD_HTTP_POST_SET_URL_LEN + 10] =  AT_CMD_HTTP_POST_SET_URL;
    char StringToSend1 [AT_CMD_HTTP_POST_SET_URL_LEN + 10];
    char url [URL_LEN + 1] = URL;
    char confirmationToSend[] = "\x1a";
    
    char PartialStringToSend2 [AT_CMD_HTTP_POST_LEN + 1] = AT_CMD_HTTP_POST;
    char StringToSend2 [AT_CMD_HTTP_POST_LEN + 10]; 

    char PartialStringToSend3 [ AT_CMD_HTTP_POST_READ_LEN + 1] =  AT_CMD_HTTP_POST_READ;
    char StringToSend3 [AT_CMD_HTTP_POST_READ_LEN + 3];  
    static bool watingForResponse = false;
    static bool urlSet = false;

    if (ATHandler == nullptr ||  refreshTime == nullptr || 
     message == nullptr || receivedMessage == nullptr || serverTargetted == nullptr) {
        return CELLULAR_TRANSCEIVER_STATUS_ERROR_NULL_POINTER;
    }

    int urlLength = strlen(url);

    int inputTimeout = INPUT_TIMEOUT;

    int responseTimeout = RESPONSE_TIMEOUT;

    // SET URL COMMAND
    sprintf(StringToSend1, "%s%d,%d", PartialStringToSend1, urlLength, inputTimeout);

    // POST COMMAND
    sprintf(StringToSend2, "%s%d,%d,%d", PartialStringToSend2, strlen(message), inputTimeout, responseTimeout);

    // READ RESPONSE COMMAND
    sprintf(StringToSend3, "%s%d", PartialStringToSend3, responseTimeout);
   
   switch (this->currentStatus) {
       case SETTING_URL:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend1);
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                uartUSB.write (StringToSend1  , strlen (StringToSend1  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
                refreshTime->write(REFRESH_TIMEOUT_1_MS);
                refreshTime->restart();
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse2) == 0) { // CONNECT
                    ////   ////   ////   ////   ////   ////     
                    ATHandler->sendATCommand(url);
                    ////   ////   ////   ////   ////   ////
                    ATHandler->sendATCommand(confirmationToSend);
                    uartUSB.write (url  , strlen (url ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    urlSet = true;
                }
                
                if (strcmp (StringToBeRead, ExpectedResponse1) == 0 && urlSet == true) { // OK
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = POSTING_DATA;
                    this->Attempts = 0; 
                    this->readyToSend = true;
                    urlSet = false;
                }
            }
            break;
        case POSTING_DATA:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend2); // POST
                this->readyToSend  = false;
                uartUSB.write (StringToSend2  , strlen (StringToSend2  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) { //
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse2) == 0) { // CONNECT
                    ////   ////   ////   ////   ////   ////  
                    ATHandler->sendATCommand(message);   // The message to be sent
                    ATHandler->sendATCommand(confirmationToSend);
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    uartUSB.write (LOG_MESSAGE_2 , strlen ( LOG_MESSAGE_2));  // debug only
                    uartUSB.write (message, strlen(message));
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    refreshTime->restart();
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
                if (strcmp (StringToBeRead, ExpectedResponse1) == 0) { // OK
  
                    refreshTime->restart();
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
                PostResult_t postResult = checkHTTPPostResult (StringToBeRead, &dataLen);
                if (postResult == POST_OK) { 
  
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    uartUSB.write (LOG_MESSAGE_3  , strlen (LOG_MESSAGE_3));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    refreshTime->restart();
                    this->currentStatus = READING_DATA;
                    this->Attempts = 0; 
                    this->readyToSend  = true;      
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
                
            }
            break;
        case READING_DATA:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend3); // POST READ
                refreshTime->restart();
                this->readyToSend  = false;
                uartUSB.write ( "\r\n",  3 );
                uartUSB.write (StringToSend3  , strlen (StringToSend3 ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
            if ( watingForResponse == false) {
                if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) { //
                    ////   ////   ////   ////   ////   ////
                    uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    ////   ////   ////   ////   ////   ////
                    if (strcmp (StringToBeRead, ExpectedResponse2) == 0) { // CONNECT
                        ////   ////   ////   ////   ////   ////  
                        watingForResponse = true;
                        refreshTime->write(REFRESH_TIMEOUT_2_MS);
                        refreshTime->restart();
                        //memset(receivedMessage, 0, 2048);
                        return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                    }
                }
            } else {
                if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) { //
                    ////   ////   ////   ////   ////   ////
                    StringToBeRead [dataLen] = '\0';
                    refreshTime->restart();
                    uartUSB.write (LOG_MESSAGE_4, strlen (LOG_MESSAGE_4));  // debug only
                    uartUSB.write (StringToBeRead  , strlen (StringToBeRead ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    watingForResponse = false;
                    this->Attempts = 0; 
                    this->currentStatus = DECODING_DATA;
                    this->readyToSend = false;
                    

                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }

            break;
        case DECODING_DATA:
            if (strlen (StringToBeRead) > 256 || strlen (StringToBeRead) <= 0) {
            //if (this->jwt->decodeJWT(StringToBeRead , payloadRetrived) == false) {
                uartUSB.write (LOG_ERROR_MESSAGE , strlen (LOG_ERROR_MESSAGE ));  // debug only
                this->readyToSend  = true;
                this->currentStatus = READING_DATA; // return one state back
                return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                break;
            }
            *newDataAvailable = true;
            strcpy (receivedMessage, StringToBeRead);
            uartUSB.write (receivedMessage , strlen (receivedMessage ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            this->currentStatus = SETTING_URL;
            this->Attempts = 0; 
            this->mobileNetworkModule->changeTransceiverState  (new DeactivatePDP (this->mobileNetworkModule, true) );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            break;

        default:
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
   } // switch end

    if (refreshTime->read()) {
        this->readyToSend = true;
        refreshTime->write(REFRESH_TIMEOUT_1_MS);
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts && watingForResponse == false) {
            this->currentStatus = SETTING_URL;
            this->Attempts = 0; 
             this->mobileNetworkModule->changeTransceiverState  (new DeactivatePDP (this->mobileNetworkModule, false) );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
        if (this->Attempts >= this->maxAttempts && watingForResponse == true) {
            this->currentStatus = SETTING_URL;
            this->Attempts = 0; 
             this->mobileNetworkModule->changeTransceiverState  (new DeactivatePDP (this->mobileNetworkModule, true) );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}

//=====[Implementations of private functions]==================================

//=====[Implementations of private methods]===================================

PostResult_t PostHTTP::checkHTTPPostResult(char * responseBuffer, int * dataLen) {
    if (strncmp(responseBuffer, "+QHTTPPOST: ", 12) == 0) {
        int result, httpStatus;
        *dataLen = -1;
        sscanf(responseBuffer, "+QHTTPPOST: %d,%d,%d", &result, &httpStatus, dataLen);

        uartUSB.write("Parsed POST result:\r\n", 23);
        uartUSB.write(responseBuffer, strlen(responseBuffer));
        uartUSB.write("\r\n", 2);
        if (result == 0  && (*dataLen > -1 && *dataLen < BUFFER_LEN)) {
            return POST_OK;
        }
        return POST_FAILURE;
    }
    return KEEP_TRYING_TO_POST;
}
