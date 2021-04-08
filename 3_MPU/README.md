# TABLE OF CONTENTS
- [TABLE OF CONTENTS](#table-of-contents)
- [MPU6050](#mpu6050)
- [Inertial Measurement Unit](#inertial-measurement-unit)
- [Initialising MPU6050](#initialising-mpu6050)
  - [Accelerometer](#accelerometer)
  - [Reading Accelerometer Data](#reading-accelerometer-data)
  - [Accelerometer Euler Angles](#accelerometer-euler-angles)
  - [Gyroscope](#gyroscope)
  - [Reading Gyroscope Data](#reading-gyroscope-data)
  - [Complementary Filter](#complementary-filter)
  - [Reading Register Data](#reading-register-data)



# MPU6050

* It is a 6-axis motion tracking device
* The MPU-6050 is a commonly used chip that combines a MEMS gyroscope and a MEMS accelerometer and uses a standard I2C bus for data transmission
* The system processor is an I2C master to the MPU6050 (default address is 0x68 but can be changed to 0x69)
* The MPU6050 can also act as an I2C master and directly accept inputs from an external compass
* This is 3.3V device and hence is safe to connect to ESP32 pins directly.


![](https://lh5.googleusercontent.com/zS7Xrh9qRU7qUwhcF6BoJwzG7Pex7cvjD3yFqS_rJA35Nuj-rDkpS9uEt6EjqmGwzEEe3k-jNw2ENILxgxKWGLO5hW6j1er4oaKXk-ciVyrCC4GvzMDzwR9l0u5CrtsNRpAI7LRqf7U)



# Inertial Measurement Unit

* These are small devices indicating changing orientation in smartphones, video game remotes, quadcopters, etc.
* The IMU has both a gyroscope which measures angular rate and an accelerometer which measures linear acceleration, and/or compasses (magnetometers)
* The number of sensor inputs in an IMU are referred to as ‘DoF’ (Degrees of Freedom), so a chip with a 3-axis gyroscope and a 3-axis accelerometer would be a 6-DOF IMU.

# Initialising MPU6050

* A sample wiring of the device to ESP32 looks as follows:
![](https://lh5.googleusercontent.com/OiTQl8mZl41Dwa_Nb6PD6yzMigvv6xV7KnY7bfW56dBSoYuK8shYet7VMev0rVWcFWhirvUIenbFn_B6jc_nmTulH4-3vInHrOZX3sW4f2lBvel_HcthTW_O89FAGwankOtBBJ8aLhY)

* When power up, MPU6050 will start in SLEEP mode. To use it, we need to first disable this SLEEP mode 
* To use the I2C protocol, we send the value 0 to register PWR_MGMT_1 (0x6b)
* The procedure to do so is as follows :
   1. Send a start sequence
   2. Send the I2C address (0x68) of the MPU6050 with the R/W bit high
   3. Send the internal register number you want to write to (0x6b : PWR_MGMT_1)
   4. Send the data byte (0x00) - This will power on the device
   5. Send the stop sequence



![](https://lh5.googleusercontent.com/HsUgcO1ojRlHZgbtl1Pyk7aWiUjs1x7XnhI5oYsa8C2gCqAbfqOGpr9fnjk0TBxWHmoIfJEQLqHUqe4Kh3KsjMLKSHAD6jQMsiEwrNyA3KLSeHiNLC3tDjzjAKN_nENu8FMp0tYS60E)

 The data byte 0x00 sets all the bits of register 107 to 0, thus disabling SLEEP mode and CYCLE mode, and powering on the MPU-6050 

![](https://lh3.googleusercontent.com/4p2RF-ezjP6jtT6S7xBQGfls0bcj7JM27W_ZsgRj5pYGQKBnrHfP85MtPncMeuu5qmSEupHdCSu2qPf4TSSBbejcN5MMqU5gLvLMaUDsw9K-Zp7vwLNIiD42d8xdfBKu0A0qxFaS6jk)



There are various terms related with the Pulse Width Modulation:- 
* **Off-Time** :- Duration of time period when the signal is low.
* **ON-Time**  :- Duration of time period when the signal is high.
* **Duty cycle** :- It is the percentage of the time period when the signal remains ON during the period of the pulse width modulation signal.
* **Period** :- It is the sum of off-time and on-time of pulse width modulation signal.

## Accelerometer

* An accelerometer is a sensor that measures the **force** of linear acceleration.
* It's output is in the form of m/s^2 of g-forces on a body. It is highly sensitive to movement in any direction.
* The MPU6050 uses a MEMS accelerometer, which consists of stationary and movable electrodes acting as **capacitor plates**
* Acceleration produces change in g-forces on the moving device, which causes displacement of the movable capacitor plates
* This causes a change in capacitance across the plates, which is detected internally by change in Voltage
* We then calculate the displacement from the in Capacitance, which helps us infer the value of acceleration


![](https://lastminuteengineers.com/wp-content/uploads/arduino/MEMS-Accelerometer-Working.gif)

![](https://lastminuteengineers.com/wp-content/uploads/arduino/Accelerometer-Animation-Labels.png)


## Reading Accelerometer Data

* The acceleration of the three axes, X,Y and Z, are stored in 3 pairs of registers on the MPU6050
* Each register on the MPU6050 is an 8-bit register
* In each of these pairs, one register stores the MSB (most significant bit) and the other stores the LSB (least significant bit)
* These 6 registers are [burst-read](#reading-register-data) and the 8-bit MSB and LSB bytes for each axis are combined to give a single 16-bit float

![](https://lh4.googleusercontent.com/ePwQEj_TeMRQU83M7t2RsXGJD2q37z-B3kSlcLdU85L4XeKxkefT_fAYHNRpRjGOYI8r0bbQWd64I7emYms9x4ST6jEAdBlcC2wdibE)


## Accelerometer Euler Angles 

* The roll and pitch angles are calculated from the accelerometer using the [formula](https://www.digikey.com/en/articles/using-an-accelerometer-for-inclination-sensing) below :
  
<img src="https://render.githubusercontent.com/render/math?math=\theta = \arctan(\frac{A_{X.OUT}}{A_{Y.OUT}})">
   
  


## Gyroscope

* A gyroscope is a sensor that measures the change in velocity (if at all) of a device rotating around its own axis.
* A gyroscope measures **changes** in angular velocity.
* The MPU6050 uses a MEMS gyroscope, which uses [Coriolis effect](https://www.youtube.com/watch?v=ti4HEgd4Fgo&t=236s)


## Reading Gyroscope Data

* The acceleration of the three axes, X,Y and Z, are stored in 3 pairs of registers on the MPU6050
* Each register on the MPU6050 is an 8-bit register
* In each of these pairs, one register stores the MSB (most significant bit) and the other stores the LSB (least significant bit)
* These 6 registers are [burst-read](#reading-register-data) and the 8-bit MSB and LSB bytes for each axis are combined to give a single 16-bit float
* The gyroscope has a sensitivity factor of about 131, i.e. 1 degree of rotation gives a reading of 131 units
* Therefore, the degrees of rotation are obtained by dividing the raw readings by 131 
* Euler angles are calculated by integrating raw readings of each axis over time

![](https://lh3.googleusercontent.com/dQ-SZL7kp5WEnFb-KXFH-FDvF7yv_2J28ycODkipFfFAAMN1lYAk0rPzKg7DaPcNDYk-BdUEJQSzApxxESam_7tnbsIOcLenjofFoUiCpAhsyuMtkWaFMcZPJrK5EMJyX9GCNHSNeeo)


## Complementary Filter
* Since gyroscopic drift accumulates over time, it is necessary to reset the rate readings from the gyroscope at frequent intervals, so the drift can be minimised
* To do this, we combine both the gyroscope and accelerometer readings in a weighted average
* The value of alpha is determined experimentally

![](http://www.pieter-jan.com/images/equations/CompFilter_Eq.gif)


## Reading Register Data

* To read/receive data from a slave, the master has to first send a START condition addressing the MPU6050
* Then the master must WRITE the requested register from which the data is to be read
* After this, instead of sending a STOP condition to stop the WRITE operation and then sending a consecutive START condition to start the READ operation,
  the I2C bus provides the **repeated start** functionality, wherein instead of sending a STOP-START condition, we send only a START condition and read the
  specified number of bytes from the register.
* The procedure to do so is as follows :
  1. Send a start sequence
  2. Send the I2C address (0x68) of the MPU6050 with the R/W bit low - This writes to the device
  3. Send the Internal register address you want to read from
  4. Send a start sequence again (repeated start)
  5. Send the I2C address of the device with the R/W bit high - This reads from the device
  6. Read the number of the requested data bytes from the registers
  7. Send the stop sequence.
  6. All write and read operations (except the last read) are answered with a ACK if successful.

 ![](http://www.i2c-bus.org/static/i2c/repeatedstart.gif)

 ![](https://i.stack.imgur.com/FHPKk.png)






