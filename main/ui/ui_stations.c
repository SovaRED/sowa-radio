#include "ui_stations.h"
#include "ui_theme.h"
#include "font_roboto_medium_16.h"
#include "font_roboto_light_15.h"
#include <string.h>

/* Заглушка — буде реалізовано в Фазі 2 (частина 2) */
static lv_obj_t *panel = NULL;
static bool      visible = false;
static ui_stations_select_cb_t select_cb = NULL;

static ui_station_t station_list[64];
static int station_count = 0;
static int current_idx   = 0;

void ui_stations_init(lv_obj_t *scr)
{
    /* Overlay панель (спочатку прихована) */
    panel = lv_obj_create(scr);
    lv_obj_set_size(panel, UI_SCR_W, UI_SCR_H);
    lv_obj_set_pos(panel, 0, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x09101A), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
    /* TODO: повна реалізація списку станцій */
}

void ui_stations_show(void)
{
    if (panel) {
        lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);
        visible = true;
    }
}

void ui_stations_hide(void)
{
    if (panel) {
        lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
        visible = false;
    }
}

bool ui_stations_is_visible(void) { return visible; }

void ui_stations_set_list(const ui_station_t *stations, int count)
{
    if (count > 64) count = 64;
    memcpy(station_list, stations, count * sizeof(ui_station_t));
    station_count = count;
}

void ui_stations_set_current(int idx) { current_idx = idx; }

void ui_stations_set_select_cb(ui_stations_select_cb_t cb) { select_cb = cb; }
