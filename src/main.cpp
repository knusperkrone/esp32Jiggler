#include <Arduino.h>
#include <BLEDevice.h>
#include <EEPROM.h>

#include "dev/ReconnectBleMouse.h"

ReconnectBleMouse bleMouse("MX Master 2S", "Logitech", random(80, 99));
uint32_t notConnectedCount = 0;

void setup() {
    Serial.begin(9600);
    Serial.println();

    Serial.print("Starting BLE ");
    Serial.flush();
    bleMouse.begin();
    Serial.println(" done");

    if (!EEPROM.begin(1024)) {
        Serial.println("Failed to init EEPROM");
        delay(1000);
    }
}

void loop() {
    long sleep_ms = 500;

    if (bleMouse.isConnected()) {
        notConnectedCount = 0;

        int delta_x = random(-5, 5);
        int delta_y = random(-5, 5);
        bleMouse.move(delta_x, delta_y);

        Serial.print("Moved mouse on ");
        Serial.print(bleMouse.getCurrentAddress().toString().c_str());
        Serial.println();

        sleep_ms = random(500, 1000);
    } else {
        notConnectedCount++;

        if (notConnectedCount % 1000 == 0) {
            Serial.println("Resetting bluetooth stack");
            bleMouse.end();
            bleMouse.begin();
        } else if (notConnectedCount == 0 || notConnectedCount % 100 == 0) {
            bool isReconnected = bleMouse.reconnect();

            Serial.print("Reconnection on: ");
            Serial.print(bleMouse.getLastConnectedAddress().toString().c_str());
            Serial.print(" => ");
            Serial.print(isReconnected ? "true" : "false");
            Serial.println();
        }
    }

    delay(sleep_ms);
}
