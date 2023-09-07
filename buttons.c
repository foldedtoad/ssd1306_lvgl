/*
 *   buttons.c
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

#include "buttons.h"

#include <zephyr/logging/log.h>
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

#define BUTTON_DEBOUNCE_DELAY_MS 350

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct {
    uint8_t    id;
    uint8_t    pin;
    uint32_t   bit;
    char  * name;
} button_info_t; 

#define SW_GPIO_DEV    DT_NODELABEL(gpio0)

#define SW0_PIN        DT_GPIO_PIN(DT_ALIAS(sw0), gpios)
#define SW1_PIN        DT_GPIO_PIN(DT_ALIAS(sw1), gpios)
#define SW2_PIN        DT_GPIO_PIN(DT_ALIAS(sw2), gpios)
#define SW3_PIN        DT_GPIO_PIN(DT_ALIAS(sw3), gpios)

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
static const struct device * gpiob;

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
void buttons_event(const struct device * gpiob,
                   struct gpio_callback * cb,
                   uint32_t pins)
{
    static uint32_t curr_time;
    static uint32_t last_time;

    buttons.current = (button_info_t *) button_get_info(pins);
    if (buttons.current->id == INVALID_ID) {
        return;
    }

    curr_time = k_uptime_get_32();

    if (curr_time < last_time + BUTTON_DEBOUNCE_DELAY_MS) {
        last_time = curr_time;
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
    gpiob = DEVICE_DT_GET(SW_GPIO_DEV);
    if (!gpiob) {
        LOG_ERR("device not found. %s", DEVICE_DT_NAME(SW_GPIO_DEV));
        return;
    }

    k_work_init(&buttons.work, buttons_worker);

    /* Init Button Interrupt */
    int flags = (GPIO_INPUT      | 
                 GPIO_ACTIVE_LOW |  
                 GPIO_PULL_UP    | 
                 GPIO_INT_EDGE);

    gpio_pin_configure(gpiob, SW0_PIN, flags);
    gpio_pin_configure(gpiob, SW1_PIN, flags);
    gpio_pin_configure(gpiob, SW2_PIN, flags);
    gpio_pin_configure(gpiob, SW3_PIN, flags);

    gpio_pin_interrupt_configure(gpiob, SW0_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW1_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW2_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(gpiob, SW3_PIN, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&buttons_cb, buttons_event,
                       BIT(SW0_PIN) |
                       BIT(SW1_PIN) |
                       BIT(SW2_PIN) |
                       BIT(SW3_PIN) );

    gpio_add_callback(gpiob, &buttons_cb);

}
