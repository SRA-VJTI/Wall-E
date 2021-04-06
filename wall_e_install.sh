# Check if IDF Exists
temp_path="${HOME}/esp/esp-idf"

if [ "$(realpath "$IDF_PATH")" = "$(realpath "$temp_path")" ]; then
    echo "ESP IDF already exists"
else
    echo "Installing ESP IDF"
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache
    cd ~
    mkdir esp && cd esp

    #ESP Toolchain & ESP-IDF
    git clone -b release/v4.2 --recursive https://github.com/espressif/esp-idf.git
    cd ~/esp/esp-idf
    ./install.sh
    alias get_idf='$HOME/esp/esp-idf/export.sh
    source ~/.bashrc
fi
