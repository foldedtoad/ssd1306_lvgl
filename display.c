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
static lv_obj_t      * slider_label;

#define SCREEN_COUNT   4

static lv_obj_t * screens [SCREEN_COUNT];

void display_timer_handler(struct k_timer * timer);
void display_task_handler(struct k_work * work);

K_TIMER_DEFINE(display_timer, display_timer_handler, NULL);

K_WORK_DEFINE(display_work, display_task_handler);

#define TICK_PERIOD   (10)

LV_IMG_DECLARE(icon1);

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
void display_btn_event(buttons_id_t btn_id)
{
    LOG_INF("%s: BTN%d", __func__, btn_id);
    
    switch (btn_id) {

        case BTN1_ID: {
            static int index = 0;  // init to first screen
            index++;
            if (index >= SCREEN_COUNT) index = 0;
            lv_scr_load(screens[index]);
            break;
        }

        case BTN2_ID:
            break;

        case BTN3_ID:
            break;

        case BTN4_ID:
            break;

        default:
            break;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_slider_event(lv_obj_t * slider, lv_event_t event)
{
    static char buf [4];  // max 3 bytes for number + null-term byte.

    if (event == LV_EVENT_VALUE_CHANGED) {
        snprintf(buf, sizeof(buf), "%u", lv_slider_get_value(slider));
        lv_label_set_text(slider_label, buf);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_screens_init(void)
{
    screens[0] = lv_obj_create(NULL, NULL);
    screens[1] = lv_obj_create(NULL, NULL);
    screens[2] = lv_obj_create(NULL, NULL);
    screens[3] = lv_obj_create(NULL, NULL);

    /*
     *  build basic screen0
     */
    lv_scr_load(screens[0]);
    lv_obj_t * screen0_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen0_label, "Pg1");
    lv_obj_align(screen0_label, screens[0], LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    /*
     *  build basic screen1
     */
    lv_scr_load(screens[1]);
    lv_obj_t * screen1_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen1_label, "Pg2");
    lv_obj_align(screen1_label, screens[1], LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    lv_obj_t * icon = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(icon, &icon1);

    /*
     *  build basic screen2
     */
    lv_scr_load(screens[2]);
    lv_obj_t * screen2_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen2_label, "Pg3");
    lv_obj_align(screen2_label, screens[2], LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    /*
     *  build basic screen3
     */
    lv_scr_load(screens[3]);
    lv_obj_t * screen3_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(screen3_label, "Pg4");
    lv_obj_align(screen3_label, screens[3], LV_ALIGN_IN_TOP_RIGHT, 0, 0);
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
    lv_scr_load(screens[0]);

    /* 
     *  Create slider object.
     */
    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_height(slider, 10);
    lv_obj_set_width(slider, 110);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_value(slider, 15, LV_ANIM_ON);

    lv_obj_set_event_cb(slider, display_slider_event); 

    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(slider_label, "0");
    lv_obj_align(slider_label, slider, LV_ALIGN_IN_BOTTOM_LEFT, 0, 10);

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
