//=====[#include guards - begin]===============================================
#ifndef _GNSS_UNAVAILABLE_H_
#define _GNSS_UNAVAILABLE_H_

//==================[Libraries]===============================================
#include "GNSSModule.h"
#include "GeopositioningState.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "Non_Blocking_Delay.h"
#include "ConfigurateConstellation.h"
#include "GNSSState.h"

//=====[Declaration of public data types]======================================
class GNSSModule;  /**< Forward declaration of GNSSModule. */
struct GNSSData;   /**< Forward declaration of GNSSData struct. */

//=====[Declaration of public classes]=========================================
/**
 * @class GNSSUnavailable
 * @brief Represents the GNSS module being disconnected or not ready.
 * @details This state blocks any attempts to retrieve positioning data. Calling
 * `enableGNSS()` transitions the module to a configuration state.
 * It is part of the State design pattern used for GNSS mode handling.
 */
class GNSSUnavailable : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    GNSSUnavailable ();

    /**
     * @brief Constructor that links this state with the GNSS module.
     * @param aGNSSModule Pointer to the GNSS module that owns this state.
     */
    GNSSUnavailable  (GNSSModule * aGNSSModule);

    /**
     * @brief Destructor.
     */
    virtual ~GNSSUnavailable   ();

    /**
     * @brief Rejects any attempt to retrieve GNSS data.
     * @details Since the GNSS module is unavailable in this state,
     * the method simply returns a disconnected status.
     * @param Geodata Pointer to store GNSS data (not used).
     * @param ATHandler Pointer to AT command handler (not used).
     * @param refreshTime Pointer to delay handler (not used).
     * @return GNSSState_t Always returns GNSS_STATE_DISCONNECTED.
     */
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime);


    /**
     * @brief Initiates the GNSS enabling process.
     * @details Transitions the GNSS module to the ConfigurateConstellation state
     * to begin GNSS setup.
     */
    void enableGNSS ();

private:
//=====[Declaration of privates atributes]=========================================
    GNSSModule* currentGNSSModule;      /**< Reference to the owning GNSS module. */
    GNSSState_t* disconnectionState;    /**< Reserved for future disconnection handling (not used). */
    bool readyToSend;                   /**< Tracks if the state is ready to process (not used). */

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _GNSS_UNAVAILABLE_H_