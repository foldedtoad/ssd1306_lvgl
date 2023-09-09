/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h> 
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "buttons.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(display, 3);


static const struct device * display_dev;

#define SCREEN_ID_0    0
#define SCREEN_ID_1    1
#define SCREEN_ID_2    2
#define SCREEN_ID_3    3
#define SCREEN_COUNT   4

#define PARAM_ID_0      0
#define PARAM_ID_1      1
#define PARAM_ID_2      2
#define PARAM_ID_3      3
#define PARAM_COUNT     3  //4

LV_IMG_DECLARE(icon1);
LV_IMG_DECLARE(icon2);
LV_IMG_DECLARE(icon3);

void display_timer_handler(struct k_timer * timer);
void display_task_handler(struct k_work * work);

K_TIMER_DEFINE(display_timer, display_timer_handler, NULL);

K_WORK_DEFINE(display_work, display_task_handler);

#define TICK_PERIOD   (10)

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_task_handler(struct k_work * work)
{  
    lv_task_handler();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_timer_handler(struct k_timer * timer)
{
    k_work_submit(&display_work);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct {
    lv_obj_t     ** object; 
    short         * value;
    short           step;
    short           max;
    short           min;
} param_t;

typedef struct {
    lv_obj_t * screen;
    int        count;
    param_t  * params;
} screens_t;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

static lv_obj_t   * screen0_slider_obj;
static short        screen0_slider_value;

static lv_obj_t   * screen1_label0_obj; 
static short        screen1_label0_value = 0;
static lv_obj_t   * screen1_label1_obj; 
static short        screen1_label1_value = 0;

static lv_obj_t   * screen2_label0_obj; 
static short        screen2_label0_value = 0;
static lv_obj_t   * screen2_label1_obj; 
static short        screen2_label1_value = 0;
static lv_obj_t   * screen2_label2_obj; 
static short        screen2_label2_value = 0;

static param_t screen0_elements [] = {
    { .object = &screen0_slider_obj,  .value = &screen0_slider_value, .step = 5, .max = 100, .min = 0},
};

static param_t screen1_elements[] = {
    { .object = &screen1_label0_obj, .value = &screen1_label0_value, .step = 1, .max = 999, .min = 0 },
    { .object = &screen1_label1_obj, .value = &screen1_label1_value, .step = 1, .max = 999, .min = 0 },
};

static param_t screen2_elements [] = {
    { .object = &screen2_label0_obj, .value = &screen2_label0_value, .step = 1, .max = 999, .min = 0 },
    { .object = &screen2_label1_obj, .value = &screen2_label1_value, .step = 1, .max = 999, .min = 0 },
    { .object = &screen2_label2_obj, .value = &screen2_label2_value, .step = 1, .max = 999, .min = 0 },
};

static param_t screen3_elements [] = {
    { .object = NULL, .value = NULL, .step = 0, .max = 0, .min = 0},
};

static screens_t screens [] = {
    { .screen = NULL, .count = 1, .params = screen0_elements },
    { .screen = NULL, .count = 2, .params = screen1_elements },
    { .screen = NULL, .count = 3, .params = screen2_elements },
    { .screen = NULL, .count = 0, .params = screen3_elements },
};
#define SCREENS_COUNT (sizeof(screens)/sizeof(screens[0]))

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_param_update(int screen_id, int param_id, bool inc)
{
    param_t * param = &screens[screen_id].params[param_id];

    if (param == NULL || *param->object == NULL)
        return;

    if (inc) *param->value += param->step;
    else     *param->value -= param->step;

    if (*param->value <= param->min)  *param->value = param->min;
    if (*param->value >  param->max)  *param->value = param->max;

    if (lv_obj_check_type(*param->object, &lv_label_class)) {
        static char value[4];
        snprintf(value, sizeof(value), "%u", *param->value);
        lv_label_set_text(*param->object, value);
        return;
    }

    if (lv_obj_check_type(*param->object, &lv_slider_class)) {
        lv_slider_set_value(*param->object, *param->value, LV_ANIM_OFF);
        LOG_INF("%s: value %d", __func__, lv_slider_get_value(*param->object));
        return;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_btn_event(buttons_id_t btn_id)
{
    static int screen_id = 0;  // init to first screen id
    static int param_id  = 0;  // init to first parameter index

    switch (btn_id) {

        case BTN1_ID:
            screen_id++;
            if (screen_id >= SCREEN_COUNT)
                screen_id = 0;
            lv_scr_load(screens[screen_id].screen);
            param_id = 0;
            LOG_INF("BTN1: screen_id(%d)", screen_id);
            break;

        case BTN2_ID:
            param_id++;
            if (param_id >= screens[screen_id].count)  param_id = 0;
            if (param_id < 0)                          param_id = 0;
            LOG_INF("BTN2: screen_id(%d) param_id(%d)", screen_id, param_id);
            break;

        case BTN3_ID:
            if (screens[screen_id].count > 0) {
                display_param_update(screen_id, param_id, true);
                LOG_INF("BTN3: +%d", screens[screen_id].params[param_id].step);
            }
            break;

        case BTN4_ID:
            if (screens[screen_id].count > 0) {
                display_param_update(screen_id, param_id, false);
                LOG_INF("BTN4: -%d", screens[screen_id].params[param_id].step);
            }

        default:
            break;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_screens_init(void)
{
    screens[0].screen = lv_obj_create(NULL);
    screens[1].screen = lv_obj_create(NULL);
    screens[2].screen = lv_obj_create(NULL);
    screens[3].screen = lv_obj_create(NULL);

    static lv_style_t style_main;
    static lv_style_t style_indicator;
    static lv_style_t style_knob;

    lv_style_init(&style_main);
    lv_style_set_text_color(&style_main, lv_color_black());
    lv_style_set_bg_color(&style_main, lv_color_white());

    lv_style_init(&style_indicator);
    lv_style_set_text_color(&style_indicator, lv_color_white());
    lv_style_set_bg_color(&style_indicator, lv_color_black());
    lv_style_set_border_width(&style_indicator, 2);
    lv_style_set_radius(&style_indicator, LV_RADIUS_CIRCLE);

    lv_style_init(&style_knob);
    lv_style_set_text_color(&style_knob, lv_color_black());
    lv_style_set_bg_color(&style_knob, lv_color_white());
    lv_style_set_border_width(&style_knob, 2);
    lv_style_set_radius(&style_knob, LV_RADIUS_CIRCLE);

    /*
     *  build basic screen0
     */
    lv_scr_load(screens[0].screen);
    lv_obj_t * screen0_label = lv_label_create(lv_scr_act());
    lv_label_set_text(screen0_label, "Pg1");
    lv_obj_align_to(screen0_label, screens[0].screen, LV_ALIGN_TOP_RIGHT, 0, 0);

    screen0_slider_obj = lv_slider_create(lv_scr_act());
    lv_slider_set_mode(screen0_slider_obj, LV_SLIDER_MODE_NORMAL);
    lv_obj_add_style(screen0_slider_obj, &style_main, LV_PART_MAIN);
    lv_obj_add_style(screen0_slider_obj, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(screen0_slider_obj, &style_knob, LV_PART_KNOB);
    lv_obj_set_height(screen0_slider_obj, 8); 
    lv_obj_set_width(screen0_slider_obj, 110);    
    lv_slider_set_range(screen0_slider_obj, 0, 100);
    lv_slider_set_value(screen0_slider_obj, 15, LV_ANIM_OFF);
    screen0_slider_value = 15;

    lv_obj_align_to(screen0_slider_obj, NULL, LV_ALIGN_CENTER, 0, 0);

    /*
     *  build basic screen1
     */
    lv_scr_load(screens[1].screen);
    lv_obj_t * screen1_page = lv_label_create(lv_scr_act());
    lv_label_set_text(screen1_page, "Pg2");
    lv_obj_align_to(screen1_page, screens[1].screen, LV_ALIGN_TOP_RIGHT, 0, 0);

    screen1_label0_obj = lv_label_create(lv_scr_act());
    lv_label_set_text(screen1_label0_obj, "0");
    lv_obj_align_to(screen1_label0_obj, screens[1].screen, LV_ALIGN_BOTTOM_LEFT, 5, -5);

    screen1_label1_obj = lv_label_create(lv_scr_act());
    lv_label_set_text(screen1_label1_obj, "0");
    lv_obj_align_to(screen1_label1_obj, screens[1].screen, LV_ALIGN_BOTTOM_RIGHT, -15, -5);

    lv_obj_t * icon_1 = lv_img_create(lv_scr_act());
    lv_img_set_src(icon_1, &icon1);
    lv_obj_align_to(icon_1, NULL, LV_ALIGN_CENTER, 0, 0);

    /*
     *  build basic screen2
     */
    lv_scr_load(screens[2].screen);
    lv_obj_t * screen2_page = lv_label_create(lv_scr_act());
    lv_label_set_text(screen2_page, "Pg3");
    lv_obj_align_to(screen2_page, screens[2].screen, LV_ALIGN_TOP_RIGHT, 0, 0);

    //
    lv_obj_t * screen2_label0_tag = lv_label_create(lv_scr_act());
    lv_label_set_text(screen2_label0_tag, "value-0");
    lv_obj_align_to(screen2_label0_tag, screens[2].screen, LV_ALIGN_TOP_RIGHT, -70, 2);

    screen2_label0_obj = lv_label_create(lv_scr_act());
    lv_label_set_text(screen2_label0_obj, "0");
    lv_obj_align_to(screen2_label0_obj, screens[2].screen, LV_ALIGN_TOP_RIGHT, -45, 2);

    //
    lv_obj_t * screen2_label1_tag = lv_label_create(lv_scr_act());
    lv_label_set_text(screen2_label1_tag, "value-1");
    lv_obj_align_to(screen2_label1_tag, screens[2].screen, LV_ALIGN_RIGHT_MID, -70, 0);

    screen2_label1_obj = lv_label_create(lv_scr_act());
    lv_label_set_text(screen2_label1_obj, "0");
    lv_obj_align_to(screen2_label1_obj, screens[2].screen, LV_ALIGN_RIGHT_MID, -45, 0);

    //
    lv_obj_t * screen2_value2_tag = lv_label_create(lv_scr_act());
    lv_label_set_text(screen2_value2_tag, "value-2");
    lv_obj_align_to(screen2_value2_tag, screens[2].screen, LV_ALIGN_BOTTOM_RIGHT, -70, -2);

    screen2_label2_obj = lv_label_create(lv_scr_act());
    lv_label_set_text(screen2_label2_obj, "0");
    lv_obj_align_to(screen2_label2_obj, screens[2].screen, LV_ALIGN_BOTTOM_RIGHT, -45, -2);

    /*
     *  build basic screen3
     */
    lv_scr_load(screens[3].screen);
    lv_obj_t * screen3_page = lv_label_create(lv_scr_act());
    lv_label_set_text(screen3_page, "Pg4");
    lv_obj_align_to(screen3_page, screens[3].screen, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t * icon_3 = lv_img_create(lv_scr_act());
    lv_img_set_src(icon_3, &icon3);
    lv_obj_align_to(icon_3, NULL, LV_ALIGN_CENTER, 0, 0);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int display_init(void)
{
    display_dev = device_get_binding(DT_NODE_FULL_NAME(DT_CHOSEN(zephyr_display)));
    if (display_dev == NULL) {
        LOG_ERR("Display device not found.");
        return -1;
    }
    LOG_INF("Display device: %s", DT_NODE_FULL_NAME(DT_CHOSEN(zephyr_display)));

    display_screens_init();

    /*
     *  First screen will be screen0
     */
    lv_scr_load(screens[0].screen);

    /*
     *  Turn on display
     */
    display_blanking_off(display_dev);

    /* 
     * Register for button press notifications.
     */
    buttons_register_notify_handler(display_btn_event);

    /*
     *  Start task handler timer loop
     */
    k_timer_start(&display_timer, K_MSEC(TICK_PERIOD), K_MSEC(TICK_PERIOD));

    return 0;
};
