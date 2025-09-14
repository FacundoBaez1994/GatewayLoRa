//=====[#include guards - begin]===============================================
#ifndef _GNSS_MODULE_H_
#define _GNSS_MODULE_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "GeopositioningState.h"
#include "PowerManager.h"
#include "GNSSUnavailable.h"
#include "GNSSState.h"

//=====[Declaration of public data types]======================================

/**
 * @struct GNSSData
 * @brief Structure representing GNSS positional and status data.
 */
struct GNSSData {
    char timestamp[20];     /**< UTC time and date in format HHMMSS.SS/DDMMYY */
    float latitude;         /**< Latitude in decimal degrees */
    float longitude;        /**< Longitude in decimal degrees */
    float hdop;             /**< Horizontal dilution of precision */
    float altitude;         /**< Altitude in meters */
    int fix;                /**< GNSS fix status (0 = no fix, 1 = fix, etc.) */
    float cog;              /**< Course over ground in degrees */
    float spkm;             /**< Speed over ground in km/h */
    float spkn;             /**< Speed over ground in knots */
    int nsat;               /**< Number of satellites in use */
};


//=====[Declaration of public classes]=========================================
/**
 * @class GNSSModule
 * @brief Manages GNSS module behavior using a state machine architecture.
 * Manages the position acquisition through a configurable
 * state-based system. It communicates with the module via AT commands.
 */
class GNSSModule {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs the GNSS module handler.
     * @param aPowerManager Pointer to a PowerManager instance.
     * @param ATHandler Pointer to an ATCommandHandler for module communication.
     */
    GNSSModule (PowerManager* aPowerManager, ATCommandHandler* ATHandler);

    /**
     * @brief Destructor. Releases internal resources.
     */
    virtual ~GNSSModule ();

    /**
     * @brief Toggles GNSS power state and updates internal state accordingly.
     */
    void startStopUpdate ();

    /**
     * @brief Performs a hardware or software reboot of the GNSS module.
     */
    void reboot ();

    /**
     * @brief Attempts to retrieve GNSS position data via the current state.
     * @param Geodata Pointer to GNSSData structure to store received values.
     * @return GNSSState_t Result status of the operation.
     */
    GNSSState_t retrivGeopositioning (GNSSData * Geodata);
    
    /**
     * @brief Replaces the current geopositioning state with a new one.
     * @param newGeopositioningState Pointer to the new state to activate.
     */
    void changeGeopositioningState  (GeopositioningState* newGeopositioningState);

    /**
     * @brief Initiates GNSS functionality based on current state logic.
     */
    void enableGNSS ();

    /**
     * @brief Returns a pointer to the PowerManager used by this module.
     * @return Pointer to PowerManager instance.
     */
    PowerManager* getPowerManager ();

    /**
     * @brief Returns a pointer to the BufferedSerial object used for communication.
     * @return Pointer to BufferedSerial instance.
     */
    BufferedSerial* getUART ();
    
    /**
     * @brief Powers off the GNSS module.
     * @return true if the operation has finished with success, false if the operation is still in process.
     */
    bool turnOff ();

    /**
     * @brief Powers on the GNSS module.
     * @return true if the operation has finished with success, false if the operation is still in process.
     */
    bool turnOn ();

private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* refreshTime;                    /**< Non-blocking delay for periodic operations */
    ATCommandHandler* ATHandler;                      /**< AT command interface */
    PowerManager* modulePowerManager;                 /**< GNSS power control */
    powerStatus_t currentPowerStatus;                 /**< Current power state */
    GeopositioningState* currentGeopositioningState;  /**< Current behavior state */

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GNSS_MODULE_H_