# TrebleShot

* TrebleShot is a file-sharing tool and this is the desktop intended version.
* Still in alpha state and for this reason, nearly everything is subject to change and probably broken.
* Uses Qt5 framework at its core and can work on Linux, Windows, MacOS.
* Unless you want to help the development, please do not use the CI builds (the release tagged as `continuous`).

For Android version follow [this link](https://github.com/genonbeta/TrebleShot/).


This project is work in progress. See what is happening [here](https://github.com/genonbeta/TrebleShot-Desktop/projects/1).


# Build
## Have these installed
* cmake (3.2.0+) - CMake
* qtbase5-dev (5.10+) - Qt5 Framework
* libkf5dnssd-dev - KDE DNS-SD Development Package

## Run these commands
* `cd` to the root directory of the project
* `git submodule update --init` to get the submodules.
* Build *trebleshot* target with CMake or run `./build_trebleshot.sh` on *nix

Latest CI Build https://github.com/genonbeta/TrebleShot-Desktop/releases/tag/continuous

# Status
[![Build Status](https://travis-ci.org/genonbeta/TrebleShot-Desktop.svg)](https://travis-ci.org/genonbeta/TrebleShot-Desktop)
