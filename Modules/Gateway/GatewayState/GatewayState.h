//=====[#include guards - begin]===============================================

#ifndef _GATEWAY_STATE_H_
#define _GATEWAY_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"
#include "GNSSModule.h"
#include "LoRa.h"
#include "motion.h"


//=====[Declaration of public data types]======================================
struct TcpSocket; ///< Forward declaration for TCP socket structure. */
struct BatteryData; ///< Forward declaration for battery status structure. 

//=====[Declaration of public classes]=========================================

/**
 * @brief Interface for the State design pattern in the gateway system.
 * @details This abstract class defines the common interface for all states of the gateway.
 * Derived classes implement specific behavior for each operational state,
 * handling interactions with sensors, communication modules, and memory.
 */
class GatewayState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Updates the power status of the gateway based on the cellular transceiver and battery status.
     * @param cellularTransceiver Pointer to the CellularModule instance.
     * @param currentBatteryStatus Pointer to the current BatteryData structure.
     */
    virtual void updatePowerStatus (CellularModule* cellularTransceiver, BatteryData* currentBatteryStatus);

    /**
     * @brief Obtains the GNSS position from the GNSS module.
     * @param currentGNSSModule Pointer to the GNSSModule instance.
     * @param currentGNSSdata Pointer to the GNSSData structure to store the retrieved data.
     */
    virtual void obtainGNSSPosition (GNSSModule* currentGNSSModule, GNSSData* currentGNSSdata);

    /**
     * @brief Connects to the mobile network using the cellular transceiver.
     * @param cellularTransceiver Pointer to the CellularModule instance.
     * @param currentCellInformation Pointer to the structure storing the current cell information.
     */
    virtual void connectToMobileNetwork (CellularModule* cellularTransceiver,
    CellInformation * currentCellInformation);

    /**
     * @brief Obtains information about neighboring cells from the cellular transceiver.
     * @param cellularTransceiver Pointer to the CellularModule instance.
     * @param neighborsCellInformation Reference to a vector storing neighbor cell information.
     * @param numberOfNeighbors Number of neighboring cells to retrieve.
     */
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );


    /**
     * @brief Formats a message with cellular, GNSS, IMU, and battery data.
     * @param formattedMessage Pointer to the output buffer for the formatted message.
     * @param aCellInfo Pointer to the current cell information.
     * @param GNSSInfo Pointer to the GNSS data.
     * @param neighborsCellInformation Reference to the vector with neighbor cell information.
     * @param imuData Pointer to the current IMU data.
     * @param IMUDataSample Reference to the vector with IMU data samples.
     * @param batteryStatus Pointer to the battery status.
     */
    virtual void formatMessage (char * formattedMessage, long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* gatewayGNSSInfo, GNSSData* receptedGNSSdata,
    const IMUData_t * receptedImuData, const BatteryData  * receptedBatteryStatus); 

    /**
     * @brief Exchanges messages with a remote server via the cellular transceiver.
     * @param cellularTransceiver Pointer to the CellularModule instance.
     * @param message Pointer to the message to send.
     * @param socketTargetted Pointer to the target TCP socket.
     * @param receivedMessage Pointer to the buffer where the received message will be stored.
     */
    virtual void exchangeMessages (CellularModule* cellularTransceiver,
    char* message, TcpSocket* socketTargetted, char* receivedMessage );

    /**
     * @brief Exchanges messages with the LoRaGateway via the LoRa transceiver Module.
     * @param LoRaModule Pointer LoRa transceiver Module instance.
     * @param message Pointer to the message to send.
     * @param receivedMessage Pointer to the buffer where the received message will be stored.
     */
    virtual void exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage );

    /**
     * @brief Puts the cellular transceiver into a low-power sleep mode.
     * @param cellularTransceiver Pointer to the CellularModule instance.
     */
    virtual void goToSleep (CellularModule * cellularTransceiver);

    /**
     * @brief Wakes up the cellular transceiver and initializes timers.
     * @param cellularTransceiver Pointer to the CellularModule instance.
     * @param latency Pointer to the NonBlockingDelay instance for latency control.
     * @param silentTimer Pointer to the NonBlockingDelay instance for silent mode timing.
     */
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency, NonBlockingDelay * silentTimer);


    virtual void sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);
    
    virtual bool waitForMessage (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATEWAY_STATE_H_