#include <assert.h>
#include "../main/include/sac_track_controller.h"
int main(void) {
    sac_track_t s = {.automatic=true, .white_straight=true};
    s.maze.last_direction = -1;
    sac_track_output_t o = sac_track_step(&s, 0, 0);
    assert(o.motor.override && o.motor.left == MAZE_SPEED && o.motor.right == MAZE_SPEED);
    o = sac_track_step(&s, 0, 500);
    assert(o.motor.left == o.motor.right && !o.motor.decision);
    o = sac_track_step(&s, 0, 1000);
    assert(s.maze.phase == MAZE_FAULT && !o.motor.left && !o.motor.right);
    o = sac_track_step(&s, 0, 1500);
    assert(s.maze.phase == MAZE_FAULT && !o.motor.left);
    o = sac_track_step(&s, 4, 1510);
    assert(o.motor.override && !o.motor.left);
    sac_track_step(&s, 4, 1700);
    o = sac_track_step(&s, 4, 1810);
    assert(s.maze.phase == MAZE_FOLLOW && !o.motor.override && o.motor.reset_pid);
    s = (sac_track_t){.white_straight=true, .white_high=true, .inverted=true};
    o = sac_track_step(&s, 31, 0);
    assert(o.motor.left == MAZE_SPEED && o.motor.right == MAZE_SPEED);
    o = sac_track_step(&s, 27, 100);
    assert(o.line_bits == 4 && o.motor.reset_pid && !o.motor.override);
    s = (sac_track_t){.white_straight=true, .inverted=true};
    o = sac_track_step(&s, 0, 0);
    assert(o.line_bits == 31 && o.motor.left == o.motor.right && !o.motor.decision);
    s = (sac_track_t){.white_straight=true};
    s.maze.phase=MAZE_TURN; s.maze.turn='L';
    o=sac_track_step(&s, 0, 50);
    assert(o.motor.left == MAZE_SPEED && o.motor.right == MAZE_SPEED);
    o=sac_track_step(&s, 4, 100);
    assert(!o.motor.override && o.motor.reset_pid);
    s.maze.phase=MAZE_FAULT;
    sac_track_step(&s, 4, 200);
    sac_track_step(&s, 7, 450); /* outer sensor cancels recovery */
    o=sac_track_step(&s, 4, 510);
    assert(o.motor.override);
    o=sac_track_step(&s, 4, 810);
    assert(!o.motor.override);
    s = (sac_track_t){.white_straight=true};
    sac_track_step(&s, 0, UINT32_MAX-500);
    o=sac_track_step(&s, 0, 500);
    assert(s.maze.phase == MAZE_FAULT && !o.motor.left);
    return 0;
}
