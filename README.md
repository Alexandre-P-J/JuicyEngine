# JuicyPixels Game Engine

## Build

```
git clone this repo
cd JuicyPixels
git submodule update --init --recursive
mkdir bin
cd bin
cmake ..
```

## How to create a new game project
Games shall be under the `project` directory, you are required to edit `projects/CMakeLists.txt` inside that directory and
to set up your directory and `projects/your_dir/CMakeLists.txt`. there are a couple example projects you can use as reference.
Shaders under your project folder are automatically compiled and packed on build.