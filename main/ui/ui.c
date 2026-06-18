#include "ui.h"

void ui_init_all(lv_obj_t *scr)
{
    /* Порядок важливий: spectrum canvas має бути під stations/settings overlay */
    ui_main_init(scr);      /* фон, статусбар, контент, vol bar, spectrum */
    ui_stations_init(scr);  /* overlay поверх головного екрану */
    ui_settings_init(scr);  /* overlay поверх всього */
}
