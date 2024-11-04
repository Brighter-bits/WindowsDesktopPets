# Desktop Pets

Yet another way to have a little flying thing whizzing around your screen with C++, not compatible with anything but windows.

A program for having a little sprite bob up and down across the screen and occasionally go up and down.

Note: This project is currently very small, cannot do much and may be very buggy.

# Downloading
## Building from Source

Download/clone the repository, then you want to build the exe with visual studio 2022 or other with the Microsoft Compiler (MSVC)

## Downloading the Exe

Go to releases and then download the latest releases



# Configuration

The program finds its sprites by looking in the paths.txt for more text files which then contain the images


For example, the paths.txt file contains the path to two text files, one of them links to all the images of the sprite moving left to right and the other all the images of the sprite moving up and down.

For the left and right sprites, the first path is the leftmost-looking sprite, while the last path is the rightmost-looking sprite.

For the up and down sprites, the first path is the upmost-looking sprite, while the last path is for the downmost-looking sprite.


If you want a better example, check out the default paths.txt and the JostExample folder.

# Notes

- Largely inspired by THDP
- Can only use PNGs currently (may add support for other file formats later, it shouldn't be too hard)
- Every PNG needs to have the same dimensions or else the program will squish them (default is 64x64 and cannot currently be changed yet)
