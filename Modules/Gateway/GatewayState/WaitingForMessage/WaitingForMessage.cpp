//=====[Libraries]=============================================================
#include "WaitingForMessage.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define TIMEOUT       1000
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
WaitingForMessage::WaitingForMessage (Gateway * gateway) {
    this->currentGateway = gateway;
}


/** 
* @brief
* 
* @param 
*/
WaitingForMessage::~WaitingForMessage() {
     this->currentGateway = NULL;
}

bool WaitingForMessage::waitForMessage (LoRaClass * LoRaModule, char * messageRecieved,
 NonBlockingDelay * timeOut){
    static bool messageReceived = false; 
    static std::vector<char> accumulatedBuffer; // Acumulador de fragmentos
    static std::string fullMessage;

    static uint64_t total_bits = 0;         // bits recibidos en total
    static uint64_t total_bit_errors = 0;   // errores totales
    static float lastBER = 0.0f;            // BER del último paquete
    static Timer timer;
    using namespace std::chrono;

    char processedMessageReceived  [2048];
    static char message[1024];
    char buffer [2048];
    char payload [1024];

    static int packetRSSI;

    long long int deviceId = 0;
    int messageNumber = 0;

    static bool firstPacketReceived = false;
    static bool firstEntryOnThisMethod = true;
    static int stringInsertCount = 0;

    uint8_t receivedBuffer[64];

    if (firstEntryOnThisMethod == true) {
        timer.start();
        timeOut->write(TIMEOUT);
        timeOut->restart();
        //uartUSB.write("time out restart\r\n", strlen("time out restart\r\n")); // Debug
        if (!LoRaModule->begin (915E6)) {
            uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            return false;
        }
        firstEntryOnThisMethod = false;
    }

    if (messageReceived  == false) {
        LoRaModule->disableInvertIQ (); // rx mode -> phase Quadrature invertion   
        int packetSize = LoRaModule->parsePacket();
        if (packetSize) {
            //uartUSB.write("Packet Received!\r\n", strlen("Packet Received!\r\n")); // Debug

             //restart timeOut Timer
            if (firstPacketReceived == false) {
                timeOut->restart ();
                //uartUSB.write("time out restart\r\n", strlen("time out restart\r\n")); // Debug
                firstPacketReceived = true;
            }

            int maxIterations = 100; // Límite para evitar un ciclo infinito
            int iterations = 0;

            // Leer los datos disponibles
            
            while (LoRaModule->available() > 0 ) {
                ssize_t bytesRead = LoRaModule->read(reinterpret_cast<uint8_t*>(buffer), sizeof(buffer));
               // ssize_t bytesRead = LoRaModule->read(reinterpret_cast<uint8_t*>(buffer), strlen (buffer));
                if (bytesRead > 0) {
                    // Enviar los bytes leídos al puerto serie
                   // uartUSB.write(buffer, bytesRead);
                    //uartUSB.write(buffer, strlen(buffer));
                    //uartUSB.write ("\r\n", strlen("\r\n"));

                    /*
                    for (ssize_t i = 0; i < bytesRead; i++) {
                        char hexByte[8];
                        snprintf(hexByte, sizeof(hexByte), "0x%02X ", (uint8_t)buffer[i]);
                        uartUSB.write(hexByte, strlen(hexByte));
                    }
                    uartUSB.write("\r\n", 2);
                    */


                    accumulatedBuffer.insert(accumulatedBuffer.end(), buffer, buffer + bytesRead);
                    fullMessage.assign(accumulatedBuffer.begin(), accumulatedBuffer.end ());
                    messageReceived = true;

                    // Debug del mensaje completo
                    //uartUSB.write("Full Message: ", strlen("Full Message: "));
                    //uartUSB.write(fullMessage.c_str(), fullMessage.length());
                    //uartUSB.write("\r\n", strlen("\r\n"));
                    

                    packetRSSI = LoRaModule->packetRssi();
                   // snprintf(message, sizeof(message), "packet RSSI: %d\r\n", packetRSSI);
                   // uartUSB.write(message, strlen(message));
        
                } //  if (bytesRead > 0) end
            } // while (LoRaModule->available() > 0 && iterations < maxIterations)  end
        } //  if (packetSize)  end
    }  else {   // si messageReceived  == true entonces:
        if (fullMessage.empty()) {
            accumulatedBuffer.clear();
            messageReceived = false;
            //uartUSB.write("Fail to process received message\r\n", strlen("Fail to process received message\r\n"));
            return false;
        }

        const char* constCharPtr = fullMessage.c_str(); 
        strncpy(processedMessageReceived, constCharPtr, sizeof(processedMessageReceived) - 1);
        processedMessageReceived[sizeof(processedMessageReceived) - 1] = '\0';

        messageReceived  = false;
        accumulatedBuffer.clear(); // Elimina todos los elementos del vector
        stringInsertCount = 0;
        fullMessage.clear();       // Elimina todo el contenido de la cadena
        //uartUSB.write("recepted message\r\n", strlen("recepted message\r\n"));
         //uartUSB.write(processedMessageReceived, strlen(processedMessageReceived));
        
        // ======== BER CALCULATION ========

// Convert processed message -> array of bytes
uint8_t* rx_buffer = reinterpret_cast<uint8_t*>(processedMessageReceived);
int msg_len = strlen(processedMessageReceived);

// Expected pattern 0xAA (10101010)
uint8_t expected = 0xAA;

uint32_t bit_errors = 0;

// Comparar bit a bit
for (int i = 0; i < msg_len; i++) {
    uint8_t diff = rx_buffer[i] ^ expected;

    for (int b = 0; b < 8; b++) {
        if (diff & (1 << b))
            bit_errors++;
    }
}

uint32_t bits_total_packet = msg_len * 8;

// Acumular
total_bits += bits_total_packet;
total_bit_errors += bit_errors;

// BER del paquete
if (bits_total_packet > 0)
    lastBER = (float)bit_errors / (float)bits_total_packet;

// BER acumulado (opcional)
float BER_total = 0.0f;
if (total_bits > 0)
    BER_total = (float)total_bit_errors / (float)total_bits;

// ======== PRINT DEBUG ========
char debugMsg[128];
/*
snprintf(debugMsg, sizeof(debugMsg),
         "BER packet: %.6f | Errors: %lu / %lu bits\r\n",
         lastBER, (unsigned long)bit_errors, (unsigned long)bits_total_packet);
uartUSB.write(debugMsg, strlen(debugMsg));
*/

uint64_t now = duration_cast<milliseconds>(timer.elapsed_time()).count();

snprintf(debugMsg, sizeof(debugMsg),
         "%d,%.8f,%llu,%llu,%llu\r\n",
         packetRSSI , BER_total, total_bit_errors, total_bits, now);
uartUSB.write(debugMsg, strlen(debugMsg));

        
        return true;

    }

    return false;
}


//=====[Implementations of private functions]==================================
