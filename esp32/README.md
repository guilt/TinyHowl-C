# ESP32-S3 target

TinyHowl-C is written so the same `src/howl.c` can run on an S3.

Not wired to IDF in v0.1. This file is the contract for the port.

## Why S3

- 240 MHz Xtensa, enough for 3-formant + noise at 16 kHz
- I2S0 TX for a cheap MAX98357A / PCM5102 DAC
- No PSRAM required: one second of PCM16 mono is 32 KB
- Frame 128 samples = 8 ms. Fill from a control task, DMA the rest.

## Pins (starter, change to match your board)

```
I2S BCLK  GPIO 15
I2S WS    GPIO 16
I2S DOUT  GPIO 17
GND / 3V3 to the DAC
```

PDM / I2S RX is TinyEar-C's problem.

## Loop

```
howl_primitive("a", buf, n);
i2s_channel_write(..., buf, n * 2, ...);
```

Keep `HOWL_RATE 16000` and `HOWL_FRAME 128`. Do not resample on the watch.

## IDF sketch

Add `src/howl.c` + `include/howl.h` as a component. `src/main.c` stays host-only
(`fopen`). An `esp32/main.c` will replace it when someone lands the I2S write.
