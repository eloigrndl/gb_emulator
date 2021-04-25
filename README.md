
# GameBoy Emulator

Second year project @ EPFL ([CS_212 course](https://edu.epfl.ch/coursebook/en/system-programming-project-CS-212)) : GameBoy Emulator

Eloi GARANDEL / Erik Wengle

===========OUR OWN GAMEBOY EMULATOR===========

### Compilation notes:
The main file (called gbsimulator) can be compiled by simply typing make in the terminal. It must be noted however, that before it can be done so, one must launch the command "export LD_LIBRARY_PATH=." once per (re-)opened terminal. To then execute the file, one must indicate the ROM file to launch in a first argument, as an example: "./gbsimulator cyberpunk2077.gb"

### Notes on progress:
We have followed the assignment as closely as possible and implemented everything that was given as a task to us. The Gameboy emulator behaves just as the real hardware! Some cartridges can be played just fine, whereas others do not; we tried blowing into the cartridge, but since this is an emulator, the game still does not work :-( (We are aware of the fact that there might be a mishap from our part, but sadly we could not find a solution in due time).

