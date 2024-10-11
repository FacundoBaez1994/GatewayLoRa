//=====[#include guards - begin]===============================================

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "mbed.h"
#include "GNSSModule.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "CellularModule.h"
#include "LoRa.h"



//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class tracker {
public:
    tracker ();
    virtual ~tracker ();
    void update();
private:
    char* formMessage (GNSSData * GNSSInfo);
    char* formMessage(CellInformation* aCellInfo, std::vector<CellInformation*> 
    &neighborsCellInformation, BatteryData  * batteryStatus); 
    char* formMessage(CellInformation* aCellInfo, GNSSData* GNSSInfo, BatteryData  * batteryStatus);
    CellularModule* cellularTransceiver;
    TcpSocket * socketTargetted;
    CellInformation * currentCellInformation; 
    LoRa * LoRaTransciver;

    GNSSModule* currentGNSSModule;
    GNSSData * currentGNSSdata;
    NonBlockingDelay * latency;
    BatteryData  * batteryStatus;

};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
