# LINE FOLLOWING

## TABLE OF CONTENTS

* [How bot follows the line?](#How-bot-follows-the-line?)
    * [Theory](#theory)
    * [Use of PID](#Use-of-pid)
        * [Proportional Term](#proportional-term)
        * [Derivative Term](#derivative-term)
        * [Integral Term](#integral-term)
* [Formula for calculation of correction](#formula-for-calculation-of-correction)
* [Algorithm](#algorithm)
* [Description of the Functions](#Description-of-the-functions)



## How bot follows the line?

### Theory

* The bot uses a 5-sensor line sensor array to detect the black line on a white surface (or vice versa).
* Each sensor gives an analog reading based on the reflectance of the surface below it.
* The sensors are assigned weights: `{-5, -3, 1, 3, 5}` corresponding to the leftmost to rightmost sensors.
* When the bot is perfectly aligned with the line, the sensors detect the line position and calculate an error value based on weighted average of activated sensors.
* The error represents how far the bot is from the center of the line and in which direction.
* If the error is positive, the bot needs to turn right; if negative, it needs to turn left.
* The motors on each side are driven at different speeds to correct the bot's position relative to the line.

### Use of PID

PID stands for **P**roportional-**I**ntegral-**D**erivative

![](https://osoyoo.com/wp-content/uploads/2018/08/pid.png)

* The desired state is the center of the line (error = 0).
* The e(t) part is the error between current position and desired line position.
* The Kp * e(t) part is the Proportional part.
* The Ki * integral e(t) part is the Integral part.
* The Kd * d/dt e(t) part is the Derivative part
* The + means they are all added together into a control signal that is passed on to the system. In our case the system is a line following robot. The control signal tells the ESP how fast to move each motor in order to keep the robot on the line.
* Finally the sensor (the line sensor array in our case) sends the new current position of the robot relative to the line back to the ESP, which calculates the error and does the whole process again. This is called feedback, and makes this type of control loop a closed-loop system (as opposed to an open-loop system, which is easier but much less accurate).

### Proportional Term 

The proportional term (gain) makes a change to the output that is proportional to the current error value. 

* The correction = Kp * Error (Here Kp is the proportionality constant)
* If Kp is Too Low, the robot will respond slowly and may drift away from the line without adequate correction.
* If Kp is too high, the robot will overreact to small errors, causing oscillations and instability as it tries to correct too aggressively.

![](https://osoyoo.com/wp-content/uploads/2018/08/kp1.png)

### Derivative Term

The derivative term slows the rate of change of the controller output, most notably near the controller set value. The Kd term helps the Kp term not overshoot the mark and reduces oscillations. Too large of a Kd term will make the robot sluggish and slow to respond to line changes, while too small of a Kd term will make it oscillate around the line.

* The derivative term helps in predicting the future behavior of the error.
* Correction = Kp * Error + Kd * Error_Rate
* Error_Rate = (Current_Error - Previous_Error) / dt
* This term helps smooth the robot's movement when approaching curves or sharp turns.

![](https://s3.amazonaws.com/embeddedrelated/user/1/pid_fig14_95431.png)

### Integral Term

The Integral term is proportional to the amount of time the error is present. The integral term accelerates the movement of the process towards the set value and eliminates any residual steady-state error that occurs with a proportional only controller.

* It accumulates past errors to eliminate steady-state error.
* For example, if the robot consistently drifts to one side due to mechanical bias, the integral term will build up to provide a permanent correction.
* If the Ki error is too large, it can cause oscillations and instability, especially when the robot encounters sharp turns.

![](https://osoyoo.com/wp-content/uploads/2018/08/ki1.png)

## Formula for calculation of correction

```
correction = Kp*(error) + Ki*(cumulative_error) + Kd*(difference)
```
Proportional Term = Kp*(Error) 

Derivative Term = Kd*(Difference), where difference = (Current Error - Previous Error)

Integral Term = Ki*(Cumulative Error)

## ALGORITHM

1. Initialize the line sensor array, motors, and read PID constants.
2. Read the 5 sensors from the line sensor array.
3. Normalize the sensor readings to a consistent range (0-1000).
4. Calculate weighted error using the formula:
   * For each sensor: if reading > BLACK_BOUNDARY (820), consider it "on line" (k = 1)
   * Calculate weighted sum: `weighted_sum = sum of (weight[i] * k)`
   * Calculate total activated sensors: `sum = sum of k`
   * Calculate position: `pos = (weighted_sum - 1) / sum`
5. Handle special case: if all sensors detect black (all black condition), use previous error to estimate direction.
6. Calculate PID correction with current error, cumulative error, and error difference.
7. Apply correction to motor speeds:
   * Left motor speed = optimum_duty_cycle + correction
   * Right motor speed = optimum_duty_cycle - correction
8. Apply bounds to motor speeds to prevent excessive values.
9. Set motor speeds using the calculated values.
10. Repeat from step 2.

## Description of the functions

* ```c
    void calculate_error()
    ```
    **Description** : Calculates the error position of the bot relative to the line using weighted average of activated sensors

    **Parameters** : None

* ```c
    void calculate_correction()
    ```
    **Description** : Calculates the PID correction value using current error, cumulative error and error difference

    **Parameters** : None

* ```c
    void line_follow_task(void *arg)
    ```
    **Description** : The main task that handles the entire line following process, reading sensors, calculating error and correction, and controlling motors

    **Parameters** : 
    * `arg` : Task argument (unused)

* ```c
    pid_const_t read_pid_const()
    ```
    **Description** : Returns the current PID constants (Kp, Ki, Kd) that were read from the tuning interface

    **Return** : A structure containing the three PID constants

* ```c
    void start_tuning_http_server()
    ```
    **Description** : Starts an HTTP server that allows remote tuning of PID parameters via a web interface

    **Parameters** : None