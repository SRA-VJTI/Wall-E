#ifndef SAC_JUNCTION_MAZE_H
#define SAC_JUNCTION_MAZE_H

#include <stdbool.h>
#include <stdint.h>

/* Sensor bits 0..4 must run physically left to right; 1 means line.
 * Pure state machine: no motor, WiFi, or RTOS ownership.
 * Left-hand exploration is intended for connected, non-looped line mazes.
 */
typedef enum { MAZE_FOLLOW, MAZE_PROBE, MAZE_TURN, MAZE_CLEAR, MAZE_FAULT } maze_phase_t;
typedef struct {
    maze_phase_t phase;
    uint32_t since, candidate_since, lost_since, centered_since;
    bool candidate, lost, centered, left, right, departed;
    char turn;
    int last_direction; /* -1 left, +1 right; retain across centred samples. */
} junction_maze_t;
typedef struct {
    bool override;
    int left, right;
    bool reset_pid;
    char decision;
} maze_output_t;

#define MAZE_SPEED 45
#define MAZE_DEBOUNCE_MS 20
#define MAZE_LOST_MS 120
#define MAZE_SEARCH_MS 180
#define MAZE_PROBE_MS 120
#define MAZE_TURN_MIN_MS 100
#define MAZE_UTURN_MIN_MS 300
#define MAZE_TURN_TIMEOUT_MS 1800
#define MAZE_CLEAR_TIMEOUT_MS 1500

static inline maze_output_t junction_maze_step(junction_maze_t *m,
                                               uint8_t bits, uint32_t now)
{
    maze_output_t o = {0};
    bool center = (bits & 0x0e) != 0;
    bool edge = (bits & 0x11) != 0;
    uint32_t elapsed = now - m->since;
    if (m->phase == MAZE_FAULT) {
        o.override = true; /* Latched stop; reboot to recover. */
        return o;
    }
    if (m->phase == MAZE_FOLLOW) {
        if (bits) {
            int position = 0;
            for (int i = 0; i < 5; ++i)
                if (bits & (1u << i)) position += i - 2;
            if (position) m->last_direction = position > 0 ? 1 : -1;
        }
        bool was_lost = m->lost;
        if (!bits) {
            if (!m->lost) { m->lost = true; m->lost_since = now; }
        } else {
            m->lost = false;
        }
        if (was_lost && bits) o.reset_pid = true;
        if (edge) {
            if (!m->candidate) { m->candidate = true; m->candidate_since = now; }
            if (now - m->candidate_since >= MAZE_DEBOUNCE_MS) {
                m->phase = MAZE_PROBE;
                m->since = now;
                m->left = (bits & 1) != 0;
                m->right = (bits & 16) != 0;
                m->candidate = false;
                m->lost = false;
                o.override = true;
                o.left = o.right = MAZE_SPEED;
                o.reset_pid = true;
            }
        } else {
            m->candidate = false;
        }
        if (m->lost) {
            /* Slow arc first, then a bounded pivot toward the last seen line.
             * Unknown direction stays straight initially and searches left later. */
            uint32_t lost_for = now - m->lost_since;
            o.override = true;
            o.reset_pid = true;
            o.left = o.right = MAZE_SPEED;
            if (m->last_direction < 0) o.left = 0;
            if (m->last_direction > 0) o.right = 0;
            if (lost_for >= MAZE_LOST_MS) {
                o.left = m->last_direction > 0 ? MAZE_SPEED : -MAZE_SPEED;
                o.right = -o.left;
            }
        }
        if (m->lost && now - m->lost_since >= MAZE_LOST_MS + MAZE_SEARCH_MS) {
            m->phase = MAZE_TURN;
            m->since = now;
            m->turn = 'B';
            m->departed = true;
            m->lost = false;
            o.override = true;
            o.left = -MAZE_SPEED; o.right = MAZE_SPEED;
            o.reset_pid = true; o.decision = 'B';
        }
        return o;
    }
    o.override = true;
    if (m->phase == MAZE_PROBE) {
        m->left |= (bits & 1) != 0;
        m->right |= (bits & 16) != 0;
        o.left = o.right = MAZE_SPEED;
        if (elapsed >= MAZE_PROBE_MS) {
            m->turn = m->left ? 'L' : center ? 'S' : m->right ? 'R' : 'B';
            o.decision = m->turn;
            m->since = now;
            m->centered = false;
            if (m->turn == 'S') {
                m->phase = MAZE_CLEAR;
            } else {
                m->phase = MAZE_TURN;
                m->departed = !center;
                o.left = m->turn == 'R' ? MAZE_SPEED : -MAZE_SPEED;
                o.right = -o.left;
            }
        }
    } else if (m->phase == MAZE_TURN) {
        o.left = m->turn == 'R' ? MAZE_SPEED : -MAZE_SPEED;
        o.right = -o.left;
        if (!center) m->departed = true;
        uint32_t minimum = m->turn == 'B' ? MAZE_UTURN_MIN_MS : MAZE_TURN_MIN_MS;
        if (elapsed >= MAZE_TURN_TIMEOUT_MS) {
            m->phase = MAZE_FAULT; o.left = o.right = 0;
        } else if (m->departed && elapsed >= minimum && (bits & 4) && !edge) {
            if (!m->centered) { m->centered = true; m->centered_since = now; }
            if (now - m->centered_since >= MAZE_DEBOUNCE_MS) {
                m->phase = MAZE_CLEAR; m->since = now; m->centered = false;
                o.left = o.right = MAZE_SPEED;
                o.reset_pid = true;
            }
        } else {
            m->centered = false;
        }
    } else if (m->phase == MAZE_CLEAR) {
        /* PID steers while junction recognition is disarmed. */
        o.override = false;
        if (center && !edge) {
            if (!m->centered) { m->centered = true; m->centered_since = now; }
            if (now - m->centered_since >= 100) {
                m->phase = MAZE_FOLLOW; m->centered = false;
                m->candidate = m->lost = false;
            }
        } else {
            m->centered = false;
        }
        if (elapsed >= MAZE_CLEAR_TIMEOUT_MS) {
            m->phase = MAZE_FAULT; o.override = true; o.left = o.right = 0;
        }
    }
    return o;
}
#endif
