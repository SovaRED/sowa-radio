#pragma once

#include "lvgl.h"
#include "ui_theme.h"
#include "ui_main.h"
#include "ui_spectrum.h"
#include "ui_stations.h"
#include "ui_settings.h"

/*
 * Ініціалізація всього UI.
 * Викликати один раз після lvgl_port_init(), всередині lvgl_port_lock().
 */
void ui_init_all(lv_obj_t *scr);
