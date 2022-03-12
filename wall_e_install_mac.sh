#
#
# IF YOU HAVE OPENED TO SEE THE CONTENT OF THIS FILE, KUDOS TO YOU
# IF YOU WANT TO KNOW WHAT AM I DOING HERE , SEE THE COMMENTS
# THEN ONLY ASK QUESTIONS IN THE GROUP
#
#

set -e
# installing homebrew
if brew --version | grep -q 'Homebrew'; then
    echo "Homebrew is already installed"
else 
    echo "installing homebrew"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    echo "homebrew installed successfully"
fi

# installing important packages
if cmake --version | grep -q 'CMake suite maintained and supported by Kitware'; then
    echo "cmake is already installed"
else 
    echo "installing cmake"
    brew install cmake
    echo "cmake is installed"
fi

if git --version | grep -q 'git version'; then
    echo "git is already installed on the system"
else
    echo "installing git"
    brew install git
    echo "git has been installed"
fi


# installing python 
if python3 --version | grep -q 'Python'; then
    echo "python3 is already installed"
else 
    echo "installing python3"
    brew install python3
    echo "installed python3"
fi

if pip3 --version | grep -q 'python 3'; then
    echo "pip3 is already installed"
else
    echo "installing python pip"
    python3 -m pip install --upgrade pip
    echo "installed python pip"
fi

############################# INSTALLING ESP_IDF ######################################
# changing to root directory
cd ~

# creating the esp directory
if [ -d ~/esp ]; then
    echo "reinstalling the esp";
    cd ~/esp/esp-idf
    git rm -rf .
    git clean -fxd
    rm -rf .git
    cd ..
    rm -r esp-idf
    rm -rf .git
    cd ..
    rm -R esp
    mkdir esp
fi

# changing directory to esp directory
cd esp

# cloning the espidf repo inside esp
git clone -b v4.2 --recursive https://github.com/espressif/esp-idf.git

# changine the python version
export new="python3"
cd ~/esp/esp-idf/tools 
sed -i idf_tools.py "1 s/python/$new/g" idf_tools.py
cd ~
# installing espressif
. $HOME/esp/esp-idf/install.sh

if echo $0 | grep -q "zsh"; then
    echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> ~/.zshrc
    exec zsh
elif echo $0 | grep -q "bash"; then
    echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> ~/.bashrc
    exec bash
fi

echo "INSTALLATION IS COMPLETE"
