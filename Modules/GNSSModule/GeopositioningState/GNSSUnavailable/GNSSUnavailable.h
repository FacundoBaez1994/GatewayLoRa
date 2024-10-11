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
class GNSSModule; //debido a declaracion adelantada
struct GNSSData;


//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class GNSSUnavailable : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    GNSSUnavailable ();
    GNSSUnavailable  (GNSSModule * aGNSSModule);
    virtual ~GNSSUnavailable   ();
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime);
    void enableGNSS ();
private:
    GNSSModule * currentGNSSModule;
    GNSSState_t * disconnectionState;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _GNSS_UNAVAILABLE_H_