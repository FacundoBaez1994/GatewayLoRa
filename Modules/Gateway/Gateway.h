//=====[#include guards - begin]===============================================
 /**
 * @mainpage Gateway LoRa Firmware
 *
 * @section intro_sec Introduction
 *
 * This project implements the firmware of a LoRa Gateway based on a
 * state machine architecture. It is responsible for receiving data packets
 * from remote tracking devices, managing LoRa communication, and forwarding
 * the received information to a backend server through the corresponding
 * network interface.
 *
 * @section features_sec Main Features
 * - State machine management for  operation as a gateway
 * - Communication via cellular and/or LoRa modules
 * - Efficient power and resource management
 * - Modular architecture oriented to embedded systems
* -  internet access via ethenet
 *
 * @section hardware_sec Supported Hardware
 * - Microcontroller: STM32 (Nucleo-L432KC)
 * - Cellular and GNSS Quectel EC21 module
 * - Ethernet Module ENC28J60
 * - RFM95 LoRa Module
 *
 *
 * @section dependencies_sec Dependencies
 * This firmware depends on the following libraries:
 * - UIPEthernet by Juraj Andrássy,
 * - LoRa by Sandeep Mistry
 * - CustomJWT by Antony Jose Kuruvilla
 * - Cypher AES by Neil Thiessen
 * - NonBlocking Delay and Miscellaneous codes from Arm Book A Beginner's-Guide-to-Designing-Embedded-System-Applications-on-Arm-Cortex-M-Microcontrollers
 *
 * @section author_sec Author
 * Facundo Baez 
 * Elordi Joaquin
 * Students from Faculty of Engineering, University of Buenos Aires (FIUBA)
 *
 * @section history_sec Version History
 * v1.0 First functional release of the gateway firmware
 */

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

#include "BufferSizes.h"


//=====[Declaration of public defines]=========================================
/**
 * @brief Enumeration representing latency levels for tracker operations.
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

/**
 * @brief Types of messages that this Gateway could sent to the remote server.
 */
typedef enum {
    LORALORA,
    LORAGNSS,
} ReceptedTypeMessage_t;


/**
 * @brief Enumeration representing the operation modes of a tracker.
 */
typedef enum {
    NORMAL_OPERATION_MODE,      /**< Standard tracking behavior */
    PURSUIT_OPERATION_MODE,      /**< Aggressive tracking mode for pursuit scenarios */
    SILENT_OPERATION_MODE,       /**< Reduced activity to minimize detection or save power */
} OperationMode_t;

/**
 * @brief Enumeration of movement-related events detected by a tracker.
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
 * @details This class represents the main control unit of the gateway device.
 * It manages communication modules, encryption/authentication mechanisms,
 * and operational states using the State design pattern.
 * It delegates specific behaviors to its current state (`GatewayState`) and handles
 * interactions between modules such as:
 * - Cellular communications
 * - GNSS positioning
 * - LoRa communications
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
    * network connectivity, message formatting and exchange, LoRa recption and
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
     * @brief Gets the current movement event as a string.
     * @param movementEventString Buffer to store the event string.
     */
    void getMovementEvent (char * movementEventString);

    /**
     * @brief retrievs the IMEI from the tracker that sent a message to this gateway
     * @return the IMEI number
     */
    long long int getReceptedIMEI ();

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

    /**
     * @brief Parces a received LoRa message.
     * @param messageToParse Pointer to the message buffer.
     * @return True if parsing succeeded, false otherwise.
     */
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
     * @return deviceId Pointer to a string to store the identifier.
     */
    char* getDeviceIdentifier ();

    /**
     * @brief obtain the identifier of this device
     * @return deviceId Pointer to a string to store the identifier.
     */
    void getDeviceIdentifier (char * deviceId);


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
     * @return hashChain a valid hash Chain
    */
    char* getPrevHashChain ();

    /**
     * @brief get the prev hash chain
     * @param hashChain a valid hash Chain
    */
    void getPrevHashChain (char* hashChain);

    
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

        
     /**
     * @brief encodes an string with JSON format to a JWT
     * @param payloadToJWT a string with JSON
     * @param jwtEncoded a string with the JSON codified into a JWT
     */
    void encodeJWT (char * payloadToJWT, char * jwtEncoded);


     /**
     * @brief decodes an string codified as a JWT to a JSON format plainmessage
     * @param jwtToDecode a string codified a a JWT
     * @param payloadRetrived a string decoded from JWT to JSON
     */
    bool decodeJWT (char * jwtToDecode, char * payloadRetrived);

    
private:
    GatewayState* currentState;            /**< Current operational state */
    OperationMode_t currentOperationMode;   /**< Current mode of operation */

    // metadata
    int sequenceMessageNumber = 0; /**< a Sequence that increses with every messages succefully sent to the remote server */
    char* deviceIdentifier; /**< a string that identifies this current device*/
    char* prevChainHash; /**< hash of the previous message sent to the remote server*/
    char* currChainHash; /**< hash of the current message about to be sent to the remote server*/

    // Ethernet
    UipEthernet * ethernetModule;  /**< pointer to the ethernet module*/
    DigitalOut * resetEth;  /**< Digital Output that controls the reset of the ethenet module*/


    deviceMotionStatus_t newMotionStatus = DEVICE_ON_MOTION;        /**< Latest motion status */
    deviceMotionStatus_t currentMotionStatus = DEVICE_ON_MOTION;    /**< Current motion status */
    MovementEvent_t currentMovementEvent = MOVING;                  /**< Current detected movement event */

    // IMU 
    std::vector<IMUData_t*> IMUDataSamples;      /**< Sampled IMU data */

     // LORA
    LoRaClass * LoRaTransciever;    /**< LoRa Module transceiver */
    NonBlockingDelay * timeout;  /**< timer that controls LoRa sending/reception times*/

    // LORA Recepted Data
    long long int IMEIRecepted; /**< IMEI of the tracker that comunicates with this gateway*/
    int RSSI; /**< received signal strength indicator */
    int loraMessageNumber = 1;    /**< interger counting the number of messages sent by LoRa */
    BatteryData* receptedBatteryData;   /**< Latest Retrived Battery data from tracker */
    GNSSData* receptedGNSSdata;  /**< Latest Retrived GNSS data from tracker */
    IMUData_t* receptedImuData;  /**< Latest  Retrived IMU data from tracker */
    char receptedTrackerEvent [20]; /**< Latest Event from tracker */
    ReceptedTypeMessage_t receptedTypeMessage; /**< Type of Message recepted from the tracker */

    
    // MN Module 
    CellularModule* cellularTransceiver;        /**< Cellular transceiver */
    RemoteServerInformation* serverTargetted;                 /**< Target TCP socket */
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

    JWTManager* jwt; ///< Pointer to JWT manager for token/signature management
};


//=====[#include guards - end]=================================================

#endif // _GATEWAY_H_
