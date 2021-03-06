/*
 *   buttons.c
 */
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#include "buttons.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(buttons, 3);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define SW_GPIO_NAME    DT_GPIO_LABEL(SW0_NODE, gpios)

#define SW0_NODE        DT_ALIAS(sw0)
#define SW1_NODE        DT_ALIAS(sw1)
#define SW2_NODE        DT_ALIAS(sw2)
#define SW3_NODE        DT_ALIAS(sw3)

#define SW_0            DT_GPIO_PIN(SW0_NODE, gpios)
#define SW_1            DT_GPIO_PIN(SW1_NODE, gpios)
#define SW_2            DT_GPIO_PIN(SW2_NODE, gpios)
#define SW_3            DT_GPIO_PIN(SW3_NODE, gpios)

#define EDGE            (GPIO_INT_EDGE | GPIO_INT_LOW_0)
#define PULL_UP         GPIO_PULL_UP

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct {
    uint8_t    id;
    uint8_t    pin;
    uint32_t   bit;
    char  * name;
} button_info_t; 

static const button_info_t button_info [] = {
    { .id = 1, .pin = SW_0, .bit = 0x00002000, .name = "SW1" },
    { .id = 2, .pin = SW_1, .bit = 0x00004000, .name = "SW2" },
    { .id = 3, .pin = SW_2, .bit = 0x00008000, .name = "SW3" },
    { .id = 4, .pin = SW_3, .bit = 0x00010000, .name = "SW4" },
};
#define BUTTONS_COUNT (sizeof(button_info)/sizeof(button_info_t))

static const button_info_t unknown = {.id=0, .pin=0 , .bit= 0, .name= "???"};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static const struct device * gpiob;
static struct gpio_callback gpio_cb_0;
static struct gpio_callback gpio_cb_1;
static struct gpio_callback gpio_cb_2;
static struct gpio_callback gpio_cb_3;

typedef struct {
    struct k_work      work;
    button_info_t    * current;
    buttons_notify_t   notify;
} buttons_t;

static buttons_t  buttons;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static const button_info_t * button_get_info(uint32_t pins)
{
    for (int i=0; i < BUTTONS_COUNT; i++) {
        if (button_info[i].bit & pins) {
            return &button_info[i];
        }
    }
    return &unknown;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void button_event(const struct device * gpiob,
                  struct gpio_callback * cb,
                  uint32_t pins)
{
    buttons.current = (button_info_t *) button_get_info(pins);
    if (buttons.current->id == INVALID_ID) {
        return;
    }

    //LOG_INF("%s pin(%d)", buttons.current->name, buttons.current->pin);

    k_work_submit(&buttons.work);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void buttons_worker(struct k_work * work)
{
    //LOG_INF("%s pin(%d)", buttons.current->name, buttons.current->pin);

    if (buttons.notify) {
        buttons.notify(buttons.current->id);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void buttons_register_notify_handler(buttons_notify_t notify)
{
    buttons.notify = notify;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void buttons_unregister_notify_handler(void)
{
    buttons.notify = NULL;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void buttons_init(void)
{
    gpiob = device_get_binding(SW_GPIO_NAME);
    if (!gpiob) {
        LOG_ERR("device not found. %s", SW_GPIO_NAME);
        return;
    }

    k_work_init(&buttons.work, buttons_worker);

    /* Init Button Interrupt */
    int flags = (GPIO_INPUT      | 
                 GPIO_ACTIVE_LOW |  
                 GPIO_PULL_UP    | 
                 GPIO_INT_EDGE);

    gpio_pin_configure(gpiob, SW_0, flags);
    gpio_pin_configure(gpiob, SW_1, flags);
    gpio_pin_configure(gpiob, SW_2, flags);
    gpio_pin_configure(gpiob, SW_3, flags);

    gpio_pin_interrupt_configure(gpiob, SW_0, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW_1, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW_2, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW_3, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&gpio_cb_0, button_event, BIT(SW_0));
    gpio_init_callback(&gpio_cb_1, button_event, BIT(SW_1));
    gpio_init_callback(&gpio_cb_2, button_event, BIT(SW_2));
    gpio_init_callback(&gpio_cb_3, button_event, BIT(SW_3));

    gpio_add_callback(gpiob, &gpio_cb_0);
    gpio_add_callback(gpiob, &gpio_cb_1);
    gpio_add_callback(gpiob, &gpio_cb_2);
    gpio_add_callback(gpiob, &gpio_cb_3);
}
