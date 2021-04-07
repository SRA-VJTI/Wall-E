# Table Of Contents
* WALL-E Installations
    * [For Windows](#for-windows)
    * [For Linux](#for-linux)
* [Commands](#commands)
# For Windows
First We are going to install ESP-IDF First and then Wall-E files

## 1. Download the installer from [here]( https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html#esp-idf-tools-installer )
<p align="center">
  <img src="https://user-images.githubusercontent.com/66636289/112637273-f8ae2180-8e63-11eb-8f96-4921fd6c1441.png">
</p>

### 2. After Downloading open .exe file, select `I accept` then click Next>.
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/2.png">
</p>

### 3. In my case I already had python3.6 and github on my system. If you don’t, then select the install python, install git options 
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/3.png">
</p>

### 4. The 1st option is only available for those already have github. For rest Choose the 'Install Git' option  
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/4.png">
</p>

### 5. 
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/5.png">
</p>

### 6.
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/6.png">
</p>

### 7. Click on Install then after Installation check all boxes and click on Finish.
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/7.png">
</p>


### 8. ESP-IDF Command Prompt & Poweshell window will pop-up :
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/8.png">
</p>

 
### 9. Run 'export.bat' in ESP-IDF Command Prompt:
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/9.png">
</p>

### 10 . Run `idf.py build`
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/10.png">
</p>

<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/10(1).png">
</p>

## Wall-E Files
## Step 1 : Cloning the Wall-E Git Repo
To clone the Repo just execute the following command on ESP-IDF Command Prompt. We are Making the project folder in the esp folder
```sh
cd %userprofile%\esp
git clone https://github.com/SRA-VJTI/Wall-E_v2.2-beta.git --recurse-submodules
cd Wall-E_v2.2

```
# For Linux 
## Step 1 : Cloning the Wall-E Git Repo
To clone the Repo just execute the following command on terminal. We are installing the project in the home folder
```sh
cd $HOME
git clone https://github.com/SRA-VJTI/Wall-E_v2.2-beta.git --recurse-submodules
cd Wall-E_v2.2-beta
```
## Step 2 : Installing the Necessary Prerequisites(ESP-IDF etc)
Run the following commands for a quick install on Linux-based systems:
```sh
cd $HOME/Wall-E_v2.2-beta
sudo chmod +x wall_e_install.sh
./wall_e_install.sh
```
After this, test the hello_world example in the same terminal; if it runs without any errors, log out & log back in.
(Connect ESP32 to your device before running the below commands)
```sh
cd ~/esp/esp-idf/examples/get-started/hello_world
idf.py flash monitor
```
# Commands
This is the Basic Procedure For Compiling and Flashing a Project code on the ESP32
## Step 1 : Set Up Environtment variables
In the terminal where you are going to use ESP-IDF, run:
* For Linux - we have created a shortcut for the command `. $HOME/esp/esp-idf/export.sh`
```sh
get_idf
```
if for some reason the `get_idf` command doesn't work you can always use the `. $HOME/esp/esp-idf/export.sh` command
* For Windows
```sh
%userprofile%\esp\esp-idf\export.bat
```
Through this command specify the Folder/Project in which we will to be using ESP-IDF 
## Step 2 : Start a Project
Now you are ready to prepare your application for ESP32.
* For Linux -
```sh
cd ~/esp
cp -r $IDF_PATH/examples/get-started/hello_world .
```
* For Windows -
```sh
cd %userprofile%\esp
xcopy /e /i %IDF_PATH%\examples\get-started\hello_world hello_world
```
## Step 3 : Connect Your Device
Now connect your ESP32 board to the computer and check under what serial port the board is visible.
Serial ports have the Name
* Linux : names like `/dev/tty`
* Windows : names like 'COM1`
## Step 4 : Configure
Now you are ready to prepare your application for ESP32.
* For Linux -
```sh
cd ~/esp/hello_world #Navigating to the file
idf.py set-target esp32 #Command for Setting the Target 
idf.py menuconfig # Command for Opening the Configuration Menu
```
* For Windows -
```sh
cd %userprofile%\esp\hello_world #Navigating to the file
idf.py set-target esp32 #Command for Setting the Target
idf.py menuconfig #Command for Opening the Configuration
```
If the previous steps have been done correctly, the following configuration menu appears :
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/project-configuration1.png">
</p>

* This menu helps us to change the Parameters. Currenty we dont need to change anything.
## Step 5 : Build the Project
Build the project by running:
* Same for Both Linux and Windows
```sh
idf.py build #Command for building the code
```
This command will compile the application and all ESP-IDF components, then it will generate the bootloader, partition table, and application binaries.
<p align="center">
  <img src="https://github.com/hashmis79/Wall-e-Installations/blob/main/Assets/build.png">
</p>

## Step 6 : Flash onto the Device
Flash the binaries that you just built (bootloader.bin, partition-table.bin and hello-world.bin) onto your ESP32 board by running.:
* Note : Press Down the Boot Button on ESP32 and then execute the Flash command
```sh
idf.py -p PORT [-b BAUD] flash 
```
* For Linux 
* PORT - /dev/ttyUSB0 (`idf.py -p /dev/ttyUSB0 -b 2000000 flash`)
* For Windows 
* PORT - /COM1 (`idf.py -p /COM1 -b 2000000 flash`)
* (Depending on the port you used for connecting the board the port can vary from /dev/ttyUSB0 and Zero can be replaced by any other consecutive number.
* and same for windows /COM1 one can be replaced by other number depending on the port you have connected esp to.)
## Step 7 : Flash onto the Device
* For seeing the output given by esp32 we use this command after flashing
```sh
idf.py flash monitor
```
* Same for both Windows and Linux


