# JuicyEngine

## What's the scope of this engine
This is a game engine in the form of a framework, this means that its distributed without an editor by design and its intended to be used by programmers.

I encourage the use of external tools to edit game resources, for example if you are in need for a tiled map, then you may use [Tiled](https://www.mapeditor.org/) and then load the exported map into the engine. The same phylosofy can be applied for spritesheets and other resources, or you can even define your own formats.

Another thing to consider is that the engine only supports 2D rendering at the moment, this is
because there are other alternatives for 3D and its imposible to compete against UE4/Unity/Godot unless you require a very specific 3D pipeline, in which case you may be implementing it yourself. This is not the case for 2D, this project aims to be a good start if you intend to code a game that requires extending any 2D generalist engine (if you need to extend an engine, better if its a framework with simpleness in mind).

This project aims to be simple, easy to extend and with portability to consoles in mind.

## Engine Developers: Get the standalone engine
Clone this repository and run:
```bash
git submodule update --init --recursive --jobs 8
```
This will download the latest version of the engine.

If you already did this but you want to update the engine
simply run this instead:
```bash
git submodule foreach git pull origin HEAD
```

## How to use this engine for a game
This repository is designed to be added as a submodule in your game repository.
A CMakeLists.txt shall be provided by your game to link against the engine as you can see in this [Example Project](https://github.com/CallMeLemon/JuicyEngine_Example_Project).
