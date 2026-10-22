#ifndef SAC_TRACK_CONTROLLER_H
#define SAC_TRACK_CONTROLLER_H
#include "junction_maze.h"

/* A single sensor interpretation feeds both PID and junction decisions. */
typedef struct {
    junction_maze_t maze;
    bool inverted, automatic, pending, locked;
    bool white_straight, white_high, crossing_white, recovering;
    uint32_t white_since, recovery_since;
    uint32_t pending_since, pending_started, switched_at;
    uint8_t proposed;
} sac_track_t;
typedef struct {
    maze_output_t motor;
    uint8_t line_bits;
    bool switched;
} sac_track_output_t;

#define SAC_WHITE_LIMIT_MS 1000
#define SAC_RECOVERY_MS 300
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
    /* Recover only on a stable centred stripe, never on a uniform field. */
    if (s->maze.phase == MAZE_FAULT) {
        bool centred = (o.line_bits & 4) && sac_interior_stripe(o.line_bits);
        if (centred) {
            if (!s->recovering) { s->recovering = true; s->recovery_since = now; }
            if (now - s->recovery_since >= SAC_RECOVERY_MS) {
                s->maze = (junction_maze_t){0};
                s->recovering = false;
                s->crossing_white = false;
                o.motor.reset_pid = true;
                return o;
            }
        } else s->recovering = false;
        o.motor.override = true;
        return o;
    }
    /* Physical white uses pre-polarity bits. It must not become a maze node
     * or a dead-end search, even when the current line polarity is inverted. */
    bool all_white = raw_bits == (s->white_high ? 31 : 0);
    if (s->white_straight && all_white) {
        if (!s->crossing_white) {
            s->crossing_white = true; s->white_since = now;
        }
        s->pending = false;
        s->maze = (junction_maze_t){0};
        o.motor.override = true; o.motor.reset_pid = true;
        if (now - s->white_since >= SAC_WHITE_LIMIT_MS) {
            s->maze.phase = MAZE_FAULT;
        } else {
            o.motor.left = o.motor.right = MAZE_SPEED;
        }
        return o;
    }
    if (s->crossing_white) {
        s->crossing_white = false;
        s->maze = (junction_maze_t){0};
        o.motor.reset_pid = true;
    }
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
    bool reset_pid = o.motor.reset_pid;
    o.motor = junction_maze_step(&s->maze, o.line_bits, now);
    o.motor.reset_pid |= reset_pid;
    return o;
}
#endif
