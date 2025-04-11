#include <Matter.h>
#include <WiFi.h>
#include <Preferences.h>

// WiFi Credentials
const char *ssid = "xx";      // Set your WiFi SSID
const char *password = "xx";  // Set your WiFi password

// Preferences to store light state
Preferences matterPref;
constexpr const char *onOffPrefKey = "OnOff";

// LED and Button Pins
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#warning "LED_BUILTIN not defined. Defaulting to pin 2."
#endif
const uint8_t ledPin = LED_BUILTIN;

#ifndef BOOT_PIN
#define BOOT_PIN 0
#warning "BOOT_PIN not defined. Defaulting to pin 0."
#endif
const uint8_t buttonPin = BOOT_PIN;

// Debounce and decommissioning
constexpr uint32_t debounceTime = 250;
constexpr uint32_t decommissioningTimeout = 5000;

// Button state
bool buttonPressed = false;
unsigned long buttonTimestamp = 0;

// Matter Light Endpoint
MatterOnOffLight OnOffLight;

// === Light Control Callback ===
bool setLightOnOff(bool state) {
  Serial.print(F("User Callback :: New Light State = "));
  Serial.println(state ? F("ON") : F("OFF"));

  digitalWrite(ledPin, state ? HIGH : LOW);
  matterPref.putBool(onOffPrefKey, state);

  return true;
}

// === Setup WiFi ===
void setupWiFi() {
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(F("."));
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("\nWiFi connection failed. Restarting..."));
    ESP.restart();
  }

  Serial.println(F("\nWiFi connected"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}

// === Handle Button Logic ===
void handleButton() {
  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    buttonTimestamp = millis();
    buttonPressed = true;
  }

  unsigned long pressDuration = millis() - buttonTimestamp;

  if (buttonPressed && pressDuration > debounceTime && digitalRead(buttonPin) == HIGH) {
    buttonPressed = false;
    Serial.println(F("User button released. Toggling Light!"));
    OnOffLight.toggle();
  }

  if (buttonPressed && pressDuration > decommissioningTimeout) {
    Serial.println(F("Decommissioning device..."));
    OnOffLight.setOnOff(false);
    Matter.decommission();
    buttonTimestamp = millis();  // prevent multiple triggers
  }
}

// === Setup Function ===
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  setupWiFi();

  matterPref.begin("MatterPrefs", false);
  bool lastState = matterPref.getBool(onOffPrefKey, true);

  // Sync LED with stored state
  digitalWrite(ledPin, lastState ? HIGH : LOW);

  // Init Matter Light Endpoint
  OnOffLight.begin(lastState);
  OnOffLight.onChange(setLightOnOff);

  // Start Matter Core
  Matter.begin();

  if (Matter.isDeviceCommissioned()) {
    Serial.println(F("Matter Node commissioned and ready."));
    OnOffLight.updateAccessory();
  }
}

// === Loop Function ===
void loop() {
  if (!Matter.isDeviceCommissioned()) {
    Serial.println(F("Waiting for Matter commissioning..."));
    Serial.print(F("Pairing code: "));
    Serial.println(Matter.getManualPairingCode());
    Serial.print(F("QR Code URL: "));
    Serial.println(Matter.getOnboardingQRCodeUrl());

    while (!Matter.isDeviceCommissioned()) {
      delay(1000);
      Serial.println(F("Still waiting..."));
    }

    Serial.println(F("Commissioning complete!"));
    OnOffLight.updateAccessory();
  }

  handleButton();
}
