LED Matrix – Game of Life
=========================

This project runs Conway's Game of Life on the SRA Board LED matrix using ESP-IDF.

- At boot, all LEDs blink once to confirm the matrix is wired correctly.
- Then a random initial pattern is generated and evolves according to Game of Life rules.

Build and flash (from this directory):

```bash
idf.py build
idf.py -p <PORT> flash monitor
```

Make sure `CONFIG_LED_MATRIX_ROWS` and `CONFIG_LED_MATRIX_COLUMNS` are set in `menuconfig` to match your board's LED matrix size.
