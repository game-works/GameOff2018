# Huntress
## A Game for GitHub Game Off 2018
## "Hybrid"

### How to compile

Requires the Emscripten toolkit and Cmake.

In the console type this to generate the makefiles

  cmake -DCMAKE_TOOLCHAIN_FILE=/home/jmaloney/emsdk/emscripten/1.37.36/cmake/Modules/Platform/Emscripten.cmake .

then type this to create the output js and web asm files

  make
  
The game can run in a web browser.  
