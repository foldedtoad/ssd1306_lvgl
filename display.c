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

#include <logging/log.h>
LOG_MODULE_REGISTER(display, 3);

static u32_t count = 0U;
static char count_str[11] = {0};
static struct device * display_dev;
static lv_obj_t * hello_world_label;
static lv_obj_t * count_label;
static lv_obj_t * slider_label;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void display_play(void)
{
    while (1) {

        if ((count % 100) == 0U) {
            sprintf(count_str, "%d", count/100U);
            lv_label_set_text(count_label, count_str);
        }
        
        lv_task_handler();
        
        k_sleep(K_MSEC(10));
        
        ++count;
    }    
}

#if 0
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
#endif

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

    lv_obj_t * slider;

    slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_height(slider, 10);
    lv_obj_set_width(slider, 110);
    lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_LEFT, 3, 0);
    lv_slider_set_value(slider, 15, LV_ANIM_ON);

#if 0
    lv_obj_set_event_cb(slider, display_slider_event); 

    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(slider_label, "0");
    //lv_obj_set_auto_realign(slider, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
#endif

    hello_world_label = lv_label_create(lv_scr_act(), NULL);

    lv_label_set_text(hello_world_label, "Hello world!");
    
    lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);

    count_label = lv_label_create(lv_scr_act(), NULL);
    
    lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    display_blanking_off(display_dev);

    return 0;
};
