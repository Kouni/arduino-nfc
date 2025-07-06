# Arduino NFC Diagnostic Tool

An Arduino-based NFC card reader and data clearing tool using the MFRC522 module. This project can detect various NFC card types, display detailed specifications, and securely clear card data.

## Features

- **Card Type Detection**: Automatically identifies MIFARE Classic, MIFARE Ultralight, NTAG series
- **Data Clearing**: Safely clears user data while preserving system settings
- **Audio Feedback**: Buzzer notifications for card detection and operation completion
- **Detailed Analysis**: Shows UID, capacity, sectors/pages, and NDEF support

## Hardware Requirements

### Components
- Arduino Uno/Nano (or compatible)
- MFRC522 RFID/NFC module
- Buzzer (optional, for audio feedback)
- Breadboard and jumper wires

### Pin Connections

| Component | Arduino Pin | Notes |
|-----------|-------------|-------|
| **MFRC522 Module** | | |
| VCC | 3.3V | ⚠️ **IMPORTANT: Use 3.3V, NOT 5V** |
| RST | Pin 9 | Reset pin |
| GND | GND | Ground connection |
| MISO | Pin 12 | SPI Master In Slave Out |
| MOSI | Pin 11 | SPI Master Out Slave In |
| SCK | Pin 13 | SPI Clock |
| SS/SDA | Pin 10 | SPI Slave Select |
| **Buzzer (Optional)** | | |
| Positive (+) | Pin 8 | Audio feedback |
| Negative (-) | GND | Ground connection |

### Wiring Diagram
```
Arduino Uno          MFRC522 Module
┌─────────────┐     ┌─────────────┐
│     3.3V    │────→│     VCC     │
│     Pin 9   │────→│     RST     │
│     GND     │────→│     GND     │
│     Pin 12  │────→│     MISO    │
│     Pin 11  │────→│     MOSI    │
│     Pin 13  │────→│     SCK     │
│     Pin 10  │────→│   SS/SDA    │
│             │     └─────────────┘
│     Pin 8   │────→  Buzzer (+)
│     GND     │────→  Buzzer (-)
└─────────────┘
```

## Supported Card Types

| Card Type | Capacity | NDEF Support |
|-----------|----------|--------------|
| MIFARE Classic 1K | 1KB | Yes (needs format) |
| MIFARE Classic 4K | 4KB | Yes (needs format) |
| MIFARE Ultralight | 64 bytes | Yes |
| NTAG213 | 180 bytes user data | Yes |
| NTAG215 | 540 bytes user data | Yes |
| NTAG216 | 928 bytes user data | Yes |

## Installation

### Using Arduino CLI
```bash
# Install MFRC522 library
arduino-cli lib install "MFRC522"

# Compile the sketch
arduino-cli compile --fqbn arduino:avr:uno arduino-nfc.ino

# Upload to Arduino (replace port as needed)
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn arduino:avr:uno arduino-nfc.ino

# Monitor serial output
arduino-cli monitor -p /dev/cu.usbmodem1101 -c baudrate=9600
```

### Using Arduino IDE
1. Install the MFRC522 library via Library Manager
2. Open `arduino-nfc.ino`
3. Select correct board and port
4. Upload the sketch
5. Open Serial Monitor (9600 baud)

## Usage

### Operation Steps
1. **Power On**: Arduino initializes and tests MFRC522 connection
2. **Place Card**: Put NFC card near the reader (1-3cm distance)
3. **Detection**: Buzzer beeps once when card is detected
4. **Analysis**: System shows card type, UID, and specifications
5. **Data Clearing**: Attempts to clear user data areas
6. **Completion**: Buzzer beeps twice when operation is complete
7. **Next Card**: Remove card and place a new one for testing

### Audio Feedback
- **1 Beep**: Card detected, analysis starting
- **2 Beeps**: Operation complete, safe to remove card

## Troubleshooting

**No card detection:**
- Check 3.3V power supply (NOT 5V)
- Verify all pin connections
- Ensure card is within 1-3cm of reader

**Authentication failures:**
- Normal for protected cards
- Default keys (FF FF FF FF FF FF) used

**No buzzer sound:**
- Verify buzzer polarity and Pin 8 connection
- Program works without buzzer

## License

GNU GENERAL PUBLIC LICENSE Version 2

---

**⚠️ Important Safety Notes:**
- Always use 3.3V for MFRC522 power
- Never connect 5V to the module
- This tool is for diagnostic and educational purposes