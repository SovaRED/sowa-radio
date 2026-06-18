#pragma once
#include "lvgl.h"
#include <stdbool.h>

/* Структура станції */
typedef struct {
    char name[64];
    char url[256];
} ui_station_t;

/* Ініціалізація панелі станцій (overlay поверх головного екрану) */
void ui_stations_init(lv_obj_t *scr);

/* Показати / сховати панель */
void ui_stations_show(void);
void ui_stations_hide(void);
bool ui_stations_is_visible(void);

/* Оновити список (після завантаження з SD) */
void ui_stations_set_list(const ui_station_t *stations, int count);

/* Позначити поточну станцію */
void ui_stations_set_current(int idx);

/* Callback — виклик при виборі станції */
typedef void (*ui_stations_select_cb_t)(int idx);
void ui_stations_set_select_cb(ui_stations_select_cb_t cb);
