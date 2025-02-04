#include <Arduino.h>

// ******************** BLE Configuration ********************
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define TARGET_ADDRESS "xx:xx:xx:xx:xx:xx" // BLE JDY-31 MAC address
#define RECONNECT_TIMEOUT 10000            // 10-second reconnect timeout

static BLEUUID SERVICE_UUID("FFE0"); // Service UUID (UART)
static BLEUUID CHAR_UUID("FFE1");    // Characteristic UUID (Read/Write)
// ******************** End of BLE Configuration ********************

// ******************** BLE Connection ********************
BLEScan *pBLEScan;
BLEClient *pClient;
bool deviceConnected = false;
unsigned long connectionAttemptTime = 0;

// Connection callbacks
class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pClient)
  {
    deviceConnected = true;
    Serial.println("Connected to target device");
  }

  void onDisconnect(BLEClient *pClient)
  {
    deviceConnected = false;
    Serial.println("Disconnected! Attempting reconnect...");
    connectionAttemptTime = millis(); // Reset reconnect timer
  }
};

// Scan callback to filter target device
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    if (advertisedDevice.getAddress().toString() == TARGET_ADDRESS)
    {
      Serial.println("Target device found!");
      pBLEScan->stop(); // Stop scan once target is found
    }
  }
};

void connectToDevice()
{
  BLEAddress targetAddress(TARGET_ADDRESS);

  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  Serial.print("Connecting to ");
  Serial.println(TARGET_ADDRESS);

  if (pClient->connect(targetAddress))
  {
    deviceConnected = true;
    connectionAttemptTime = 0;
  }
  else
  {
    Serial.println("Connection failed!");
  }
}
// ******************** End of BLE Connection ********************

// ******************** BLE Data Processing ********************
BLERemoteService *pService = nullptr;
BLERemoteCharacteristic *pChar = nullptr;

std::string value;

int sensorData = 24;

void dataProcessing()
{
  if (!pService)
  {
    pService = pClient->getService(SERVICE_UUID); // Access Service
    if (!pService)
    {
      Serial.println("Service not found!");
      return;
    }
  }

  if (!pChar)
  {
    pChar = pService->getCharacteristic(CHAR_UUID); // Access Characteristic
    if (!pChar)
    {
      Serial.println("Characteristic not found!");
      return;
    }
  }

  // Read data
  std::string value = pChar->readValue();
  if (!value.empty()) // Check if data not empty
  {
    // sensorData = value;
    sensorData = atoi(value.c_str());

    Serial.print("Received Data: ");
    Serial.println(value.c_str());
  }
}
// ******************** End of BLE Data Processing ********************

// ******************** Blynk Configuration ********************
#define BLYNK_TEMPLATE_ID     "TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME   "TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN      "AUTH_TOKEN"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials.
char ssid[] = "SSID";
char pass[] = "PASS";

#define BLYNK_PIN V1

BlynkTimer timer;

void myTimerEvent()
{
  // Note: Do not send more that 10 values per second.
  Blynk.virtualWrite(BLYNK_PIN, sensorData);
  Serial.println("Send data: " + sensorData);
}
// ******************** End of Blynk Configuration ********************

void setup()
{
  Serial.begin(115200);

  // ******************** BLE Setup ********************
  BLEDevice::init("ESP32_Gateway");

  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  delay(500);

  // ******************** Blynk Setup ********************
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);

  // Setup a function to be called every 1 seconds
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer

  if (!deviceConnected)
  {
    if (connectionAttemptTime == 0) // First attempt
    {
      Serial.println("Starting device scan...");
      pBLEScan->start(0);        // 0 = indefinite scan (until target found)
      connectionAttemptTime = 1; // Reset for new attempt cycle

      connectToDevice();
    }

    // Reconnect logic with timeout
    if (millis() - connectionAttemptTime > RECONNECT_TIMEOUT)
    {
      Serial.println("\nReconnecting...");

      connectToDevice();
      connectionAttemptTime = millis();

      return;
    }
  }

  dataProcessing();
}
