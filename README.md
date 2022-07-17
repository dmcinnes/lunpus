# Hunt the Lunpus

A hand-held 7-segement display game based on the ATtiny84.

![lunpus](https://user-images.githubusercontent.com/16557/178653913-8a91fd65-4db3-4b30-b797-d2b1e6d01d4c.jpg)

Based on the old [Hunt the Wumpus](https://en.wikipedia.org/wiki/Hunt_the_Wumpus) text-based computer game originally published as BASIC source code in [Creative Computing](https://en.wikipedia.org/wiki/Creative_Computing_(magazine)). The original is based on a dodecahedron maze but I first came across it as a grid-based cave game on the [TI-99/4A](https://en.wikipedia.org/wiki/TI-99/4A) we had as kids.

This handheld version has been renamed Hunt the _Lunpus_ for obvious reasons once you consider the constraints of the display 😉

**[Video of the game in action!](https://youtu.be/uNveLRCgQw0)**

## Rules

You find yourself in a dark cave with the elusive Lunpus! You have your trusty bow and four arrows, let's see if you can defeat the Lunpus before it eats you!

There are dangers in the cave:

* *Superbats* will pick you up and drop you into a random location.
* *Slime Pits* are certain death if you accidently stumble upon one!
* *The Lunpus* may eat you if you bother its sleep.

You can only see walls that are next to you, however the dangers 
* Blinking decimal points will warn you there is a *Buperbat* nearby.
* The sound of wind will warn you there is a *Slime Pit* nearby.
* The sound of snoring will warn you *The Lunpus* is nearby.

You only have 4 arrows, use them wisely! Once you have shot them all you lose. The Lunpus eventally finds and eats you.

## Directions

* Press `ARROW` to start a game.
* Use the direction buttons (`NORTH`, `SOUTH`, `EAST`, `WEST`) to move around the cave. Walls appear as glowing segments.
* Press `ARROW` to show you how many arrows you have left. Press a direction to fire one, otherwise press `ARROW` again to cancel.
* Once you win (or more likely die) press `ARROW` again to start a new game.

## Flashing the ATtiny84

### Requirements

* The latest [Arduino IDE](https://www.arduino.cc/en/software).
* [SevSegShift library](https://github.com/bridystone/SevSegShift) (install this under `Tools > Manage Libraries...` inside of the Arduino IDE).
* [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) to add Arduino support for ATtiny chips (Install this under `Tools > Board: "xxx" > Boards Manager...` of the Arduino IDE).
* [Arduino-Makefile](https://github.com/sudar/Arduino-Makefile) to use the [included Makefile](Makefile) for CLI development.

Once these are installed, wire up your ATtiny with a programmer. I use the [AVR Pocket Programmer](https://www.sparkfun.com/products/9825). If you use something different you will need to change settings in the [Makefile](Makefile).

First set the fuses to get the proper 8 MHz internal clock:
```cli
make set_fuses
```

Once that is successful you can flash the chip:
```cli
make ispload
```

The `ispload` command flashes the program and will also flash the EEPROM which is where all the music, text and "graphics" reside. The 8K flash memory is almost entirely taken up by the program (8146 bytes 99.4% Full) and the EEPROM is alsmot full as well (465 bytes 90.8% Full).

## PCB

The [pcb directory](https://github.com/dmcinnes/lunpus/tree/main/pcb) contains [EasyEDA](https://easyeda.com/) project files that define the schematic and PCB construction. Also in this directory is the latest generation of Gerber files for fabracation.

## Schematic

![Schematic_wumpus_2022-07-12](https://user-images.githubusercontent.com/16557/178660201-9c5f815e-daab-49e6-a2a7-c4c80a61295c.svg)

[![intro screen](https://user-images.githubusercontent.com/16557/178656952-58d82706-e010-420b-a561-4aeffd06e17f.gif)](https://youtu.be/uNveLRCgQw0)

## License

<a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">Lunpus</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="http://dougmcinnes.com" property="cc:attributionName" rel="cc:attributionURL">Doug McInnes</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
