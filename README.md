# AI_color_detector

### xChips required:

* CS11
* SH01
* SL06
* OD01

### Additional components required
* SD card
* SD card reader

### Usage:
* Connect all the xChips
* Insert SD card to the CS11
* Copy and pasted "colors.txt" to the SD card main directory
* Flash the Arduino code
* Instructions are displayed on OD01
* Train CS11 against each color by showing real color to SL06, you may end the training at any time.
* After training, test with different shades of any color, the OD01 will display the approximate color name.

This code is based on AI kNN classification algorithm, whith k being 1. 
