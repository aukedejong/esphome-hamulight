# esphome-hamulight
ESPHome Light implementation to switch and dim Hamulight L2046 LED spots

This software runs on a ESP32 with RF433 sender connected to an output port.

An extra feature I added: When the light is turned off, it first dims it to the lowest luminosity before turning off.
When turning back on, it increased the luminosity to previous level.  
Also, changing dim level is not instant but gradually.

## The messages

### Turning on and off message

The Hamulight remote uses one fixed signal to toggle the light. It's sends an 148 symbol
signal using OOK modulation. The symbol period is 200 micrseconds.  
So switching the output pin on and off with the correct timing recreates the remotes signal.

Toggle message:
> 01010111111000000100010001110111010001110100011101000100010001110111010001110111011101000111010001000100010001000100011101000100010001110100011101010

### Dim message

The remotes usss a circlular touch button to dim the light.
The dim signal is build up of 5 parts.

> [prefix] [dim data] [glue] [prefix] [dim data]


Pefix part:

> 010101111110000001000100011101110100011101000111010001000100011101110100011101110100

Glue part:

> 0101

Dim data:

This part of the message can be generated.
The dim value is a 15 bit number. Every bit in this number is converter, where 1 becomes 0111 and 0 becomes 0100.

> 1 = 0111  
> 0 = 0100

So thie 15 bit number becomes a 60 bit message part.

The ranges of this number is al little strange.

Max luminosity is 24357.  
This number can be increased to 32767 to dim the light.  
To further dim the light the number is 256 or higher till 20479, which is the most dimmed.

> Dimmed light - <- - >- - - Full light  
> 20479  to  256  -  32767  to  24357


