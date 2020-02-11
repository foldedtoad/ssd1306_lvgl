/*
 * Copyright (c) 20120 Robin Callender <robin@callender-consulting.com>
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
LOG_MODULE_REGISTER(display_btn, 3);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void btn1_click_action(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        LOG_INF("BUTTON-1 action");
    }
}

static void btn2_click_action(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        LOG_INF("BUTTON-2 action");
    }
}

static void btn3_click_action(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        LOG_INF("BUTTON-3 action");
    }
}

static void btn4_click_action(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        LOG_INF("BUTTON-4 action");
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static bool display_btn_read_sw1(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    int btn_pr;

    static uint32_t last_btn_id = SW1_ID; /* Store the last pressed button id */

    btn_pr = buttons_get_state(SW1_ID);

    /*
     *  Is there a button press? 
     *    btn_pr >= 0 indicated button ID
     *    btn_pr = -1 indicated no button was pressed
     */
    if (btn_pr >= 0) {
        //LOG_INF("SW1 Pressed");
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
static bool display_btn_read_sw2(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    int btn_pr;

    static uint32_t last_btn_id = SW2_ID;

    btn_pr = buttons_get_state(SW2_ID);

    /*
     *  Is there a button press? 
     *    btn_pr >= 0 indicated button ID
     *    btn_pr = -1 indicated no button was pressed
     */
    if (btn_pr >= 0) {
        //LOG_INF("SW2 Pressed");
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
static bool display_btn_read_sw3(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    int btn_pr;

    static uint32_t last_btn_id = SW3_ID;

    btn_pr = buttons_get_state(SW3_ID);

    /*
     *  Is there a button press? 
     *    btn_pr >= 0 indicated button ID
     *    btn_pr = -1 indicated no button was pressed
     */
    if (btn_pr >= 0) {
        //LOG_INF("SW3 Pressed");
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
static bool display_btn_read_sw4(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    int btn_pr;

    static uint32_t last_btn_id = SW4_ID;

    btn_pr = buttons_get_state(SW4_ID);

    /*
     *  Is there a button press? 
     *    btn_pr >= 0 indicated button ID
     *    btn_pr = -1 indicated no button was pressed
     */
    if (btn_pr >= 0) {
        //LOG_INF("SW4 Pressed");
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
void display_btn_init(void)
{
    /*
     *  Each pair defines a point within a button corresponding to click point.
     */
    static lv_point_t   points_array[] =  { 
        {40,  20},  // BTN1
        {60,  20},  // BTN2
        {80,  20},  // BTN3
        {100, 20}   // BTN4
    };

    /*
     *  Create input devices and register them.
     */
    lv_indev_drv_t indev_drv1;
    lv_indev_drv_t indev_drv2;
    lv_indev_drv_t indev_drv3;
    lv_indev_drv_t indev_drv4;

    lv_indev_drv_init(&indev_drv1);
    indev_drv1.type = LV_INDEV_TYPE_BUTTON;
    indev_drv1.read_cb = display_btn_read_sw1;

    lv_indev_drv_init(&indev_drv2);
    indev_drv2.type = LV_INDEV_TYPE_BUTTON;
    indev_drv2.read_cb = display_btn_read_sw2;

    lv_indev_drv_init(&indev_drv3);
    indev_drv3.type = LV_INDEV_TYPE_BUTTON;
    indev_drv3.read_cb = display_btn_read_sw3;

    lv_indev_drv_init(&indev_drv4);
    indev_drv4.type = LV_INDEV_TYPE_BUTTON;
    indev_drv4.read_cb = display_btn_read_sw4;

    lv_indev_t * indev1 = lv_indev_drv_register(&indev_drv1);
    lv_indev_t * indev2 = lv_indev_drv_register(&indev_drv2);
    lv_indev_t * indev3 = lv_indev_drv_register(&indev_drv3);
    lv_indev_t * indev4 = lv_indev_drv_register(&indev_drv4);

    lv_indev_set_button_points(indev1, points_array);
    lv_indev_set_button_points(indev2, points_array);
    lv_indev_set_button_points(indev3, points_array);
    lv_indev_set_button_points(indev4, points_array);

    /*
     *  Define button(s)
     */
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos( btn1, 30, 15);    /* Set its position */
    lv_obj_set_size(btn1, 15, 15);    /* Set its size */
    lv_obj_set_event_cb(btn1, btn1_click_action);

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn1);
    lv_obj_set_pos( btn2, 50, 15);    /* Set its position */
    lv_obj_set_size(btn2, 15, 15);    /* Set its size */
    lv_obj_set_event_cb(btn2, btn2_click_action);

    lv_obj_t * btn3 = lv_btn_create(lv_scr_act(), btn1);
    lv_obj_set_pos( btn3, 70, 15);    /* Set its position */
    lv_obj_set_size(btn3, 15, 15);    /* Set its size */
    lv_obj_set_event_cb(btn3, btn3_click_action);

    lv_obj_t * btn4 = lv_btn_create(lv_scr_act(), btn1);
    lv_obj_set_pos( btn4, 90, 15);    /* Set its position */
    lv_obj_set_size(btn4, 15, 15);    /* Set its size */
    lv_obj_set_event_cb(btn4, btn4_click_action);
}
