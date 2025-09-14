//=====[Libraries]=============================================================
#include "SendingAck.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingForMessage.h"
#include "ConnectingToMobileNetwork.h"

//=====[Declaration of private defines]========================================
#define BACKOFFTIME        100
#define MAX_CHUNK_SIZE     255
#define FLY_TIME           100
#define MAX_TRIES           3
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
SendingAck::SendingAck (Gateway * gateway, long long int deviceId, int messageNumber) {
    this->currentGateway = gateway;
    this->IdDevice = deviceId;
    this->messageNumber = messageNumber;
}

SendingAck::~SendingAck() {
     this->currentGateway = NULL;
}

void SendingAck::sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime) {
    //char buffer [1024] = "helloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwF-16";
    static char buffer [2048];
    static bool firstChunkSent = false;
    static bool firstDelayPassed = false;
    static bool messageFormatted = false;
    static bool firstEntryOnThisMethod = true;
    static size_t stringIndex = 0;
    static size_t numberOffAckSendings = 0;

    
    if (firstEntryOnThisMethod == true) {
        backoffTime->write(BACKOFFTIME);
        backoffTime->restart();
        firstEntryOnThisMethod = false;
    }


    if (firstDelayPassed == false) {
        if (backoffTime->read()) {
            firstDelayPassed = true;
            uartUSB.write("Backoff passed\r\n", strlen("Backoff passed\r\n"));
        } else {
            return;
        }
    }

    if (messageFormatted == false) {
        uartUSB.write ("Sending plaintext ACKmessage:\r\n", strlen ("Sending plaintext ACKmessage:\r\n"));  // debug only

         snprintf(buffer, sizeof(buffer), "%lld,%d,ACK", this->IdDevice, this->messageNumber); //

        uartUSB.write ( buffer, strlen (buffer));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (this->currentGateway->prepareLoRaMessage ( buffer, strlen (buffer)) == false) {
            return;
        }
        uartUSB.write ("coded message ACK:\r\n", strlen ("coded message ACK:\r\n"));  // debug only
        uartUSB.write ( buffer, strlen ( buffer));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));

        size_t originalLength = strlen(buffer);

        // Copiar la cadena original y agregar '|'
        
        buffer[originalLength ] = '|';  // Agregar '|'
        buffer[originalLength + 1] = '|';      // Asegurar terminación nula
        buffer[originalLength + 2] = '\0';      // Asegurar terminación nula

        uartUSB.write("\r\n", strlen("\r\n"));
        uartUSB.write ( buffer, strlen ( buffer));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));
        messageFormatted = true; 
    }


    if (backoffTime->read() || firstChunkSent == false) {
        firstChunkSent = true;
        backoffTime->write(FLY_TIME);
        backoffTime->restart();

        size_t totalLength = strlen(buffer);
        size_t chunkSize = MAX_CHUNK_SIZE;  // Fragmentos de 50 bytes
        LoRaModule->idle();                     
        LoRaModule->enableInvertIQ();  
        size_t currentChunkSize = (totalLength - stringIndex < chunkSize) ? (totalLength - stringIndex) : chunkSize;
        uartUSB.write("\r\n", strlen("\r\n"));
        uartUSB.write ( buffer, strlen (buffer));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));
        LoRaModule->beginPacket();
        LoRaModule->write((uint8_t*)(buffer + stringIndex), currentChunkSize);
        //LoRaModule->write(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));

        //LoRaModule->write((uint8_t*)(buffer), strlen (buffer));
        LoRaModule->endPacket();
        stringIndex += chunkSize;
        if (stringIndex  > totalLength) {
            numberOffAckSendings++;
            //firstChunkSent = false;
            stringIndex = 0;
            backoffTime->restart();
            if (numberOffAckSendings < MAX_TRIES) {
                return;
            }
            numberOffAckSendings = 0;
            uartUSB.write("\r\n", strlen("\r\n"));
            uartUSB.write ("Changing State to WaitingForMessage:\r\n", 
            strlen ("Changing State to WaitingForMessage:\r\n"));  // debug only
            uartUSB.write("\r\n", strlen("\r\n"));
            firstDelayPassed = false;
            messageFormatted = false;
            firstEntryOnThisMethod = true;
            messageFormatted = false; 
            stringIndex = 0;
            this->currentGateway->changeState(new ConnectingToMobileNetwork (this->currentGateway));
            return;
        }
    }
}

 bool SendingAck::waitForMessage (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut){
      return false;
 }
//=====[Implementations of private functions]==================================
