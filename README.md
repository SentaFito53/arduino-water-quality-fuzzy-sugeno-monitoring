# 📘 Project: Monitoring Sensor TDS, pH, dan Turbidity dengan Arduino Mega

## 🧾 Deskripsi Proyek

Proyek ini bertujuan untuk memonitor kualitas air dengan tiga parameter utama: **TDS**, **pH**, dan **Turbidity**. Sistem menggunakan **Arduino Mega** sebagai mikrokontroler utama, serta **LCD 20x4** sebagai tampilan data secara real-time.

Sensor yang digunakan:

* Sensor TDS Gravity
* Sensor pH
* Sensor Turbidity

Library yang digunakan:

* `Wire.h`
* `EEPROM.h`
* `LiquidCrystal_I2C.h`
* `GravityWaterQuality.h` (custom library untuk manajemen dan kalibrasi sensor)

## 🔧 Perangkat Keras

* Arduino Mega 2560
* Sensor TDS (DFRobot Gravity)
* Sensor pH
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

## ⚙️ Cara Kalibrasi Sensor 

### Kalibrasi TDS

1. Masukkan perintah berikut melalui Serial Monitor Arduino:

```
ENTERTDS
```

2. Celupkan probe ke larutan standar (misalnya 1413 mg/L), kemudian kirim:

```
CALTDS:1413
```

3. Setelah muncul `>>>Calibration Successful!<<<`, simpan nilai:

```
EXITTDS
```

### Kalibrasi pH

The pH is computed using a linear model from two reference voltages

1. Masukkan perintah berikut melalui Serial Monitor Arduino:

```
ENTERPH
```

2. Celupkan probe ke buffer pH 7, lalu kirim:

```
PH:<nilai ph>
```

3. Ulangi dengan buffer pH 4, kirim lagi `PH:<nilai ph>`.
4. Setelah muncul pesan berhasil, simpan dengan:

```
EXITPH
```
> **Catatan:** Voltage pH 7 berada antara 1322-1678 mV dan pH 4 antara 1854-2210 mV.


### Kalibrasi Turbidity

1. Masukkan perintah berikut melalui Serial Monitor Arduino:

```
ENTERTURB
```

2. Celupkan sensor ke air jernih (kalibrasi titik pertama), lalu kirim:

```
TURB:<nilai turb>
```

3. Ulangi dengan air keruh (kalibrasi titik kedua), kirim lagi `TURB:<nilai turb>`.
4. Setelah muncul pesan berhasil, simpan dengan:

```
EXITTURB
```

> **Catatan:** Kalibrasi harus dilakukan pada dua titik kalibrasi (dua kondisi kekeruhan air).

## 🧪 Contoh Output LCD

```
pH   : 7.01
Turb : 133 NTU
TDS  : 100 ppm
Qual : BAIK
```

## 🛠 Troubleshooting

* Pastikan semua sensor mendapat tegangan yang sesuai (5V).
* Jika data tampak tidak sesuai, ulangi proses kalibrasi.
* Gunakan cairan standar pH dan TDS untuk akurasi lebih tinggi.

## Fungsi Keanggotaan Fuzzy
![Fungsi Keanggotaan Fuzzy](https://github.com/SentaFito53/arduino-water-quality-fuzzy-sugeno-monitoring/blob/main/Fungsi%20Keanggotaan.png)
