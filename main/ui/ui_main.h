#pragma once
#include "lvgl.h"

/* Стани відтворення */
typedef enum {
    UI_PLAY_STATE_STOPPED = 0,
    UI_PLAY_STATE_PLAYING,
    UI_PLAY_STATE_PAUSED,
} ui_play_state_t;

/* Стани статусбару */
typedef enum {
    UI_STATUS_OK = 0,     /* показує IP */
    UI_STATUS_BUFFERING,  /* крутилка + "Буферизація" */
    UI_STATUS_CONNECTING, /* крутилка + "Підключення" */
    UI_STATUS_NO_WIFI,    /* ⚠ Немає мережі */
    UI_STATUS_ERROR,      /* ⚠ Помилка потоку */
} ui_status_t;

/* ── Ініціалізація ──────────────────────── */
void ui_main_init(lv_obj_t *scr);

/* ── Оновлення даних ────────────────────── */
void ui_main_set_station(const char *name);
void ui_main_set_track(const char *track);
void ui_main_set_counter(int idx, int total);
void ui_main_set_clock(int hours, int minutes);
void ui_main_set_play_state(ui_play_state_t state);
void ui_main_set_status(ui_status_t status, const char *ip);
void ui_main_set_volume(int vol_pct);
void ui_main_set_mute(bool muted);
void ui_main_set_wifi_rssi(int rssi_dbm);
