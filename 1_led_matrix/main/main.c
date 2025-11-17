#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include "led_matrix.h"

static const char *TAG = "led_matrix";

static void blink_all(led_matrix *m)
{
    ESP_ERROR_CHECK(led_matrix_set_data_raw(m, 0xFFFFFFFF));
    ESP_ERROR_CHECK(led_matrix_write(m, LED_MATRIX_OUTPUT_PAR));
    vTaskDelay(pdMS_TO_TICKS(1500));

    ESP_ERROR_CHECK(led_matrix_set_data_raw(m, 0x00000000));
    ESP_ERROR_CHECK(led_matrix_write(m, LED_MATRIX_OUTPUT_PAR));
    vTaskDelay(pdMS_TO_TICKS(200));
}

static void game_of_life(led_matrix *m, int rows, int cols, TickType_t frame_ms)
{
    uint32_t frame = 0;

    if (rows == 5 && cols == 6) {
        // Fixed pattern chosen to give a long, interesting evolution on a 5x6 toroidal grid
        frame = 0x0BFE020C;
    } else if (rows >= 3 && cols >= 3) {
        // Fallback: a horizontal "blinker" centered in the matrix
        int r = rows / 2;
        int c0 = cols / 2 - 1;
        for (int dc = 0; dc < 3; dc++) {
            int c = c0 + dc;
            int idx = r * cols + c;
            frame |= (1UL << idx);
        }
    } else {
        // Fallback: turn on the middle row
        int r = rows / 2;
        for (int c = 0; c < cols; c++) {
            int idx = r * cols + c;
            frame |= (1UL << idx);
        }
    }
    uint32_t next = 0;

    while (1) {
        next = 0;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {

                int idx = r * cols + c;
                int alive = (frame >> idx) & 1;

                int n = 0;
                for (int dr = -1; dr <= 1; dr++)
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue;
                        int rr = (r + dr + rows) % rows;
                        int cc = (c + dc + cols) % cols;
                        int nidx = rr * cols + cc;
                        n += (frame >> nidx) & 1;
                    }

                int new_state = alive ? (n == 2 || n == 3) : (n == 3);

                if (new_state) {
                    next |= (1UL << idx);
                }
            }
        }

        frame = next;

        ESP_ERROR_CHECK(led_matrix_set_data(m, frame));
        ESP_ERROR_CHECK(led_matrix_write(m, LED_MATRIX_OUTPUT_PAR));
        vTaskDelay(pdMS_TO_TICKS(frame_ms));
    }
}

void app_main(void)
{
    led_matrix m = led_matrix_init();
    ESP_LOGI(TAG, "Shift register pins -> SDATA: %d, SRCLK: %d, RCLK: %d",
             m.config.sdata, m.config.srclk, m.config.rclk);

    blink_all(&m);
    game_of_life(&m, CONFIG_LED_MATRIX_ROWS, CONFIG_LED_MATRIX_COLUMNS, 150);
}
