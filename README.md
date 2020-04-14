# PokittoLib

Library for making programs on Pokitto hardware

Can be used with:
- FemtoIDE complete development suite (Win/Linux/Mac) for Pokitto hardware by Felipe Manga https://github.com/felipemanga/FemtoIDE
- Code::Blocks to create a Windows target (and Linux pending)
- EmBitz to create a Pokitto hardware binary https://www.embitz.org/

## Building for Pokitto hardware with Femto

* get FemtoIDE release from https://github.com/felipemanga/FemtoIDE/releases , it includes PokittoLib
* FemtoIDE also has a Pokitto emulator - you can see your code running directly on your computer, no Pokitto device needed

## Building with Code::Blocks (for Windows target 64-bit)

* get latest pokittolib from https://github.com/pokitto/PokittoLib
* get Examples from https://github.com/pokitto/Examples
* add /Examples directory NEXT to /Pokitto folder 
* get codeblocks-20.03mingw-setup.exe from http://www.codeblocks.org/downloads/26
* install CodeBlocks, run it
* open Pokitto_sim.cbp from \PokittoLib\Pokitto\POKITTO_SIM

## Building for Pokitto hardware with EmBitz

* get EmBitz from https://www.embitz.org/
* update the GCC included with EmBitz by copying a newer, C++17 compatible version of GCC on top
  - you can copy the gcc folders from FemtoIDE, which is in the folder \femto\IDE-windows-v0.2.0\windows\arm
  - first make a BACKUP copy of the old \EmBitz\1.11\share\em_armgcc folder
  - replace folders in \EmBitz\1.11\share\em_armgcc folder with the folders in \femto\IDE-windows-v0.2.0\windows\arm
  - now replace arm-none-eabi-objcopy.exe in em_armgcc/bin with the older one from the BACKUP/bin folder ... this is because there is an unfixed issue with arm-none-eabi-objcopy.exe in new GCC releases
* open EmBitz and open PokittoEmBitz.ebp under the PokittoLib directory




