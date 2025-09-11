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
     * @param aCellInfo Pointer to the primary cellular cell info.
     * @param GNSSInfo Pointer to GNSS data.
     * @param neighborsCellInformation Vector of pointers to neighboring cell info.
     * @param imuData Pointer to IMU data structure.
     * @param IMUDataSamples Vector of IMU data samples.
     * @param batteryStatus Pointer to battery data.
     */
    virtual void formatMessage (char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const std::vector<CellInformation*> &neighborsCellInformation,
    const IMUData_t * imuData,  const std::vector<IMUData_t*> &IMUDataSamples, const BatteryData  * batteryStatus); 

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
    void formatMessage(char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const IMUData_t * imuData, const BatteryData  * batteryStatus, char * gatewayEvent);

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
    void formatMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const std::vector<CellInformation*> &neighborsCellInformation, const IMUData_t * imuData,
     const BatteryData  * batteryStatus, char * gatewayEvent);

    /**
     * @brief Creates a JWT type message combining several cellular info , inertial data,
     * battery status, in order to be send to a remote server through Mobile network
     * @param formattedMessage Output buffer for the formatted message.
     * @param IMEI long long int with the IMEI of the device.
     * @param GNSSInfo Pointer to GNSS data.
     * @param imuData Pointer to IMU data structure.
     * @param batteryStatus Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatMessage(char* formattedMessage, long long int IMEI,
    const GNSSData* GNSSInfo, const IMUData_t* imuData, const BatteryData* batteryStatus, 
    char * gatewayEvent);

    /**
     * @brief Creates a JWT type message combining several cellular info , inertial data,
     * battery status, in order to be send to a remote server through Mobile network
     * @param formattedMessage Output buffer for the formatted message.
     * @param IMEI long long int with the IMEI of the device.
     * @param GNSSInfo Pointer to GNSS data.
     * @param imuData Pointer to IMU data structure.
     * @param IMUDataSamples Vector of IMU data structure with inertial samples
     * @param batteryStatus Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatMessage(char* formattedMessage, long long int IMEI, const IMUData_t* inertialData, 
    const std::vector<IMUData_t*> &IMUDataSamples, 
    const BatteryData* batteryStatus, char* gatewayEvent);

    /**
     * @brief Creates a message combining several cellular info, GNSS related info, inertial data,
     * battery status, in order to be send to a LoRaGateway through a LoRa transceiver Module
     * @param formattedMessage Output buffer for the formatted message.
     * @param aCellInfo Pointer to the primary cellular cell info.
     * @param GNSSInfo Pointer to GNSS data.
     * @param imuData Pointer to IMU data structure.
     * @param batteryData Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatLoRaMessage(char* formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const IMUData_t* imuData, 
    const BatteryData* batteryStatus, char* gatewayEvent);

    /**
     * @brief Creates a message combining cellular info, GNSS related info, inertial data,
     * battery status, in order to be send to a LoRaGateway through a LoRa transceiver Module
     * @param formattedMessage Output buffer for the formatted message.
     * @param aCellInfo Pointer to the primary cellular cell info.
     * @param imuData Pointer to IMU data structure.
     * @param batteryData Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatLoRaMessage(char* formattedMessage, const CellInformation* aCellInfo, 
    const IMUData_t* imuData, const BatteryData* batteryStatus, char* gatewayEvent);


    /**
     * @brief Creates a message combining GNSS related info, inertial data,
     * battery status, in order to be saved on a non volatile memory
     * @param formattedMessage Output buffer for the formatted message.
     * @param GNSSInfo Pointer to GNSS data.
     * @param inertialData Pointer to IMU data structure.
     * @param batteryData Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatGNSSMemoryMessage(char* formattedMessage, const GNSSData* GNSSInfo, 
    const IMUData_t* inertialData, const BatteryData* batteryStatus, char* gatewayEvent);

    /**
     * @brief Creates a message combining GNSS related info, inertial data,
     * battery status, to be saved on a non volatile memory.
     * @param formattedMessage Output buffer for the formatted message.
     * @param aCellInfo Pointer to the primary cellular cell info.
     * @param neighborsCellInformation Vector of pointers to neighboring cell info.
     * @param imuData Pointer to IMU data structure.
     * @param batteryStatus Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatMNMNMemoryMessage (char* formattedMessage, const CellInformation* aCellInfo, 
    const std::vector<CellInformation*> &neighborsCellInformation, const IMUData_t* imuData,
     const BatteryData* batteryStatus, char* gatewayEvents);

    /**
     * @brief Creates a message combining GNSS related info, inertial data,
     * battery status, in order tto be saved on a non volatile memory.
     * @param formattedMessage Output buffer for the formatted message.
     * @param aCellInfo Pointer to the primary cellular cell info.
     * @param GNSSInfo Pointer to GNSS data.
     * @param inertialData Pointer to IMU data structure.
     * @param batteryData Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatMemoryMessage(char* formattedMessage, const CellInformation* aCellInfo, const GNSSData* GNSSInfo, 
    const IMUData_t * inertialData, const BatteryData* batteryStatus, char* gatewayEvent);


    /**
     * @brief Creates a message combining GNSS related info, inertial data,
     * battery status, in order to be saved on a non volatile memory
     * @param formattedMessage Output buffer for the formatted message.
     * @param inertialData Pointer to IMU data structure.
     * @param IMUDataSamples Vector of IMU data structure with inertial samples.
     * @param batteryData Pointer to battery data.
     * @param gatewayEvent String showing a movement event.
    */
    void formatMemoryMessage(char* formattedMessage, const IMUData_t* inertialData, 
    const std::vector<IMUData_t*> &IMUDataSamples, const BatteryData* batteryStatus, char* gatewayEvent);

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