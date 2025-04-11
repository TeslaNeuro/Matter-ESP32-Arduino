#include <Matter.h>
#include <WiFi.h>
#include <Preferences.h>

// Wi-Fi settings
const char *ssid = "xx";         // Change this to your WiFi SSID
const char *password = "xx";   // Change this to your WiFi password

// Pins
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
const uint8_t ledPin = LED_BUILTIN;

#ifndef BOOT_PIN
#define BOOT_PIN 0
#endif
const uint8_t buttonPin = BOOT_PIN;

// Matter Endpoint
MatterOnOffLight OnOffLight;
Preferences matterPref;
constexpr const char *onOffPrefKey = "OnOff";

// Button debouncing
bool buttonPressed = false;
unsigned long buttonTimestamp = 0;
const unsigned long debounceTime = 250; // ms
const unsigned long decommissioningTime = 5000; // ms

// Wi-Fi connection handling
bool wifiConnected = false;
unsigned long wifiStartAttempt = 0;
const unsigned long wifiTimeout = 15000;  // ms

// Matter commissioning
bool matterWasCommissioned = false;
unsigned long commissioningPrintTime = 0;
const unsigned long commissioningInterval = 5000;  // ms
unsigned long ledBlinkTime = 0;
const unsigned long ledBlinkInterval = 500;  // ms

// Wi-Fi non-blocking connection
void connectWiFiNonBlocking() {
  if (!wifiConnected) {
    if (WiFi.status() != WL_CONNECTED) {
      if (millis() - wifiStartAttempt > wifiTimeout) {
        Serial.println(F("Wi-Fi connection timeout. Restarting..."));
        ESP.restart();
      }
    } else {
      wifiConnected = true;
      Serial.println(F("\nWi-Fi connected"));
      Serial.print(F("IP address: "));
      Serial.println(WiFi.localIP());
    }
  }
}

// Initialize Matter protocol
void initMatter() {
  matterPref.begin("MatterPrefs", false);
  bool lastState = matterPref.getBool(onOffPrefKey, true);
  digitalWrite(ledPin, lastState ? HIGH : LOW);
  OnOffLight.begin(lastState);
  OnOffLight.onChange(setLightOnOff);  // Set the user callback function

  Matter.begin();
  matterWasCommissioned = Matter.isDeviceCommissioned();
  if (matterWasCommissioned) {
    Serial.println(F("Matter node is commissioned and ready."));
    OnOffLight.updateAccessory();
  }
}

// Commissioning status check (non-blocking)
void checkCommissioningStatus() {
  if (!Matter.isDeviceCommissioned()) {
    if (millis() - commissioningPrintTime > commissioningInterval) {
      commissioningPrintTime = millis();
      Serial.println(F("Waiting for Matter commissioning..."));
      Serial.print(F("Pairing code: "));
      Serial.println(Matter.getManualPairingCode());
      Serial.print(F("QR Code URL: "));
      Serial.println(Matter.getOnboardingQRCodeUrl());
    }

    // Blink the LED to indicate commissioning is in progress
    if (millis() - ledBlinkTime > ledBlinkInterval) {
      ledBlinkTime = millis();
      digitalWrite(ledPin, !digitalRead(ledPin));  // Toggle LED state
    }

  } else if (!matterWasCommissioned) {
    Serial.println(F("Commissioning complete!"));
    OnOffLight.updateAccessory();
    matterWasCommissioned = true;
  }
}

// User callback function for light state changes
bool setLightOnOff(bool state) {
  Serial.printf("User Callback :: New Light State = %s\r\n", state ? "ON" : "OFF");
  digitalWrite(ledPin, state ? HIGH : LOW);
  matterPref.putBool(onOffPrefKey, state);
  return true;  // Return success to Matter core
}

// Handle button state non-blocking (debounced)
void handleButtonNonBlocking() {
  bool isPressed = digitalRead(buttonPin) == LOW;
  unsigned long currentTime = millis();

  if (isPressed && !buttonPressed) {
    buttonPressed = true;
    buttonTimestamp = currentTime;
  }

  if (buttonPressed) {
    unsigned long pressDuration = currentTime - buttonTimestamp;

    if (!isPressed && pressDuration > debounceTime) {
      buttonPressed = false;
      Serial.println(F("Button released: toggling light."));
      OnOffLight.toggle();
    }

    if (isPressed && pressDuration > decommissioningTime) {
      Serial.println(F("Button held: decommissioning device."));
      OnOffLight.setOnOff(false);
      Matter.decommission();
      buttonTimestamp = currentTime; // Avoid multiple triggers
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.print(F("Connecting to Wi-Fi: "));
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  wifiStartAttempt = millis();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartAttempt < wifiTimeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to Wi-Fi.");
    ESP.restart();  // Reset if not connected
  } else {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  initMatter();
}

void loop() {
  connectWiFiNonBlocking(); // Check Wi-Fi status non-blocking
  checkCommissioningStatus(); // Monitor and report commissioning status
  handleButtonNonBlocking(); // Handle button input non-blocking
}
