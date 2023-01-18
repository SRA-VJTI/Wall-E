#!/usr/bin/env bash
set -e

echo "installing ESP IDF"

_shell_="${SHELL#${SHELL%/*}/}"

_sudo(){
    if ! command -v sudo &> /dev/null
    then
        if ! [ $(id -u) = 0 ]; then
           echo "The script need to be run as root." >&2
           exit 1
        fi
        "$@"
    else
        sudo "$@"
    fi
}

# Check whether esp-idf has already been installed
if [ -d $HOME/esp/esp-idf ]; then
    echo "You have installed esp-idf" && exit 0
fi

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)
        export DEBIAN_FRONTEND=noninteractive
        _sudo apt update && _sudo apt upgrade -y
        _sudo apt install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 -y
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
git clone -b v4.4.3 --recursive https://github.com/espressif/esp-idf.git
pushd esp-idf
./install.sh
popd
popd

#Check if installation is successfull 
. $HOME/esp/esp-idf/export.sh 
idf.py --version | (grep "v4.4" && echo "Installation successfull! Please restart your computer for the changes to take effect.") \
    || (echo "installation failed" && exit 1) 

echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> $HOME/."$_shell_"rc
