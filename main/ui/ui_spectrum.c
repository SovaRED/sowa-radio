#include "ui_spectrum.h"
#include "ui_theme.h"
#include <string.h>
#include <math.h>

#define SPEC_BARS   96
#define SPEC_BAR_W  7
#define SPEC_GAP    1

static lv_obj_t     *canvas;
static lv_draw_buf_t draw_buf;
static uint8_t       cbuf[UI_SCR_W * UI_SPECTRUM_H * 4];

static float bar_h[SPEC_BARS];
static float bar_tgt[SPEC_BARS];
static float bar_vel[SPEC_BARS];
static float bar_fall[SPEC_BARS];
static float sim_phase[SPEC_BARS];
static float sim_freq[SPEC_BARS];
static bool  is_playing = false;
static float sim_t = 0.0f;

/* ── Прямі оголошення ──────────────────────────────────── */
static void spec_timer_cb(lv_timer_t *timer);
static void spec_draw(void);

/* ── Ініціалізація ─────────────────────────────────────── */
void ui_spectrum_init(lv_obj_t *scr)
{
    for (int i = 0; i < SPEC_BARS; i++) {
        bar_h[i]     = 0.0f;
        bar_tgt[i]   = 0.0f;
        bar_vel[i]   = 0.07f + ((float)(i % 7)) * 0.01f;
        bar_fall[i]  = 0.035f + ((float)(i % 5)) * 0.007f;
        sim_phase[i] = ((float)i / SPEC_BARS) * 6.28f;
        sim_freq[i]  = 0.6f + ((float)(i % 11)) * 0.13f;
    }

    lv_draw_buf_init(&draw_buf, UI_SCR_W, UI_SPECTRUM_H,
                     LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO,
                     cbuf, sizeof(cbuf));

    canvas = lv_canvas_create(scr);
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_obj_set_size(canvas, UI_SCR_W, UI_SPECTRUM_H);
    lv_obj_set_pos(canvas, 0, UI_SCR_H - UI_VOL_H - UI_SPECTRUM_H);
    lv_obj_set_style_opa(canvas, LV_OPA_COVER, 0);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_SCROLLABLE);

    lv_timer_create(spec_timer_cb, 33, NULL);
}

/* ── Таймер ────────────────────────────────────────────── */
static void spec_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    sim_t += 0.016f;

    for (int i = 0; i < SPEC_BARS; i++) {
        if (is_playing) {
            float pos = (float)i / SPEC_BARS;
            float n1 = sinf(sim_t * sim_freq[i] + sim_phase[i]) * 0.5f + 0.5f;
            float n2 = sinf(sim_t * sim_freq[i] * 0.6f + sim_phase[i] * 1.4f) * 0.5f + 0.5f;
            float n3 = sinf(sim_t * 1.8f + (float)i * 0.13f) * 0.5f + 0.5f;
            float bass = (pos < 0.15f) ? (1.0f - pos / 0.15f) * 0.35f : 0.0f;
            bar_tgt[i] = (n1 * 0.5f + n2 * 0.3f + n3 * 0.2f) * (0.85f + bass);
            if (bar_tgt[i] > 1.0f) bar_tgt[i] = 1.0f;
            if (bar_tgt[i] < 0.03f) bar_tgt[i] = 0.03f;
            bar_h[i] += (bar_tgt[i] - bar_h[i]) * bar_vel[i];
        } else {
            bar_h[i] *= (1.0f - bar_fall[i]);
            if (bar_h[i] < 0.002f) bar_h[i] = 0.0f;
        }
    }
    spec_draw();
}

/* ── Відмалювання ──────────────────────────────────────── */
static void spec_draw(void)
{
    int H = UI_SPECTRUM_H;

    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    for (int i = 0; i < SPEC_BARS; i++) {
        if (bar_h[i] < 0.005f) continue;

        int bh = (int)(bar_h[i] * (float)H * 0.92f);
        if (bh < 1) bh = 1;
        int bx = i * (SPEC_BAR_W + SPEC_GAP);
        int by = H - bh;
        float hf = (float)bh / ((float)H * 0.9f);
        if (hf > 1.0f) hf = 1.0f;

        lv_draw_rect_dsc_t dsc;
        lv_draw_rect_dsc_init(&dsc);
        dsc.radius = 2;
        dsc.bg_opa = LV_OPA_COVER;
        dsc.bg_color = lv_color_make(140, 210, 255);
        dsc.bg_grad.dir = LV_GRAD_DIR_VER;
        dsc.bg_grad.stops[0].color = lv_color_make(140, 210, 255);
        dsc.bg_grad.stops[0].opa   = (uint8_t)((0.35f + hf * 0.6f) * 255.0f);
        dsc.bg_grad.stops[0].frac  = 0;
        dsc.bg_grad.stops[1].color = lv_color_make(20, 70, 170);
        dsc.bg_grad.stops[1].opa   = 13;
        dsc.bg_grad.stops[1].frac  = 255;
        dsc.bg_grad.stops_count    = 2;

        lv_area_t area = {bx, by, bx + SPEC_BAR_W - 1, H - 1};
        lv_draw_rect(&layer, &dsc, &area);

        if (bh > 8) {
            lv_draw_rect_dsc_t hdsc;
            lv_draw_rect_dsc_init(&hdsc);
            hdsc.radius = 1;
            hdsc.bg_opa = (uint8_t)((0.5f + hf * 0.5f) * 255.0f);
            hdsc.bg_color = lv_color_make(200, 235, 255);
            hdsc.bg_grad.dir = LV_GRAD_DIR_NONE;
            lv_area_t ha = {bx, by, bx + SPEC_BAR_W - 1, by + 2};
            lv_draw_rect(&layer, &hdsc, &ha);
        }
    }

    lv_canvas_finish_layer(canvas, &layer);
}

/* ── Публічні функції ──────────────────────────────────── */
void ui_spectrum_update(const float *fft_data, int len)
{
    for (int i = 0; i < SPEC_BARS && i < len; i++) {
        bar_tgt[i] = fft_data[i];
        if (bar_tgt[i] > 1.0f) bar_tgt[i] = 1.0f;
        if (bar_tgt[i] < 0.0f) bar_tgt[i] = 0.0f;
    }
}

void ui_spectrum_set_playing(bool playing)
{
    is_playing = playing;
}
