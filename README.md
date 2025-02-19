# 🔥 Temperature Control System with PID - Arduino Project

## 📌 Overview
This project is an **Arduino-based temperature control system** that uses a **PID controller** to maintain a target temperature. The system includes:
- **LCD Display** for real-time temperature readings and menu navigation.
- **Buttons** for user input to set temperature and timing.
- **MOSFET Control** for heating elements.
- **EEPROM Storage** to save settings.

## 🛠️ Features
- 🔹 **PID-based temperature control** for precise heating.
- 🔹 **User interface with buttons** (OK, Cancel, Previous, Next) to set temperature and duration.
- 🔹 **LCD display output** for status updates and user guidance.
- 🔹 **EEPROM integration** to store user-defined settings.
- 🔹 **Three-phase process**: Heating, Maintaining, Cooling.

## 🔧 Hardware Requirements
- ✅ **Arduino Board** (Uno, Mega, or similar)
- ✅ **LCD Display** (16x2 or 20x4)
- ✅ **Temperature Sensor** (e.g., LM35, DS18B20)
- ✅ **MOSFET** (for controlling heating elements)
- ✅ **Push Buttons** (OK, Cancel, Prev, Next)
- ✅ **Power Supply** (suitable for the heating element)

## 🔨 Code Overview
### 📌 Main Components
- **PID Controller (`PID_v1.h`)**: Handles precise temperature adjustments.
- **LCD Display (`LiquidCrystal.h`)**: Provides real-time status updates.
- **EEPROM (`EEPROM.h`)**: Saves user-defined temperature settings.
- **State Machine**: Manages user navigation through different menus.

### 📜 Key Variables
- **`tempC`** → Current temperature reading.
- **`targetTemp`** → User-defined target temperature.
- **`timp_incalzire`** → Heating duration.
- **`timp_mentinere`** → Hold temperature duration.
- **`timp_racire`** → Cooling duration.
