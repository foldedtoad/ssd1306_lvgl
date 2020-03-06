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

#define SW_GPIO_NAME    DT_GPIO_KEYS_SW0_GPIOS_CONTROLLER

#define SW0_PIN         DT_ALIAS_SW0_GPIOS_PIN
#define SW1_PIN         DT_ALIAS_SW1_GPIOS_PIN
#define SW2_PIN         DT_ALIAS_SW2_GPIOS_PIN
#define SW3_PIN         DT_ALIAS_SW3_GPIOS_PIN

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct {
    u8_t    id;
    u8_t    pin;
    u32_t   bit;
    char  * name;
} button_info_t; 

static const button_info_t button_info [] = {
    { .id = BTN1_ID, .pin = SW0_PIN, .bit = BIT(SW0_PIN), .name = "BTN1" },
    { .id = BTN2_ID, .pin = SW1_PIN, .bit = BIT(SW1_PIN), .name = "BTN2" },
    { .id = BTN3_ID, .pin = SW2_PIN, .bit = BIT(SW2_PIN), .name = "BTN3" },
    { .id = BTN4_ID, .pin = SW3_PIN, .bit = BIT(SW3_PIN), .name = "BTN4" },
};
#define BUTTONS_COUNT (sizeof(button_info)/sizeof(button_info_t))

static const button_info_t unknown = {.id=INVALID_ID, .pin=0 , .bit= 0, .name= "???"};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static struct device * gpiob;

static struct gpio_callback buttons_cb;

typedef struct {
    struct k_work      work;
    button_info_t    * current;
    buttons_notify_t   notify;
} buttons_t;

static buttons_t  buttons;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static const button_info_t * button_get_info(u32_t pins)
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
void buttons_event(struct device * gpiob, struct gpio_callback * cb, u32_t pins)
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

    gpio_pin_configure(gpiob, SW0_PIN, GPIO_INPUT | DT_ALIAS_SW0_GPIOS_FLAGS);
    gpio_pin_configure(gpiob, SW1_PIN, GPIO_INPUT | DT_ALIAS_SW1_GPIOS_FLAGS);
    gpio_pin_configure(gpiob, SW2_PIN, GPIO_INPUT | DT_ALIAS_SW2_GPIOS_FLAGS);
    gpio_pin_configure(gpiob, SW3_PIN, GPIO_INPUT | DT_ALIAS_SW3_GPIOS_FLAGS);

    gpio_pin_interrupt_configure(gpiob, SW0_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW1_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW2_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW3_PIN, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&buttons_cb, buttons_event,
                       BIT(SW0_PIN) | BIT(SW1_PIN) | BIT(SW2_PIN) | BIT(SW3_PIN));

    gpio_add_callback(gpiob, &buttons_cb);

}
