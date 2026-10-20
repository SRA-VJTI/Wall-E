#include <assert.h>
#include "../main/include/junction_maze.h"
int main(void) {
    junction_maze_t m = {0};
    maze_output_t o = junction_maze_step(&m, 4, 0);
    assert(!o.override);
    o = junction_maze_step(&m, 5, 10);
    assert(!o.override);
    o = junction_maze_step(&m, 5, 30);
    assert(o.override && m.phase == MAZE_PROBE);
    o = junction_maze_step(&m, 4, 150);
    assert(o.decision == 'L' && o.left < 0 && o.right > 0);
    junction_maze_step(&m, 0, 180);
    junction_maze_step(&m, 4, 260);
    o = junction_maze_step(&m, 4, 280);
    assert(m.phase == MAZE_CLEAR && o.reset_pid);
    junction_maze_step(&m, 4, 290);
    o = junction_maze_step(&m, 4, 390);
    assert(m.phase == MAZE_FOLLOW && !o.override);
    m = (junction_maze_t){0};
    junction_maze_step(&m, 20, 0);
    junction_maze_step(&m, 20, 20);
    o = junction_maze_step(&m, 4, 140);
    assert(o.decision == 'S');
    m = (junction_maze_t){0};
    junction_maze_step(&m, 16, 0);
    junction_maze_step(&m, 16, 20);
    o = junction_maze_step(&m, 0, 140);
    assert(o.decision == 'R' && o.left > 0 && o.right < 0);
    o = junction_maze_step(&m, 0, 1940);
    assert(m.phase == MAZE_FAULT && o.override && !o.left && !o.right);
    o = junction_maze_step(&m, 4, 2000);
    assert(o.override && !o.left && !o.right);
    m = (junction_maze_t){0};
    junction_maze_step(&m, 0, 0);
    o = junction_maze_step(&m, 0, 119);
    assert(!o.override);
    o = junction_maze_step(&m, 0, 120);
    assert(o.decision == 'B');
    m = (junction_maze_t){0};
    junction_maze_step(&m, 1, UINT32_MAX - 10);
    o = junction_maze_step(&m, 1, 10);
    assert(m.phase == MAZE_PROBE && o.override);
    return 0;
}
