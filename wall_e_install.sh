#!/usr/bin/env bash
set -e

echo "installing ESP IDF"

_shell_="${SHELL#${SHELL%/*}/}"
# Check whether esp-idf has already been installed
if [ -d $HOME/esp/esp-idf ]; then
    echo "You have already installed esp-idf!"
else
    # System Detection and ESP-IDF Installation
    unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)
		# Check the version of Linux
		if grep -q "Arch" /etc/os-release; then
			if [ "$CI" = "true" ]; then
				echo "Running in Docker Container."
				# Update the core and extra databases
				pacman -Sy --noconfirm
				pacman -S --noconfirm git wget flex bison gperf python python-pip python-setuptools cmake ninja ccache libffi openssl dfu-util libusb
				pacman -S --noconfirm python-virtualenv
				pacman -Syu --noconfirm
			else
				echo "Running on Arch Linux host."
				# Update the core and extra databases
				su -c "pacman -Sy"

				if [ -z "$(sudo --version)" ]; then
					# Steps to be taken if sudo is not installed
					su -c "pacman -S --noconfirm sudo"
					su -c "usermod -aG wheel $USER"
					su -c "echo 'wheel ALL=(ALL:ALL) ALL' >> /etc/sudoers"
					echo "Reboot and run this script again for changes to take place"
					exit 0
				fi

				sudo pacman -S --noconfirm tzdata

				# Time to change the system time
				currentTimezone="$(readlink /etc/localtime)"
				sudo ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime
				sudo hwclock --systohc

				sudo groupadd -f uucp
				sudo usermod -aG uucp $USER || echo "Failed to add user to uucp group"
				sudo pacman -S --noconfirm git wget flex bison gperf python python-pip python-setuptools cmake ninja ccache libffi openssl dfu-util libusb
				sudo pacman -S --noconfirm python-virtualenv
				sudo pacman -Syu

				# Change system time back to original
				sudo ln -fs "$currentTimezone" /etc/localtime
			fi
		elif grep -q -E "Ubuntu|Debian" /etc/os-release; then

			echo "Running on Ubuntu host."
	        su -c "apt update && apt install sudo -y"
    	    sudo apt install tzdata -y
    
			# Set timezone non-interactively
			export DEBIAN_FRONTEND=noninteractive
			sudo ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime
			echo "Etc/UTC" | sudo tee /etc/timezone > /dev/null
			sudo dpkg-reconfigure --frontend noninteractive tzdata
        
			sudo usermod -aG dialout $USER || echo "Failed to add user to dialout group"
			sudo apt install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 -y
			sudo apt install python3-venv -y
		fi
        ;;
    Darwin*)
        if brew --version | grep -q 'Homebrew'; then
            echo "Homebrew is already installed"
        else 
            echo "installing homebrew"
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            echo "homebrew installed successfully"
        fi
        brew install git cmake ninja dfu-util python3  
       ;;
    *)          echo "Not supported :${unameOut}"
esac

    # Create ESP Directory
    mkdir -p "$HOME/esp"
    pushd "$HOME"/esp || (echo "Error: Cannot Make Directory" && exit 1)

    # Clone ESP-IDF Repository
    git clone -b release/v5.1 --recursive https://github.com/espressif/esp-idf.git --depth 1
    cd $HOME/esp/esp-idf
    ./install.sh esp32

    # Check if installation is successful
    . $HOME/esp/esp-idf/export.sh 
    idf.py --version | (grep "v5.1" && echo "Installation successful! Please restart your computer for the changes to take effect.") \
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
    echo "You have already Cloned Wall-E Repository!"
fi
