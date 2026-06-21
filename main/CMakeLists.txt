idf_component_register(
    SRCS
        "main.c"

        "ui/ui.c"
        "ui/ui_main.c"
        "ui/ui_spectrum.c"
        "ui/ui_stations.c"
        "ui/ui_settings.c"

        "fonts/font_roboto_light_15.c"
        "fonts/font_roboto_medium_16.c"
        "fonts/font_seven_segment_96.c"

    INCLUDE_DIRS
        "."
        "ui"
        "fonts"

    REQUIRES
        esp_lvgl_adapter
        lvgl
        esp_timer
        esp_lcd
        driver
        nvs_flash
        hw_init
)
