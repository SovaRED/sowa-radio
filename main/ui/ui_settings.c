#include "ui_settings.h"
#include "ui_theme.h"

/* Заглушка — буде реалізовано в Фазі 6 */
static lv_obj_t *panel = NULL;

void ui_settings_init(lv_obj_t *scr)
{
    panel = lv_obj_create(scr);
    lv_obj_set_size(panel, UI_SCR_W, UI_SCR_H);
    lv_obj_set_pos(panel, 0, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x09101A), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
}

void ui_settings_show(void)
{
    if (panel) lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);
}

void ui_settings_hide(void)
{
    if (panel) lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
}
