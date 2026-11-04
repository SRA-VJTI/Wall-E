#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include "led_matrix.h"
#include "pin_defs.h"

static const char *TAG = "led_matrix";

uint8_t rows[LED_MATRIX_HEIGHT] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static void blink_all(led_matrix_t *m)
{
    ESP_ERROR_CHECK(led_matrix_draw_bitmap(m, rows));
    ESP_ERROR_CHECK(led_matrix_show(m));
    vTaskDelay(pdMS_TO_TICKS(1500));

    ESP_ERROR_CHECK(led_matrix_clear(m));
    vTaskDelay(pdMS_TO_TICKS(200));
}

static void game_of_life(led_matrix_t *m, int rows, int cols, TickType_t frame_ms)
{
    // Fixed pattern chosen to give a long, interesting evolution
    // on the 8x8 toroidal grid used by this project.
    uint64_t frame = UINT64_C(0x84536184DB32C529);
    uint64_t next = UINT64_C(0);

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
                    next |= (UINT64_C(1) << idx);
                }
            }
        }

        frame = next;

        uint8_t bitmap[LED_MATRIX_HEIGHT] = {0};

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if ((frame >> (r * cols + c)) & UINT64_C(1)) {
                    bitmap[r] |= (1U << (cols - 1 - c));
                }
            }
        }
        ESP_ERROR_CHECK(led_matrix_draw_bitmap(m, bitmap));
        ESP_ERROR_CHECK(led_matrix_show(m));
        vTaskDelay(pdMS_TO_TICKS(frame_ms));
    }
}

void app_main(void)
{
    led_matrix_t m;
    ESP_ERROR_CHECK(led_matrix_init(&m));
    ESP_LOGI(TAG, "MAX7219 pins -> DIN: %d, CLK: %d, LOAD: %d", LED_MATRIX_DIN, LED_MATRIX_CLK, LED_MATRIX_LOAD);
    blink_all(&m);
    game_of_life(&m, LED_MATRIX_HEIGHT, LED_MATRIX_WIDTH, 150);
}
