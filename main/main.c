/*
 * SOWA Radio — головний файл
 * Фаза 2: UI bring-up
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "hw_init.h"
#include "esp_lv_adapter.h"
#include "lvgl.h"
#include "ui/ui.h"
#include "ui/ui_main.h"

static const char *TAG = "sowa_radio";

/* ── Задача оновлення годинника ─────────────────────────── */
static void lvgl_tick_task(void *arg)
{
    static int h = 12, m = 30, s = 0;
    while (1) {
        if (esp_lv_adapter_lock(-1) == ESP_OK) {
            s++;
            if (s >= 60) { s = 0; m++; }
            if (m >= 60) { m = 0; h = (h + 1) % 24; }
            if (s == 0) {
                ui_main_set_clock(h, m);
            }
            esp_lv_adapter_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ── app_main ────────────────────────────────────────────── */
void app_main(void)
{
    ESP_LOGI(TAG, "SOWA Radio starting...");

    /* NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* LCD init */
    esp_lcd_panel_handle_t    display_panel     = NULL;
    esp_lcd_panel_io_handle_t display_io_handle = NULL;
    esp_lv_adapter_rotation_t rotation          = ESP_LV_ADAPTER_ROTATE_0;
    esp_lv_adapter_tear_avoid_mode_t tear_mode  = ESP_LV_ADAPTER_TEAR_AVOID_MODE_DEFAULT_RGB;

    ESP_LOGI(TAG, "Init LCD %dx%d (RGB)", HW_LCD_H_RES, HW_LCD_V_RES);
    ESP_ERROR_CHECK(hw_lcd_init(&display_panel, &display_io_handle, tear_mode, rotation));

    /* LVGL adapter init */
    esp_lv_adapter_config_t adapter_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(esp_lv_adapter_init(&adapter_cfg));

    /* Реєстрація дисплею */
    esp_lv_adapter_display_config_t disp_cfg =
        ESP_LV_ADAPTER_DISPLAY_RGB_DEFAULT_CONFIG(
            display_panel, display_io_handle,
            HW_LCD_H_RES, HW_LCD_V_RES, rotation);
    lv_display_t *disp = esp_lv_adapter_register_display(&disp_cfg);
    if (disp == NULL) {
        ESP_LOGE(TAG, "Failed to register display");
        return;
    }

    /* Touch init */
    esp_lcd_touch_handle_t touch_handle = NULL;
    
//    ESP_ERROR_CHECK(hw_touch_init(&touch_handle, rotation));
    // СТАЛО:
    esp_err_t touch_ret = hw_touch_init(&touch_handle, rotation);
    if (touch_ret != ESP_OK) {
        ESP_LOGW(TAG, "Touch init failed (0x%x), continuing without touch", touch_ret);
        touch_handle = NULL;
    }

    if (touch_handle != NULL) {
        esp_lv_adapter_touch_config_t touch_cfg =
            ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, touch_handle);
        if (esp_lv_adapter_register_touch(&touch_cfg) == NULL) {
            ESP_LOGW(TAG, "Failed to register touch");
        }
    }    
//
    esp_lv_adapter_touch_config_t touch_cfg =
        ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, touch_handle);
    if (esp_lv_adapter_register_touch(&touch_cfg) == NULL) {
        ESP_LOGE(TAG, "Failed to register touch");
        return;
    }

    /* Старт LVGL */
    ESP_ERROR_CHECK(esp_lv_adapter_start());
    ESP_LOGI(TAG, "LVGL adapter started");

    /* Ініціалізація UI */
    if (esp_lv_adapter_lock(-1) == ESP_OK) {
        ui_init_all(lv_scr_act());
        esp_lv_adapter_unlock();
    }
    ESP_LOGI(TAG, "UI initialized");

    /* Задача годинника */
    xTaskCreate(lvgl_tick_task, "lvgl_tick", 4096, NULL, 5, NULL);

    /* Головний цикл */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
