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

    hello_world_label = lv_label_create(lv_scr_act(), NULL);

    lv_label_set_text(hello_world_label, "Hello world!");
    
    lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);

    count_label = lv_label_create(lv_scr_act(), NULL);
    
    lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    display_blanking_off(display_dev);

    return 0;
};
