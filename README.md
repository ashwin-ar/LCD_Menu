With an aim to implement almost all baisc functions in arduino like LCD,TIMERS,EEPROM MEMORY ACCESS AND SAVE,PROGRESS BAR(in LCD display) this project is made.
Hope this helps some.

A 20x4 LCD is controlled with an Arduino and a keypad.This has 2 different operating modes and 1 menu.
___
With Manual and auto being the two modes of operation,this is used to produce output for a certain period.

*Manual Mode:*
* This provides high signal in a pin for certain period for set no of times when a physical button or keypad button /* is pressed.
![Manualmode snip](https://github.com/ashwin-ar/LCD_Menu/blob/master/Screenshots/Manualmode_snip.jpg)
![Manualmode snip2](https://github.com/ashwin-ar/LCD_Menu/blob/master/Screenshots/Manualmode_snip2.png)

*Auto Mode:*
* This automatically turns on and off a pin for a certain period for set no of times without any button to start.
![Automode snip](https://github.com/ashwin-ar/LCD_Menu/blob/master/Screenshots/Automode_snip.png)

*Edit menu*
* This edit menu is where the times of the repeating operation ,ON time and OFF time can be changed.
* This features options to save in the memory and read from it everytime on start.So that you can retrive your previous personalized values.
![Editmenu snip](https://github.com/ashwin-ar/LCD_Menu/blob/master/Screenshots/Editmenu_snip.png)
---
*Application:*
* Where automation of a certain repeating process is done.
