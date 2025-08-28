//=====[#include guards - begin]===============================================
#ifndef _CONFIGURATE_CONSTELLATION_H_
#define _CONFIGURATE_CONSTELLATION_H_

//==================[Libraries]===============================================
#include "GNSSModule.h"
#include "GeopositioningState.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "Non_Blocking_Delay.h"
#include "TurningOnGNSS.h"
#include "GNSSState.h"

//=====[Declaration of public data types]======================================
class GNSSModule;  //!< Forward declaration of GNSSModule
struct GNSSData;   //!< Forward declaration of GNSSData

//=====[Declaration of public classes]=========================================
/**
 * @class ConfigurateConstellation
 * @brief Represents the GNSS configuration state for selecting satellite constellations.
 * @details This state sets up the GNSS module to use a specific satellite constellation 
 * (e.g., GPS + Beidou). It is part of the State design pattern used to manage 
 * GNSS state transitions.
 */
class ConfigurateConstellation : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    ConfigurateConstellation ();

    /**
     * @brief Parameterized constructor.
     * @param aGNSSModule Pointer to the GNSS module using this state.
     */
    ConfigurateConstellation  (GNSSModule * aGNSSModule);


    virtual ~ConfigurateConstellation   ();

    /**
     * @brief Handles the logic to configure GNSS constellation.
     * @param Geodata Pointer to the structure where GNSS data would be stored (unused in this state).
     * @param ATHandler Pointer to the AT command handler.
     * @param refreshTime Pointer to a non-blocking delay for pacing command attempts.
     * @return GNSSState_t Result of the operation indicating current state.
     */
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime);

    /**
     * @brief Triggers state transition logic (no-op in this state).
     */
    void enableGNSS ();
private:
//=====[Declaration of privates atributes]=========================================
    GNSSModule* currentGNSSModule;  //!< Pointer to the associated GNSS module.
    bool readyToSend;               //!< Indicates if the state is ready to send a command.
    int numberOfTries;             //!< Current number of retries attempted.
    int maxTries;                  //!< Maximum allowed retry attempts.

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONFIGURATE_CONSTELLATION_H_