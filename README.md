# Matter ESP32 Playground

Test and experimentation playground for running Matter Protocol (formerly CHIP) on ESP32 using the Arduino framework. Use this project to explore commissioning, control endpoints, and learn Matter device basics.

There are more example files provided by Arduino IDE as well in case you want to test them first.

![image](https://github.com/user-attachments/assets/9e614d6d-5294-4fcf-a9f3-1c639e8970fc)

---

## 🛠️ Setup Instructions

### ✅ Prerequisites

- **Arduino IDE 2.x** (recommended)
- **Any ESP32 Microcontroller** (e.g., ESP32-DevKitC, ESP32 WROOM 32E, ESP32-C3)
- **ESP32 v3.20+** (via Boards Manager)
- **Matter Arduino Library** from Espressif
- USB cable and serial monitor

---

### ⚙️ 1. Set Partition Scheme to "Huge App"

To run Matter successfully, you **must** set the correct partition scheme due to binary size.

#### Steps:

Since Matter uses a significant amount of program (flash) memory, it's imperative to allocate the correct memory partitions for your board. Otherwise, the code may fail to compile or run properly. I highly recommend creating a custom partition scheme, as this allows you to make better use of the ESP32's resources when working with the Arduino framework.

See how to make custom partitions on Arduino IDE the simple way: https://youtu.be/EuHxodrye6E

1. Open Arduino IDE.
2. Go to `Tools > Partition Scheme`.
3. Select: `Huge App (3MB No OTA)`  
   🔸 This increases the space available for your sketch.

4. Recommended Flash Size:
   - Go to `Tools > Flash Size` and choose `4MB (32Mb)` or more.

---

### 🧱 2. Recommended Board Settings

| Option             | Value                        |
|--------------------|------------------------------|
| **Board**          | ESP32 Dev Module             |
| **Partition Scheme** | Huge App (3MB No OTA)     |
| **Flash Frequency** | 80 MHz                      |
| **Upload Speed**    | 921600 (or 115200 if unstable) |

---

### 🚀 3. Upload & Run

1. Plug in your ESP32.
2. Open the `.ino` sketch.
3. Click **Upload**.
4. Open **Serial Monitor** at `115200 baud`.

You'll see output like:

```plaintext
WiFi connected
IP address: 192.168.x.x
Matter Node is not commissioned yet.
Manual pairing code: 34970112345
QR code: https://...
```
You'll have to scan or paste the QR code on a local browser connected via the same network as your ESP32. Then you just setup your matter device on apple homekit, alexa home and etc.

Since Matter uses a significant amount of flash memory due to the large binary size of the Matter Arduino library, I highly recommend using a chip with at least 8MB of flash to future-proof against memory limitations during development.
