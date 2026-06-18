#pragma once
#include "lvgl.h"

/* ═══════════════════════════════════════════
   КОЛЬОРИ
═══════════════════════════════════════════ */
#define UI_C_BG             lv_color_hex(0x09101A)  /* фон */
#define UI_C_SURFACE        lv_color_hex(0x000000)  /* статусбар/волбар (+ opa) */
#define UI_C_ACCENT         lv_color_hex(0x1E88E5)  /* синій акцент */
#define UI_C_ACCENT2        lv_color_hex(0x29B6F6)  /* світліший акцент */
#define UI_C_ACCENT_DARK    lv_color_hex(0x0D47A1)  /* темніший акцент */
#define UI_C_TEXT           lv_color_hex(0xE8F4FD)  /* основний текст */
#define UI_C_TEXT_DIM       lv_color_hex(0x78C3FF)  /* приглушений текст */
#define UI_C_TEXT_MUTE      lv_color_hex(0x3A5068)  /* дуже приглушений */
#define UI_C_BORDER         lv_color_hex(0x1A2C40)  /* межі */
#define UI_C_ERROR          lv_color_hex(0xEF5350)  /* помилка/mute */
#define UI_C_SUCCESS        lv_color_hex(0x5CB85C)  /* успіх/батарея */
#define UI_C_CLOCK          lv_color_hex(0xE8F5FF)  /* колір годинника */
#define UI_C_STATION_NAME   lv_color_hex(0x78C3FF)  /* назва станції */
#define UI_C_TRACK          lv_color_hex(0xDCEEFF)  /* назва треку */

/* ═══════════════════════════════════════════
   ПРОЗОРІСТЬ
═══════════════════════════════════════════ */
#define UI_OPA_BAR          LV_OPA_40   /* статусбар і волбар ~rgba(0,0,0,.42) */
#define UI_OPA_BTN          10          /* фон кнопок ~rgba(255,255,255,.06) */
#define UI_OPA_BTN_BORDER   15          /* межа кнопок ~rgba(255,255,255,.11) */

/* ═══════════════════════════════════════════
   РОЗМІРИ ЕКРАНУ
═══════════════════════════════════════════ */
#define UI_SCR_W            800
#define UI_SCR_H            480
#define UI_STATUS_H         54
#define UI_VOL_H            70
#define UI_MAIN_H           (UI_SCR_H - UI_STATUS_H - UI_VOL_H)  /* 356 */
#define UI_SPECTRUM_H       165

/* ═══════════════════════════════════════════
   РАДІУСИ
═══════════════════════════════════════════ */
#define UI_R_BTN            16   /* кнопки PREV/PLAY/NEXT */
#define UI_R_HBTN           10   /* кнопки хедера */
#define UI_R_MUTE           13   /* кнопка mute */
#define UI_R_VOL            12   /* слайдер гучності */

/* ═══════════════════════════════════════════
   РОЗМІРИ ЕЛЕМЕНТІВ
═══════════════════════════════════════════ */
#define UI_HBTN_W           46
#define UI_HBTN_H           40
#define UI_BTN_PREV_W       110
#define UI_BTN_PLAY_W       238
#define UI_BTN_NEXT_W       110
#define UI_BTN_CTRL_H       60
#define UI_MUTE_W           58
#define UI_MUTE_H           50
