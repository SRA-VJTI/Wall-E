# LED Matrix — Conway's Game of Life

This example uses the SRA Board's **8 × 8 LED matrix**, driven by a **MAX7219** LED driver, to display Conway's Game of Life. It first turns on all 64 LEDs as a quick display check and then evolves a fixed starting pattern.

## How the MAX7219 works

The MAX7219 is an LED-display driver. Instead of having the ESP32 directly control and repeatedly refresh all 64 LEDs, the ESP32 sends commands to the MAX7219, which stores the display data and continuously multiplexes the LEDs. The chip also provides brightness control.

The SRA Board communicates with the MAX7219 through three signals defined in [`pin_defs.h`](../components/sra-board-component/include/pin_defs.h):

| Signal | Purpose |
| --- | --- |
| `DIN` | Serial data from the ESP32 to the MAX7219 |
| `CLK` | Clocks each data bit into the driver |
| `LOAD` | Latches a completed command into the driver's register |

Each command consists of **16 bits**: an 8-bit register address followed by an 8-bit value. The driver sends these bytes through SPI and pulses `LOAD` to apply the command.

For an 8 × 8 matrix, the important MAX7219 registers are:

| Address | Register | Meaning |
| --- | --- | --- |
| `0x01`–`0x08` | Digit 0–7 | Eight LED on/off bits for each selected digit line |
| `0x09` | Decode mode | `0x00` disables BCD decoding so individual LED bits can be controlled |
| `0x0A` | Intensity | Brightness level `0x00`–`0x0F` |
| `0x0B` | Scan limit | `0x07` enables scanning all eight digit lines |
| `0x0C` | Shutdown | `0x01` enables normal display operation; `0x00` shuts down display scanning |
| `0x0F` | Display test | `0x00` disables the all-LEDs test mode |

The `led_matrix_init()` function configures these registers for normal 8 × 8 matrix operation. Application code should use the matrix API instead of writing MAX7219 registers directly.

## How hexadecimal values control the LEDs

Each displayed *logical row* contains eight pixels, so a single `uint8_t` can represent the entire row. Each bit controls one LED:

- `1` means **on**; `0` means **off**.
- **Bit 7** (the most-significant bit) is the **leftmost** pixel.
- **Bit 0** (the least-significant bit) is the **rightmost** pixel.

For example, the logical row value `0x81` is `10000001` in binary, so only its first and last LEDs light up:

```text
Bit position:   7 6 5 4 3 2 1 0
Binary:         1 0 0 0 0 0 0 1
LEDs:           # . . . . . . #
Hexadecimal:    0x81
```

Other examples:

| Hex value | Binary | Appearance (`#` = on) |
| --- | --- | --- |
| `0x00` | `00000000` | `........` |
| `0xFF` | `11111111` | `########` |
| `0x18` | `00011000` | `...##...` |
| `0xAA` | `10101010` | `#.#.#.#.` |

An entire 8 × 8 picture can therefore be represented by an array of eight bytes:

```c
led_matrix_t matrix;
ESP_ERROR_CHECK(led_matrix_init(&matrix));

uint8_t bitmap[LED_MATRIX_HEIGHT] = {
    0x81, // #......#
    0x42, // .#....#.
    0x24, // ..#..#..
    0x18, // ...##...
    0x18, // ...##...
    0x24, // ..#..#..
    0x42, // .#....#.
    0x81  // #......#
};

ESP_ERROR_CHECK(led_matrix_draw_bitmap(&matrix, bitmap));
ESP_ERROR_CHECK(led_matrix_show(&matrix));
```

This draws an **X**. `led_matrix_draw_bitmap()` changes the in-memory framebuffer; `led_matrix_show()` sends it to the hardware.

**Important board-specific detail:** The SRA Board's MAX7219 digit registers address **physical columns**, not logical rows. In its hardware mapping, register `0x01` corresponds to the leftmost column, with bit 7 at the top and bit 0 at the bottom. The component's `led_matrix_show()` function **transposes the logical row bitmap into this physical column layout** before sending it over SPI. You should supply normal top-to-bottom rows as shown above; do **not** transpose them yourself. For example, the low-level register/value pair `0x01, 0x80` turns on the top LED of the first physical column.

## LED matrix API

The example includes `led_matrix.h`, provided by the `sra-board-component` submodule.

| Function | Purpose |
| --- | --- |
| `led_matrix_init(&matrix)` | Initialize the MAX7219, SPI connection and framebuffer (default brightness: 4) |
| `led_matrix_set_pixel(&matrix, x, y, on)` | Set one buffered pixel, with `x` and `y` from `0` to `7` |
| `led_matrix_set_row(&matrix, row, pixels)` | Set one buffered logical row using an 8-bit value |
| `led_matrix_draw_bitmap(&matrix, bitmap)` | Copy eight logical row bytes into the framebuffer |
| `led_matrix_show(&matrix)` | Send the framebuffer to the physical display |
| `led_matrix_clear(&matrix)` | Clear the framebuffer and update the display immediately |
| `led_matrix_set_brightness(&matrix, value)` | Adjust brightness from `0` to `15` |

`led_matrix_set_pixel()`, `led_matrix_set_row()` and `led_matrix_draw_bitmap()` only update the framebuffer; call `led_matrix_show()` afterward to display those changes.

See [`led_matrix.h`](../components/sra-board-component/include/led_matrix.h) for the full API, including display enable/disable and deinitialization.

## What this example does

The firmware in [`main/main.c`](main/main.c) performs these steps:

1. Initialize the matrix using `led_matrix_init()`.
2. Turn **all 64 LEDs on for 1,500 ms**, then clear the display for 200 ms.
3. Start Conway's Game of Life from a **fixed** 64-bit pattern (`0x84536184DB32C529`).
4. For each cell, count the eight surrounding neighbors. The edges **wrap around** (a toroidal 8 × 8 grid).
5. A living cell survives with two or three living neighbors; a dead cell becomes alive with exactly three neighbors.
6. Convert the next generation into eight row bytes, display them with `led_matrix_draw_bitmap()` and `led_matrix_show()`, and repeat every **150 ms**.

The initial pattern is deterministic; it is **not randomly generated** on each boot. See [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) for the evolution rules.

## Build and flash

Clone the repository with its submodules, activate your working ESP-IDF environment, and run these commands **from `1_led_matrix/`**:

```bash
git clone --recursive https://github.com/SRA-VJTI/Wall-E.git
cd Wall-E/1_led_matrix
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Replace `/dev/ttyUSB0` with your board's serial port. To stop the monitor, press **Ctrl+]**.

The current example targets **ESP32-S3** by default, via `sdkconfig.defaults`. The matrix size is fixed at **8 × 8** in `led_matrix.h`; there are no `CONFIG_LED_MATRIX_ROWS` or `CONFIG_LED_MATRIX_COLUMNS` options to set for this example.

For the MAX7219 register definitions and serial-interface details, refer to the [MAX7219/MAX7221 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX7219-MAX7221.pdf).
