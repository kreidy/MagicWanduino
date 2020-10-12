#include "BLEDevice.h"

BLEUUID serviceUUID("67fa1ab7-7b00-4561-a613-6b334e088c12");
BLEUUID shapeCharUUID("729b3b92-833d-4bf5-8db9-baf6da4bed15");

boolean doConnect = false;
volatile boolean isConnected = false;
boolean doScan = false;
BLERemoteCharacteristic* pRemoteShapeCharacteristic;
BLEAdvertisedDevice* myDevice;

long last_read_time = 0;
long read_interval = 333;

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    isConnected = false;
//    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    // Serial.print("Forming a connection to ");
    // Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
//    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remote BLE Server.
    pClient->connect(myDevice);
    // Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      // Serial.print("Failed to find our service UUID: ");
      // Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
     // Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteShapeCharacteristic = pRemoteService->getCharacteristic(shapeCharUUID);
    if (pRemoteShapeCharacteristic == nullptr) {
      // Serial.print("Failed to find our acceleration characteristic UUID: ");
      // Serial.println(shapeCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
     // Serial.println(" - Found our characteristic");

    isConnected = true;
    return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
     // Serial.print("BLE Advertised Device found: ");
     // Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

// Read the value of the characteristics
void read_shape() {
  if(pRemoteShapeCharacteristic->canRead()) {
    std::string shape_str = pRemoteShapeCharacteristic->readValue();
    int readShape = *(int*)(shape_str.data());
    Serial.write(readShape);
  }
}

void setup() {
  Serial.begin(9600);
  BLEDevice::init("cli");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(120, true);
}

void loop() {
  if (doConnect) {
    if (connectToServer()) {
       // Serial.println("We are now connected to the BLE Server.");
    } else {
      // Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  while (isConnected) {
    long current_time = millis();
    
    if (current_time - last_read_time > read_interval) {
      last_read_time = current_time;
      read_shape();
    }
  }
 
  if (doScan) {
    doScan = false;
    delete myDevice;
    
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(120, true);
  }
}
