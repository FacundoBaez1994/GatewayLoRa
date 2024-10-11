//=====[#include guards - begin]===============================================

#ifndef _OBTAINING_POSITION_INFORMATION_H_
#define _OBTAINING_POSITION_INFORMATION_H_

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
class ObtainingPositionInformation : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    ObtainingPositionInformation ();
    ObtainingPositionInformation  (GNSSModule * aGNSSModule);
    virtual ~ObtainingPositionInformation   ();
    GNSSState_t retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime) ;
    void enableGNSS ();
private:
    bool retrivPositionInfo (char *response);
    GNSSModule * currentGNSSModule;
    bool readyToSend;
    char utc[10];          // <UTC> in HHMMSS.SS format
    float latitude;        // <latitude> in decimal format
    float longitude;       // <longitude> in decimal format
    float hdop;            // <hdop> Horizontal precision: 0.5-99.9
    float altitude;        // <altitude> The altitude of the antenna away from the sea level (unit:meters)
    int fix;               // <fix> GNSS mode 2 = 2D and 3 = 3D
    float cog;         // <cog> course over ground base over true north Format ddd:mm (unit: degrees:minutes)
    float spkm;           // <spkm> speed in km/h
    float spkn;            // <spkn> speed in knots
    char date[7];  // <date> in format DDMMYY
    int nsat;         // <nsat> number of satelites

    int numberOfTries;
    int maxTries;

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _OBTAINING_POSITION_INFORMATION_H_