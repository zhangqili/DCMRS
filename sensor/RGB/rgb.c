/*
 * rgb.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "rgb.h"

ATTR_NOCACHE_NOINIT_RAM_SECTION uint16_t RGB_Buffer[RGB_BUFFER_LENGTH];

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
}

void sram_init()
{
    uint32_t i;

    for (i = 0; i < 400; i++) {
        RGB_Buffer[i] = 0x0000;
    }
    for (i = 400; i < RGB_BUFFER_LENGTH; i++) {
        RGB_Buffer[i] = ZERO_PULSE;
    }
}

void RGB_Set(uint8_t r,uint8_t g,uint8_t b,uint16_t index)
{
  for (uint8_t i = 0;i < 8;i++)
  {
    RGB_Buffer[400+index*24+i]      = (g << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
    RGB_Buffer[400+index*24+i + 8]  = (r << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
    RGB_Buffer[400+index*24+i + 16] = (b << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
  }
}

void RGB_Flash()
{
    for (uint8_t i=1;i<127;i++)
    {
      RGB_Set(i,i,i,0);
      RGB_Set(i,i,i,1);
      RGB_Set(i,i,i,2);
      RGB_Set(i,i,i,3);
      //HAL_Delay(2);
    }
    for (uint8_t i=128;i>3;i--)
    {
      RGB_Set(i,i,i,0);
      RGB_Set(i,i,i,1);
      RGB_Set(i,i,i,2);
      RGB_Set(i,i,i,3);
      //HAL_Delay(2);
    }
    RGB_TurnOff();
}

void RGB_TurnOff()
{
	for (uint8_t i=0;i<LED_NUM;i++)
    {
        RGB_Set(0,0,0,i);
    }
}

void RGB_Init()
{
    struct bflb_dma_channel_lli_pool_s tx_llipool[1];
    struct bflb_dma_channel_lli_transfer_s tx_transfers[1];

    struct bflb_spi_config_s spi_cfg = {
        .freq = 16 * 1000 * 1000,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE1,
        .data_width = SPI_DATA_WIDTH_16BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_SPI0_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };

    board_spi0_gpio_init();
    sram_init();

    spi0 = bflb_device_get_by_name("spi0");
    bflb_spi_init(spi0, &spi_cfg);
    bflb_spi_link_txdma(spi0, true);

    printf("\n\rspi dma test\n\r");

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    bflb_dma_channel_init(dma0_ch0, &tx_config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    tx_transfers[0].src_addr = (uint32_t)RGB_Buffer;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    tx_transfers[0].nbytes = RGB_BUFFER_LENGTH*2;


    int used_count = bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 1, tx_transfers, 1);
    bflb_dma_channel_lli_link_head(dma0_ch0, tx_llipool, used_count);
    bflb_dma_channel_start(dma0_ch0);
    RGB_TurnOff();
}