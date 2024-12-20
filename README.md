# CrusaderAutoMarket

## Description
AutoMarket for Stronghold Crusader (Steam version only).

## Installation
Inject the .dll into the running Crusader process. E.g. via CheatEngine.

## Usage
During the game hit ALT+M to open the AutoMarket overlay.
Settings are saved globally, not per save game.  
The "Enabled" status is not saved and must be re-enabled each game to prevent accidental buyings/sellings due to global settings.

![grafik](https://github.com/user-attachments/assets/d40d0e54-d55f-4b67-9309-a99ab525bce4)

## Compilation
The project should be compiled in x86 configuration.
The project makes use of [nlohmann/json](https://github.com/nlohmann/json) which needs to be installed and added to "additional includes".
