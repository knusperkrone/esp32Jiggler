#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

#include "BleMouse.h"

class ReconnectBleMouse : public BleMouse, BLEServerCallbacks {
   public:
    /**
     * BleMouse contract
     */
    ReconnectBleMouse(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel);
    void end();  // parent method is just a stub

    /*
     * Buisness logic
     */
    bool reconnect();
    BLEAddress getCurrentAddress();
    BLEAddress getLastConnectedAddress();

   protected:
    /**
     * BleMouse contract
     */
    virtual void onStarted(BLEServer *pServer);

   private:
    BLEServer *pServer;
    esp_bd_addr_t connectedAddr;

    /**
     *  BLEServerCallbacks contract
     */
    void onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param);
    void onDisconnect(BLEServer *pServer);
    void onMtuChanged(BLEServer *pServer, esp_ble_gatts_cb_param_t *param);

    bool persistLastConnectedAddr(esp_bd_addr_t val);
    void readLastConnectedAddr(esp_bd_addr_t val);
};
