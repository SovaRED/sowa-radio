#pragma once
#include "lvgl.h"

/* Ініціалізація canvas спектрограми */
void ui_spectrum_init(lv_obj_t *scr);

/* Оновити дані (викликати з аудіо callback або таймера) */
/* fft_data: масив амплітуд 0.0–1.0, len: кількість бінів */
void ui_spectrum_update(const float *fft_data, int len);

/* Вмикати/вимикати анімацію (пауза = затухання) */
void ui_spectrum_set_playing(bool playing);
