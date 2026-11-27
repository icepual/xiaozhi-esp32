### N16R8 Terminal 1.54-inch variant

This variant targets an ESP32-S3 N16R8 breadboard with a **240×240 SPI ST7789**
display, an INMP441 microphone, and an I2S amplifier such as the MAX98357A.
Verify the controller, amplifier and pinout against your kit before flashing.

With ESP-IDF active:

```sh
python3 scripts/build.py bread-compact-wifi-lcd \
  --name bread-compact-wifi-lcd-n16r8-terminal-154
```

The variant uses the board's existing pinout:

- LCD MOSI/SDA 47, SCLK/SCL 21, DC 40, RST 45, CS 41, backlight 42.
- INMP441 WS 4, SCK 5, SD 6.
- I2S amplifier DIN 7, BCLK 15, LRCLK 16.

Check module supply ratings and share ground. The variant ships with the
Terminal face enabled, 16 MB flash partitions and octal PSRAM. A module
without a CS pin may require a different display variant. For the initial
installation, use the full flash image so the partition table is installed.
