# UnblockMe
Automatically solves Unblock Me puzzles

<img src="https://github.com/NivenT/UnblockMe/blob/master/screenshots/img0.png" alt="Screenshot" width="400" height="400"/>

This program loads an Unblock Me puzzle from a file (text or image), and then provides a step-by-step solution of the puzzle.

## Details
* Puzzles can be loaded in from images or text files (look at the sample .puz files for examples)
  * The image loading is very crude, and not guarenteed to give the correct results
  * At the very least, it works on all the images the repo comes with
* Currently, to change which puzzle is used, you have to manually change the code

## Requirements
* [CImg](http://cimg.eu/reference/structcimg__library_1_1CImg.html)
