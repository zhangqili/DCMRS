#include "main.h"
#include "display.h"
#include "fezui.h"
#include "fezui_var.h"


struct bflb_device_s *i2c0;

u8g2_t u8g2;
int32_t count=0;

int main(void)
{
    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 1000000);

    fezui_init(&fezui);

    while (1) {
    }
}
