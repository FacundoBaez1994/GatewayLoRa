//=====[#include guards - begin]===============================================
#ifndef _TURNING_ON_GNSS_H_
#define _TURNING_ON_GNSS_H_

//==================[Libraries]===============================================
#include "GNSSModule.h"
#include "GeopositioningState.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "Non_Blocking_Delay.h"
#include "ObtainingPositionInformation.h"
#include "GNSSState.h"


//=====[Declaration of public data types]======================================
class GNSSModule;  /**< Forward declaration of GNSSModule. */
struct GNSSData;   /**< Forward declaration of GNSSData struct. */

//=====[Declaration of public classes]=========================================
/**
 * @class TurningOnGNSS
 * @brief Concrete state that attempts to turn on the GNSS receiver module
 * in orden to let it enable for operation.
 * It represents the state in
 *
 */
class TurningOnGNSS : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    TurningOnGNSS  ();

    /**
     * @brief Constructs the state with a reference to the GNSS module.
     * @param aGNSSModule Pointer to the associated GNSSModule.
     */
    TurningOnGNSS  (GNSSModule* aGNSSModule);

    /**
     * @brief Destructor.
     */
    virtual ~TurningOnGNSS   ();

    /**
     * @brief Attempts to enable GNSS and transitions to next state on success.
     * @param Geodata Pointer to the GNSSData structure to fill.
     * @param ATHandler Pointer to the AT command handler for communication.
     * @param refreshTime Non-blocking delay used for polling timing.
     * @return GNSSState_t indicating connection or processing status.
    */
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime);
    
    /**
     * @brief This method is intentionally left empty in this state.
     */
    void enableGNSS ();
private:
    GNSSModule* currentGNSSModule; /**< Pointer to the GNSS module context. */
    bool readyToSend;              /**< Indicates if command can be sent. */
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _TURNING_ON_GNSS_H_