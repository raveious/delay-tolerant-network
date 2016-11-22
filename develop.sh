#!/bin/bash

# Get the rest of the submodule(s)
git submodule init
git submodule update

# Get TinyOS tools from package manager
sudo apt-get -y install automake tinyos-tools
