# SmartSign
A battery powered e-Paper sign for conference rooms.

![](docs/smartsign_rendering.jpg)

## Components
* DFRobot FireBeetle ESP32 IOT Microcontroller
* Waveshare 7.5inch e-Paper B V1 (640x384)
* 3000 mAh LiPo Battery
* RFID RC522 Reader
* Mechanical endstop to wake ESP when card is inserted
* 3mm red LED with 47µF Capacitor and N-Channel FET to keep LED state when ESP is in deep sleep
* 2x tactile switch buttons (12x12x7.3mm)
* some high ohm resistors for the LED and pull down of button pins
