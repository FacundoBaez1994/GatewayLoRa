//=====[#include guards - begin]===============================================
#ifndef _OBTAINING_POSITION_INFORMATION_H_
#define _OBTAINING_POSITION_INFORMATION_H_

//==================[Libraries]================================================
#include "GNSSModule.h"
#include "GeopositioningState.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "Non_Blocking_Delay.h"
#include "TurningOnGNSS.h"
#include "GNSSState.h"
#include "timeStampTools.h"

//=====[Declaration of private defines]========================================
#define UTC_LEN  10
#define DATE_LEN  7

//=====[Declaration of public data types]======================================
class GNSSModule;   /**< Forward declaration of GNSSModule. */
struct GNSSData;    /**< Forward declaration of GNSSData struct. */

//=====[Declaration of public classes]=========================================
/**
 * @class ObtainingPositionInformation
 * @brief State that requests and parses GNSS location data from the module.
 * This class is part of the State design pattern and manages the logic
 * for sending AT commands to retrieve geolocation data (NMEA format)
 * and updating the GNSSData structure.
 */
class ObtainingPositionInformation : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    ObtainingPositionInformation ();

    /**
     * @brief Constructor with reference to the GNSS module.
     * @param aGNSSModule Pointer to the GNSS module using this state.
     */
    ObtainingPositionInformation  (GNSSModule* aGNSSModule);

    /**
     * @brief Destructor.
    */
    virtual ~ObtainingPositionInformation   ();

    /**
     * @brief Retrieves location data from the GNSS module.
     * Sends AT commands to request GNSS information, parses it, and fills in
     * the provided GNSSData structure if successful.
     * @param Geodata Pointer to a structure where parsed GNSS data will be stored.
     * @param ATHandler Pointer to the handler responsible for sending and reading AT commands.
     * @param refreshTime Pointer to a non-blocking delay used to pace retries.
     * @return GNSSState_t indicating the status of the operation.
    */
    GNSSState_t retrivGeopositioning (GNSSData* Geodata, ATCommandHandler* ATHandler,
     NonBlockingDelay* refreshTime);

    /**
     * @brief This method has no effect in this state.
    */
    void enableGNSS ();
private:
//=====[Declaration of privates methods]==========================================
    /**
     * @brief Parses a GNSS response string and stores the result in internal attributes.
     * @param response Pointer to the AT command response string to parse.
     * @return true if the string was parsed successfully, false otherwise.
     */
    bool retrivPositionInfo (char *response);

//=====[Declaration of privates atributes]=========================================
    GNSSModule* currentGNSSModule; /**< Reference to the owning GNSS module. */
    bool readyToSend;              /**< Indicates whether a command can be sent. */
    char utc[UTC_LEN];             /**< UTC time in HHMMSS format. */
    float latitude;                /**< Latitude in decimal degrees. */
    float longitude;               /**< Longitude in decimal degrees. */
    float hdop;                    /**< Horizontal dilution of precision. */
    float altitude;                /**< Altitude in meters. */
    int fix;                       /**< Fix type: 2D or 3D. */
    float cog;                     /**< Course over ground. */
    float spkm;                    /**< Speed in km/h. */
    float spkn;                    /**< Speed in knots. */
    char date[DATE_LEN ];          /**< Date in DDMMYY format. */
    int nsat;                      /**< Number of satellites used. */
    int numberOfTries;             /**< Current retry count. */
    int maxTries;                  /**< Maximum number of allowed retries. */
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _OBTAINING_POSITION_INFORMATION_H_