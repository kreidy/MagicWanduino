/*
  IMU Classifier and Bluetooth Transmitter

  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU, once enough samples are read, it then uses a
  TensorFlow Lite (Micro) model to try to classify the movement as a known gesture.

  Most simply put, once classified, the gesture is transmitted (as an integer) to
  a Bluetooth receiving board. In BLE terms, this board is the peripheral device,
  which writes the classified gestures to a BLE characteristic. A central device
  connects to this peripheral and reads the gestures from that characteristic.

  The circuit:
  - Arduino Nano 33 BLE Sense board.

  Created by Kyle Reidy
  Based on code from Don Coleman, Dominic Pajak, Sandeep Mistry
  
*/

#include <Arduino_LSM9DS1.h>

#include <ArduinoBLE.h>

#include <TensorFlowLite.h>
#include <tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h>
#include <tensorflow/lite/experimental/micro/micro_error_reporter.h>
#include <tensorflow/lite/experimental/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "model.h"

BLEService* myService = nullptr;
// shapeCharacteristic will hold 3 for triangle, 4 for square, 5 for star, 0 otherwise
BLEIntCharacteristic* shapeCharacteristic = nullptr;

const float accelerationThreshold = 3.5; // threshold of significant in G's
const int numSamples = 30;
const int sampleInterval = 50;

long lastWriteTime = 0;
const int leaveShapeDuration = 1500;

long lastSampleTime = 0;
int samplesRead = numSamples;

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::ops::micro::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize];

// array to map gesture index to a name
const char* GESTURES[] = {
  "square",
  "triangle",
  "star"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

  // initialize BLE
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  myService = new BLEService("67fa1ab7-7b00-4561-a613-6b334e088c12");
  shapeCharacteristic = new BLEIntCharacteristic("729b3b92-833d-4bf5-8db9-baf6da4bed15",
    BLERead );

  BLE.setLocalName("sen");
  BLE.setAdvertisedService(*myService);
  myService->addCharacteristic(*shapeCharacteristic);
  BLE.addService(*myService);
  shapeCharacteristic->writeValue(1);

  BLE.advertise();
  Serial.println("Finished setup");
}

void loop() {
  BLEDevice centralDevice = BLE.central();

  if (centralDevice) {
    Serial.println("Connected!");
    digitalWrite(LED_BUILTIN, HIGH);

    while (centralDevice.connected()) {

      float aX, aY, aZ;

      long currentTime = millis();
      if (currentTime - lastWriteTime > leaveShapeDuration) {
        shapeCharacteristic->writeValue(1);
        lastWriteTime = currentTime;
      }

      // wait for significant motion
      if (samplesRead == numSamples) {
        if (IMU.accelerationAvailable()) {
          // read the acceleration data
          IMU.readAcceleration(aX, aY, aZ);
    
          // sum up the absolutes
          float aSum = fabs(aX) + fabs(aY) + fabs(aZ);
    
          // check if it's above the threshold
          if (aSum >= accelerationThreshold) {
            // reset the sample read count
            samplesRead = 0;
          }
        }
      }
    
      // check if the all the required samples have been read since
      // the last time the significant motion was detected
      if (samplesRead < numSamples) {
        currentTime = millis();
        
        // check if new acceleration AND gyroscope data is available
        if (IMU.accelerationAvailable() && (currentTime - lastSampleTime > sampleInterval)) {
          lastSampleTime = currentTime;
          lastWriteTime = currentTime;
          
          // read the acceleration and gyroscope data
          IMU.readAcceleration(aX, aY, aZ);
    
          // normalize the IMU data between 0 to 1 and store in the model's
          // input tensor
          tflInputTensor->data.f[samplesRead * 3 + 0] = (aX + 4.0) / 8.0;
          tflInputTensor->data.f[samplesRead * 3 + 1] = (aY + 4.0) / 8.0;
          tflInputTensor->data.f[samplesRead * 3 + 2] = (aZ + 4.0) / 8.0;
    
          samplesRead++;
    
          if (samplesRead == numSamples) {
            // Run inferencing
            TfLiteStatus invokeStatus = tflInterpreter->Invoke();
            if (invokeStatus != kTfLiteOk) {
              Serial.println("Invoke failed!");
              while (1);
              return;
            }
    
            // Loop through the output tensor values from the model
            for (int i = 0; i < NUM_GESTURES; i++) {
              Serial.print(GESTURES[i]);
              Serial.print(": ");
              Serial.println(tflOutputTensor->data.f[i], 6);
              if (tflOutputTensor->data.f[i] > 0.9) {
                shapeCharacteristic->writeValue(indexToShape(i));
              }
            }
            Serial.println();
          }
        }
      }
    }

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Disconnected");
  }
  
  
}

int indexToShape(int index) {
  if (index == 0) {
    return 4;
  }
  if (index == 1) {
    return 3;
  }
  return 5;
}
