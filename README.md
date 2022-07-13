# Hunt the Lunpus

A hand-held 7-segement display game based on the ATtiny84.

![lunpus](https://user-images.githubusercontent.com/16557/178653913-8a91fd65-4db3-4b30-b797-d2b1e6d01d4c.jpg)

Based on the old [Hunt the Wumpus](https://en.wikipedia.org/wiki/Hunt_the_Wumpus) text-based computer game originally published as BASIC source code in [Creative Computing](https://en.wikipedia.org/wiki/Creative_Computing_(magazine)). The original is based on a dodecahedron maze but I first came across it as a grid-based cave game on the [TI-99/4A](https://en.wikipedia.org/wiki/TI-99/4A) we had as kids.

This handheld version has been renamed Hunt the _Lunpus_ for obvious reasons once you consider the constraints of the display 😉

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


![intro screen](https://user-images.githubusercontent.com/16557/178656952-58d82706-e010-420b-a561-4aeffd06e17f.gif)
