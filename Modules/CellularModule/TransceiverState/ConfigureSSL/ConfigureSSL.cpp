//=====[Libraries]=============================================================

#include "ConfigureSSL.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_GENERIC_EXPECTED_RESPONSE   "OK"
#define AT_CMD_GENERIC_EXPECTED_RESPONSE_LEN  (sizeof( AT_CMD_GENERIC_EXPECTED_RESPONSE) - 1)

#define AT_CMD_SETTING_SSL_CONTEXT  "AT+QHTTPCFG=\"sslctxid\",1"
#define AT_CMD_SETTING_SSL_CONTEXT_LEN  (sizeof(AT_CMD_SETTING_SSL_CONTEXT) - 1)

#define AT_CMD_SETTING_SSL_VERSION  "AT+QSSLCFG=\"sslversion\",1,1"
#define AT_CMD_SETTING_SSL_VERSION_LEN  (sizeof(AT_CMD_SETTING_SSL_VERSION) - 1)

#define AT_CMD_SETTING_SSL_CIPHER_SUITE  "AT+QSSLCFG=\"ciphersuite\",1,0x0005"
#define AT_CMD_SETTING_SSL_CIPHER_SUITE_LEN  (sizeof(AT_CMD_SETTING_SSL_CIPHER_SUITE) - 1)

#define AT_CMD_SETTING_SSL_SECLEVEL "AT+QSSLCFG=\"seclevel\",1,0"
#define AT_CMD_SETTING_SSL_SECLEVEL_LEN (sizeof(AT_CMD_SETTING_SSL_SECLEVEL) - 1)


#define LOG_MESSAGE "Setting SSL \r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define BUFFER_LEN 128

#define REFRESH_TIME_MS 20000
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ConfigureSSL::ConfigureSSL () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_SSL_CONTEXT;
}

ConfigureSSL::ConfigureSSL (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_SSL_CONTEXT;
}


ConfigureSSL::~ConfigureSSL () {
    this->mobileNetworkModule = nullptr;
}

void ConfigureSSL::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t ConfigureSSL::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_GENERIC_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_GENERIC_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;
    char StringToSend1 [AT_CMD_SETTING_SSL_CONTEXT_LEN + 1] = AT_CMD_SETTING_SSL_CONTEXT;
    char StringToSend2 [AT_CMD_SETTING_SSL_VERSION_LEN + 1] = AT_CMD_SETTING_SSL_VERSION;
    char StringToSend3 [AT_CMD_SETTING_SSL_CIPHER_SUITE_LEN + 1] = AT_CMD_SETTING_SSL_CIPHER_SUITE;
    char StringToSend4 [AT_CMD_SETTING_SSL_SECLEVEL_LEN + 1] = AT_CMD_SETTING_SSL_SECLEVEL;

   switch (this->currentStatus) {
       case SETTING_SSL_CONTEXT:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend1); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                uartUSB.write (StringToSend1  , strlen (StringToSend1  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////  
                refreshTime->write(REFRESH_TIME_MS);
                refreshTime->restart();
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = SETTING_SSL_VERSION;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;
        case SETTING_SSL_VERSION:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend2); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSend2  , strlen (StringToSend2 ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////  
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = SETTING_SSL_LEVEL;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }

            break;

        case SETTING_SSL_CIPHER_SUITE:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend3); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSend3  , strlen (StringToSend3 ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////  
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = SETTING_SSL_LEVEL;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;

        case SETTING_SSL_LEVEL:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend4); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                uartUSB.write (StringToSend4  , strlen (StringToSend4 ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only 
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only


                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = SETTING_SSL_CONTEXT;
                    this->readyToSend  = true;
                    this->mobileNetworkModule->changeTransceiverState(new PostHTTP(this->mobileNetworkModule));
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }


            break;
            
        default:
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
   }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts) {
            return CELLULAR_TRANSCEIVER_STATUS_FAIL_TO_ACTIVATE_PDP;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
