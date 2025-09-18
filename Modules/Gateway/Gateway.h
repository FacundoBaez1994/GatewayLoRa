//=====[#include guards - begin]===============================================
/**
 * @file Gateway.h
 * @brief Declares the Gateway class, which manages the state machine for tracking operations.
 *
 * This class is responsible for controlling the device's tracking behavior,
 * including state transitions, message handling, and management of
 * communication modules. It follows the State design pattern, where
 * different states encapsulate specific behavior.
 */

#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"
#include "GNSSModule.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "GatewayState.h"
#include "GatewayStatus.h"
#include "UipEthernet.h"
#include "motion.h"

#include "LoRa.h"

#include "MessageHandler.h"
#include "MessageHandlerStatus.h"
#include "AuthenticationGenerator.h"
#include "AuthenticationVerifier.h"
#include "BaseMessageHandler.h"
#include "ChecksumVerifier.h"
#include "ChecksumGenerator.h"
#include "Decrypter.h"
#include "Encrypter.h"
#include "DecrypterBase64.h"
#include "EncrypterBase64.h"


//=====[Declaration of public defines]=========================================
/**
 * @brief Enumeration representing latency levels for gateway operations.
 */
typedef enum {
    EXTREMELY_LOW_LATENCY,
    VERY_LOW_LATENCY,
    LOW_LATENCY,
    MEDIUM_LATENCY,
    HIGH_LATENCY,
    VERY_HIGH_LATENCY,
    EXTREMELY_HIGH_LATENCY,
} LatencyLevel_t;

typedef enum {
    LORALORA,
    LORAGNSS,
} ReceptedTypeMessage_t;


/**
 * @brief Enumeration representing the operation modes of the gateway.
 */
typedef enum {
    NORMAL_OPERATION_MODE,      /**< Standard tracking behavior */
    PURSUIT_OPERATION_MODE,      /**< Aggressive tracking mode for pursuit scenarios */
    SILENT_OPERATION_MODE,       /**< Reduced activity to minimize detection or save power */
} OperationMode_t;

/**
 * @brief Enumeration of movement-related events detected by the gateway.
 */
typedef enum {
    MOVING,                 /**< Device is currently in motion */
    STOPPED,                /**< Device is stationary */
    PARKING,                /**< Device is stopped after being in motion */
    MOVEMENT_RESTARTED,     /**< Movement resumed after being stopped */
} MovementEvent_t;



//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/**
 * @brief High-level GNSS gateway class coordinating all subsystems.
 * @details This class represents the main control unit of the tracking device.
 * It manages sensors, communication modules, encryption/authentication mechanisms,
 * and operational states using the State design pattern.
 * It delegates specific behaviors to its current state (`GatewayState`) and handles
 * interactions between modules such as:
 * - Cellular communications
 * - GNSS positioning
 * - IMU sensing
 * - EEPROM storage
 * - Encryption, decryption, and authentication
 */
class Gateway {
public:
    /**
     * @brief Constructs the gateway and initializes modules.
     */
    Gateway ();

    /**
     * @brief Destroys the gateway and releases allocated resources.
     */
    virtual ~Gateway ();

    /**
    * @brief Main periodic update function.
    * @details This method acts as the primary execution loop for the Gateway class.
    * It invokes the current state's behavior by calling the state's update methods,
    * orchestrating the sequence of operations required for the device to function correctly.
    * During each call, it handles sensor calibration, power management, GNSS positioning,
    * network connectivity, message formatting and exchange, data saving/loading, and
    * sleep management. The update function is designed to be called repeatedly, typically
    * from the main program loop (or from a even higher class that uses a gateway) 
    * or timer interrupt, ensuring the Gateway's state machine
    * progresses and the device operates responsively in real time.
    */
    void update();

    /**
     * @brief Changes the current operational state of the gateway.
     * @param newGatewayState Pointer to the new state instance.
     */
    void changeState  (GatewayState * newGatewayState);

    /**
     * @brief Updates the movement event based on IMU readings and motion status.
     */
    void updateMovementEvent ();

    /**
     * @brief Sets the current movement event using a string identifier.
     * @param event String representing the movement event.
     */
    void setMovementEvent (char * event);

    /**
     * @brief Gets the current movement event as an enumeration.
     * @return Current movement event.
     */
    MovementEvent_t getMovementEvent ();

    /**
     * @brief Gets the current movement event as a string.
     * @param movementEventString Buffer to store the event string.
     */
    void getMovementEvent (char * movementEventString);

    /**
     * @brief Sets the latency level for gateway operations.
     * @param level Latency level.
     */
    void setLatency(LatencyLevel_t level);

    /**
     * @brief Retrieves the current operation mode.
     * @return Current operation mode.
     */
    OperationMode_t getOperationMode ();

    /**
     * @brief 
     * @return
     */
    long long int getReceptedIMEI ();

    /**
     * @brief Sets the current operation mode.
     * @param newOperationMode New operation mode.
     */
    void setOperationMode (OperationMode_t newOperationMode);

    /**
     * @brief Configures the silent mode timer.
     * @param hours Duration of silent mode in hours.
     */
    void setSilentTimer (int hours);

    /**
     * @brief Actualize the keep alive timer.
     * @details when the gateway is in STOPPED MODE, it would send a message
     * periodically but with and increase latency based on the current latency.
     */
    void actualizeKeepAliveLatency ();

    /**
     * @brief Encrypts a message using the configured encryption chain.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if encryption succeeded, false otherwise.
     */
    bool encryptMessage (char * message, unsigned int messageSize);

    /**
     * @brief Decrypts a message using the configured decryption chain.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if decryption succeeded, false otherwise.
     */
    bool decryptMessage (char * message, unsigned int messageSize);
    
    /**
     * @brief Prepares a LoRa message for transmission.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if preparation succeeded, false otherwise.
     */
    bool prepareLoRaMessage (char * message, unsigned int messageSize);

    /**
     * @brief Processes a received LoRa message.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if processing succeeded, false otherwise.
     */
    bool processLoRaMessage (char * message, unsigned int messageSize);

    /**
     * @brief Retrivs the current numeration of messages sent via LoRa.
     * @return an int with.
     */
    int getLoraMessageNumber ();

    /**
     * @brief set the current numeration of messages recepted via LoRa.
     * @param  messageNumber an integer with the current message number.
     */
    void setLoRaMessageNumber (int messageNumber);


    bool parseReceptedLoRaMessage (char * messageToParse);

    
    /**
     * @brief obtain the url Main path to the server
     * @param urlPathChannel Pointer to a string to store the path.
     */
    void getUrlPathMainChannel ( char * urlPathChannel);

    /**
     * @brief obtain the url Secondary path to the server
     * @param urlPathChannel Pointer to a string to store the path.
     */
    void getUrlPathSecondaryChannel ( char * urlPathChannel);

    /**
     * @brief obtain the identifier of this device
     * @param deviceId Pointer to a string to store the identifier.
     */
    void getDeviceIdentifier ( char * deviceId);

    /**
     * @brief obtain the current Sequence number
     * @returns an integrer with the current Sequence Number.
     */
    int getSequenceNumber ();

    /**
     * @brief increase the sequence Number by one
     */
    void increaseSequenceNumber ();

    /**
     * @brief copies the current chain hash on the prev chain hash
     */
    void progressOnHashChain ();

     /**
     * @brief set a hash chain as the current one
     * @param hashChain a valid hash Chain
     */
    void setCurrentHashChain (char * hashChain);

    /**
     * @brief get the prev hash chain
     * @param hashChain a valid hash Chain
    */
    void getPrevHashChain (char * hashChain);

    
    /**
     * @brief get the current message type recieved on LoRa
     * @param ReceptedTypeMessage_t the type of message recieved on the LoRa Receptor
    */
    ReceptedTypeMessage_t getReceptedTypeMessage ( );

    
     /**
     * @brief set the RSSI of the current message recepted
     * @param RSSI a int representing the RSSI 
     */
    void setCurrentRSSI (int newRSSI);
    
private:
    GatewayState* currentState;            /**< Current operational state */
    NonBlockingDelay* latency;             /**< Latency timer */
    NonBlockingDelay* silentKeepAliveTimer;         /**< Silent mode timer */
    LatencyLevel_t latencyLevel;
    OperationMode_t currentOperationMode;   /**< Current mode of operation */

    // metadata
    int sequenceMessageNumber = 0;
    char* urlPathMainChannel;
    char* urlPathSecondaryChannel;
    char* deviceIdentifier;
    char* prevChainHash;
    char* currChainHash;

    // Ethernet
    UipEthernet * ethernetModule;
    DigitalOut * resetEth;


    deviceMotionStatus_t newMotionStatus = DEVICE_ON_MOTION;        /**< Latest motion status */
    deviceMotionStatus_t currentMotionStatus = DEVICE_ON_MOTION;    /**< Current motion status */
    MovementEvent_t currentMovementEvent = MOVING;                  /**< Current detected movement event */

    // IMU 
    std::vector<IMUData_t*> IMUDataSamples;      /**< Sampled IMU data */

     // LORA
    LoRaClass * LoRaTransciever;    /**< LoRa Module transceiver */
    NonBlockingDelay * timeout;

    // LORA Recepted Data
    long long int IMEIRecepted;
    int RSSI;
    int loraMessageNumber = 1;    /**< interger counting the number of messages sent by LoRa */
    BatteryData* receptedBatteryData;  
    GNSSData* receptedGNSSdata;  /**< Latest Retrived GNSS data from tracker */
    IMUData_t* receptedImuData;  /**< Latest  Retrived IMU data from tracker */
    char receptedTrackerEvent [20];
    ReceptedTypeMessage_t receptedTypeMessage;

    
    // MN Module 
    CellularModule* cellularTransceiver;        /**< Cellular transceiver */
    TcpSocket* socketTargetted;                 /**< Target TCP socket */
    CellInformation* currentCellInformation;   /**< Current cell information */
    std::vector<CellInformation*> neighborsCellInformation;    /**< Neighbor cell data */

    // GNSS Module 
    GNSSModule* currentGNSSModule;      /**< GNSS receiver module */
    GNSSData* gatewayGNSSdata;         /**< Latest GNSS data */

    // Battery
    BatteryData* batteryStatus;        /**< Current battery status */

   // Message Handlers
    //MessageHandler* encrypter;          /**< Encryption handler */
    MessageHandler* encrypterBase64;    /**< Base64 encryption handler */
    MessageHandler* authgen;            /**< Authentication generator */
    MessageHandler* ckgen;             /**< Checksum generator */
    MessageHandler* checksumVerifier;  /**< Checksum verifier */
    MessageHandler* authVer;           /**< Authentication verifier */
    //MessageHandler* decrypter;         /**< Decryption handler */
    MessageHandler* decrypterBase64;   /**< Base64 decryption handler */

};


//=====[#include guards - end]=================================================

#endif // _GATEWAY_H_
