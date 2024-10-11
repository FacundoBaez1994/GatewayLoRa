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
struct GNSSData;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class  GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    virtual GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * handler,
     NonBlockingDelay * refreshTime) ;
    virtual void enableGNSS ();
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _GEOPOSITIONING_STATE_H_