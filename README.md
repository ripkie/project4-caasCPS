# 📡 ESP32 IoT Trigger System (Tema 3)

## 📌 Deskripsi
Project ini merupakan bagian dari **Tema 3: Face Detection Login Web App + IoT Control**.

ESP32 berfungsi sebagai **device IoT** yang menerima perintah dari backend (NestJS) melalui HTTP, kemudian mengontrol:
- 🔌 Relay (misalnya untuk pintu)
- 💡 LED (indikator status)
- 🔊 Buzzer (notifikasi)

---

## ⚙️ Fitur
- 🌐 HTTP Server di ESP32
- 🔄 Auto reconnect WiFi
- 📡 Endpoint untuk kontrol device
- 🔔 Notifikasi buzzer (success & error)
- 💡 Indikator LED
- 🔌 Kontrol relay

---

## 🧠 Arsitektur Sistem
Browser (Face Detection)
↓
NestJS API
↓
HTTP POST /trigger
↓
ESP32
↓
Relay + LED + Buzzer


---

## 🔌 Hardware

| Komponen | Pin ESP32 |
|----------|----------|
| Relay    | GPIO 5   |
| LED      | GPIO 8   |
| Buzzer   | GPIO 6   |

### Catatan:
- Buzzer menggunakan **low level trigger**
- Relay bisa active HIGH atau LOW (sesuaikan di kode)
- LED wajib pakai resistor

---

## 📡 Konfigurasi WiFi

Edit di kode:
```cpp
const char *WIFI_SSID = "NAMA_WIFI";
const char *WIFI_PASSWORD = "PASSWORD_WIFI";
```
--- 

## 🚀 Endpoint API
### 1. Status ESP32
GET /status

Response:
```json
{
  "status": "online",
  "ip": "192.168.x.x"
}
```