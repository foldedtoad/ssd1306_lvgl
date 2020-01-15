/*
 *   buttons.c
 */
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(buttons, 3);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define SW_GPIO_NAME    DT_GPIO_KEYS_SW0_GPIOS_CONTROLLER
#define SW_0            DT_ALIAS_SW0_GPIOS_PIN
#define SW_1            DT_ALIAS_SW1_GPIOS_PIN
#define SW_2            DT_ALIAS_SW2_GPIOS_PIN
#define SW_3            DT_ALIAS_SW3_GPIOS_PIN
#define EDGE            (GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW)
#define PULL_UP         GPIO_PUD_PULL_UP

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
    { .id = 1, .pin = DT_ALIAS_SW0_GPIOS_PIN, .bit = 0x00002000, .name = "SW1" },
    { .id = 2, .pin = DT_ALIAS_SW1_GPIOS_PIN, .bit = 0x00004000, .name = "SW2" },
    { .id = 3, .pin = DT_ALIAS_SW2_GPIOS_PIN, .bit = 0x00008000, .name = "SW3" },
    { .id = 4, .pin = DT_ALIAS_SW3_GPIOS_PIN, .bit = 0x00010000, .name = "SW4" },
};
#define BUTTONS_COUNT (sizeof(button_info)/sizeof(button_info_t))

static const button_info_t unknown = {.id=0, .pin=0 , .bit= 0, .name= "???"};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static struct device * gpiob;
static struct gpio_callback gpio_cb_0;
static struct gpio_callback gpio_cb_1;
static struct gpio_callback gpio_cb_2;
static struct gpio_callback gpio_cb_3;

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
void button_event(struct device * gpiob, struct gpio_callback * cb, u32_t pins)
{
    static const button_info_t * info;

    info = button_get_info(pins);

    LOG_INF("Button: %s pin(%d)", info->name, info->pin);
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

    gpio_pin_configure(gpiob, SW_0, GPIO_DIR_IN | GPIO_INT |  PULL_UP | EDGE);
    gpio_pin_configure(gpiob, SW_1, GPIO_DIR_IN | GPIO_INT |  PULL_UP | EDGE);
    gpio_pin_configure(gpiob, SW_2, GPIO_DIR_IN | GPIO_INT |  PULL_UP | EDGE);
    gpio_pin_configure(gpiob, SW_3, GPIO_DIR_IN | GPIO_INT |  PULL_UP | EDGE);

    gpio_init_callback(&gpio_cb_0, button_event, BIT(SW_0));
    gpio_init_callback(&gpio_cb_1, button_event, BIT(SW_1));
    gpio_init_callback(&gpio_cb_2, button_event, BIT(SW_2));
    gpio_init_callback(&gpio_cb_3, button_event, BIT(SW_3));

    gpio_add_callback(gpiob, &gpio_cb_0);
    gpio_add_callback(gpiob, &gpio_cb_1);
    gpio_add_callback(gpiob, &gpio_cb_2);
    gpio_add_callback(gpiob, &gpio_cb_3);

    gpio_pin_enable_callback(gpiob, SW_0);
    gpio_pin_enable_callback(gpiob, SW_1);
    gpio_pin_enable_callback(gpiob, SW_2);
    gpio_pin_enable_callback(gpiob, SW_3);
}
