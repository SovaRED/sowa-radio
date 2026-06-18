/*
 * radio_player — головний файл
 * Фаза 2: UI bring-up (головний екран, спектрограма, заглушки аудіо/SD)
 *
 * Базується на Viewe lvgl_v9_demo_4_3inch BSP.
 * BSP ініціалізує: дисплей (ST7262E43 RGB), тач (GT911), PSRAM.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"

/* LVGL + BSP */
#include "lvgl.h"
#include "esp_lvgl_port.h"

/* UI */
#include "ui/ui.h"

static const char *TAG = "radio";

/* ── Налаштування BSP (скопійовано з demo) ─────────────── */
/* Всі параметри дисплею/тачу вже задані в sdkconfig через
   Viewe sdkconfig.defaults.esp32s3 — нічого міняти не треба */

/* ── Таймер оновлення годинника ────────────────────────── */
static void clock_timer_cb(void *arg)
{
    /* Отримати час з системи (після NTP синхронізації — Фаза 4) */
    /* Поки — просто лічильник для демо */
    static int fake_h = 12, fake_m = 0, fake_s = 0;
    fake_s++;
    if (fake_s >= 60) { fake_s = 0; fake_m++; }
    if (fake_m >= 60) { fake_m = 0; fake_h = (fake_h + 1) % 24; }

    /* Оновлення UI потрібно виконувати в LVGL task */
    /* Тут — просто збереження; ui_main_set_clock викликати з lvgl task */
    (void)fake_h; (void)fake_m;
}

/* ── LVGL task ─────────────────────────────────────────── */
static void lvgl_tick_task(void *arg)
{
    static int tick_h = 12, tick_m = 30, tick_s = 0;

    while (1) {
        /* Захоплення mutex LVGL */
        if (lvgl_port_lock(0)) {
            /* Оновити годинник кожну хвилину */
            tick_s++;
            if (tick_s >= 60) {
                tick_s = 0;
                tick_m++;
                if (tick_m >= 60) { tick_m = 0; tick_h = (tick_h + 1) % 24; }
                ui_main_set_clock(tick_h, tick_m);
            }

            lvgl_port_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ── app_main ──────────────────────────────────────────── */
void app_main(void)
{
    ESP_LOGI(TAG, "Radio Player starting...");

    /* NVS (потрібно для WiFi credentials — Фаза 3) */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* BSP ініціалізація (дисплей + тач + PSRAM вже налаштовані в sdkconfig) */
    /* esp_lvgl_port ініціалізує LVGL і створює display handle */
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    /* Отримати активний екран і побудувати UI */
    if (lvgl_port_lock(0)) {
        lv_obj_t *scr = lv_scr_act();
        ui_init_all(scr);
        lvgl_port_unlock();
    }

    ESP_LOGI(TAG, "UI initialized");

    /* Задача оновлення годинника */
    xTaskCreate(lvgl_tick_task, "lvgl_tick", 4096, NULL, 5, NULL);

    /* Головний цикл — поки порожній (Фаза 3 додасть аудіо pipeline) */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
