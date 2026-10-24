/*
 * STEP 3 -- fixes the two problems seen on step 2. NOT COMPILED, treat as first version.
 *
 * Problem 1: after drifting onto black the bot stopped for good (task ended) and needed a power cycle.
 *   Fix: no code path ends the task any more except the END box. When a turn or the boot acquire
 *   fails, the bot stops and WAITS (wait_for_line) until the line is seen 3 reads in a row, then
 *   carries on. So you can put it back on the white line and it restarts by itself.
 *
 * Problem 2: at a right turn it spun a full anticlockwise revolution.
 *   Cause: when the line was lost, u_turn() ALWAYS spun left. If the line was to the right, the bot
 *   had to rotate ~270 degrees the long way round, crossing black on the way.
 *   Fix: g_last_side remembers which side the line was last seen on. Lost-line recovery spins
 *   TOWARD that side. A true dead end (line last seen centred) still spins left.
 *   Also: side branches seen just before the node trigger are latched (preL/preR) and passed into
 *   run_node(), so a right/left branch is no longer missed if it was only visible briefly.
 *
 * STEP 4: removed the last way the task could end for good. Step 3 could still stop permanently on
 * "END reached" (a wide patch, cnt >= END_MIN_SENSORS, seen after END_ARM_MS). A drift onto a wide
 * patch looks the same as the END box, so it ended the task and needed a power cycle. Now END just
 * stops the motors and waits; it resumes by itself once a normal line (1..3 sensors) is seen again.
 * Waiting logs now print the sensor values so you can see what the bot thinks it is on.
 *
 * STEP 5: sharp-turn handling. NOT COMPILED, treat as first version.
 *   1. Sharp-turn mode: when the line is at the extreme edge (outer sensor on, centre off, <=2 sensors),
 *      the inside wheel is driven in REVERSE (SHARP_IN_DUTY) and the outside wheel at SHARP_OUT_DUTY,
 *      instead of being clamped to lower_duty_cycle. PID state is still updated every loop.
 *   2. Faster reaction when the line vanishes off the edge: if the last read that saw the line had an
 *      outer sensor on (g_edge) and a known side, recovery starts after LOST_EDGE_MS (40) instead of
 *      LOST_MS (120). A line last seen centred still waits the full LOST_MS (gaps / real dead ends).
 *   3. Speed scales down with |error|: base = optimum - SLOW_K*|error| (floored at lower_duty_cycle).
 *   SHARP_*, SLOW_K and LOST_EDGE_MS are starting guesses: tune on the track.
 *
 * Kept from step 2: re-arm after nodes, debounced pivots, split timeouts, END armed from line acquired.
 * Timeout values are still guesses: tune from the "took N ms" logs.
 * ASSUMPTION: sra_board.h provides MOTOR_BACKWARD, bound(), map(), line_sensor_array.
 */

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sra_board.h"
#include "tuning_http_server.h"
#include "esp_netif.h"
#include "lwip/ip4_addr.h"

#define MODE NORMAL_MODE
#define BLACK_MARGIN 4095
#define WHITE_MARGIN 0
#define bound_LSA_LOW 0
#define bound_LSA_HIGH 1000
#define BLACK_BOUNDARY  820

#define LINE_IS_ABOVE_BOUNDARY 1
#define sees_line(v) (LINE_IS_ABOVE_BOUNDARY ? ((v) > BLACK_BOUNDARY) : ((v) <= BLACK_BOUNDARY))

#define ACQUIRE_MAX_MS  3000   // boot: drive straight this long looking for the line, then wait for it
#define END_ARM_MS      6000   // END box can only trigger this long after the line was acquired
#define DEBUG_PRINT     1

/* ---------------- maze config (tune) ---------------- */
#define CROSS_DUTY          40
#define CROSS_MS            120    // TUNE FIRST
#define TURN_DUTY           40
#define PIVOT_TIMEOUT_MS    1200
#define UTURN_TIMEOUT_MS    2000
#define TURN_CONFIRM        2
#define LOST_MS             120    // line lost while last seen centred => dead end / gap
#define LOST_EDGE_MS        40     // line lost after being seen at the edge => sharp corner, react fast
#define SHARP_OUT_DUTY      40     // outside wheel in sharp-turn mode
#define SHARP_IN_DUTY       25     // inside wheel REVERSE duty in sharp-turn mode (0 = just stop it)
#define SLOW_K              0.2f   // base speed reduction per unit of |error| (error is scaled x10, max ~50)
#define NODE_CONFIRM        2
#define NODE_REARM_READS    3
#define END_MIN_SENSORS     4
#define RESUME_READS        3      // consecutive reads with the line visible before resuming from a wait

static const char *TAG = "maze";

const int weights[5] = {-5, -3, 1, 3, 5};

int optimum_duty_cycle = 46;
int lower_duty_cycle = 30;
int higher_duty_cycle = 60;
float left_duty_cycle = 0, right_duty_cycle = 0;

float error = 0, prev_error = 0, difference, cumulative_error, correction;

line_sensor_array line_sensor_readings;

static int64_t start_ms = 0;
static bool    g_line_seen = false;
static int64_t g_acquire_since = 0;
static bool    g_edge = false;           // last read that saw the line had an outer sensor on
static int     g_last_side = 0;          // -1 line was last seen on the left, +1 right, 0 centred
static motor_handle_t motor_a_0, motor_a_1;
static adc_handle_t line_sensor;

/* ================= YOUR PID CODE (unchanged) ================= */

void calculate_correction()
{
    error = error * 10;
    difference = error - prev_error;
    cumulative_error += error;

    cumulative_error = bound(cumulative_error, -30, 30);

    correction = read_pid_const().kp * error + read_pid_const().ki * cumulative_error + read_pid_const().kd * difference;
    prev_error = error;
}

void calculate_error()
{
    int all_black_flag = 1;
    float weighted_sum = 0, sum = 0;
    float pos = 0;
    int k = 0;

    for(int i = 0; i < 5; i++)
    {
        if(sees_line(line_sensor_readings.adc_reading[i]))
        {
            all_black_flag = 0;
            k = 1;
        }
        else
        {
            k = 0;
        }
        weighted_sum += (float)(weights[i]) * k;
        sum = sum + k;
    }

    if(sum != 0)
    {
        pos = (weighted_sum - 1) / sum;
    }

    if(all_black_flag == 1)
    {
        if(prev_error > 0)
        {
            error = 2.5;
        }
        else
        {
            error = -2.5;
        }
    }
    else
    {
        error = pos;
    }
}

/* ================= helpers ================= */

static inline int64_t now_ms(void) { return esp_timer_get_time() / 1000; }

static void set_wheel(motor_handle_t m, float v)
{
    if (v >= 0) set_motor_speed(m, MOTOR_FORWARD,  bound(v, 0, 100));
    else        set_motor_speed(m, MOTOR_BACKWARD, bound(-v, 0, 100));
}

static void drive(float left, float right)
{
    set_wheel(motor_a_0, left);
    set_wheel(motor_a_1, right);
}

static void read_and_normalize(void)
{
    line_sensor_readings = read_line_sensor(line_sensor);
    for(int i = 0; i < 5; i++)
    {
        line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
        line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, bound_LSA_LOW, bound_LSA_HIGH);
        line_sensor_readings.adc_reading[i] = 1000 - (line_sensor_readings.adc_reading[i]);
    }
}

static int get_bits(bool on[5])
{
    int c = 0;
    for (int i = 0; i < 5; i++) {
        on[i] = sees_line(line_sensor_readings.adc_reading[i]);
        c += on[i];
    }
    return c;
}

/* remember which side of the bot the line was last on (call whenever any sensor sees the line) */
static void update_last_side(const bool on[5])
{
    int l = on[0] * 2 + on[1];
    int r = on[4] * 2 + on[3];
    if (l > r)       g_last_side = -1;
    else if (r > l)  g_last_side = +1;
    else if (on[2])  g_last_side = 0;
    /* l == r > 0 with centre off: ambiguous, keep previous value */
}

static void follow_step(const bool on[5], int cnt)
{
    calculate_error();
    calculate_correction();      // always run, so prev_error / integral stay current

    /* sharp-turn mode: line at the far edge, centre dark. Inside wheel goes backwards. */
    if ((on[0] || on[4]) && !on[2] && cnt <= 2 && !(on[0] && on[4])) {
        if (on[0]) {             // line on the left -> turn left -> left wheel is the inside wheel
            left_duty_cycle  = -SHARP_IN_DUTY;
            right_duty_cycle = SHARP_OUT_DUTY;
        } else {                 // line on the right
            left_duty_cycle  = SHARP_OUT_DUTY;
            right_duty_cycle = -SHARP_IN_DUTY;
        }
        drive(left_duty_cycle, right_duty_cycle);
        return;
    }

    /* normal PD: slow the base speed down as the error grows */
    float base = optimum_duty_cycle - SLOW_K * fabsf(error);
    if (base < lower_duty_cycle)   base = lower_duty_cycle;
    if (base > optimum_duty_cycle) base = optimum_duty_cycle;

    left_duty_cycle  = bound((base + correction), lower_duty_cycle, higher_duty_cycle);
    right_duty_cycle = bound((base - correction), lower_duty_cycle, higher_duty_cycle);

    set_motor_speed(motor_a_0, MOTOR_FORWARD, left_duty_cycle);
    set_motor_speed(motor_a_1, MOTOR_FORWARD, right_duty_cycle);
}

static void reset_pid(void)
{
    prev_error = 0;
    cumulative_error = 0;
}

/* Stop and wait until the line is visible RESUME_READS reads in a row, then return.
 * Used instead of ending the task, so repositioning the bot by hand restarts it. */
static void wait_for_line(int max_cnt)
{
    bool on[5];
    int hits = 0;
    int64_t last_log = 0;
    drive(0, 0);
    ESP_LOGW(TAG, "waiting for line (stopped)");
    while (hits < RESUME_READS) {
        read_and_normalize();
        int cnt = get_bits(on);
        hits = (cnt > 0 && cnt <= max_cnt) ? hits + 1 : 0;
        if (now_ms() - last_log >= 1000) {
            last_log = now_ms();
            ESP_LOGW(TAG, "waiting for line... r=[%d %d %d %d %d] cnt=%d",
                     (int)line_sensor_readings.adc_reading[0], (int)line_sensor_readings.adc_reading[1],
                     (int)line_sensor_readings.adc_reading[2], (int)line_sensor_readings.adc_reading[3],
                     (int)line_sensor_readings.adc_reading[4], cnt);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    reset_pid();
    ESP_LOGI(TAG, "line found again, resuming");
}

/* ================= turning ================= */

/* Wait until the centre sensor has had state `want_on` for TURN_CONFIRM reads in a row. */
static bool wait_centre(bool want_on, int64_t t0, int timeout_ms)
{
    bool on[5];
    int hits = 0;
    while (now_ms() - t0 < timeout_ms) {
        read_and_normalize(); get_bits(on);
        if (on[2] == want_on) {
            if (++hits >= TURN_CONFIRM) return true;
        } else {
            hits = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return false;
}

/* dir: -1 = left, +1 = right. Spin until the centre sensor leaves the old line, then finds the next. */
static void pivot(int dir)
{
    drive(dir < 0 ? -TURN_DUTY : TURN_DUTY, dir < 0 ? TURN_DUTY : -TURN_DUTY);

    int64_t t0 = now_ms();
    bool found = wait_centre(false, t0, PIVOT_TIMEOUT_MS) &&
                 wait_centre(true,  t0, PIVOT_TIMEOUT_MS);
    drive(0, 0);

    if (found) {
        ESP_LOGI(TAG, "pivot took %d ms", (int)(now_ms() - t0));
        g_last_side = 0;
    } else {
        ESP_LOGE(TAG, "pivot: line not found after %d ms", (int)(now_ms() - t0));
        wait_for_line(5);
    }
    vTaskDelay(pdMS_TO_TICKS(40));
    reset_pid();
}

/* spin in direction dir (-1 left, +1 right) until the centre sensor sees the line */
static bool spin_find(int dir, int timeout_ms)
{
    drive(dir < 0 ? -TURN_DUTY : TURN_DUTY, dir < 0 ? TURN_DUTY : -TURN_DUTY);
    bool found = wait_centre(true, now_ms(), timeout_ms);
    drive(0, 0);
    return found;
}

/* Line lost. Spin toward the side it was last seen on (drift / right-left corner).
 * If it was last seen centred (true dead end) default to left. If that fails, try the other
 * way for longer, and if that fails too, stop and wait for the line instead of giving up. */
static void recover_line(void)
{
    int dir = (g_last_side == 0) ? -1 : g_last_side;
    int64_t t0 = now_ms();

    ESP_LOGI(TAG, "line lost, spinning %s (last side=%d)", dir < 0 ? "LEFT" : "RIGHT", g_last_side);
    bool found = spin_find(dir, UTURN_TIMEOUT_MS);
    if (!found) {
        ESP_LOGW(TAG, "not found, trying other way");
        found = spin_find(-dir, 2 * UTURN_TIMEOUT_MS);
    }

    if (found) {
        ESP_LOGI(TAG, "recover took %d ms", (int)(now_ms() - t0));
        g_last_side = 0;
    } else {
        ESP_LOGE(TAG, "recover failed after %d ms", (int)(now_ms() - t0));
        wait_for_line(5);
    }
    g_edge = false;
    vTaskDelay(pdMS_TO_TICKS(40));
    reset_pid();
}

/* ================= node handling ================= */

/* preL/preR: side branches already seen on the approach. Returns true if END reached. */
static bool run_node(bool preL, bool preR)
{
    bool on[5];
    bool seenL = preL, seenR = preR;

    drive(CROSS_DUTY, CROSS_DUTY);
    int64_t t0 = now_ms();
    while (now_ms() - t0 < CROSS_MS) {
        read_and_normalize(); get_bits(on);
        seenL |= on[0];
        seenR |= on[4];
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    read_and_normalize();
    int cnt = get_bits(on);

    if (cnt >= END_MIN_SENSORS) {
        if (now_ms() - start_ms > END_ARM_MS) {
            drive(0, 0);
            ESP_LOGI(TAG, "END reached (cnt=%d) - stopped, will resume if a normal line is seen", cnt);
            wait_for_line(END_MIN_SENSORS - 1);   // stays stopped on the wide END box; resumes on a normal line
            start_ms = now_ms();
            g_last_side = 0;
            return false;
        }
        ESP_LOGW(TAG, "wide area but END not armed yet -> keep going");
        reset_pid();
        return false;
    }

    bool S = on[1] || on[2] || on[3];
    ESP_LOGI(TAG, "node  L=%d S=%d R=%d", seenL, S, seenR);

    /* left-hand priority: left, straight, right, else dead end */
    if (seenL)      { ESP_LOGI(TAG, "-> LEFT");  pivot(-1); }
    else if (S)     { ESP_LOGI(TAG, "-> STRAIGHT"); reset_pid(); }
    else if (seenR) { ESP_LOGI(TAG, "-> RIGHT"); pivot(+1); }
    else            { ESP_LOGI(TAG, "-> UTURN"); g_last_side = 0; recover_line(); }

    return false;
}

/* ================= TASK ================= */

void line_follow_task(void* arg)
{
    ESP_ERROR_CHECK(enable_motor_driver(&motor_a_0, MOTOR_A_0));
    ESP_ERROR_CHECK(enable_motor_driver(&motor_a_1, MOTOR_A_1));

    ESP_ERROR_CHECK(enable_line_sensor(&line_sensor));
    ESP_ERROR_CHECK(enable_bar_graph());

#ifdef CONFIG_ENABLE_OLED
    ESP_ERROR_CHECK(init_oled());
    vTaskDelay(100);
    lv_obj_clean(lv_scr_act());
#endif

    bool on[5];
    int node_cnt = 0;
    bool preL = false, preR = false;   // side branches latched while the wide bar is being confirmed
    bool node_armed = true;
    int rearm_cnt = 0;
    int64_t lost_since = 0;
    int64_t last_print = 0;
    start_ms = now_ms();
    g_acquire_since = start_ms;

    while(true)
    {
        read_and_normalize();
        int cnt = get_bits(on);

#if DEBUG_PRINT
        if (now_ms() - last_print >= 100) {
            last_print = now_ms();
            ESP_LOGI(TAG, "r=[%d %d %d %d %d] on=%d%d%d%d%d cnt=%d side=%d err=%.1f duty L=%.0f R=%.0f",
                     (int)line_sensor_readings.adc_reading[0], (int)line_sensor_readings.adc_reading[1],
                     (int)line_sensor_readings.adc_reading[2], (int)line_sensor_readings.adc_reading[3],
                     (int)line_sensor_readings.adc_reading[4],
                     on[0], on[1], on[2], on[3], on[4], cnt, g_last_side, error, left_duty_cycle, right_duty_cycle);
        }
#endif

        /* ---- ACQUIRE (boot only) ---- */
        if (!g_line_seen) {
            static int seen_cnt = 0;
            if (cnt > 0) {
                if (++seen_cnt >= 3) {
                    g_line_seen = true;
                    seen_cnt = 0;
                    start_ms = now_ms();
                    reset_pid();
                    ESP_LOGI(TAG, "line acquired");
                }
            } else {
                seen_cnt = 0;
            }
            if (!g_line_seen) {
                if (now_ms() - g_acquire_since > ACQUIRE_MAX_MS) {
                    ESP_LOGE(TAG, "line not found at boot");
                    wait_for_line(5);             // stops and waits; resumes once the line is seen
                    g_line_seen = true;
                    start_ms = now_ms();
                    node_armed = false; rearm_cnt = 0; node_cnt = 0;
                    continue;
                }
                drive(optimum_duty_cycle, optimum_duty_cycle);
                vTaskDelay(10 / portTICK_PERIOD_MS);
                continue;
            }
        }

        if (cnt > 0) { update_last_side(on); g_edge = (on[0] || on[4]); }

        /* line lost for LOST_MS: drift or dead end -> recover toward the side it was last seen */
        if (cnt == 0) {
            if (!lost_since) lost_since = now_ms();
            int lost_limit = (g_edge && g_last_side != 0) ? LOST_EDGE_MS : LOST_MS;
            if (now_ms() - lost_since >= lost_limit) {
                lost_since = 0;
                recover_line();
                node_armed = false; rearm_cnt = 0; node_cnt = 0; preL = preR = false;
                continue;
            }
        } else {
            lost_since = 0;
        }

        /* node detection with re-arm */
        bool wide = (on[0] || on[4]) && cnt >= 3;
        if (!node_armed) {
            node_cnt = 0; preL = preR = false;
            if (!wide) {
                if (++rearm_cnt >= NODE_REARM_READS) { node_armed = true; rearm_cnt = 0; }
            } else {
                rearm_cnt = 0;
            }
        } else if (wide) {
            preL |= on[0];
            preR |= on[4];
            if (++node_cnt >= NODE_CONFIRM) {
                node_cnt = 0;
                bool end = run_node(preL, preR);
                preL = preR = false;
                node_armed = false; rearm_cnt = 0;
                if (end) break;
                continue;
            }
        } else {
            node_cnt = 0; preL = preR = false;
        }

        follow_step(on, cnt);

#ifdef CONFIG_ENABLE_OLED
        if (read_pid_const().val_changed)
        {
            esp_netif_ip_info_t ip_info;
            esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);

            const char *ip_str_value = ip4addr_ntoa((const ip4_addr_t *)&ip_info.ip);
            display_pid_values(read_pid_const().kp, read_pid_const().ki, read_pid_const().kd, ip_str_value);

            reset_val_changed_pid_const();
        }
#endif

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    drive(0, 0);     // only reached after END
    vTaskDelete(NULL);
}

void app_main()
{
    start_tuning_http_server();
    xTaskCreate(&line_follow_task, "line_follow_task", 6144, NULL, 1, NULL);
}
