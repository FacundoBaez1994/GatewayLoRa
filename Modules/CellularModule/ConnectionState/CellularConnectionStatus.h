#ifndef _CELLULAR_CONNECTION_STATUS_H_
#define _CELLULAR_CONNECTION_STATUS_H_

typedef enum {
    CELLULAR_CONNECTION_STATUS_UNAVAIBLE,                         //!< Module is unavailable (e.g., off or not responding)
    CELLULAR_CONNECTION_STATUS_DISCONNECTED,                      //!< Module is on but not connected to a network
    CELLULAR_CONNECTION_STATUS_MODULE_DISCONNECTED,               //!< System cannot communicate with the module
    CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT,                 //!< Attempting to establish connection
    CELLULAR_CONNECTION_STATUS_POOR_SIGNAL,                       //!< Signal is too weak to connect
    CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_REGISTER,             //!< Cannot register to a cellular network
    CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_RETRIV_OPERATORS_INFO, //!< Failed to retrieve operator information
    CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_RETRIV_DATETIME,      //!< Failed to retrieve date/time from network
    CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_ATTACH_TO_PACKET_SERVICE, //!< Cannot attach to packet-switched data service
    CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_SET_PDP_CONTEXT,      //!< PDP context configuration failed
    CELLULAR_CONNECTION_STATUS_INVALID_SIM,                       //!< SIM card is missing, locked or invalid
    CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK,             //!< Successfully connected to the network
    CELLULAR_CONNECTION_STATUS_ERROR_NULL_POINTER               //!< The method has detected an nullptr pass as param
} CellularConnectionStatus_t;

#endif // _CELLULAR_CONNECTION_STATUS_H_
