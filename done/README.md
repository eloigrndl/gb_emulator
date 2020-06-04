===========OUR OWN GAMEBOY EMULATOR===========

AUTHORS:
Eloi Garandel, Erik Wengle

COMPILATION NOTES:
The main file (called gbsimulator) can be compiled by simply typing make in the terminal. It must be noted however, that before it can be done so, one must launch the command "export LD_LIBRARY_PATH=." once per (re-)opened terminal. To then execute the file, one must indicate the ROM file to launch in a first argument, as an example: "./gbsimulator cyberpunk2077.gb"

NOTES ON PROGRESS:
We have followed the assignment as closely as possible and implemented everything that was given as a task to us. The Gameboy emulator behaves just as the real hardware! Some cartridges can be played just fine, whereas others do not; we tried blowing into the cartridge, but since this is an emulator, the game still does not work :-( (We are aware of the fact that there might be a mishap from our part, but sadly we could not find a solution in due time).

NOTES ON SPENT TIME:
We have on average spent nine hours a week for the project. Even though the tasks at the beginning were not that difficult to implemented, we still needed to adapt to our new environment (more specifically, makefiles, terminal-commands, compilation rules...), whereas in the following weeks we struggled due to mistakes that had been made in the past, which only came to light once we put everything together, which made the debugging a cumbersome feat (see our final result...). We mostly worked together, hence the nine hours on average applies to both of us.