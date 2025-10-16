//=====[Libraries]=============================================================

#include "ConfigurePDPForHTTP.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_GENERIC_EXPECTED_RESPONSE   "OK"
#define AT_CMD_GENERIC_EXPECTED_RESPONSE_LEN  (sizeof( AT_CMD_GENERIC_EXPECTED_RESPONSE) - 1)

#define AT_CMD_ASSING_PDPCONTEXT_TO_HTTP_STACK  "AT+QHTTPCFG=\"contextid\",1"
#define AT_CMD_ASSING_PDPCONTEXT_TO_HTTP_STACK_LEN  (sizeof(AT_CMD_ASSING_PDPCONTEXT_TO_HTTP_STACK) - 1)

#define LOG_MESSAGE "Configuring PDP\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define AT_CMD_CONFIGURING_PDP_PROFILE  "AT+QICSGP=1,1,\"internet.movil\",\"internet\",\"internet\",3"
#define AT_CMD_CONFIGURING_PDP_PROFILE_LEN  (sizeof(AT_CMD_CONFIGURING_PDP_PROFILE) - 1)

#define AT_CMD_ACTIVATING_PDP_CONTEXT  "AT+QIACT=1"
#define AT_CMD_ACTIVATING_PDP_CONTEXT_LEN  (sizeof(AT_CMD_ACTIVATING_PDP_CONTEXT ) - 1)

#define AT_CMD_QUERING_PDP_CONTEXT  "AT+QIACT?"
#define AT_CMD_QUERING_PDP_CONTEXT_LEN  (sizeof(AT_CMD_QUERING_PDP_CONTEXT ) - 1)

#define AT_CMD_CONFIGURING_SERVER_DNS "AT+QIDNSCFG=1,\"8.8.8.8\",\"8.8.4.4\"" // GOOGLE DNS SERVER
#define AT_CMD_CONFIGURING_SERVER_DNS_LEN  (sizeof(AT_CMD_CONFIGURING_SERVER_DNS ) - 1)

#define AT_CMD_PING_SERVER_DNS "AT+QIDNSGIP=1,\"intent-lion-loudly.ngrok-free.app\""
#define AT_CMD_PING_SERVER_DNS_LEN  (sizeof(AT_CMD_PING_SERVER_DNS ) - 1)

#define REFRESH_TIME_MS 20000

#define BUFFER_LEN 128
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ConfigurePDPForHTTP::ConfigurePDPForHTTP () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = ASSING_PDPCONTEXT_TO_HTTP_STACK;
}

ConfigurePDPForHTTP::ConfigurePDPForHTTP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = ASSING_PDPCONTEXT_TO_HTTP_STACK;
}

ConfigurePDPForHTTP::~ConfigurePDPForHTTP () {
    this->mobileNetworkModule = nullptr;
    this->Attempts = 0; 
}

void ConfigurePDPForHTTP::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t ConfigurePDPForHTTP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_GENERIC_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_GENERIC_EXPECTED_RESPONSE;
    char StringToSend1 [AT_CMD_ASSING_PDPCONTEXT_TO_HTTP_STACK_LEN + 1] = AT_CMD_ASSING_PDPCONTEXT_TO_HTTP_STACK;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;
    char StringToSend2 [AT_CMD_CONFIGURING_PDP_PROFILE_LEN + 1] = AT_CMD_CONFIGURING_PDP_PROFILE;
    char StringToSend3 [AT_CMD_ACTIVATING_PDP_CONTEXT_LEN + 1] = AT_CMD_ACTIVATING_PDP_CONTEXT;
    char StringToSend4 [AT_CMD_QUERING_PDP_CONTEXT_LEN + 1] = AT_CMD_QUERING_PDP_CONTEXT;
    char StringToSend5 [AT_CMD_CONFIGURING_SERVER_DNS_LEN + 1] = AT_CMD_CONFIGURING_SERVER_DNS;

    //char StringToSend6 [50] = "AT+QIDNSGIP=1,\"dns.google\"";
    char StringToSend6 [AT_CMD_PING_SERVER_DNS_LEN + 1] = AT_CMD_PING_SERVER_DNS;

    
    if (ATHandler == nullptr ||  refreshTime == nullptr || 
     message == nullptr || receivedMessage == nullptr || serverTargetted == nullptr) {
        return CELLULAR_TRANSCEIVER_STATUS_ERROR_NULL_POINTER;
    }
   
   switch (this->currentStatus) {
       case ASSING_PDPCONTEXT_TO_HTTP_STACK:
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
                
            if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = CONFIGURING_PDP_PROFILE;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
        break;
        case CONFIGURING_PDP_PROFILE:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend2); // AT+QICSGP=1,1,\"internet.movil\",\"\" ,\"\" ,0
                this->readyToSend  = false;
                uartUSB.write (StringToSend2  , strlen (StringToSend2  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = ACTIVATING_PDP_CONTEXT;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;
        case ACTIVATING_PDP_CONTEXT:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend3);
                this->readyToSend  = false;
                uartUSB.write (StringToSend3  , strlen (StringToSend3  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    
                    this->readyToSend  = true;
                    this->currentStatus =  QUERING_PDP_CONTEXT;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;
        case QUERING_PDP_CONTEXT:
             if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend4);
                this->readyToSend  = false;
                uartUSB.write (StringToSend4  , strlen (StringToSend4  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                            
            if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {  // shows IP
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only 
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////
                this->readyToSend  = true;
                // CHANGE STATE
                this->currentStatus =  CONFIGURING_SERVER_DNS;
                return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }
            break;
        case CONFIGURING_SERVER_DNS:
            //this->currentStatus = PING_SERVER_DNS;
            //return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;

            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend5);
                this->readyToSend  = false;
                uartUSB.write (StringToSend5  , strlen (StringToSend5  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                            
            if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {  // shows IP
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only 
                uartUSB.write ( "\r\n",  3 );  // debug only
                
                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->readyToSend  = true;
                    
                    this->currentStatus = ACTIVATING_PDP_CONTEXT;
                    //this->currentStatus =  PING_SERVER_DNS;
                    this->Attempts = 0; 
                    this->mobileNetworkModule->changeTransceiverState(new ConfigureSSL(this->mobileNetworkModule));
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }

            break;
        case PING_SERVER_DNS:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend6);
                this->readyToSend  = false;
                uartUSB.write (StringToSend6  , strlen (StringToSend6  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                            
            if ( ATHandler->readATResponse (StringToBeRead, BUFFER_LEN) == true) {  // shows IP
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only 
                uartUSB.write ( "\r\n",  3 );  // debug only
                
                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    //this->currentStatus = ACTIVATING_PDP_CONTEXT;
                    this->readyToSend  = true;
                    
                    // CHANGE STATE
                    //this->mobileNetworkModule->changeTransceiverState(new PostHTTP(this->mobileNetworkModule));
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }

            break;
        default:
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
   }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts) {
            this->Attempts = 0; 
            return CELLULAR_TRANSCEIVER_STATUS_FAIL_TO_ACTIVATE_PDP;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
