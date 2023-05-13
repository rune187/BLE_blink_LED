/**
  Turn the LED on and off with BLE connection. Lights up when 01 is sent after connection, turns off when 00 is sent.
  Note that HIGH and LOW of LED are opposite to arduino in XIAO nrf52840
*/


#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int r_led = LED_BUILTIN; // pin to use for the LED
const int g_led = LEDG;
const int b_led = LEDB;

void setup() {
  Serial.begin(115200);
  // while (!Serial);

  // set LED pin to output mode
  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pinMode(b_led, OUTPUT);

  // initialize leds
  init_leds();

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("XIAO BLE_led_controller");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void init_leds() {
  digitalWrite(r_led, HIGH);
  digitalWrite(g_led, HIGH);
  digitalWrite(b_led, HIGH);
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();
  digitalWrite(b_led, HIGH);
  delay(200);
  digitalWrite(b_led, LOW);
  delay(200);

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    digitalWrite(g_led, LOW);
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
  while (central.connected()) {
        if (switchCharacteristic.written()) {
          if (switchCharacteristic.value()) {
            Serial.println("LED on");
            digitalWrite(r_led, LOW); // changed from HIGH to LOW  
          } else {
            Serial.println(F("LED off"));
            digitalWrite(r_led, HIGH); // changed from LOW to HIGH
          }
        }
      }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    init_leds();
  }
}