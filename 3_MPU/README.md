# TABLE OF CONTENTS
- [Table of Contents](#table-of-contents)
- [MPU6050](#mpu6050)
- [Inertial Measurement Unit](#inertial-measurement-unit)
- [Initialising MPU6050](#initialising-mpu6050)
- [Components of MPU6050](#components-of-mpu)
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
* The system processor is an [I2C](https://www.geeksforgeeks.org/i2c-communication-protocol/) master to the MPU6050 (default address is 0x68 but can be changed to 0x69)
* The MPU6050 can also act as an I2C master and directly accept inputs from an external compass
* This is 3.3V device and hence is safe to connect to ESP32 pins directly.
* Direction of the axes are defined on the MPU itself.
  As you can see in this picture

<div align="center">
<img src="https://www.elementzonline.com/image/cache/catalog/data/products/Sensors%20and%20Modules/Accelerometers/MPU6050/sku_154602_2-1000x1000.jpg"  width="280px" >

Illustrative image of the MPU6050
</div>


# Inertial Measurement Unit

* These are small devices indicating changing orientation in smartphones, video game remotes, quadcopters, etc.
* The IMU has both a gyroscope which measures **angular rate** and an accelerometer which measures **linear acceleration**, and/or compasses (magnetometers)
* The number of sensor inputs in an IMU are referred to as ‘DoF’ (Degrees of Freedom), so a chip with a **3-axis gyroscope** and a **3-axis accelerometer** would be a 6-DOF IMU.

# Initialising MPU6050

* A sample wiring of the device to ESP32 looks as follows:
<div align = "center">
<img src="https://i.pinimg.com/originals/c6/57/83/c657835e84aaf91832a770ea0d7d0767.jpg" width="720px" >
</div>

* When power up, MPU6050 will start in SLEEP mode. To use it, we need to first disable this SLEEP mode 
* To use the I2C protocol, we send the value 0 to register PWR_MGMT_1 (0x6b)
* The procedure to do so is as follows :
   1. Send a start sequence
   2. Send the I2C address (0x68) of the MPU6050 with the R/W bit high
   3. Send the internal register number you want to write to (0x6b : PWR_MGMT_1)
   4. Send the data byte (0x00) - This will power on the device
   5. Send the stop sequence

<div align = "center">
<img src="https://fabacademy.org/2022/labs/cidi/students/jorge-roig/images/Week14/Comunication_I2C.png" width="480px">


<img src="https://www.teachmemicro.com/wp-content/uploads/2018/04/I2C-SDA-SCL-01.jpg" width="500px">

 The data byte 0x00 sets all the bits of register 107 to 0, thus disabling SLEEP mode and CYCLE mode, and powering on the MPU-6050 

<img src="https://3.bp.blogspot.com/-8nx1dT_rvYk/XExlIx-nRfI/AAAAAAAABJI/WpHyiidimvk2Y81sTzQiyPtwEW9AEDcIwCLcBGAs/s1600/pulse%2Bsiganal%2Bavg%2Bvalue.png" width="600px">
</div>

There are various terms related with the Pulse Width Modulation:- 
* **Off-Time** :- Duration of time period when the signal is low.
* **ON-Time**  :- Duration of time period when the signal is high.
* **Duty cycle** :- It is the percentage of the time period when the signal remains ON during the period of the pulse width modulation signal.
* **Period** :- It is the sum of off-time and on-time of pulse width modulation signal.

# Components of MPU
  MPU6050 contains two sensors 
  1. **3-axis accelerometer** : It gives linear acceleration but if some rotation is observed then it also gives the values of the euler angles.
  2. **3-axis gyroscope** : It gives angular velocities with the help of coriolis force with a dual tuning fork assembly.
## Accelerometer

* An accelerometer is a sensor that measures the **force** of linear acceleration.
* It's output is in the form of m/s^2 of g-forces on a body. It is highly sensitive to movement in any direction.
* The MPU6050 uses a MEMS accelerometer, which consists of stationary and movable electrodes acting as **capacitor plates**
* Acceleration produces change in g-forces on the moving device, which causes displacement of the movable capacitor plates
* This causes a change in capacitance across the plates, which is detected internally by change in Voltage
* We then calculate the displacement from the in Capacitance, which helps us infer the value of acceleration

<div align="center">
<img src="https://lastminuteengineers.com/wp-content/uploads/arduino/MEMS-Accelerometer-Working.gif" width="600px">
<img src="https://lastminuteengineers.com/wp-content/uploads/arduino/Accelerometer-Animation-Labels.png" width="600px">


[Explanation of accelerometer giving euler angles](https://youtu.be/UWY_TIDkcjs)
</div>

## Reading Accelerometer Data

* The acceleration of the three axes, X,Y and Z, are stored in 3 pairs of registers on the MPU6050
* Each register on the MPU6050 is an 8-bit register
* In each of these pairs, one register stores the MSB (most significant bit) and the other stores the LSB (least significant bit)
* These 6 registers are [burst-read](#reading-register-data) and the 8-bit MSB and LSB bytes for each axis are combined to give a single 16-bit float
* The accelerometer has a *sensitivity factor* of about **16,384**, i.e. 1g of acceleration gives a reading of 16384 units.

<div align="center">
<img src="https://www.azoft.com/wp-content/uploads/2017/04/walk-1-with-magnitude-of-vector-oscillations-1.png" width = "600px">


The above data is obtained by integrating the acceleration values over time twice.
</div>


## Accelerometer Euler Angles 

* We just saw that the accelerometer gives us linear acceleration, then how is it giving euler angles too?
  * Answer is simple, if we put the whole assembly of the Accelerometer under two giant capacitive plates then if the rotation occurs then the **Proof Mass** will vibrate up and down creating difference in capacitance
  * This difference in capacitance will give us the **coriolis force** which has the magnitude of 2 *v* *x* *Ω*.
      * Ω - Angular velocity in the directional vector which is inside the plane of proof mass.
      * v - Linear velocity along any two directions of the proof mass.
  * Now, We have coriolis force and linear acceleration, So what should we do further? 
    * We should first convert the data from the accelerometer with time to get the linear velocity in the respective direction
    * Then, with the help of the magnitude of the coriolis force we will have the angles in each of the directions i.e. Roll, Pitch, and Yaw.
* The roll and pitch angles are calculated from the accelerometer using the [formula](https://www.digikey.com/en/articles/using-an-accelerometer-for-inclination-sensing) below :
  
<div align="center">
<img src="https://community.st.com/sfc/servlet.shepherd/version/renditionDownload?rendition=THUMB720BY480&versionId=0680X000006HeQF&operationContext=CHATTER&contentId=05T0X00000OO3Kg&page=0" width = "360px">
</div>
   
## Gyroscope

* A gyroscope is a sensor that measures the change in velocity (if at all) of a device rotating around its own axis.
* The MPU6050 uses a MEMS gyroscope, which uses [Coriolis effect](https://www.youtube.com/watch?v=ti4HEgd4Fgo&t=236s)
* To get a better understanding of the coriolis force please go through this [video](https://youtu.be/GeyDf4ooPdo).
* A general gyroscope has a dual tuning fork assembly. 

<div align = center>
<img src = "https://cdn.comsol.com/wordpress/sites/1/2019/10/piezoelectric-rate-gyroscope-labeled.png" width="600px">
</div>

* Why dual tuning fork? 
  * The vibrations in the two arms of the tuning fork is in the opposite directions which helps the gyroscope to produces vertical vibrations using the coriolis effect.
  * The sensing mass is the primary source for the measurement of the coriolis force through which we can understand the 
* A gyroscope measures **changes** in angular velocity.
* The illustration shows how the actual MEMS Gyroscope assembly looks like and how the motion in different axes takes place. 
* Instead of two tuning fork assembly it uses to proof masses attached assembly which moves in different direction in the opposite sides when rotation is observed (Along a specific axis).

<div align = "center">
<img src="https://www.coventor.com/wp-content/uploads/2019/11/MEMS-software-animation.gif" >
<img src="https://lastminuteengineers.b-cdn.net/wp-content/uploads/arduino/MPU6050-Accel-Gyro-Working-Yaw-Mode-Output.gif">
</div>

## Reading Gyroscope Data

* The acceleration of the three axes, X,Y and Z, are stored in 3 pairs of registers on the MPU6050.
* Each register on the MPU6050 is an 8-bit register.
* In each of these pairs, one register stores the MSB (most significant bit) and the other stores the LSB (least significant bit).
* These 6 registers are [burst-read](#reading-register-data) and the 8-bit MSB and LSB bytes for each axis are combined to give a single 16-bit float.
* The gyroscope has a *sensitivity factor* of about **131**, i.e. 1 degree of rotation gives a reading of 131 units.
* Therefore, the degrees of rotation are obtained by dividing the raw readings by 131.
* Euler angles are calculated by integrating raw readings of each axis over time.
* The below illustration shows the uncalibrated and calibrated gyroscope readings which are obtained by post processing the uncalibrated readings from the gyroscope.

<div align = "center">
<img src="https://cdn-learn.adafruit.com/guides/cropped_images/000/002/887/medium640/sensors_image.png?1580089120" width="480px">
</div>


## Complementary Filter
* Since [gyroscopic drift](https://www.analog.com/en/analog-dialogue/raqs/raq-issue-139.html#:~:text=The%20gyroscope%20drift%20is%20mainly,most%20sensitive%20to%20this%20drift.) accumulates over time, it is necessary to reset the rate readings from the gyroscope at frequent intervals, so the drift can be minimised.
* We are getting the readings for the angles from two sensors i.e. gyroscope and the accelerometer, so in order to get rid of the errors in the gyroscopic drift we will need to combine this data in some form to get an optimum estimate of the current state. 
* To do this, we combine both the gyroscope and accelerometer readings in a weighted average. This is what we call as Sensor Fusion.
* Now we know that, the sensitivity factor of accelerometer i.e. 16384 is far more than the sensitivity factor off the gyroscope i.e. 131.
* So, Naturally we need to combine these data in such a way that the gyroscope data and accelerometer both have some significance in the final fused reading.
* For this, we use a constant which is α.
Expression:
            y= α * y + (1−α) * x
        Where, y is the final reading
        α is the constant which is always < 1.
* The value of alpha is determined experimentally.

<div align="center">
<img src="https://i.stack.imgur.com/Hugl7.png" width = "600px">
<img src="https://i.stack.imgur.com/Tflpc.gif" width = "600px">
</div>


## Reading Register Data

What is Register Data?
Through this quick read you might have understood the basic role of MPU6050 i.e. Gyroscope and Accelerometer combined. 

Now, after all the internal processing of those change in capacitance the sensor gives some data as output and we need that data to be readable. 

so....

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

<div align= "center">
<img src="http://www.i2c-bus.org/static/i2c/repeatedstart.gif">

<img src =  "https://i.stack.imgur.com/FHPKk.png">

</div>




