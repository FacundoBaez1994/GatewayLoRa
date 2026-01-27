//=====[#include guards - begin]===============================================

#ifndef _GATEWAY_BASE_STATE_H_
#define _GATEWAY_BASE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayState.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway; ///< Forward declaration of the Gateway class.


//=====[Declaration of public classes]=========================================
/**
 * @brief Abstract base class for gateway states (State design pattern).
 * This class provides default (empty) implementations for all methods declared
 * in the `GatewayState` interface. It allows derived state classes to override
 * only the methods they need, avoiding the need to implement unused functionality.
 */
class GatewayBaseState : public GatewayState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @copydoc GatewayState::updatePowerStatus()
     * Default implementation: does nothing.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    /**
     * @copydoc GatewayState::obtainGNSSPosition()
     * Default implementation: does nothing.
     */
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);


    /**
     * @copydoc GatewayState::connectToMobileNetwork()
     * Default implementation: does nothing.
     */
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);

    /**
     * @copydoc GatewayState::obtainNeighborCellsInformation()
     * Default implementation: does nothing.
     */
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );

    /**
     * @copydoc GatewayState::formatMessage()
     * Default implementation: does nothing.
    */
    virtual void formatMessage (char * formattedMessage, long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* gatewayGNSSInfo, GNSSData* receptedGNSSdata,
    const IMUData_t * receptedImuData, const BatteryData  * receptedBatteryStatus); 

    
    /**
     * @copydoc GatewayState::exchangeMessages(CellularModule * cellularTransceiver,
     *  char * message,  RemoteServerInformation* serverTargetted, char * receivedMessage)
     * Default implementation: does nothing.
     */
    virtual void exchangeMessages (CellularModule * cellularTransceiver,
    char * message,  RemoteServerInformation* serverTargetted, char * receivedMessage );

    /**
     * @copydoc GatewayState::exchangeMessages(LoRaClass * LoRaModule, char * message, 
     * char * receivedMessage)
     * Default implementation: does nothing.
     */
    virtual void exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage );
    
    /**
     * @copydoc GatewayState::goToSleep()
     * Default implementation: does nothing.
     */
    virtual void goToSleep (CellularModule * cellularTransceiver);

    /**
     * @copydoc GatewayState::awake()
     * Default implementation: does nothing.
     */
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency, NonBlockingDelay * silentTimer);

    /**
     * @copydoc GatewayState::sendAcknowledgement
     * Default implementation: does nothing.
     */
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);

    /**
     * @copydoc GatewayState::waitForMessage
     * Default implementation: does nothing.
     */
    virtual bool waitForMessage (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);


    /**
     * @copydoc GatewayState::connectEthernetToLocalNetwork
     * Default implementation: does nothing.
     */
    virtual void connectEthernetToLocalNetwork (UipEthernet * ethernetModule, NonBlockingDelay * delay);

    /**
     * @copydoc GatewayState::exchangeMessagesThroughEthernet
     * Default implementation: does nothing.
     */
    virtual void exchangeMessagesThroughEthernet (UipEthernet * ethernetModule, NonBlockingDelay * delay, char * payload);

    /**
     * @copydoc GatewayState::queryUTCTimeViaRemoteServer
     * Default implementation: does nothing.
     */
    virtual void queryUTCTimeViaRemoteServer (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATEWAY_BASE_STATE_H_