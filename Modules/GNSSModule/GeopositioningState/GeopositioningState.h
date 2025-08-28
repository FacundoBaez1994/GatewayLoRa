//=====[#include guards - begin]===============================================

#ifndef _GEOPOSITIONING_STATE_H_
#define _GEOPOSITIONING_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "GNSSModule.h"
#include "GNSSState.h"

//=====[Declaration of public data types]======================================
struct GNSSData; /**< Forward declaration of GNSSData structure. */

//=====[Declaration of public classes]=========================================
/**
 * @class GeopositioningState
 * @brief Abstract interface for GNSS geopositioning states (State pattern).
 */
class  GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Attempts to gather the current GNSS position of the device.
     * This method should be implemented by concrete states to define how GNSS
     * the GNSS module is configured and queried through AT Commands
     * @param Geodata Pointer to a GNSSData structure to be fill with data.
     * @param handler Pointer to the ATCommandHandler used for communication.
     * @param refreshTime Pointer to a NonBlockingDelay for timing control.
     * @return GNSSState_t Result status of the operation.
     */
    virtual GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * handler,
     NonBlockingDelay * refreshTime) ;

    /**
     * @brief Requests activation or enabling of GNSS functionality.
     * Concrete states should implement this method to handle enabling logic
     * appropriate to their context.
    */
    virtual void enableGNSS ();
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _GEOPOSITIONING_STATE_H_