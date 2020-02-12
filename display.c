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
#include "display_btn.h"
#include "buttons.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(display, 3);

static struct device * display_dev;
static lv_obj_t      * slider;
static lv_obj_t      * slider_label;

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
void display_slider_label_update(void)
{
    static char buf [4];  // max 3 bytes for number + null-term byte.

    snprintf(buf, sizeof(buf), "%u", lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_slider_update(int sw_id)
{
    int value = lv_slider_get_value(slider);

    switch (sw_id) {

        case SW1_ID:
            value += 5;
            lv_slider_set_value(slider, value, LV_ANIM_ON);
            break;

        case SW2_ID:
            value -= 5;
            lv_slider_set_value(slider, value, LV_ANIM_ON);
            break;

        default:
            break;
    }

    display_slider_label_update();
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

    /* 
     *  Create slider object.
     */
    slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_height(slider, 10);
    lv_obj_set_width(slider, 110);
    lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_LEFT, 3, 0);
    lv_slider_set_value(slider, 10, LV_ANIM_ON);

    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(slider_label, "0");
    lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    display_slider_label_update();

    /* 
     *  Initialize external hardware buttons
     */
    display_btn_init();

    /*
     *  Turn on display
     */
    display_blanking_off(display_dev);

    /*
     *  Start task handler timer loop
     */
    k_timer_start(&display_timer, K_MSEC(TICK_PERIOD), K_MSEC(TICK_PERIOD));

    return 0;
};
