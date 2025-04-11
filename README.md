# Matter ESP32 Playground

Test and experimentation playground for running Matter (formerly CHIP) on ESP32 using the Arduino framework.  
Use this project to explore commissioning, control endpoints, and learn Matter device basics.

---

## 🛠️ Setup Instructions

### ✅ Prerequisites

- **Arduino IDE 2.x** (recommended)
- **ESP32 board** (e.g., ESP32-DevKitC, ESP32-C3)
- **ESP32 Arduino Core v2.0.11+** (via Boards Manager)
- **Matter Arduino Library** from Espressif
- USB cable and serial monitor

---

### ⚙️ 1. Set Partition Scheme to "Huge App"

To run Matter successfully, you **must** set the correct partition scheme due to binary size.

#### Steps:

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
| **Core Debug Level**| None                        |

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
