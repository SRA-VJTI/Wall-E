#!/usr/bin/env bash
echo "installing ESP IDF"

_shell_="${SHELL#${SHELL%/*}/}"
# Check whether esp-idf has already been installed
if [ -d $HOME/esp/esp-idf ]; then
    echo "You already have installed esp-idf!"
else
    # System Detection and ESP-IDF Installation
    unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)
        sudo apt update && sudo apt upgrade -y
        sudo usermod -a -G dialout $USER
        sudo apt install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 -y
        sudo apt install python3-venv
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

    # Create ESP Directory
    mkdir -p "$HOME/esp"
    pushd "$HOME"/esp || (echo "Error: Cannot Make Directory" && exit 1)

    # Clone ESP-IDF Repository
    git clone -b release/v5.1 --recursive https://github.com/espressif/esp-idf.git
    cd $HOME/esp/esp-idf
    ./install.sh esp32
    popd
    popd

    # Check if installation is successful
    . $HOME/esp/esp-idf/export.sh 
    idf.py --version | (grep "v5.1.2" && echo "Installation successful! Please restart your computer for the changes to take effect.") \
        || (echo "Installation failed" && exit 1) 
    # Set IDF Alias
    echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> $HOME/."$_shell_"rc
fi

# Clone Wall-E repository
if [ ! -d "$HOME/Wall-E" ]; then
    cd "$HOME" || (echo "Error: Could not navigate to Home" && exit 1)
    echo "Cloning Wall-E"
    git clone --recursive https://github.com/SRA-VJTI/Wall-E.git
else 
    echo "You already have Cloned esp-idf!"
fi