# 🦉 SOWA Radio

**Автономний настільний інтернет-радіоприймач на ESP32-S3**

Вмикаєш — грає. Сенсорний екран, зрозумілий для бабусі, гнучкий для гіка.

![License](https://img.shields.io/badge/license-CC%20BY--NC%204.0-blue)
![Platform](https://img.shields.io/badge/platform-ESP32--S3-green)
![Framework](https://img.shields.io/badge/framework-ESP--IDF%20v5.5.4-orange)
![UI](https://img.shields.io/badge/UI-LVGL%20v9-purple)

---

## 📺 Демо

> *(фото/відео пристрою — додати після збірки)*

---

## ✨ Можливості

- 🎵 Потокове відтворення інтернет-радіо (MP3, AAC)
- 📺 Сенсорний екран 4.3" IPS 800×480
- 🌙 Темний UI з живою спектрограмою
- ⏰ Годинник SevenSeg з синім glow
- 📋 Список станцій — swipe-up або кнопка меню
- 🔊 Слайдер гучності з drag-badge
- 🌐 Веб-інтерфейс `radio.local` для керування з браузера
- 📁 Вся конфігурація на SD карті у відкритих форматах
- 🔄 OTA оновлення прошивки через веб
- 📻 Імпорт/експорт M3U плейлистів

---

## 🔧 Апаратура

| Компонент | Деталі |
|-----------|--------|
| Плата | Viewe UEDX80480043E-WB-A |
| МК | ESP32-S3 (240MHz, 8MB PSRAM Octal) |
| Дисплей | 4.3" IPS 800×480 RGB, ST7262E43 |
| Тач | GT911 (capacitive, I2C) |
| Аудіо | Зовнішній I2S DAC (моно) |
| Сховище | microSD |
| Мережа | WiFi 2.4GHz |

---

## 🛠 Технічний стек

| Шар | Технологія |
|-----|------------|
| Фреймворк | ESP-IDF v5.5.4 |
| UI | LVGL v9 |
| BSP | Viewe esp_lvgl_adapter |
| Аудіо | ESP-ADF: http_stream → mp3/aac → i2s |
| Веб | ESP HTTP Server + WebSocket + mDNS |
| Час | SNTP |
| OTA | ESP HTTPS OTA |

---

## 📁 Структура проекту

```
sowa-radio/
├── main/
│   ├── main.c              # app_main, ініціалізація
│   ├── ui/
│   │   ├── ui_theme.h      # кольори, розміри, константи
│   │   ├── ui_main.h/c     # головний екран
│   │   ├── ui_spectrum.h/c # спектрограма (canvas)
│   │   ├── ui_stations.h/c # панель станцій
│   │   └── ui_settings.h/c # налаштування
│   ├── fonts/              # LVGL шрифти (SevenSeg, Roboto)
│   ├── audio/              # аудіо pipeline (Фаза 3)
│   └── data/               # робота з SD картою (Фаза 4)
├── common/
│   └── hw_init/            # BSP: дисплей, тач, PSRAM
├── design/
│   └── radio_ui_v5.html    # референс дизайну UI
├── INFO.md                 # детальний опис проекту
├── ROADMAP.md              # план розробки та прогрес
└── CMakeLists.txt
```

---

## 🚀 Швидкий старт

### Вимоги

- ESP-IDF v5.5.4
- Плата Viewe UEDX80480043E-WB-A
- microSD карта

### Збірка та прошивка

```bash
# Клонувати репо
git clone https://github.com/SovaRED/sowa-radio.git
cd sowa-radio

# Встановити таргет
idf.py set-target esp32s3

# Зібрати
idf.py build

# Прошити (замінити PORT на свій)
idf.py -p PORT flash monitor
```

### Конфігурація SD карти

Створи файли на SD карті:

**`/stations.json`**
```json
{
  "stations": [
    {
      "name": "Radio NV",
      "url": "https://online.nv.ua/nv-audio.mp3"
    },
    {
      "name": "Promodo Radio",
      "url": "https://online.promodj.com/promodj.128.mp3"
    }
  ]
}
```

**`/config.json`**
```json
{
  "volume": 65,
  "last_station": 0
}
```

WiFi credentials вводяться при першому запуску через onboarding екран.

---

## 🗺 Roadmap

| Фаза | Назва | Статус |
|------|-------|--------|
| 1 | Hardware bring-up | ✅ Завершено |
| 2 | LVGL UI | 🔄 В процесі |
| 3 | Аудіо (WiFi + HTTP stream + I2S) | ⏳ |
| 4 | SD карта + дані | ⏳ |
| 5 | Веб-інтерфейс (radio.local) | ⏳ |
| 6 | Полірування | ⏳ |

Детальний план: [ROADMAP.md](ROADMAP.md)

---

## 📄 Ліцензія

© 2026 (SovaRED), Київ, Україна

Цей проект розповсюджується на умовах **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)**.

**Некомерційне використання** — вільно за умови зазначення автора.

**Комерційне використання** (продаж пристроїв, використання в комерційних продуктах, використання коду або ідей у комерційних проектах) — **заборонено без письмового дозволу автора**.

Для отримання комерційної ліцензії звертайтесь: **shashel777@gmail.com**

[![CC BY-NC 4.0](https://licensebuttons.net/l/by-nc/4.0/88x31.png)](https://creativecommons.org/licenses/by-nc/4.0/)

---

## 🤝 Контрибуція

Issues та Pull Requests вітаються для некомерційних покращень.
При будь-якому використанні або модифікації — зазнач автора оригіналу.

---

*Зроблено в Україні 🇺🇦*
