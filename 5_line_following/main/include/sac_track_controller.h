#ifndef SAC_TRACK_CONTROLLER_H
#define SAC_TRACK_CONTROLLER_H
#include "junction_maze.h"

/* A single sensor interpretation feeds both PID and junction decisions. */
typedef struct {
    junction_maze_t maze;
    bool inverted, automatic, pending, locked;
    uint32_t pending_since, pending_started, switched_at;
    uint8_t proposed;
} sac_track_t;
typedef struct {
    maze_output_t motor;
    uint8_t line_bits;
    bool switched;
} sac_track_output_t;

#define SAC_POLARITY_CONFIRM_MS 60
#define SAC_POLARITY_LOCK_MS 250

/* Accept only a contiguous interior stripe, with both outer sensors background.
 * Uniform fields, off-array lines and split stripes provide no inversion evidence.
 */
static inline bool sac_interior_stripe(uint8_t bits)
{
    if (!bits || (bits & 0x11)) return false;
    while (!(bits & 1)) bits >>= 1;
    return (bits & (bits + 1)) == 0;
}

static inline sac_track_output_t sac_track_step(sac_track_t *s,
                                               uint8_t raw_bits, uint32_t now)
{
    sac_track_output_t o = {0};
    raw_bits &= 31;
    o.line_bits = s->inverted ? (raw_bits ^ 31) : raw_bits;
    if (s->locked && now - s->switched_at >= SAC_POLARITY_LOCK_MS)
        s->locked = false;
    uint8_t alternative = o.line_bits ^ 31;
    bool eligible = s->automatic && !s->locked &&
                    s->maze.phase == MAZE_FOLLOW &&
                    sac_interior_stripe(alternative);
    if (eligible) {
        /* Pause junction recognition immediately, before its 20 ms debounce.
         * Advance slowly to collect evidence; do not steer using the background.
         */
        if (!s->pending) s->pending_started = now;
        if (s->pending && now - s->pending_started >= 300) {
            s->maze.phase = MAZE_FAULT;
            s->pending = false;
            o.motor.override = true;
            return o;
        }
        if (!s->pending || s->proposed != alternative) {
            s->pending = true;
            s->pending_since = now;
            s->proposed = alternative;
        }
        if (now - s->pending_since >= SAC_POLARITY_CONFIRM_MS) {
            s->inverted = !s->inverted;
            s->pending = false; s->locked = true; s->switched_at = now;
            s->maze = (junction_maze_t){.phase = MAZE_CLEAR, .since = now};
            o.line_bits = alternative;
            o.switched = true;
        }
        o.motor.override = true;
        o.motor.left = o.motor.right = MAZE_SPEED;
        o.motor.reset_pid = true;
        return o;
    }
    s->pending = false;
    o.motor = junction_maze_step(&s->maze, o.line_bits, now);
    return o;
}
#endif
