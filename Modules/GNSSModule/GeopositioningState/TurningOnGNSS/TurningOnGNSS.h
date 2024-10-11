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
class GNSSModule; //debido a declaracion adelantada
struct GNSSData;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class TurningOnGNSS : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    TurningOnGNSS  ();
    TurningOnGNSS  (GNSSModule * aGNSSModule);
    virtual ~TurningOnGNSS   ();
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime);
    void enableGNSS ();
private:
    GNSSModule * currentGNSSModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _TURNING_ON_GNSS_H_