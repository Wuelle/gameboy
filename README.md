# Gameboy C
This is my gameboy emulator, written in C.
It uses `cmocka` for unit testing.

## Usage
To run the emulator, you will need two things:

* The boot ROM for the gameboy
* The game ROM for the game you want to run
For legal reasons, i cant provide either.

Execute
```
make gb
./gameboy
```
## License
This project is licensed under either of
* Apache License, Version 2.0, ([LICENSE-APACHE](LICENSE-APACHE) or
  http://www.apache.org/licenses/LICENSE-2.0)
* MIT license ([LICENSE-MIT](LICENSE-MIT) or
  http://opensource.org/licenses/MIT)
at your option.

## Sources
* Overall Great site: https://gbdev.io/pandocs/
* Gameboy sprite format: https://www.huderlem.com/demos/gameboy2bpp.html
* How the VRAM is structured: https://www.copetti.org/writings/consoles/game-boy/#graphics
* Amazing Blog: http://www.codeslinger.co.uk/pages/projects/gameboy
