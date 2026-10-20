# SAC junction maze mode

This is optional left-hand exploration for non-looped line mazes. It does not map loops, calculate a shortest route, persist a route, infer a finish marker, or infer the complete SAC course layout. A cross is treated as a junction, never automatically as the finish.

## Enable and build (ESP-IDF PowerShell)

First commit or stash any local changes before pulling SAC. From the repository root:

```powershell
git switch SAC
git pull --ff-only origin SAC
cd 5_line_following
idf.py menuconfig
```

Under Wall-E Configuration > SAC Junction Maze enable left-hand junction maze exploration. Automatic polarity switching defaults on inside this mode; disable it for a fixed-color track if required. Set Start with inverted maze line polarity to match the starting surface. Save, then run:

```powershell
idf.py build
idf.py -p COM5 flash monitor
```

Replace COM5 with the board port. Disable the option to restore the original line-following algorithm. WiFi tuning and other example projects are unchanged. During probe/turn/stop overrides the task skips OLED refresh and PID motor writes; the HTTP server remains in its own task.

## Behavior

Normal segments use PID. Persistent outer-sensor activity triggers a slow forward probe which remembers left/right exits, then samples the center for a straight exit. Priority is Left > Straight > Right > Back. Sustained line loss triggers a U-turn. Turns must leave the old line before reacquiring a centered line. Junction detection remains disarmed until the robot clears the intersection. Turn or clearance timeout latches a motor stop until reboot. Decisions print as L/S/R/B in the serial monitor.

## Mandatory physical calibration

Raise the wheels first and verify MOTOR_A_0 is the physical left motor, MOTOR_A_1 is right, and positive commands drive forward. Correct wiring if needed. Verify sensor bit order is physical left to right; use Reverse physical sensor order when necessary.

The input uses the original normalization and threshold 820: a normalized reading greater than 820 is considered line. Check actual readings over line/background. The starting polarity is configured with Start with inverted maze line polarity. Automatic switching can subsequently change it; disable Automatically switch line polarity to keep it fixed. Test at low speed on your actual surface.

Tune MAZE_SPEED and timing constants in main/include/junction_maze.h. The 120 ms forward probe must place the axle at the junction and the sensors beyond its transverse bar; the correct duration depends on sensor offset, speed, battery and line width. A probe that is too short can misclassify a T as straight. MAZE_TURN_MIN_MS and MAZE_UTURN_MIN_MS must prevent reacquiring the wrong branch. Sensor-guided turns still require these timing constraints. The 120 ms loss threshold can mistake long gaps for dead ends; this mode must be enabled only for the junction maze section if the course has gaps. Long solid patches may cause a timeout stop.

Test isolated left/right corners, T junctions from every direction, crosses, dead ends and nearby consecutive junctions before a full maze run. There is no finish detection: stop the robot manually at the finish. No claim of full-course compatibility is made for gaps, loops or other SAC obstacles. Color transitions require the conditions below and physical testing.

## Verification

tests/test_junction_maze.c covers debounce, L/S/R/B decisions, turn reacquisition, clearance, timeout latching and timer wrap. GitHub Actions compiles this host test and separately builds the enabled maze firmware; the existing workflow builds the normal examples. Build results must be checked in Actions. Hardware validation remains necessary.

## Combined automatic polarity and maze control

The same task still owns both motors. sac_track_controller.h first interprets polarity, then feeds the same line bits to PID and junction recognition. With automatic polarity enabled, an opposite-color contiguous stripe strictly inside the outer sensors starts a slow forward confirmation. Junction recognition is suspended immediately during confirmation. An unchanged candidate for 60 ms flips polarity, resets PID, and uses the existing clearance state to avoid counting the boundary as a node. A 250 ms lockout prevents immediate flips back. An unstable candidate that persists for 300 ms latches a stop; reboot after correcting the cause. A disappearing candidate cancels confirmation.

Uniform readings never trigger a polarity flip. Polarity stays fixed during probing, turning and clearance, and a latched fault cannot be bypassed by inversion. Starting inverted and reversed sensor order remain supported. Legacy following with maze mode disabled remains available.

This is a heuristic for a narrow stripe on a contrasting background, ideally crossing the color boundary on a straight section. It cannot distinguish every junction from inversion using only five sensors: a split line with both outer sensors active and a contiguous interior gap can look identical. It also cannot confirm a stripe outside the interior sensors, and a boundary encountered during an active turn is not handled automatically. Test both transition directions, curves, crosses, T-junctions and consecutive obstacles on the actual course; disable automatic polarity if the track has ambiguous patterns. No automatic gap recovery, node mapping, route replay or finish detector is added.

The SAC workflow now tests the combined controller and builds automatic mode plus manual inverted mode with reversed sensors. Host tests cover both transition directions, confirmation cancellation, lockout, uniform fields, junction priority, turn isolation, timer wrap, unstable candidates and every static five-bit input. Hardware tests are still required.
