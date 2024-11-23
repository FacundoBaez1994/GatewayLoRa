//=====[Libraries]=============================================================

#include "Gateway.h"
#include "Debugger.h" // due to global usbUart





//=====[Declaration of private defines]========================================
#define LATENCY        50
#define IP      "192.168.1.35"
#define GATEWAY "192.168.1.1"
#define NETMASK "255.255.255.0"

const uint8_t   MAC[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
/** 
* @brief Contructor method creates a new trackerGPS instance ready to be used
*/
Gateway::Gateway () {
    this->LoRaTransciever = new LoRaClass ();
    if (!this->LoRaTransciever->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    this->ethernetModule = new UipEthernet (MAC, PB_5, PB_4, PB_3, PA_4);  // mac, mosi, miso, sck, cs
    this->resetEth =  new DigitalOut (PA_1);
    this->resetEth->write(HIGH);
    this->timer = new NonBlockingDelay (LATENCY);
    this->currentState = new WaitingForMessage (this);


    /*
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);
    char StringToSendUSB [50] = "Tracker initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    
    this->cellularTransceiver = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransceiver->getPowerManager()
    , this->cellularTransceiver->getATHandler());
    //both share the same power manager and ATHandler (uart)

    this->socketTargetted = new TcpSocket;
    this->socketTargetted->IpDirection = new char[16]; // 
    strcpy(this->socketTargetted->IpDirection, "186.19.62.251");
    this->socketTargetted->TcpPort = 123;

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->date  = new char [10];
    this->currentCellInformation->time  = new char [10];
    this->currentCellInformation->band = new char [20];

    this->currentGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;
    */
}


Gateway::~Gateway() {
    delete this->currentState;
    this->currentState = NULL;
    delete this->LoRaTransciever; 
    this->LoRaTransciever = NULL;
    delete this->ethernetModule; 
    this->ethernetModule = NULL;
    delete this->resetEth; 
    this->resetEth = NULL;
    delete this->timer;
    this->timer = NULL;
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void Gateway::update () {
    this->currentState->receiveMessage (this->LoRaTransciever, this->timer);
    this->currentState->sendAcknowledgement (this->LoRaTransciever, this->timer);
    this->currentState->sendTCPMessage (this->ethernetModule, this->timer);
}


void Gateway::changeState  (GatewayState * newState) {
    delete this->currentState;
    this->currentState = newState;
}

//=====[Implementations of private methods]==================================
void Gateway::LoRa_rxMode() {
    LoRa.disableInvertIQ();               // Disable I/Q inversion for reception
   // LoRa.receive();                       // Set receive mode
}

void Gateway::LoRa_txMode() {
    LoRa.idle();                          // Standby mode
    LoRa.enableInvertIQ();                // Enable I/Q inversion for transmission
}