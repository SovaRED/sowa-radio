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
static lv_obj_t *lbl_clock_h;       /* години окремо */
static lv_obj_t *lbl_clock_col;     /* двокрапка окремо — для блимання */
static lv_obj_t *lbl_clock_m;       /* хвилини окремо */
static lv_obj_t *lbl_track;
static lv_obj_t *lbl_counter;
static lv_obj_t *lbl_status_center;
static lv_obj_t *lbl_play_icon;
static lv_obj_t *lbl_mute_icon;
static lv_obj_t *vol_slider;
static lv_obj_t *vol_badge;
static lv_obj_t *lbl_vol_badge;
static lv_obj_t *wifi_bars[4];
static lv_obj_t *lbl_wifi_dbm;
static lv_obj_t *lbl_bat_pct;
static lv_obj_t *bat_fill;

/* Таймер приховування badge */
static lv_timer_t *badge_hide_timer = NULL;

/* Стилі */
static lv_style_t style_screen;
static lv_style_t style_bar;
static lv_style_t style_ctrl_btn;
static lv_style_t style_ctrl_btn_pr;  /* pressed стан */
static lv_style_t style_hbtn;
static lv_style_t style_hbtn_pr;
static bool styles_inited = false;

/* ─── Callbacks ────────────────────────────────────────── */
static void btn_menu_cb(lv_event_t *e)
{
    (void)e;
    ui_stations_show();
}

static void btn_gear_cb(lv_event_t *e)
{
    (void)e;
    ui_settings_show();
}

static void badge_hide_cb(lv_timer_t *t)
{
    (void)t;
    if (vol_badge) lv_obj_add_flag(vol_badge, LV_OBJ_FLAG_HIDDEN);
    badge_hide_timer = NULL;
}

static void vol_slider_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);

    /* Оновити badge */
    if (lbl_vol_badge) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", val);
        lv_label_set_text(lbl_vol_badge, buf);
    }

    /* Позиціонувати badge над повзунком */
    if (vol_badge) {
        lv_obj_clear_flag(vol_badge, LV_OBJ_FLAG_HIDDEN);

        /* Розрахунок X-позиції: пропорційно значенню слайдера */
        int slider_x = lv_obj_get_x(slider);
        int slider_w = lv_obj_get_width(slider);
        /* Слайдер знаходиться в bar яка на UI_SCR_H - UI_VOL_H */
        int bar_y = UI_SCR_H - UI_VOL_H;
        int badge_w = lv_obj_get_width(vol_badge);
        int badge_h = lv_obj_get_height(vol_badge);

        int cx = slider_x + (val * slider_w / 100);
        int bx = cx - badge_w / 2;
        int by = bar_y - badge_h - 12;

        /* Обмежити щоб не вилазив за межі */
        if (bx < 4) bx = 4;
        if (bx + badge_w > UI_SCR_W - 4) bx = UI_SCR_W - 4 - badge_w;

        lv_obj_set_pos(vol_badge, bx, by);
    }

    /* Перезапустити таймер приховування */
    if (badge_hide_timer) {
        lv_timer_reset(badge_hide_timer);
    } else {
        badge_hide_timer = lv_timer_create(badge_hide_cb, 1500, NULL);
        lv_timer_set_repeat_count(badge_hide_timer, 1);
    }
}

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

    /* Pressed стан кнопок — синє підсвічування */
    lv_style_init(&style_ctrl_btn_pr);
    lv_style_set_bg_color(&style_ctrl_btn_pr, UI_C_ACCENT);
    lv_style_set_bg_opa(&style_ctrl_btn_pr, 56);   /* ~rgba(33,150,243,.22) */
    lv_style_set_border_color(&style_ctrl_btn_pr, UI_C_ACCENT);
    lv_style_set_border_opa(&style_ctrl_btn_pr, 140);

    lv_style_init(&style_hbtn);
    lv_style_set_bg_color(&style_hbtn, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&style_hbtn, 13);
    lv_style_set_border_color(&style_hbtn, lv_color_hex(0xFFFFFF));
    lv_style_set_border_opa(&style_hbtn, 23);
    lv_style_set_border_width(&style_hbtn, 1);
    lv_style_set_radius(&style_hbtn, UI_R_HBTN);
    lv_style_set_shadow_width(&style_hbtn, 0);
    lv_style_set_text_color(&style_hbtn, lv_color_hex(0xC8DCFF));

    lv_style_init(&style_hbtn_pr);
    lv_style_set_bg_color(&style_hbtn_pr, UI_C_ACCENT);
    lv_style_set_bg_opa(&style_hbtn_pr, 64);
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

    /* Кнопка ☰ */
    lv_obj_t *btn_menu = lv_obj_create(bar);
    lv_obj_add_style(btn_menu, &style_hbtn, 0);
    lv_obj_add_style(btn_menu, &style_hbtn_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_menu, UI_HBTN_W, UI_HBTN_H);
    lv_obj_align(btn_menu, LV_ALIGN_LEFT_MID, 12, 0);
    lv_obj_clear_flag(btn_menu, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_menu, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_m = lv_label_create(btn_menu);
    lv_label_set_text(lbl_m, LV_SYMBOL_LIST);
    lv_obj_set_style_text_color(lbl_m, lv_color_hex(0xC8DCFF), 0);
    lv_obj_center(lbl_m);
    lv_obj_add_event_cb(btn_menu, btn_menu_cb, LV_EVENT_CLICKED, NULL);

    /* Кнопка ⚙ */
    lv_obj_t *btn_gear = lv_obj_create(bar);
    lv_obj_add_style(btn_gear, &style_hbtn, 0);
    lv_obj_add_style(btn_gear, &style_hbtn_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_gear, UI_HBTN_W, UI_HBTN_H);
    lv_obj_align(btn_gear, LV_ALIGN_LEFT_MID, 12 + UI_HBTN_W + 8, 0);
    lv_obj_clear_flag(btn_gear, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_gear, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_g = lv_label_create(btn_gear);
    lv_label_set_text(lbl_g, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(lbl_g, lv_color_hex(0xC8DCFF), 0);
    lv_obj_center(lbl_g);
    lv_obj_add_event_cb(btn_gear, btn_gear_cb, LV_EVENT_CLICKED, NULL);

    /* Центр: статус */
    lbl_status_center = lv_label_create(bar);
    lv_obj_set_width(lbl_status_center, 380);
    lv_obj_align(lbl_status_center, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(lbl_status_center, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_status_center, UI_C_TEXT_MUTE, 0);
    lv_obj_set_style_text_align(lbl_status_center, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbl_status_center, "");

    /* WiFi bars + dBm */
    static const uint8_t bar_h[4] = {5, 9, 13, 17};
    lv_obj_t *wifi_cont = lv_obj_create(bar);
    lv_obj_set_size(wifi_cont, 28, 17);
    lv_obj_align(wifi_cont, LV_ALIGN_RIGHT_MID, -140, 0);
    lv_obj_set_style_bg_opa(wifi_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wifi_cont, 0, 0);
    lv_obj_set_style_pad_all(wifi_cont, 0, 0);
    lv_obj_clear_flag(wifi_cont, LV_OBJ_FLAG_SCROLLABLE);
    for (int i = 0; i < 4; i++) {
        wifi_bars[i] = lv_obj_create(wifi_cont);
        lv_obj_set_size(wifi_bars[i], 4, bar_h[i]);
        lv_obj_set_pos(wifi_bars[i], i * 7, 17 - bar_h[i]);
        lv_obj_set_style_bg_color(wifi_bars[i], lv_color_hex(0x64B4FF), 0);
        lv_obj_set_style_bg_opa(wifi_bars[i], 46, 0);
        lv_obj_set_style_border_width(wifi_bars[i], 0, 0);
        lv_obj_set_style_radius(wifi_bars[i], 1, 0);
    }

    /* dBm підпис */
    lbl_wifi_dbm = lv_label_create(bar);
    lv_obj_set_style_text_font(lbl_wifi_dbm, &font_roboto_light_15, 0);
    lv_obj_set_style_text_color(lbl_wifi_dbm, lv_color_hex(0x7090A0), 0);
    lv_obj_align(lbl_wifi_dbm, LV_ALIGN_RIGHT_MID, -106, 0);
    lv_label_set_text(lbl_wifi_dbm, "");

    /* Батарея */
    lv_obj_t *bat_cont = lv_obj_create(bar);
    lv_obj_set_size(bat_cont, 60, 20);
    lv_obj_align(bat_cont, LV_ALIGN_RIGHT_MID, -12, 0);
    lv_obj_set_style_bg_opa(bat_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bat_cont, 0, 0);
    lv_obj_set_style_pad_all(bat_cont, 0, 0);
    lv_obj_clear_flag(bat_cont, LV_OBJ_FLAG_SCROLLABLE);

    /* Відсоток */
    lbl_bat_pct = lv_label_create(bat_cont);
    lv_obj_set_style_text_font(lbl_bat_pct, &font_roboto_light_15, 0);
    lv_obj_set_style_text_color(lbl_bat_pct, lv_color_hex(0x90B0A0), 0);
    lv_obj_align(lbl_bat_pct, LV_ALIGN_LEFT_MID, 0, 0);
    lv_label_set_text(lbl_bat_pct, "78%");

    /* Корпус батареї */
    lv_obj_t *bat_body = lv_obj_create(bat_cont);
    lv_obj_set_size(bat_body, 28, 14);
    lv_obj_align(bat_body, LV_ALIGN_RIGHT_MID, -4, 0);
    lv_obj_set_style_bg_opa(bat_body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(bat_body, lv_color_hex(0x5CB85C), 0);
    lv_obj_set_style_border_opa(bat_body, 140, 0);
    lv_obj_set_style_border_width(bat_body, 2, 0);
    lv_obj_set_style_radius(bat_body, 3, 0);
    lv_obj_set_style_pad_all(bat_body, 2, 0);
    lv_obj_clear_flag(bat_body, LV_OBJ_FLAG_SCROLLABLE);

    /* Заливка */
    bat_fill = lv_obj_create(bat_body);
    lv_obj_set_size(bat_fill, 17, LV_PCT(100));  /* 78% від 22px */
    lv_obj_align(bat_fill, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x5CB85C), 0);
    lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bat_fill, 0, 0);
    lv_obj_set_style_radius(bat_fill, 1, 0);
    lv_obj_clear_flag(bat_fill, LV_OBJ_FLAG_SCROLLABLE);

    /* Полюс батареї (+) — маленький прямокутник справа */
    lv_obj_t *bat_tip = lv_obj_create(bar);
    lv_obj_set_size(bat_tip, 3, 6);
    lv_obj_align(bat_tip, LV_ALIGN_RIGHT_MID, -4, 0);
    lv_obj_set_style_bg_color(bat_tip, lv_color_hex(0x5CB85C), 0);
    lv_obj_set_style_bg_opa(bat_tip, 115, 0);
    lv_obj_set_style_border_width(bat_tip, 0, 0);
    lv_obj_set_style_radius(bat_tip, 1, 0);
}

/* ─── Фоновий glow ─────────────────────────────────────── */
static void create_bg_glow(lv_obj_t *scr)
{
    /* Лівий glow — помаранчево-коричневий внизу зліва */
    lv_obj_t *gl_l = lv_obj_create(scr);
    lv_obj_set_size(gl_l, 420, 300);
    lv_obj_set_pos(gl_l, -80, UI_SCR_H - 300 + 80);
    lv_obj_set_style_bg_color(gl_l, lv_color_hex(0xBE500F), 0);
    lv_obj_set_style_bg_opa(gl_l, 64, 0);   /* ~0.25 */
    lv_obj_set_style_border_width(gl_l, 0, 0);
    lv_obj_set_style_radius(gl_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_shadow_width(gl_l, 80, 0);
    lv_obj_set_style_shadow_color(gl_l, lv_color_hex(0xBE500F), 0);
    lv_obj_set_style_shadow_opa(gl_l, 64, 0);
    lv_obj_clear_flag(gl_l, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    /* Правий glow — синій внизу справа */
    lv_obj_t *gl_r = lv_obj_create(scr);
    lv_obj_set_size(gl_r, 500, 340);
    lv_obj_set_pos(gl_r, UI_SCR_W - 500 + 80, UI_SCR_H - 340 + 100);
    lv_obj_set_style_bg_color(gl_r, lv_color_hex(0x145AC8), 0);
    lv_obj_set_style_bg_opa(gl_r, 51, 0);   /* ~0.2 */
    lv_obj_set_style_border_width(gl_r, 0, 0);
    lv_obj_set_style_radius(gl_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_shadow_width(gl_r, 80, 0);
    lv_obj_set_style_shadow_color(gl_r, lv_color_hex(0x145AC8), 0);
    lv_obj_set_style_shadow_opa(gl_r, 51, 0);
    lv_obj_clear_flag(gl_r, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}

/* ─── Основний контент ─────────────────────────────────── */
static void create_main_content(lv_obj_t *scr)
{
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, UI_SCR_W, UI_MAIN_H);
    lv_obj_set_pos(cont, 0, UI_STATUS_H);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    /* Назва станції */
    lbl_station_name = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl_station_name, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_station_name, UI_C_STATION_NAME, 0);
    lv_obj_set_style_text_letter_space(lbl_station_name, 3, 0);
    lv_label_set_text(lbl_station_name, "RADIO HITS FM");
    lv_obj_align(lbl_station_name, LV_ALIGN_TOP_MID, 0, 10);

    /* Годинник — три окремих label в рядок */
    lv_obj_t *clk_cont = lv_obj_create(cont);
    lv_obj_set_size(clk_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(clk_cont, LV_ALIGN_TOP_MID, 0, 32);
    lv_obj_set_style_bg_opa(clk_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(clk_cont, 0, 0);
    lv_obj_set_style_pad_all(clk_cont, 0, 0);
    lv_obj_set_layout(clk_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(clk_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(clk_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(clk_cont, LV_OBJ_FLAG_SCROLLABLE);

    lbl_clock_h = lv_label_create(clk_cont);
    lv_obj_set_style_text_font(lbl_clock_h, &font_seven_segment_96, 0);
    lv_obj_set_style_text_color(lbl_clock_h, UI_C_CLOCK, 0);
    lv_obj_set_style_text_letter_space(lbl_clock_h, 10, 0);
    lv_label_set_text(lbl_clock_h, "00");

    lbl_clock_col = lv_label_create(clk_cont);
    lv_obj_set_style_text_font(lbl_clock_col, &font_seven_segment_96, 0);
    lv_obj_set_style_text_color(lbl_clock_col, UI_C_CLOCK, 0);
    lv_obj_set_style_pad_left(lbl_clock_col, 4, 0);
    lv_obj_set_style_pad_right(lbl_clock_col, 4, 0);
    lv_label_set_text(lbl_clock_col, ":");

    lbl_clock_m = lv_label_create(clk_cont);
    lv_obj_set_style_text_font(lbl_clock_m, &font_seven_segment_96, 0);
    lv_obj_set_style_text_color(lbl_clock_m, UI_C_CLOCK, 0);
    lv_obj_set_style_text_letter_space(lbl_clock_m, 10, 0);
    lv_label_set_text(lbl_clock_m, "00");

    /* Трек */
    lbl_track = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl_track, &font_roboto_light_15, 0);
    lv_obj_set_style_text_color(lbl_track, UI_C_TRACK, 0);
    lv_obj_set_width(lbl_track, 540);
    lv_label_set_long_mode(lbl_track, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(lbl_track, "Coldplay - Adventure of a Lifetime");
    lv_obj_align(lbl_track, LV_ALIGN_TOP_MID, 0, 148);

    /* Лічильник станцій */
    lbl_counter = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl_counter, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_counter, UI_C_TEXT_MUTE, 0);
    lv_obj_set_style_text_letter_space(lbl_counter, 2, 0);
    lv_label_set_text(lbl_counter, "1 / 8");
    lv_obj_align(lbl_counter, LV_ALIGN_TOP_MID, 0, 172);

    /* Кнопки PREV / PLAY / NEXT */
    int row_w = UI_BTN_PREV_W + 10 + UI_BTN_PLAY_W + 10 + UI_BTN_NEXT_W;
    int row_x = (UI_SCR_W - row_w) / 2;
    int row_y = 192;

    /* PREV */
    lv_obj_t *btn_prev = lv_obj_create(cont);
    lv_obj_add_style(btn_prev, &style_ctrl_btn, 0);
    lv_obj_add_style(btn_prev, &style_ctrl_btn_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_prev, UI_BTN_PREV_W, UI_BTN_CTRL_H);
    lv_obj_set_pos(btn_prev, row_x, row_y);
    lv_obj_clear_flag(btn_prev, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_prev, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_prev = lv_label_create(btn_prev);
    lv_obj_set_style_text_font(lbl_prev, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_prev, lv_color_hex(0xDCEEFF), 0);
    lv_label_set_text(lbl_prev, LV_SYMBOL_PREV);
    lv_obj_center(lbl_prev);

    /* PLAY */
    lv_obj_t *btn_play = lv_obj_create(cont);
    lv_obj_add_style(btn_play, &style_ctrl_btn, 0);
    lv_obj_add_style(btn_play, &style_ctrl_btn_pr, LV_STATE_PRESSED);
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
    lv_obj_add_style(btn_next, &style_ctrl_btn_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn_next, UI_BTN_NEXT_W, UI_BTN_CTRL_H);
    lv_obj_set_pos(btn_next, row_x + UI_BTN_PREV_W + 10 + UI_BTN_PLAY_W + 10, row_y);
    lv_obj_clear_flag(btn_next, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_next, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *lbl_next = lv_label_create(btn_next);
    lv_obj_set_style_text_font(lbl_next, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_next, lv_color_hex(0xDCEEFF), 0);
    lv_label_set_text(lbl_next, LV_SYMBOL_NEXT);
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

    /* Mute кнопка */
    lv_obj_t *btn_mute = lv_obj_create(bar);
    lv_obj_set_size(btn_mute, UI_MUTE_W, UI_MUTE_H);
    lv_obj_align(btn_mute, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(btn_mute, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(btn_mute, 18, 0);
    lv_obj_set_style_border_color(btn_mute, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(btn_mute, 26, 0);
    lv_obj_set_style_border_width(btn_mute, 1, 0);
    lv_obj_set_style_radius(btn_mute, UI_R_MUTE, 0);
    lv_obj_set_style_shadow_width(btn_mute, 0, 0);
    /* Pressed стан mute */
    lv_obj_set_style_bg_color(btn_mute, UI_C_ACCENT, LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn_mute, 56, LV_STATE_PRESSED);
    lv_obj_clear_flag(btn_mute, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn_mute, LV_OBJ_FLAG_CLICKABLE);
    lbl_mute_icon = lv_label_create(btn_mute);
    lv_obj_set_style_text_font(lbl_mute_icon, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_mute_icon, UI_C_TEXT, 0);
    lv_label_set_text(lbl_mute_icon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_center(lbl_mute_icon);

    /* Слайдер гучності */
    vol_slider = lv_slider_create(bar);
    lv_obj_set_height(vol_slider, 20);
    lv_obj_set_pos(vol_slider, UI_MUTE_W + 14, (UI_VOL_H - 20) / 2);
    lv_obj_set_width(vol_slider, UI_SCR_W - 16 - 16 - UI_MUTE_W - 14);
    lv_slider_set_value(vol_slider, 65, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(vol_slider, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(vol_slider, 20, 0);
    lv_obj_set_style_border_color(vol_slider, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(vol_slider, 13, 0);
    lv_obj_set_style_border_width(vol_slider, 1, 0);
    lv_obj_set_style_radius(vol_slider, UI_R_VOL, 0);
    lv_obj_set_style_bg_color(vol_slider, lv_color_hex(0x1E88E5), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(vol_slider, lv_color_hex(0x0D47A1), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(vol_slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(vol_slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(vol_slider, UI_R_VOL, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(vol_slider, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(vol_slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(vol_slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_pad_all(vol_slider, 4, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(vol_slider, UI_C_ACCENT, LV_PART_KNOB);
    lv_obj_set_style_shadow_width(vol_slider, 15, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(vol_slider, LV_OPA_70, LV_PART_KNOB);

    /* Callback на зміну значення */
    lv_obj_add_event_cb(vol_slider, vol_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* Vol badge — спочатку прихований, на scr (не в bar) щоб не обрізатись */
    vol_badge = lv_obj_create(scr);
    lv_obj_set_size(vol_badge, 130, 80);
    lv_obj_set_style_bg_color(vol_badge, lv_color_hex(0x0A2864), 0);
    lv_obj_set_style_bg_opa(vol_badge, 237, 0);
    lv_obj_set_style_border_color(vol_badge, lv_color_hex(0x64B4FF), 0);
    lv_obj_set_style_border_width(vol_badge, 1, 0);
    lv_obj_set_style_border_opa(vol_badge, 77, 0);
    lv_obj_set_style_radius(vol_badge, 16, 0);
    lv_obj_set_style_shadow_width(vol_badge, 0, 0);
    lv_obj_set_style_pad_all(vol_badge, 4, 0);
    lv_obj_clear_flag(vol_badge, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(vol_badge, LV_OBJ_FLAG_HIDDEN);

    /* "VOLUME" підпис */
    lv_obj_t *lbl_vol_title = lv_label_create(vol_badge);
    lv_obj_set_style_text_font(lbl_vol_title, &font_roboto_medium_16, 0);
    lv_obj_set_style_text_color(lbl_vol_title, lv_color_hex(0x8CC8FF), 0);
    lv_obj_set_style_text_letter_space(lbl_vol_title, 3, 0);
    lv_label_set_text(lbl_vol_title, "VOLUME");
    lv_obj_align(lbl_vol_title, LV_ALIGN_TOP_MID, 0, 4);

    /* Число */
    lbl_vol_badge = lv_label_create(vol_badge);
    lv_obj_set_style_text_font(lbl_vol_badge, &font_seven_segment_96, 0);
    lv_obj_set_style_text_color(lbl_vol_badge, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(lbl_vol_badge, "65");
    lv_obj_align(lbl_vol_badge, LV_ALIGN_BOTTOM_MID, 0, -4);
}

/* ─── Публічна ініціалізація ───────────────────────────── */
void ui_main_init(lv_obj_t *scr)
{
    styles_init();
    lv_obj_add_style(scr, &style_screen, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    create_bg_glow(scr);       /* фоновий glow — перший (знизу) */
    create_status_bar(scr);
    create_main_content(scr);
    create_vol_bar(scr);
    ui_spectrum_init(scr);     /* canvas спектрограми поверх glow, під контентом */
}

/* ─── Щосекундний тік ──────────────────────────────────── */
void ui_main_tick(int h, int m, bool colon)
{
    if (!lbl_clock_h || !lbl_clock_m || !lbl_clock_col) return;
    char buf[4];
    snprintf(buf, sizeof(buf), "%02d", h);
    lv_label_set_text(lbl_clock_h, buf);
    snprintf(buf, sizeof(buf), "%02d", m);
    lv_label_set_text(lbl_clock_m, buf);
    /* Блимання двокрапки */
    lv_obj_set_style_text_color(lbl_clock_col, UI_C_CLOCK,
        colon ? 0 : LV_STATE_DEFAULT);
    lv_obj_set_style_opa(lbl_clock_col,
        colon ? LV_OPA_90 : LV_OPA_10, 0);
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
    ui_main_tick(h, m, true);
}

void ui_main_set_play_state(ui_play_state_t state)
{
    if (!lbl_play_icon) return;
    if (state == UI_PLAY_STATE_PLAYING) {
        lv_label_set_text(lbl_play_icon, LV_SYMBOL_PAUSE "  PAUSE");
    } else {
        lv_label_set_text(lbl_play_icon, LV_SYMBOL_PLAY "  PLAY");
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
            lv_label_set_text(lbl_status_center, LV_SYMBOL_WARNING " Немає мережі");
            break;
        case UI_STATUS_ERROR:
            lv_obj_set_style_text_color(lbl_status_center, UI_C_ERROR, 0);
            lv_label_set_text(lbl_status_center, LV_SYMBOL_WARNING " Помилка потоку");
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

    if (lbl_wifi_dbm) {
        if (rssi_dbm < 0) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d dBm", rssi_dbm);
            lv_label_set_text(lbl_wifi_dbm, buf);
        } else {
            lv_label_set_text(lbl_wifi_dbm, "");
        }
    }
}

void ui_main_set_battery(int pct)
{
    if (lbl_bat_pct) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d%%", pct);
        lv_label_set_text(lbl_bat_pct, buf);
    }
    if (bat_fill) {
        /* Ширина заливки: max 22px (внутрішній простір корпусу) */
        int w = (22 * pct) / 100;
        if (w < 1) w = 1;
        lv_obj_set_width(bat_fill, w);
        /* Колір: зелений >20%, червоний <=20% */
        lv_color_t col = (pct > 20) ? lv_color_hex(0x5CB85C) : UI_C_ERROR;
        lv_obj_set_style_bg_color(bat_fill, col, 0);
    }
}
