#!/usr/bin/env bash

echo "installing ESP IDF"

_shell_="${SHELL#${SHELL%/*}/}"

# Check whether esp-idf has already been installed
if [ -d $HOME/esp/esp-idf ]; then
    echo "You have installed esp-idf" && exit 0
fi

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)
        sudo apt update && sudo apt upgrade -y
        sudo apt install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 -y
        ;;
    Darwin*)
        if brew --version | grep -q 'Homebrew'; then
            echo "Homebrew is already installed"
        else 
            echo "installing homebrew"
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            echo "homebrew installed successfully"
        fi
        brew install git cmake ninja dfu-util python3 pip 
       ;;
    *)          echo "Not supported :${unameOut}"
esac

mkdir -p "$HOME/esp"
pushd "$HOME"/esp || (echo "Error: Cannot Make Directory" && exit 1)

#ESP Toolchain & ESP-IDF
git clone -b release/v4.2 --recursive https://github.com/espressif/esp-idf.git
pushd esp-idf
./install.sh
popd
popd

#Check if installation is successfull 
. $HOME/esp/esp-idf/export.sh 
idf.py --version | (grep "v4.2" && echo "Installation successfull") \
    || (echo "installation failed" && exit 1) 

echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> $HOME/."$_shell_"rc
