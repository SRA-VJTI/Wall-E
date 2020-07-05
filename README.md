# Wall-E_v2.2-beta

*Official Repository for the self-balancing-line-following bot workshop, implemented with ESP32*

Run the following commands for a quick install on **Linux-based systems**:

```bash
sudo chmod +x wall_e_install.sh
./wall_e_install.sh
```
After this, test the **hello_world** example in the ***same terminal***; if it runs without any errors, log out & log back in.
```bash
cd ~/esp/esp-idf/examples/get-started/hello_world
idf.py flash monitor
```

## Commands

Following are the commands for the current CMake-based build in ESP-IDF:
```bash
idf.py fullclean #Cleaning the build

idf.py menuconfig #Setting up the various parameters for your code

idf.py -p PORT -b BAUD flash #Flashing the firmware (code) onto your ESP

idf.py monitor #View the output on the serial monitor
```
Here, PORT specifies the serial port (e.g. /dev/ttyUSB0); BAUD specifies the flash-speed in bps(115200 - 2000000). 
These parameters are optional.
 
 ## Resources 

Follow this [link](https://drive.google.com/folderview?id=1YOPTwCC7MfX2BODy53GABtRUfmiz-6P-) for all the reference material related to the workshop.
