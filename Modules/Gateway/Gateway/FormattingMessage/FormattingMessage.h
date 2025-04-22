//=====[#include guards - begin]===============================================

#ifndef _FORMATTING_MESSAGE_H_
#define _FORMATTING_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayState.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class FormattingMessage : public GatewayState {
public:
//=====[Declaration of public methods]=========================================
    FormattingMessage (Gateway * gateway, gatewayStatus_t gatewayStatus);
    virtual ~FormattingMessage ();
   virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );
    virtual void formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
      BatteryData  * batteryStatus); 
    virtual void exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage );
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
    virtual void goToSleep (CellularModule * cellularTransceiver);
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency);
private:
    void formatMessage(char * formattedMessage, CellInformation* aCellInfo,
     GNSSData* GNSSInfo, BatteryData  * batteryStatus);
    void formatMessage(char * formattedMessage, CellInformation* aCellInfo, 
    std::vector<CellInformation*> &neighborsCellInformation, BatteryData  * batteryStatus); 

    //bool checkResponse (char * response, char * retrivMessage);
    Gateway * gateway;
    gatewayStatus_t currentStatus;


//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _FORMATTING_MESSAGE_H_