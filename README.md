# pestacle

A node-based environment to setup real-time multimedia installations. Nodes are
provided with plugins. The following plugins are available

1. `png` : load PNG pictures
2. `ffmpeg` : stream any video files supported by FFMPEG
3. `arducam` : stream video from Arducam ToF camera
4. `matrix-io` : export to NPY files

*pestacle* in its very early development stage, this repository exist for my
own convenience, don't expect documentation for now.

## Compilation

Compilation have been tested on Linux, MacOS, Windows with MSYS2 UCRT64. GCC and Clang
are both supported. You will need `make`, `pkg-config` and `SDL2`.

1. Edit the `PLUGINS_BUILD_LIST` list in `config.mkg` to select the plugins you want
2. `make`
3. The executable and the plugins are in the build directory
4. `make clean` will undo the build

Tip : use `make -j N` with `N` a number close to the number of available CPU cores
for a vastly improved compilation speed.

## Usage

For now, as no documentation is available, browse the demos directory. After compilation
run a demo doing `./build/pestacle ./demos/mouse-motion.txt`.

## Authors

* **Alexandre Devert** - *Initial work* - [marmakoide](https://github.com/marmakoide)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
