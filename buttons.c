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
#define SW1_PIN         DT_ALIAS_SW0_GPIOS_PIN
#define SW2_PIN         DT_ALIAS_SW1_GPIOS_PIN
#define SW3_PIN         DT_ALIAS_SW2_GPIOS_PIN
#define SW4_PIN         DT_ALIAS_SW3_GPIOS_PIN
#define ACTIVE          GPIO_INT_ACTIVE_LOW
#define PULL_UP         GPIO_PUD_PULL_UP

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct {
    int     id;
    u8_t    pin;
    u32_t   bit;
    char  * name;
} button_info_t; 

static const button_info_t button_info [] = {
    { .id = SW1_ID, .pin = SW1_PIN, .bit = 0x00002000, .name = "SW1" },
    { .id = SW2_ID, .pin = SW2_PIN, .bit = 0x00004000, .name = "SW2" },
    { .id = SW3_ID, .pin = SW3_PIN, .bit = 0x00008000, .name = "SW3" },
    { .id = SW4_ID, .pin = SW4_PIN, .bit = 0x00010000, .name = "SW4" },
};
#define BUTTONS_COUNT (sizeof(button_info)/sizeof(button_info_t))

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static struct device * gpiob;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int buttons_get_state(int id)
{
    int state;

    if (id <0 || id >= BUTTONS_COUNT) {
        return -1;
    }

    gpio_pin_read(gpiob, button_info[id].pin, &state);

    if (state == 0) {  // note button-pressed state inversion
        return button_info[id].id;
    }
    return NO_PRESS;
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

    gpio_pin_configure(gpiob, SW1_PIN, GPIO_DIR_IN | PULL_UP | ACTIVE);
    gpio_pin_configure(gpiob, SW2_PIN, GPIO_DIR_IN | PULL_UP | ACTIVE);
    gpio_pin_configure(gpiob, SW3_PIN, GPIO_DIR_IN | PULL_UP | ACTIVE);
    gpio_pin_configure(gpiob, SW4_PIN, GPIO_DIR_IN | PULL_UP | ACTIVE);
}
