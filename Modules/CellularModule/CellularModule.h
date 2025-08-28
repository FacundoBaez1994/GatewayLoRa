//=====[#include guards - begin]===============================================
#ifndef _CELLULAR_MODULE_H_
#define _CELLULAR_MODULE_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "TransceiverState.h"
#include "ActivatePDP.h"
#include "PowerManager.h"
#include "TransceiverUnavailable.h"
#include "ConnectionUnavailableState.h"
#include "PowerOFFState.h"
#include "PowerState.h"

//=====[Declaration of public data types]======================================
class ConnectionState; ///<  Mandatory Forward declaration due to circular dependency
struct BatteryData;

/**
 * @struct TcpSocket
 * @brief Represents a TCP socket endpoint.
 */
 struct TcpSocket {
    char * IpDirection; ///< IPv4 address as a string
    int TcpPort;        ///< TCP port number
 };

/**
 * @struct CellInformation
 * @brief Contains information about a Base Transceiver Station (BTS) of the Mobile Network.
 */
struct CellInformation {
    float signalLevel;          ///< Signal strength level measured in dB
    long long int IMEI;         ///< Device IMEI number
    int accessTechnology;       ///< Access technology indicator (GSM, UMTS, LTE)
    int registrationStatus;     ///< Registration status (as integer to reduce data length)
    int channel;                ///< Channel number
    char *  timestamp;          ///< Timestamp string
    char * band;                ///< Frequency band string
    int tech;                   ///< Technology generation (2=2G, 3=3G, 4=4G)
    int lac;                    ///< Location Area Code
    int cellId;                 ///< Cell ID
    int  mcc;                   ///< Mobile Country Code
    int mnc;                  ///< Mobile Network Code
 };


//=====[Declaration of public classes]=========================================


/**
 * @class CellularModule
 * @brief High hierarchy Class. Manages cellular module operation including power, connection, and communication.
 * This class handles the cellular module lifecycle, state transitions, sending and
 * receiving messages, and power management. It uses the State design pattern
 * through `ConnectionState` and `TransceiverState` attributes to model dynamic behaviors.
 */
class CellularModule {
public:

//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs the CellularModule object and initializes internal components.
    */
    CellularModule ( );

    /**
     * @brief Destructor. Frees allocated resources.
     */
    virtual ~CellularModule ();

    /**
     * @brief Updates the module power status and manages state changes accordingly.
     * Delegates on PowerManager
     */
    void startStopUpdate ();

    /**
     * @brief Powers off the cellular module.
     * @return true if the operation has finished with success, false if the operation is still in process.
     */
    bool turnOff ();

    /**
     * @brief Powers on the cellular module.
     * @return true if the operation has finished with success, false if the operation is still in process.
     */
    bool turnOn ();

    /**
     * @brief Reboots the cellular module.
     * @return true if the operation has finished with success, false if the operation is still in process.
     * Delegates on PowerManager
     */
    bool reboot ();

    /**
     * @brief Awakens the cellular module from mode sleep.
     * Delegates on PowerManager
     */
    void awake ();

    /**
     * @brief Puts the cellular module into sleep mode.
     * Delegates on PowerManager
     * @return true if the operation has finished with success, false if the operation is still in process.
     */
    bool goToSleep ();

    /**
     * @brief Measures and retrieves the current battery data. Delegates on PowerManager
     * @param currentBatteryData Pointer to a BatteryData struct to fill with the measurement.
     * @return true if the operation has finished with success, false if the operation is still in process.
     */
    bool measureBattery (BatteryData * currentBatteryData);

    /**
     * @brief Attempts to connect the module to the mobile network. Delegates on ConnectionState (State Pattern).
     * @param currentCellInformation Pointer to a structure that will be filled with current cell data.
     * @return Connection status after attempting to connect.
     */
    CellularConnectionStatus_t connectToMobileNetwork (CellInformation * currentCellInformation);
    
    /**
     * @brief Sends and receives messages through the transceiver.
     * Delegates on TrancieverState (State Pattern).
     * @param message Pointer to the message to send.
     * @param socketTargetted Target socket (IP and port).
     * @param receivedMessage Buffer to store the received message from the server response.
     * @param newDataAvailable Flag set to true if a new message is received.
     * @return Transceiver status after the operation.
     */
    CellularTransceiverStatus_t exchangeMessages (char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);

    /**
     * @brief Replaces the current connection state (State pattern).
     * @param newConnectionState Pointer to the new state object.
     */   
    void changeConnectionState  (ConnectionState * newConnectionState);

    bool retrivNeighborCellsInformation ( std::vector<CellInformation*> &neighborsCellInformation
    , int numberOfNeighbors);

    /**
     * @brief Enables the transceiver functionality via its current state.
     */
    void enableTransceiver ();

    /**
     * @brief Enables the connection to the mobile network.
     */
    void enableConnection ();

    /**
     * @brief Replaces the current transceiver state (State pattern).
     * @param newTransceiverState Pointer to the new state object.
     */
    void changeTransceiverState  (TransceiverState * newTransceiverState);


    /**
     * @brief Switches between SIM cards.
     */
    void switchSIMCARD ();

    /**
     * @brief Returns a pointer to the module's PowerManager.
     * @return Pointer to PowerManager instance.
     */
    PowerManager* getPowerManager ();

    /**
     * @brief Returns a pointer to the module's UART interface.
     * @return Pointer to BufferedSerial instance.
     */
    BufferedSerial* getUART ();

    /**
     * @brief Returns a pointer to the module's ATCommandHandler.
     * @return Pointer to ATCommandHandler instance.
     */
    ATCommandHandler* getATHandler ();
private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* refreshTime; //!< Timer used for managing non-blocking delays.
    ATCommandHandler* ATHandler; //!< Handles sending and receiving AT commands.
    ConnectionState* currentConnectionState; //!< Current connection state (State pattern).
    TransceiverState* currentTransceiverState; //!< Current transceiver state (State pattern)
    PowerManager * modulePowerManager; //!< Manages power-related features of the module.
    DigitalOut* simCardSwitchOutput;  //!< Controls SIM card switching through a GPIO pin.
    bool watingForResponse;  //!< Tracks whether the module is waiting for a response.
    powerStatus_t currentPowerStatus; //!< Current power status of the module.


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CELLULAR_MODULE_H_