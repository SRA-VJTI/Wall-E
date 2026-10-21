#include <assert.h>
#include "../main/include/sac_curve_steering.h"
#include "../main/include/sac_track_controller.h"
int main(void) {
    sac_track_t s = {.automatic = true};
    sac_track_output_t o = sac_track_step(&s, 4, 0);
    assert(o.line_bits == 4 && !o.motor.override);
    o = sac_track_step(&s, 27, 10);
    assert(s.pending && o.motor.override && !o.motor.decision);
    o = sac_track_step(&s, 27, 69);
    assert(!o.switched && !s.inverted);
    o = sac_track_step(&s, 27, 70);
    assert(o.switched && s.inverted && o.line_bits == 4 && o.motor.reset_pid);
    sac_track_step(&s, 27, 80);
    sac_track_step(&s, 27, 180);
    assert(s.maze.phase == MAZE_FOLLOW);
    o = sac_track_step(&s, 4, 200);
    assert(!o.switched); /* lockout */
    o = sac_track_step(&s, 4, 330);
    assert(s.pending);
    o = sac_track_step(&s, 4, 390);
    assert(o.switched && !s.inverted && o.line_bits == 4);
    s = (sac_track_t){.automatic = true};
    sac_track_step(&s, 27, 0);
    o = sac_track_step(&s, 4, 30);
    assert(!s.pending && !s.inverted && !o.switched);
    s = (sac_track_t){.automatic = true};
    sac_track_step(&s, 31, 0);
    o = sac_track_step(&s, 31, 20);
    assert(!o.switched && s.maze.phase == MAZE_PROBE);
    s = (sac_track_t){.automatic = true};
    sac_track_step(&s, 0, 0);
    o = sac_track_step(&s, 0, 300);
    assert(!o.switched && o.motor.decision == 'B');
    s = (sac_track_t){.inverted = true};
    o = sac_track_step(&s, 27, 0);
    assert(o.line_bits == 4 && !s.pending); /* manual mode */
    s = (sac_track_t){.automatic = true};
    sac_track_step(&s, 5, 0);
    sac_track_step(&s, 5, 20);
    o = sac_track_step(&s, 4, 140);
    assert(o.motor.decision == 'L' && !s.inverted);
    s = (sac_track_t){.automatic = true};
    s.maze.phase = MAZE_TURN; s.maze.turn = 'L';
    o = sac_track_step(&s, 27, 50);
    assert(!s.pending && !o.switched);
    s = (sac_track_t){.automatic = true};
    sac_track_step(&s, 27, UINT32_MAX - 20);
    o = sac_track_step(&s, 27, 40);
    assert(o.switched);
    s = (sac_track_t){.automatic = true};
    for (uint32_t t = 0; t <= 300; t += 10)
        o = sac_track_step(&s, (t / 10) % 2 ? 25 : 27, t);
    assert(s.maze.phase == MAZE_FAULT && o.motor.override && !o.motor.left);
    for (unsigned b = 0; b < 32; ++b) {
        s = (sac_track_t){.automatic = true};
        for (uint32_t t = 0; t < 2000; t += 10) {
            o = sac_track_step(&s, b, t);
            assert(o.line_bits <= 31);
            assert(o.motor.left >= -MAZE_SPEED && o.motor.left <= MAZE_SPEED);
        }
    }
    float left, right;
    sac_curve_steer(0, 0, 57, 65, 0, &left, &right);
    assert(left == 57 && right == 57);
    sac_curve_steer(4, 36, 57, 65, 0, &left, &right);
    assert(left == 65 && right == 0);
    sac_curve_steer(-4, -36, 57, 65, 0, &left, &right);
    assert(left == 0 && right == 65);
    sac_curve_steer(0, 0, 57, 65, 1, &left, &right);
    assert(left == 45 && right == 45);
    sac_curve_steer(2, 0, 57, 65, 0, &left, &right);
    assert(left == 51 && right == 51);
    return 0;
}
