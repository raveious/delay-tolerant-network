#!/bin/bash

# Get the rest of the submodule(s)
git submodule init
git submodule update

# Build TinyOS tools
cd tinyos-main/tools
./Bootstrap
./configure
make
sudo make install
cd ../..

# MSP430 utilities
sudo apt-get -y install gcc-msp430
