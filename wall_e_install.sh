#!/usr/bin/env bash

if [ -d $HOME/esp/esp-idf ]; then
    echo "You have installed esp-idf" && exit 0
fi

echo "Installing ESP IDF"
sudo apt update && sudo apt upgrade -y
sudo apt install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 -y
mkdir -p "$HOME/esp"
pushd "$HOME"/esp || (echo "Error: Cannot Make Directory" && exit 1)

#ESP Toolchain & ESP-IDF
git clone -b release/v4.2 --recursive https://github.com/espressif/esp-idf.git
cd "$HOME"/esp/esp-idf
./install.sh
echo "alias get_idf='. \$HOME/esp/esp-idf/export.sh'" >> ~/.bashrc
source ~/.bashrc
