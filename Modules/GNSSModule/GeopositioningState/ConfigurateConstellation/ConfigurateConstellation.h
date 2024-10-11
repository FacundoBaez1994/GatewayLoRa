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
class GNSSModule; //debido a declaracion adelantada
struct GNSSData;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConfigurateConstellation : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    ConfigurateConstellation ();
    ConfigurateConstellation  (GNSSModule * aGNSSModule);
    virtual ~ConfigurateConstellation   ();
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime) ;
    void enableGNSS ();
private:
    GNSSModule * currentGNSSModule;
    bool readyToSend;

    int numberOfTries;
    int maxTries;

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONFIGURATE_CONSTELLATION_H_