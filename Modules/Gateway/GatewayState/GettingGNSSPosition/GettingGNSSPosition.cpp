//=====[Libraries]=============================================================
#include "GettingGNSSPosition.h"
#include "Gateway.h"
#include "Debugger.h" // due to global usbUart
#include "ConnectingToMobileNetwork.h"
#include "FormattingMessage.h"
#include "WaitingForMessage.h"

//=====[Declaration of private defines]========================================
#define LOG_MSG_WAITING_FOR_LORA_MESSAGE   "Waiting For LoRa Message from the Tracker\r\n"
#define LOG_MSG_GNSS_OBTAINED              "GNSS OBTAIN!!!!\r\n"
#define LOG_MSG_GNSS_UNAVAILABLE           "GNSS UNAVAILABLE!!!!\r\n"

//=====[Implementations of public methods]===================================

GettingGNSSPosition::GettingGNSSPosition(Gateway * gateway) {
    this->gateway = gateway;
}

GettingGNSSPosition::~GettingGNSSPosition() {
    this->gateway = nullptr;
}

void GettingGNSSPosition::updatePowerStatus(CellularModule * cellularTransceiver,
                                            BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
}

void GettingGNSSPosition::obtainGNSSPosition(GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    static GNSSState_t GnssCurrentStatus;
    static bool moduleTurningOff = false; 

    // SIN GNSS
    // this->gateway->changeState(new ConnectingToMobileNetwork(this->gateway, GATEWAY_STATUS_GNSS_UNAVAILABLE));
    // return;

    if (moduleTurningOff) {
        if (currentGNSSModule->turnOff()) {
            uartUSB.write(LOG_MSG_WAITING_FOR_LORA_MESSAGE, strlen(LOG_MSG_WAITING_FOR_LORA_MESSAGE));
            this->gateway->changeState(new WaitingForMessage(this->gateway));
            moduleTurningOff = false;
            return;
        }
        return;
    }
    
    currentGNSSModule->enableGNSS();
    GnssCurrentStatus = currentGNSSModule->retrivGeopositioning(currentGNSSdata);

    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_OBTAIN) {
        uartUSB.write(LOG_MSG_GNSS_OBTAINED, strlen(LOG_MSG_GNSS_OBTAINED));
        moduleTurningOff = true;
    }

    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_UNAVAILABLE) {
        uartUSB.write(LOG_MSG_GNSS_UNAVAILABLE, strlen(LOG_MSG_GNSS_UNAVAILABLE));
        moduleTurningOff = true;
    }
}
