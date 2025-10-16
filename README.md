# ProtCom

**ProtCom** is an embedded system designed to measure **temperature** and **humidity** using the **AHT20 sensor**, displaying the results on a **16x2 LCD** and supporting **UART-based command control**.

---

## 📘 Overview

ProtCom communicates through **UART**, receiving **case-insensitive commands** to perform various operations such as reading sensor data, resetting the sensor, or displaying help information.

---

## 🧭 Supported Commands

| Command | Description | Example |
|----------|--------------|----------|
| `HELP` | Displays a detailed help message listing all available commands, arguments, and their usage. | `HELP` |
| `GET <OPTION> [UNIT]` | Reads data from the AHT20 sensor. The `<OPTION>` defines which property to measure, and `[UNIT]` defines the temperature unit. | `GET TEMP C` |
| `RESET` | Resets the AHT20 sensor. | `RESET` |

### 🔹 Options for `GET`
- `TEMP` — Reads temperature only.  
- `HUM` — Reads humidity only.  
- `TEMP&HUM` — Reads both temperature and humidity.  

### 🔹 Units for `GET`
- `C` — Celsius (default if omitted)  
- `F` — Fahrenheit  
- `K` — Kelvin  

---

## 📺 Display

Measurement results are shown on a **16x2 LCD**, automatically updating after each successful read operation.

---

## ⚙️ Technical Details

- **Sensor:** AHT20 (I²C communication)  
- **Display:** 16x2 LCD (I²C via PCF8574T)  
- **Interface:** UART (for commands)  
- **Supported Baud Rates:** 9600bs  

