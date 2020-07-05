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
    git clone --recursive https://github.com/espressif/esp-idf.git
    cd esp-idf
    ./install.sh
    . $HOME/esp/esp-idf/export.sh
    echo ". $HOME/esp/esp-idf/export.sh" >> ~/.profile
    source ~/.profile
fi

# Wall-E files and custom configurations
cd ~
git clone https://github.com/SRA-VJTI/Wall-E_v2.1



