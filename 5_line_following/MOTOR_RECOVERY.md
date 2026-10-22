# Motor stop and white-surface recovery

Update SAC, then rebuild and flash from 5_line_following.

In menuconfig > Wall-E Configuration > SAC Junction Maze enable maze exploration and Drive straight across all-white areas. Check the dashboard with the complete sensor array over physical white. If all readings exceed the threshold (820), enable White surface reads ABOVE threshold; if all are at or below threshold, leave it disabled. A mixed reading means calibration/alignment is needed. Do not infer physical white from the line/background labels: those change with polarity.

Physical white takes priority over junction probing and turns, sends equal forward commands of 45% duty, resets PID history and cancels the old junction. After at most 1000 ms of continuous white, it stops. It cannot measure heading without feedback; equal PWM is straight drive intent, not heading hold.

After a controller fault, a contiguous interior stripe including the middle sensor, with both outer sensors clear, must persist for 300 ms before following resumes automatically. This means repositioning onto a line can restart the wheels. Uniform fields cannot clear a fault. Recovery uses the current polarity; reboot if polarity was set incorrectly.

The WiFi startup wait is now bounded to 10 seconds, so lack of an IP cannot hold startup indefinitely. WiFi/filesystem initialization errors still require the serial log. Motor commands now check return codes in the PID path as well as the maze path. Serial logs report controller phase transitions with bits and requested PWM.

Tests cover both white signal directions, inverted polarity, straight commands despite previous turn direction, timeout, fault retention, stable-line recovery, recovery cancellation and timer wrap. Existing maze and polarity tests are retained. Hardware operation has not been verified. If commanded PWM is nonzero but wheels do not turn, inspect motor battery, switch, wiring and driver rather than assuming a sensor problem.
