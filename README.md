# MK_II-Code
The repository for all code on the MK II

## How to Build/Compile/Flash/etc.

The easiest way to get everything set up is to run the `setup.sh` script. This will loop through all of the directories and run the correct `cmake` command in an out-of-source build fashion.

```bash
$ ./setup.sh

// OR

$ bash setup.sh
```

Once this step is complete, each folder in the `boards/` directory will contain a subfolder called `build/`. You can `cd` into this `build/` folder and run `make help` in order to see the different options available. The most basic is just running `make` and compiling all of the files, but you can also run `make flash` in order to flash code onto an ATmega.

*Note: You have to be in the `./boards/{BoardName}/build/` directory for the `make` command to function properly. This is due to the fact that we are running an "out-of-source" build, which means we can very easily clean-up any mess by simply deleting the `build/` directory and re-running `setup.sh`.*

### A Quick Example

I enjoy learning by example, so let us run through a scenario where I want to flash code for the `Hackerboard` onto my personal Hackerboard. 

```bash
$ git clone https://github.com/olin-electric-motorsports/MK_II-Code.git
$ cd MK_II-Code/
$ ./setup.sh # If this doesn't work you can also just use `bash setup.sh`
$ cd boards/Hackerboard/build
$ make flash
```

See how easy that is!

## How to add a new Board

Woopie! Look at you go, starting to write your own code *from scratch* for the ATmega on the PCB *you* designed. Have a cookie. Before you go about wrecking havoc on the beautiful build system I painstakingly developed, I'm going to teach you how to make your new board's code work flawlessly with the reset of the code.

We are currently using `cmake`, which is a program that constructs Makefiles based on rules. Each board's folder has a little file in it called `CMakeLists.txt`. This is the file that tells `cmake` how you want it to generate Makefiles.

Simply copy and paste one of the `CMakeLists.txt` files from another folder into your board's folder, and change the name of the board by editing this line:

```cmake
project ({BoardName} C)
```

There you go! Now you can just run `./setup.sh` in the root directory and you are all set!

