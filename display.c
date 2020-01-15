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

void display_timer_handler(struct k_timer * timer);
void display_task_handler(struct k_work * work);

K_TIMER_DEFINE(display_timer, display_timer_handler, NULL);

K_WORK_DEFINE(display_work, display_task_handler);

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
void display_slider_event(lv_obj_t * slider, lv_event_t event)
{
    static char buf [4];  // max 3 bytez for number + null-term byte.

    if (event == LV_EVENT_VALUE_CHANGED) {
        snprintf(buf, sizeof(buf), "%u", lv_slider_get_value(slider));
        lv_label_set_text(slider_label, buf);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static int my_btn_read(void)
{
    /*
     *  Use buttons_read() to get actual state of switch(es)
     */
    return -1;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
bool display_buttons_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    int btn_pr;

    static uint32_t last_btn_id = 2; /* Store the last pressed button id */

    btn_pr = my_btn_read();   /* Get the ID (0,1,2...) of the pressed button */

    /*
     *  Is there a button press? 
     *    btn_pr >= 0 indicated button ID
     *    btn_pr = -1 indicated no button was pressed
     */
    if (btn_pr >= 0) { 
        last_btn_id = btn_pr;               /* Save the pressed button ID */
        data->state = LV_INDEV_STATE_PR;    /* Set the pressed state */
    }
    else {
        data->state = LV_INDEV_STATE_REL;   /* Set the released state */
    }

    data->btn_id = last_btn_id;   /* Return the last button ID */

    return false;                 /* No buffering, so no more data read */
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

    /* 
     *  Create slider object.
     */
    lv_obj_t * slider;

    slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_height(slider, 10);
    lv_obj_set_width(slider, 110);
    lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_LEFT, 3, 0);
    lv_slider_set_value(slider, 15, LV_ANIM_ON);

    lv_obj_set_event_cb(slider, display_slider_event); 

    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(slider_label, "0");
    lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*
     *  Create input device and register it.
     */
    lv_indev_drv_t indev_drv;

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_BUTTON;
    indev_drv.read_cb = display_buttons_read;

    lv_indev_t * indev = lv_indev_drv_register(&indev_drv);
    (void) indev;

    /*
     *  Turn on display
     */
    display_blanking_off(display_dev);

    /*
     *  Start task handler timer loop
     */
    k_timer_start(&display_timer, K_MSEC(10), K_MSEC(10));

    return 0;
};
