//=====[#include guards - begin]===============================================
#ifndef _TURNING_OFF_GNSS_H_
#define _TURNING_OFF_GNSS_H_

//==================[Libraries]===============================================
#include "GNSSModule.h"
#include "GeopositioningState.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "Non_Blocking_Delay.h"
#include "GNSSUnavailable.h" 
#include "GNSSState.h"

//=====[Declaration of public data types]======================================
class GNSSModule;   /**< Forward declaration of GNSSModule. */
struct GNSSData;    /**< Forward declaration of GNSSData struct. */

//=====[Declaration of public classes]=========================================
/**
 * @class TurningOffGNSS
 * @brief Concrete state that attempts to disable the GNSS receiver.
 * This class is part of the State design pattern. It represents the state in
 * which the system sends the command to power off or deactivate the GNSS module.
 */
class TurningOffGNSS : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    TurningOffGNSS  ();

    /**
     * @brief Constructs the state with a reference to the GNSS module.
     * @param aGNSSModule Pointer to the associated GNSSModule.
     */
    TurningOffGNSS  (GNSSModule * aGNSSModule);

    /**
     * @brief Destructor.
     */
    virtual ~TurningOffGNSS   ();

    /**
     * @brief Attempts to disable the GNSS module.
     * @param Geodata Pointer to the structure where data would be stored (unused in this state).
     * @param ATHandler Pointer to the AT command handler used for communication.
     * @param refreshTime Pointer to a non-blocking delay for managing command retry timing.
     * @return GNSSState_t indicating disconnection or continued processing.
     */
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime);

    /**
     * @brief This method is intentionally left empty in this state.
    */
    void enableGNSS ();
    
private:
//=====[Declaration of privates atributes]=========================================
    GNSSModule* currentGNSSModule;  /**< Pointer to the GNSS module context. */
    bool readyToSend;               /**< Indicates whether a command can be sent. */

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _TURNING_OFF_GNSS_H_