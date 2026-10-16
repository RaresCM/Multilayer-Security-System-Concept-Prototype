## Concepts learnt

This project has taught me:
* How to scan, detect and translate a matrix keypad input to a number or letter
* How a RFID reader works and how to read and use the detected UID from a card or fob 
* How a sound sensor works and is tuned for specific locations and uses using its own hardware adjustable resistor
* The operation of a PIR sensor and using its own tunning resistors to set desired times and detection distances according to its datasheet
* The use of ISR routines to process an detection inputs even if a piece of code is currently executing so nothing gets missed
* Use of a analog tilt ball sensor to detect any tampering with the device for when its inside an enclosure
* Debouncing logic for a keypad being required as well as pressed key detection

## Issues Encountered

* The RFID logic works well but doesn't have any advanced security like any spoofing or brute force protection
* The PIR sensor under performs in terms of distance compared to what the datasheet specifies due to being cheap
* The sound sensor sometimes doesn't detect medium to low sounds 
