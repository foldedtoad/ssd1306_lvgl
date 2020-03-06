/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#include "display.h"
#include "buttons.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(display, 3);

static struct device * display_dev;

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


void display_timer_handler(struct k_timer * timer);
void display_task_handler(struct k_work * work);

K_TIMER_DEFINE(display_timer, display_timer_handler, NULL);

K_WORK_DEFINE(display_work, display_task_handler);

#define TICK_PERIOD   (10)

LV_IMG_DECLARE(icon1);
LV_IMG_DECLARE(icon2);
LV_IMG_DECLARE(icon3);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_task_handler(struct k_work * work)
{
    lv_tick_inc(TICK_PERIOD);
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
    lv_obj_type_t types;

    //LOG_INF("%s: screen_id(%u), param_id(%d)", __func__, screen_id, param_id);

    param_t * param = &screens[screen_id].params[param_id];

    if (param == NULL || *param->object == NULL)
        return;

    lv_obj_get_type(*param->object, &types);
    //LOG_INF("%s: type: %s", __func__, types.type[0]);

    if (inc) *param->value += param->step;
    else     *param->value -= param->step;

    if (*param->value <= param->min)  *param->value = param->min;
    if (*param->value >  param->max)  *param->value = param->max;

    if (strcmp("lv_label", types.type[0]) == 0) {
        static char value[4];
        snprintf(value, sizeof(value), "%u", *param->value);
        lv_label_set_text(*param->object, value);
        return;
    }

    if (strcmp("lv_slider", types.type[0]) == 0) {
        lv_slider_set_value(*param->object, *param->value, LV_ANIM_ON);
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

    //LOG_INF("%s: BTN%d", __func__, btn_id);
    
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
                LOG_INF("BTN3: -%d", screens[screen_id].params[param_id].step);
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
    screens[0].screen = lv_obj_create(NULL, NULL);
    screens[1].screen = lv_obj_create(NULL, NULL);
    screens[2].screen = lv_obj_create(NULL, NULL);
    screens[3].screen = lv_obj_create(NULL, NULL);

    lv_theme_t * mono = lv_theme_mono_init(0, NULL);
    lv_theme_set_current(mono);

    /*
     *  build basic screen0
     */
    lv_scr_load(screens[0].screen);
    lv_obj_t * screen0_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen0_label, "Pg1");
    lv_obj_align(screen0_label, screens[0].screen, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    screen0_slider_obj = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_height(screen0_slider_obj, 10);
    lv_obj_set_width(screen0_slider_obj, 110);
    lv_obj_align(screen0_slider_obj, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_value(screen0_slider_obj, 15, LV_ANIM_ON);
    screen0_slider_value = 15;

    /*
     *  build basic screen1
     */
    lv_scr_load(screens[1].screen);
    lv_obj_t * screen1_page = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen1_page, "Pg2");
    lv_obj_align(screen1_page, screens[1].screen, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    screen1_label0_obj = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen1_label0_obj, "0");
    lv_obj_align(screen1_label0_obj, screens[1].screen, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5);

    screen1_label1_obj = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen1_label1_obj, "0");
    lv_obj_align(screen1_label1_obj, screens[1].screen, LV_ALIGN_IN_BOTTOM_RIGHT, -15, -5);

    lv_obj_t * icon_1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(icon_1, &icon1);
    lv_obj_align(icon_1, NULL, LV_ALIGN_CENTER, 0, 0);

    /*
     *  build basic screen2
     */
    lv_scr_load(screens[2].screen);
    lv_obj_t * screen2_page = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_page, "Pg3");
    lv_obj_align(screen2_page, screens[2].screen, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    //
    lv_obj_t * screen2_label0_tag = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_label0_tag, "value-0");
    lv_obj_align(screen2_label0_tag, screens[2].screen, LV_ALIGN_IN_TOP_RIGHT, -70, 2);

    screen2_label0_obj = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_label0_obj, "0");
    lv_obj_align(screen2_label0_obj, screens[2].screen, LV_ALIGN_IN_TOP_RIGHT, -45, 2);

    //
    lv_obj_t * screen2_label1_tag = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_label1_tag, "value-1");
    lv_obj_align(screen2_label1_tag, screens[2].screen, LV_ALIGN_IN_RIGHT_MID, -70, 0);

    screen2_label1_obj = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_label1_obj, "0");
    lv_obj_align(screen2_label1_obj, screens[2].screen, LV_ALIGN_IN_RIGHT_MID, -45, 0);

    //
    lv_obj_t * screen2_value2_tag = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_value2_tag, "value-2");
    lv_obj_align(screen2_value2_tag, screens[2].screen, LV_ALIGN_IN_BOTTOM_RIGHT, -70, -2);

    screen2_label2_obj = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_label2_obj, "0");
    lv_obj_align(screen2_label2_obj, screens[2].screen, LV_ALIGN_IN_BOTTOM_RIGHT, -45, -2);

#if 0
    lv_obj_t * icon_2 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(icon_2, &icon2);
#endif

    /*
     *  build basic screen3
     */
    lv_scr_load(screens[3].screen);
    lv_obj_t * screen3_page = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen3_page, "Pg4");
    lv_obj_align(screen3_page, screens[3].screen, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    lv_obj_t * icon_3 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(icon_3, &icon3);
    lv_obj_align(icon_3, NULL, LV_ALIGN_CENTER, 0, 0);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int display_init(void)
{
    display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

    if (display_dev == NULL) {
        LOG_ERR("device not found. %s", CONFIG_LVGL_DISPLAY_DEV_NAME);
        return -1;
    }

    lv_init();

    display_screens_init();

    /*
     *  First screen will be screen1
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
