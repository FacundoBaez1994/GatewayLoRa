//=====[#include guards - begin]===============================================
#ifndef _FORMATTING_MESSAGE_H_
#define _FORMATTING_MESSAGE_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Gateway.h"
#include "JWTManager.h"

#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include <cstring>
#include <cstdio>

//=====[Declaration of public data types]======================================
class Gateway; ///< Forward declaration to avoid circular dependency

//=====[Declaration of public classes]=========================================
/**
 * @brief State class that formats messages for transmission or storage.
 * @details Implements the "FormattingMessage" state in the gateway state machine.
 * Provides multiple overloaded methods to format messages including cellular data,
 * GNSS data, IMU data, battery status, and other parameters.
 * Supports formatting for regular transmission and LoRa.
 * Utilizes JWTManager for message signing or token generation if needed.
 */
class FormattingMessage : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor
     * @param gateway Pointer to the Gateway instance managing states and data.
     * @param gatewayStatus Current status of the gateway.
     */
    FormattingMessage (Gateway * gateway, gatewayStatus_t gatewayStatus);

    /**
     * @brief Constructor
     * @param gateway Pointer to the Gateway instance managing states and data.
     */
    FormattingMessage (Gateway * gateway);

    /**
     * @brief Destructor
     */
    virtual ~FormattingMessage ();

    /**
     * @brief Updates the power status of the cellular transceiver.
     * @param cellularTransceiver Pointer to the cellular module.
     * @param currentBatteryStatus Pointer to current battery data.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    /**
     * @brief Formats a message combining cellular info, GNSS data, inertial data,
     * battery status, and neighbors' cell info for transmission or storage.
     * @param formattedMessage Output buffer for the formatted message.
     * @param gatewayGNSSInfo Pointer to GNSS data related to the gatway position.
     * @param receptedGNSSdata Pointer to GNSS data related to the tracker.
     * @param receptedImuData Pointer to IMU data structure.
     * @param receptedBatteryStatus Pointer to battery data.
     */
    virtual void formatMessage (char * formattedMessage, long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* gatewayGNSSInfo, GNSSData* receptedGNSSdata,
    const IMUData_t * receptedImuData, const BatteryData  * receptedBatteryStatus); 

private:
//=====[Declaration of privates methods]=========================================

    void addMetaData(char *messageToAddMetaData);

    /**
     * @brief Creates a JWT type message combining cellular info, GNSS data, inertial data,
     * battery status, in order to be send to a remote server through Mobile network
     * @param formattedMessage Output buffer for the formatted message.
     * @param aCellInfo Pointer to the primary cellular cell info.
     * @param GNSSInfo Pointer to GNSS data.
     * @param imuData Pointer to IMU data structure.
     * @param IMUDataSamples Vector of IMU data samples.
     * @param batteryStatus Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatLORAGNSSMessage (char * formattedMessage, const long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* receptedGNSSdata, 
    const IMUData_t * receptedImuData, 
    const BatteryData  * receptedBatteryStatus); 

    /**
     * @brief Creates a JWT type message combining several cellular info , inertial data,
     * battery status, in order to be send to a remote server through Mobile network
     * @param formattedMessage Output buffer for the formatted message.
     * @param aCellInfo Pointer to the primary cellular cell info.
     * @param neighborsCellInformation Vector of pointers to neighboring cell info.
     * @param imuData Pointer to IMU data structure.
     * @param IMUDataSamples Vector of IMU data samples.
     * @param batteryStatus Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatLORALORAMessage (char * formattedMessage, long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* gatewayGNSSInfo,
    const IMUData_t * receptedImuData, const BatteryData  * receptedBatteryStatus); 


//=====[Declaration of privates atributes]=========================================
    Gateway* gateway; ///< Pointer to gateway instance for state transitions and data access
    gatewayStatus_t currentStatus; ///< Current gateway status
    JWTManager* jwt; ///< Pointer to JWT manager for token/signature management
    
    char* messageBuffer; ///<
    int sizeOfMessageBuffer; ///< 
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _FORMATTING_MESSAGE_H_