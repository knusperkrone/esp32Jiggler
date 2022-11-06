
#include "ReconnectBleMouse.h"

#include <Arduino.h>
#include <EEPROM.h>

#include "esp32-hal-log.h"

/**
 * BleMouse contract
 */
ReconnectBleMouse::ReconnectBleMouse(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) : BleMouse(deviceName, deviceManufacturer, batteryLevel) {}

void ReconnectBleMouse::end() {
    pServer = NULL;
    BleMouse::end();
}

/**
 * BLEServerCallbacks contract contract
 */
void ReconnectBleMouse::onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param) {
    BleMouse::connectionStatus->onConnect(pServer);
    memcpy(connectedAddr, param->connect.remote_bda, sizeof(esp_bd_addr_t));
}

void ReconnectBleMouse::onDisconnect(BLEServer *pServer) {
    BleMouse::connectionStatus->onDisconnect(pServer);
    memset(connectedAddr, 0, sizeof(esp_bd_addr_t));
    pServer->getAdvertising()->start();  // Known bug in BLEServer implementation (https://github.com/espressif/arduino-esp32/issues/6016)
}

void ReconnectBleMouse::onMtuChanged(BLEServer *pServer, esp_ble_gatts_cb_param_t *param) {
    BleMouse::connectionStatus->onMtuChanged(pServer, param);
}

/**
 * BleMouse contract
 */
void ReconnectBleMouse::onStarted(BLEServer *pServer) {
    pServer->setCallbacks(this);
    this->pServer = pServer;
}

/**
 * Buisness logic
 */
bool ReconnectBleMouse::reconnect(void) {
    esp_bd_addr_t lastAddr;
    readLastConnectedAddr(lastAddr);

    BLEAddress bleAddress = BLEAddress(lastAddr);
    return pServer->connect(bleAddress);
}

BLEAddress ReconnectBleMouse::getCurrentAddress() {
    return BLEAddress(connectedAddr);  // connectedAddr is  nulled or valid
}

BLEAddress ReconnectBleMouse::getLastConnectedAddress() {
    esp_bd_addr_t lastSaved;
    readLastConnectedAddr(lastSaved);
    return BLEAddress(lastSaved);
}

void ReconnectBleMouse::readLastConnectedAddr(esp_bd_addr_t val) {
    EEPROM.readBytes(0, val, sizeof(esp_bd_addr_t));
}

bool ReconnectBleMouse::persistLastConnectedAddr(esp_bd_addr_t val) {
    EEPROM.writeBytes(0, val, sizeof(esp_bd_addr_t));
    return EEPROM.commit();
}
