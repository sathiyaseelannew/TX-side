# STM32F446RE + NRF24L01 + BMP280 (Transmitter)

This project implements a wireless sensor transmitter using STM32F446RE.
It reads temperature and pressure from the BMP280 sensor and sends the data wirelessly using NRF24L01.

## Hardware Used

* STM32F446RE
* NRF24L01
* BMP280

## Features

* Reads temperature and pressure via SPI (BMP280)
* Converts float values into integer format
* Packs data into struct (8 bytes)
* Transmits data wirelessly using NRF24L01
* Bare-metal (register-level) implementation

## Data Format

```
typedef struct {
    uint32_t temp;
    uint32_t press;
} Data;
```

* Temperature and pressure are scaled (value × 100)
* Total payload size = 8 bytes

## Working

1. STM32 reads BMP280 sensor values
2. Data is converted and packed into struct
3. NRF24L01 transmits the payload via SPI
4. Data is sent every 500 ms

## Pin Connections

### NRF24L01 (SPI1)

| NRF  | STM32 |
| ---- | ----- |
| VCC  | 3.3V  |
| GND  | GND   |
| CE   | PA8   |
| CSN  | PA4   |
| SCK  | PA5   |
| MOSI | PA7   |
| MISO | PA6   |

### BMP280 (SPI)

| BMP280 | STM32         |
| ------ | ------------- |
| SCK    | PA5           |
| MOSI   | PA7           |
| MISO   | PA6           |
| CS     | GPIO (custom) |

## Notes

* NRF payload size must match receiver (8 bytes)
* NRF does not reset automatically, so status registers are cleared during init
* Correct pointer usage (`&data`) is required for transmission

## Future Improvements

* Add CRC or checksum for data validation
* Multi-node wireless communication
* Gateway integration (ESP32 / Cloud)
