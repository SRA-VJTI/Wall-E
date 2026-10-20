# SAC junction maze mode

This is optional left-hand exploration for non-looped line mazes. It does not map loops, calculate a shortest route, persist a route, infer a finish marker, or automatically change line polarity. A cross is treated as a junction, never automatically as the finish.

## Enable and build (ESP-IDF PowerShell)

First commit or stash any local changes before pulling SAC. From the repository root:

```powershell
git switch SAC
git pull --ff-only origin SAC
cd 5_line_following
idf.py menuconfig
```

Under Wall-E Configuration > SAC Junction Maze enable left-hand junction maze exploration. Save, then run:

```powershell
idf.py build
idf.py -p COM5 flash monitor
```

Replace COM5 with the board port. Disable the option to restore the original line-following algorithm. WiFi tuning and other example projects are unchanged. During probe/turn/stop overrides the task skips OLED refresh and PID motor writes; the HTTP server remains in its own task.

## Behavior

Normal segments use PID. Persistent outer-sensor activity triggers a slow forward probe which remembers left/right exits, then samples the center for a straight exit. Priority is Left > Straight > Right > Back. Sustained line loss triggers a U-turn. Turns must leave the old line before reacquiring a centered line. Junction detection remains disarmed until the robot clears the intersection. Turn or clearance timeout latches a motor stop until reboot. Decisions print as L/S/R/B in the serial monitor.

## Mandatory physical calibration

Raise the wheels first and verify MOTOR_A_0 is the physical left motor, MOTOR_A_1 is right, and positive commands drive forward. Correct wiring if needed. Verify sensor bit order is physical left to right; use Reverse physical sensor order when necessary.

The input uses the original normalization and threshold 820: a normalized reading greater than 820 is considered line. Check actual readings over line/background. Enable Invert maze line polarity if necessary; inversion is manual. Test at low speed on your actual surface.

Tune MAZE_SPEED and timing constants in main/include/junction_maze.h. The 120 ms forward probe must place the axle at the junction and the sensors beyond its transverse bar; the correct duration depends on sensor offset, speed, battery and line width. A probe that is too short can misclassify a T as straight. MAZE_TURN_MIN_MS and MAZE_UTURN_MIN_MS must prevent reacquiring the wrong branch. Sensor-guided turns still require these timing constraints. The 120 ms loss threshold can mistake long gaps for dead ends; this mode must be enabled only for the junction maze section if the course has gaps. Long solid patches may cause a timeout stop.

Test isolated left/right corners, T junctions from every direction, crosses, dead ends and nearby consecutive junctions before a full maze run. There is no finish detection: stop the robot manually at the finish. No claim of full-course compatibility is made for gaps, inversion transitions, loops or other SAC obstacles.

## Verification

tests/test_junction_maze.c covers debounce, L/S/R/B decisions, turn reacquisition, clearance, timeout latching and timer wrap. GitHub Actions compiles this host test and separately builds the enabled maze firmware; the existing workflow builds the normal examples. Build results must be checked in Actions. Hardware validation remains necessary.
