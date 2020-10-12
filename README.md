# MagicWanduino
 
This is a "magic wand" shape matching game that utilizes gesture recognition, BLE communication, and serial communication.
[Video Demonstation](https://youtu.be/bBsMakcZ1H0)

Boards used:
* [Arduino Nano 33 BLE Sense](https://store.arduino.cc/usa/nano-33-ble-sense-with-headers)
* [ESP32-DevKitC](https://www.mouser.com/ProductDetail/Espressif-Systems/ESP32-DevKitC?qs=chTDxNqvsyn3pn4VyZwnyQ%3D%3D)
* [Arduino Uno](https://store.arduino.cc/usa/arduino-uno-rev3)

Gesture recognition is performed by the Arduino Nano 33 BLE Sense board using a machine learning approach and the TensorFLow Lite framework. Gestures were collected by the board and a model was trained using [this Google Colab notebook](https://colab.research.google.com/github/arduino/ArduinoTensorFlowLiteTutorials/blob/master/GestureToEmoji/arduino_tinyml_workshop.ipynb).

A Bluetooth Low Energy (BLE) connection is established between the Nano and the ESP32-DevKitC board, which serves the role of the BLE central device. The Nano (the peripheral device) classifies gestures using the TensorFlow Lite model and writes these gestures to a BLE characteristic. The ESP32 reads these gestures from the characteristic and transmits them to the Arduino Uno via a wired serial connection. The Uno determines whether the gesture received was the correct matching gesture and writes visual effects to the LED matrix accordingly.

This project was designed by Kyle Reidy and originally submitted as a final project of choice for Computer Science 119, Fundamentals of Embedded Networked Systems, taught by Professor Xiang "Anthony" Chen at UCLA in Spring 2020.
