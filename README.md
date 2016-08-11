# Simple_SD_Init

Was looking through stuff from undergrad and found this implementation of SD card using 
SPI on the MSP430. It's not exactly the most robust code but I remember it taking a very 
long time to implement so here is the code for anyone who's interested. 

It's based off this [application note](http://alumni.cs.ucr.edu/~amitra/sdcard/Additional/sdcard_appnote_foust.pdf) 
by F. Foust and adapted to work with the MSP430F5529.

## Notes:

- Biggest differences from Foust's code is in sd.h/sd.c
- UART code is included because the original use case also needed UART communication
- Make sure to change pinout to match the MSP430 model you are using