#include "temt6000.h"

void TEMT6000_Init(uint8_t adc_channel)
{
    struct bflb_adc_channel_s chan[1] = {
    { .pos_chan = adc_channel,
      .neg_chan = ADC_CHANNEL_GND }};
    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = true;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, chan, 1);
}

void TEMT6000_Read(uint32_t *dat)
{
        bflb_adc_start_conversion(adc);

        while (bflb_adc_get_count(adc) < 1) {
            bflb_mtimer_delay_ms(1);
        }

        struct bflb_adc_result_s result;
        uint32_t raw_data = bflb_adc_read_raw(adc);
        printf("raw data:%08x\r\n", raw_data);
        bflb_adc_parse_result(adc, &raw_data, &result, 1);
        printf("pos chan %d,%d mv \r\n", result.pos_chan, result.millivolt);
        bflb_adc_stop_conversion(adc);
        //bflb_mtimer_delay_ms(100);
}