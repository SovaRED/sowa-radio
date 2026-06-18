#include "ui_main.h"
#include "ui_theme.h"
#include "ui_spectrum.h"
#include "ui_stations.h"
#include "ui_settings.h"
#include "font_roboto_light_15.h"
#include "font_roboto_medium_16.h"
#include "font_seven_segment_96.h"
#include <stdio.h>
#include <stdbool.h>

/* ─── Приватні хендли ──────────────────────────────────── */
static lv_obj_t *lbl_station_name;
static lv_obj_t *lbl_clock;
static lv_obj_t *lbl_track;
static lv_obj_t *lbl_counter;
static lv_obj_t *lbl_status_center;

static lv_obj_t *btn_play;
static lv_obj_t *lbl_play_icon;

static lv_obj_t *btn_mute;
static lv_obj_t *lbl_mute_icon;
static lv_obj_t *vol_slider;
static lv_obj_t *vol_badge;        /* SevenSeg badge 140px над слайдером */
static lv_obj_t *lbl_vol_badge;

/* WiFi bars */
static lv_obj_t *wifi_bars[4];

/* Стилі (статичні — ініціалізуються один раз) */
static lv_style_t style_screen;
static lv_style_t style_bar;        /* статусбар і волбар */
static lv_style_t style_ctrl_btn;   /* PREV/PLAY/NEXT */
static lv_style_t style_hbtn;       /* кнопки хедера */
static bool styles_inited = false;

/* ─── Ініціалізація стилів ─────────────────────────────── */
static void styles_init(void)
{
    if (styles_inited) return;
    styles_inited = true;

    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, UI_C_BG);
    lv_style_set_bg_opa(&style_screen, LV_OPA_COVER);
    lv_style_set_border_width(&style_screen, 0);
    lv_style_set_pad_all(&style_screen, 0);

    lv_style_init(&style_bar);
    lv_style_set_bg_color(&style_bar, UI_C_SURFACE);
    lv_style_set_bg_opa(&style_bar, UI_OPA_BAR);
    lv_style_set_border_width(&style_bar, 0);
    lv_style_set_radius(&style_bar, 0);
    lv_style_set_pad_all(&style_bar, 0);

    lv_style_init(&style_ctrl_btn);
    lv_style_set_bg_color(&style_ctrl_btn, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&style_ctrl_btn, UI_OPA_BTN);
    lv_style_set_border_color(&style_ctrl_btn, lv_color_hex(0xFFFFFF));
    lv_style_set_border_opa(&style_ctrl_btn, UI_OPA_BTN_BORDER);
    lv_style_set_border_width(&style_ctrl_btn, 1);
    lv_style_set_radius(&style_ctrl_btn, UI_R_BTN);
    lv_style_set_shadow_width(&style_ctrl_btn, 0);

    lv_style_init(&style_hbtn);
    lv_style_set_bg_color(&style_hbtn, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&style_hbtn, 13);   /* ~5% */
    lv_style_set_border_color(&style_hbtn, lv_color_hex(0xFFFFFF));
    lv_style_set_border_opa(&style_hbtn, 23); /* ~9% */
    lv_style_set_border_width(&style_hbtn, 1);
    lv_style_set_radius(&style_hbtn, UI_R_HBTN);
    lv_style_set_shadow_width(&style_hbtn, 0);
    lv_style_set_text_color(&style_hbtn, lv_color_hex(0xC8DCFF));
}

/* ─── Статусбар (54px, верх) ───────────────────────────── */
static void create_status_bar(lv_obj_t *scr)
{
    lv_obj_t *bar = lv_obj_create(scr);
    lv_obj_add_style(bar, &style_bar, 0);
    lv_obj_set_size(bar, UI_SCR_W, UI_STATUS_H);
    lv_obj_set_pos(bar, 0, 0);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(bar, UI_C_BORDER, 0);
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_border_opa(bar, LV_OPA_COVER, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    /* Кнопка ☰ (відкрити панель станцій) */
    lv_obj_t *btn_menu = lv_obj_create(bar);
    lv_obj_add_style(btn_menu, &style_hbtn, 0);
    lv_obj_set_size(btn_menu, UI_HBTN_W, UI_HBTN_H);
    lv_obj_align(btn_menu, LV_ALIGN_LEFT_MID, 12, 0);
    lv_obj_clear_flag(btn_menu, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_menu, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_m = lv_label_create(btn_menu);
    lv_label_set_text(lbl_m, LV_SYMBOL_LIST);
    lv_obj_set_style_text_color(lbl_m, lv_color_hex(0xC8DCFF), 0);
    lv_obj_center(lbl_m);
    lv_obj_add_event_cb(btn_menu, [](lv_event_t *e) {
        ui_stations_show();
    }, LV_EVENT_CLICKED, NULL);

    /* Кнопка ⚙ (налаштування) */
    lv_obj_t *btn_gear = lv_obj_create(bar);
    lv_obj_add_style(btn_gear, &style_hbtn, 0);
    lv_obj_set_size(btn_gear, UI_HBTN_W, UI_HBTN_H);
    lv_obj_align(btn_gear, LV_ALIGN_LEFT_MID, 12 + UI_HBTN_W + 8, 0);
    lv_obj_clear_flag(btn_gear, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_gear, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_g = lv_label_create(btn_gear);
    lv_label_set_text(lbl_g, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(lbl_g, lv_color_hex(0xC8DCFF), 0);
    lv_obj_center(lbl_g);
    lv_obj_add_event_cb(btn_gear, [](lv_event_t *e) {
        ui_settings_show();
    }, LV_EVENT_CLICKED, NULL);

    /* Центр: статус (IP / buffering / error) */
    lbl_status_center = lv_label_create(bar);
    lv_obj_set_width(lbl_status_center, 380);
    lv_obj_align(lbl_status_center, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(lbl_status_center, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_status_center, UI_C_TEXT_MUTE, 0);
    lv_obj_set_style_text_align(lbl_status_center, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbl_status_center, "");

    /* WiFi bars (праворуч) */
    static const uint8_t bar_h[4] = {5, 9, 13, 17};
    lv_obj_t *wifi_cont = lv_obj_create(bar);
    lv_obj_set_size(wifi_cont, 28, 17);
    lv_obj_align(wifi_cont, LV_ALIGN_RIGHT_MID, -90, 0);
    lv_obj_set_style_bg_opa(wifi_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wifi_cont, 0, 0);
    lv_obj_set_style_pad_all(wifi_cont, 0, 0);
    lv_obj_clear_flag(wifi_cont, LV_OBJ_FLAG_SCROLLABLE);
    for (int i = 0; i < 4; i++) {
        wifi_bars[i] = lv_obj_create(wifi_cont);
        lv_obj_set_size(wifi_bars[i], 4, bar_h[i]);
        lv_obj_set_pos(wifi_bars[i], i * 7, 17 - bar_h[i]);
        lv_obj_set_style_bg_color(wifi_bars[i], lv_color_hex(0x64B4FF), 0);
        lv_obj_set_style_bg_opa(wifi_bars[i], 46, 0); /* вимкнений бар ~18% */
        lv_obj_set_style_border_width(wifi_bars[i], 0, 0);
        lv_obj_set_style_radius(wifi_bars[i], 1, 0);
    }
}

/* ─── Основний контент (між статусбаром і волбаром) ────── */
static void create_main_content(lv_obj_t *scr)
{
    /* Прозорий контейнер */
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, UI_SCR_W, UI_MAIN_H);
    lv_obj_set_pos(cont, 0, UI_STATUS_H);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    /* Назва станції (16px Medium, uppercase, letter-spacing 3px) */
    lbl_station_name = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl_station_name, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_station_name, UI_C_STATION_NAME, 0);
    lv_obj_set_style_text_letter_space(lbl_station_name, 3, 0);
    lv_label_set_text(lbl_station_name, "RADIO HITS FM");
    lv_obj_align(lbl_station_name, LV_ALIGN_TOP_MID, 0, 10);

    /* Годинник SevenSeg 96px */
    lbl_clock = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl_clock, &font_seven_segment_96, 0);
    lv_obj_set_style_text_color(lbl_clock, UI_C_CLOCK, 0);
    lv_obj_set_style_text_letter_space(lbl_clock, 10, 0);
    /* Синє glow через shadow */
    lv_obj_set_style_text_opa(lbl_clock, LV_OPA_COVER, 0);
    lv_label_set_text(lbl_clock, "00:00");
    lv_obj_align(lbl_clock, LV_ALIGN_TOP_MID, 0, 32);

    /* Назва треку — scrolling */
    lbl_track = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl_track, &font_roboto_light_15, 0);
    lv_obj_set_style_text_color(lbl_track, UI_C_TRACK, 0);
    lv_obj_set_width(lbl_track, 540);
    lv_label_set_long_mode(lbl_track, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(lbl_track, "Coldplay - Adventure of a Lifetime");
    lv_obj_align(lbl_track, LV_ALIGN_TOP_MID, 0, 148);

    /* Лічильник X / N */
    lbl_counter = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl_counter, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_counter, UI_C_TEXT_MUTE, 0);
    lv_obj_set_style_text_letter_space(lbl_counter, 2, 0);
    lv_label_set_text(lbl_counter, "1 / 8");
    lv_obj_align(lbl_counter, LV_ALIGN_TOP_MID, 0, 172);

    /* ── Кнопки управління ── */
    /* Рядок: PREV(110) + gap(10) + PLAY(238) + gap(10) + NEXT(110) = 478px */
    int row_w = UI_BTN_PREV_W + 10 + UI_BTN_PLAY_W + 10 + UI_BTN_NEXT_W;
    int row_x = (UI_SCR_W - row_w) / 2;
    int row_y = 192;  /* відступ від верху cont */

    /* PREV */
    lv_obj_t *btn_prev = lv_obj_create(cont);
    lv_obj_add_style(btn_prev, &style_ctrl_btn, 0);
    lv_obj_set_size(btn_prev, UI_BTN_PREV_W, UI_BTN_CTRL_H);
    lv_obj_set_pos(btn_prev, row_x, row_y);
    lv_obj_clear_flag(btn_prev, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_prev, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_prev = lv_label_create(btn_prev);
    lv_obj_set_style_text_font(lbl_prev, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_prev, lv_color_hex(0xDCEEFF), 0);
    lv_label_set_text(lbl_prev, "<<");
    lv_obj_center(lbl_prev);

    /* PLAY */
    btn_play = lv_obj_create(cont);
    lv_obj_add_style(btn_play, &style_ctrl_btn, 0);
    lv_obj_set_size(btn_play, UI_BTN_PLAY_W, UI_BTN_CTRL_H);
    lv_obj_set_pos(btn_play, row_x + UI_BTN_PREV_W + 10, row_y);
    lv_obj_clear_flag(btn_play, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_play, LV_OBJ_FLAG_CLICKABLE);
    lbl_play_icon = lv_label_create(btn_play);
    lv_obj_set_style_text_font(lbl_play_icon, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_play_icon, lv_color_hex(0xDCEEFF), 0);
    lv_label_set_text(lbl_play_icon, LV_SYMBOL_PLAY "  PLAY");
    lv_obj_center(lbl_play_icon);

    /* NEXT */
    lv_obj_t *btn_next = lv_obj_create(cont);
    lv_obj_add_style(btn_next, &style_ctrl_btn, 0);
    lv_obj_set_size(btn_next, UI_BTN_NEXT_W, UI_BTN_CTRL_H);
    lv_obj_set_pos(btn_next, row_x + UI_BTN_PREV_W + 10 + UI_BTN_PLAY_W + 10, row_y);
    lv_obj_clear_flag(btn_next, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_next, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_next = lv_label_create(btn_next);
    lv_obj_set_style_text_font(lbl_next, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_next, lv_color_hex(0xDCEEFF), 0);
    lv_label_set_text(lbl_next, ">>");
    lv_obj_center(lbl_next);
}

/* ─── Volume bar (70px, низ) ───────────────────────────── */
static void create_vol_bar(lv_obj_t *scr)
{
    lv_obj_t *bar = lv_obj_create(scr);
    lv_obj_add_style(bar, &style_bar, 0);
    lv_obj_set_size(bar, UI_SCR_W, UI_VOL_H);
    lv_obj_set_pos(bar, 0, UI_SCR_H - UI_VOL_H);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_color(bar, UI_C_BORDER, 0);
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_border_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_left(bar, 16, 0);
    lv_obj_set_style_pad_right(bar, 16, 0);
    lv_obj_set_style_pad_top(bar, 0, 0);
    lv_obj_set_style_pad_bottom(bar, 0, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    /* Кнопка Mute (58×50) */
    btn_mute = lv_obj_create(bar);
    lv_obj_set_size(btn_mute, UI_MUTE_W, UI_MUTE_H);
    lv_obj_align(btn_mute, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(btn_mute, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(btn_mute, 18, 0);
    lv_obj_set_style_border_color(btn_mute, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(btn_mute, 26, 0);
    lv_obj_set_style_border_width(btn_mute, 1, 0);
    lv_obj_set_style_radius(btn_mute, UI_R_MUTE, 0);
    lv_obj_set_style_shadow_width(btn_mute, 0, 0);
    lv_obj_clear_flag(btn_mute, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_mute, LV_OBJ_FLAG_CLICKABLE);
    lbl_mute_icon = lv_label_create(btn_mute);
    lv_obj_set_style_text_font(lbl_mute_icon, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_mute_icon, UI_C_TEXT, 0);
    lv_label_set_text(lbl_mute_icon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_center(lbl_mute_icon);

    /* Volume slider */
    vol_slider = lv_slider_create(bar);
    lv_obj_set_height(vol_slider, 20);
    lv_obj_set_pos(vol_slider, UI_MUTE_W + 14, (UI_VOL_H - 20) / 2);
    lv_obj_set_width(vol_slider, UI_SCR_W - 16 - 16 - UI_MUTE_W - 14);
    lv_slider_set_value(vol_slider, 65, LV_ANIM_OFF);

    /* Стиль фону слайдера */
    lv_obj_set_style_bg_color(vol_slider, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(vol_slider, 20, 0);
    lv_obj_set_style_border_color(vol_slider, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(vol_slider, 13, 0);
    lv_obj_set_style_border_width(vol_slider, 1, 0);
    lv_obj_set_style_radius(vol_slider, UI_R_VOL, 0);

    /* Стиль індикатора (заповненої частини) */
    lv_obj_set_style_bg_color(vol_slider, lv_color_hex(0x1E88E5), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(vol_slider, lv_color_hex(0x0D47A1), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(vol_slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(vol_slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(vol_slider, UI_R_VOL, LV_PART_INDICATOR);

    /* Стиль thumb */
    lv_obj_set_style_bg_color(vol_slider, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(vol_slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(vol_slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_pad_all(vol_slider, 4, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(vol_slider, UI_C_ACCENT, LV_PART_KNOB);
    lv_obj_set_style_shadow_width(vol_slider, 15, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(vol_slider, LV_OPA_70, LV_PART_KNOB);

    /* Volume badge (SevenSeg, над слайдером, прихований за замовчуванням) */
    vol_badge = lv_obj_create(scr);
    lv_obj_set_size(vol_badge, 160, 90);
    lv_obj_set_style_bg_color(vol_badge, lv_color_hex(0x0A2864), 0);
    lv_obj_set_style_bg_opa(vol_badge, 237, 0);  /* ~93% */
    lv_obj_set_style_border_color(vol_badge, lv_color_hex(0x64B4FF), 0);
    lv_obj_set_style_border_width(vol_badge, 1, 0);
    lv_obj_set_style_border_opa(vol_badge, 77, 0);
    lv_obj_set_style_radius(vol_badge, 16, 0);
    lv_obj_set_style_shadow_width(vol_badge, 0, 0);
    lv_obj_clear_flag(vol_badge, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(vol_badge, LV_OBJ_FLAG_HIDDEN);
    /* Текст у badge */
    lbl_vol_badge = lv_label_create(vol_badge);
    lv_obj_set_style_text_font(lbl_vol_badge, &font_seven_segment_96, 0);
    lv_obj_set_style_text_color(lbl_vol_badge, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(lbl_vol_badge, "65");
    lv_obj_center(lbl_vol_badge);
}

/* ─── Публічна ініціалізація ───────────────────────────── */
void ui_main_init(lv_obj_t *scr)
{
    styles_init();
    lv_obj_add_style(scr, &style_screen, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    create_status_bar(scr);
    create_main_content(scr);
    create_vol_bar(scr);
    ui_spectrum_init(scr);   /* canvas — поверх контенту, під vol bar */
}

/* ─── Оновлення даних ──────────────────────────────────── */
void ui_main_set_station(const char *name)
{
    if (lbl_station_name) lv_label_set_text(lbl_station_name, name);
}

void ui_main_set_track(const char *track)
{
    if (lbl_track) lv_label_set_text(lbl_track, track);
}

void ui_main_set_counter(int idx, int total)
{
    if (!lbl_counter) return;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d / %d", idx + 1, total);
    lv_label_set_text(lbl_counter, buf);
}

void ui_main_set_clock(int h, int m)
{
    if (!lbl_clock) return;
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
    lv_label_set_text(lbl_clock, buf);
}

void ui_main_set_play_state(ui_play_state_t state)
{
    if (!lbl_play_icon) return;
    switch (state) {
        case UI_PLAY_STATE_PLAYING:
            lv_label_set_text(lbl_play_icon, LV_SYMBOL_PAUSE "  PAUSE");
            break;
        default:
            lv_label_set_text(lbl_play_icon, LV_SYMBOL_PLAY "  PLAY");
            break;
    }
}

void ui_main_set_status(ui_status_t status, const char *ip)
{
    if (!lbl_status_center) return;
    switch (status) {
        case UI_STATUS_OK:
            lv_obj_set_style_text_color(lbl_status_center, UI_C_TEXT_MUTE, 0);
            lv_label_set_text(lbl_status_center, ip ? ip : "");
            break;
        case UI_STATUS_BUFFERING:
            lv_obj_set_style_text_color(lbl_status_center, lv_color_hex(0xA0D2FF), 0);
            lv_label_set_text(lbl_status_center, "Буферизація...");
            break;
        case UI_STATUS_CONNECTING:
            lv_obj_set_style_text_color(lbl_status_center, lv_color_hex(0xA0D2FF), 0);
            lv_label_set_text(lbl_status_center, "Підключення...");
            break;
        case UI_STATUS_NO_WIFI:
            lv_obj_set_style_text_color(lbl_status_center, UI_C_ERROR, 0);
            lv_label_set_text(lbl_status_center, "Немає мережі");
            break;
        case UI_STATUS_ERROR:
            lv_obj_set_style_text_color(lbl_status_center, UI_C_ERROR, 0);
            lv_label_set_text(lbl_status_center, "Помилка потоку");
            break;
    }
}

void ui_main_set_volume(int vol_pct)
{
    if (!vol_slider) return;
    lv_slider_set_value(vol_slider, vol_pct, LV_ANIM_OFF);
    if (lbl_vol_badge) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", vol_pct);
        lv_label_set_text(lbl_vol_badge, buf);
    }
}

void ui_main_set_mute(bool muted)
{
    if (!lbl_mute_icon) return;
    if (muted) {
        lv_label_set_text(lbl_mute_icon, LV_SYMBOL_MUTE);
        lv_obj_set_style_text_color(lbl_mute_icon, UI_C_ERROR, 0);
    } else {
        lv_label_set_text(lbl_mute_icon, LV_SYMBOL_VOLUME_MAX);
        lv_obj_set_style_text_color(lbl_mute_icon, UI_C_TEXT, 0);
    }
}

void ui_main_set_wifi_rssi(int rssi_dbm)
{
    /* Кількість активних барів: -50+ = 4, -65+ = 3, -75+ = 2, -85+ = 1, < -85 = 0 */
    int active;
    if      (rssi_dbm >= -50) active = 4;
    else if (rssi_dbm >= -65) active = 3;
    else if (rssi_dbm >= -75) active = 2;
    else if (rssi_dbm >= -85) active = 1;
    else                      active = 0;

    for (int i = 0; i < 4; i++) {
        if (!wifi_bars[i]) continue;
        lv_obj_set_style_bg_opa(wifi_bars[i],
            (i < active) ? LV_OPA_COVER : 46, 0);
    }
}
