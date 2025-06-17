# 📘 Project: Monitoring Sensor TDS, pH, dan Turbidity dengan Arduino Mega

## 🧾 Deskripsi Proyek

Proyek ini bertujuan untuk memonitor kualitas air dengan tiga parameter utama: **TDS**, **pH**, dan **Turbidity**. Sistem menggunakan **Arduino Mega** sebagai mikrokontroler utama, serta **LCD 20x4** sebagai tampilan data secara real-time.

Sensor yang digunakan:

* Sensor TDS Gravity
* Sensor pH Gravity
* Sensor Turbidity

Library yang digunakan:

* `Wire.h`
* `EEPROM.h`
* `LiquidCrystal_I2C.h`
* `GravityWaterQuality.h` (custom library untuk manajemen dan kalibrasi sensor)

## 🔧 Perangkat Keras

* Arduino Mega 2560
* Sensor TDS (DFRobot Gravity)
* Sensor pH (DFRobot Gravity)
* Sensor Turbidity
* LCD 20x4 I2C
* Breadboard dan kabel jumper

## 📦 Instalasi

1. Clone repositori ini:

```bash
git clone https://github.com/SentaFito53/arduino-water-quality-fuzzy-sugeno-monitoring.git
```

2. Pastikan Anda sudah menambahkan custom library `GravityWaterQuality.h` ke dalam folder `libraries` Arduino Anda.

3. Upload program utama ke Arduino Mega menggunakan Arduino IDE.

## 📟 Wiring Schematic

| Komponen         | Arduino Mega Pin |
| ---------------- | ---------------- |
| Sensor TDS       | A2               |
| Sensor pH        | A1               |
| Sensor Turbidity | A0               |
| LCD SDA          | SDA (20)         |
| LCD SCL          | SCL (21)         |

## 🔎 Tampilan di LCD

Baris-baris LCD:

1. pH: `7.00`
2. TDS: `350 ppm`
3. EC: `0.54 ms/cm`
4. Turbidity: `NTU: 120`

## ⚙️ Cara Kalibrasi Sensor

### Kalibrasi TDS

1. Masukkan perintah berikut melalui Serial Monitor Arduino:

```
ENTER
```

2. Celupkan probe ke larutan EC standar (misalnya 1413us/cm), kemudian kirim:

```
CAL:1413
```

3. Setelah muncul `>>>Calibration Successful!<<<`, simpan nilai:

```
EXIT
```

### Kalibrasi pH

1. Masukkan perintah berikut melalui Serial Monitor Arduino:

```
ENTERPH
```

2. Celupkan probe ke buffer pH 7, lalu kirim:

```
CALPH
```

3. Ulangi dengan buffer pH 4, kirim lagi `CALPH`.
4. Setelah muncul pesan berhasil, simpan dengan:

```
EXITPH
```

> **Catatan:** Voltage pH 7 berada antara 1322-1678 mV dan pH 4 antara 1854-2210 mV.

## 🧪 Contoh Output Serial

```
pH: 7.01
TDS: 357 ppm
EC: 0.58 ms/cm
Turbidity: 121 NTU
```

## 🛠 Troubleshooting

* Pastikan semua sensor mendapat tegangan yang sesuai (5V).
* Jika data tampak tidak sesuai, ulangi proses kalibrasi.
* Gunakan cairan standar pH dan EC untuk akurasi lebih tinggi.

## Fungsi Keanggotaan Fuzzy
![Fungsi Keanggotaan Fuzzy](https://github.com/SentaFito53/arduino-water-quality-fuzzy-sugeno-monitoring/blob/main/Fungsi%20Keanggotaan.png)
