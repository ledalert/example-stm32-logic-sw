#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>

void dma1_transmit(uint32_t src, uint32_t dst, int length, int channel) {
	dma_channel_reset(DMA1, channel);

	dma_set_peripheral_address(DMA1, channel, dst);
	dma_set_memory_address(DMA1, channel, src);
	dma_set_number_of_data(DMA1, channel, length);
	dma_set_read_from_memory(DMA1, channel);
	dma_enable_memory_increment_mode(DMA1, channel);
	dma_set_peripheral_size(DMA1, channel, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, channel, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, channel, DMA_CCR_PL_VERY_HIGH);

	dma_enable_channel(DMA1, channel);

}


void dma1_transmit_pp(uint32_t src, uint32_t dst, int length, int channel) {
	dma_channel_reset(DMA1, channel);

	dma_set_peripheral_address(DMA1, channel, dst);
	dma_set_memory_address(DMA1, channel, src);
	dma_set_number_of_data(DMA1, channel, length);
	dma_set_read_from_memory(DMA1, channel);
	dma_disable_memory_increment_mode(DMA1, channel);
	dma_set_peripheral_size(DMA1, channel, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, channel, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, channel, DMA_CCR_PL_VERY_HIGH);

	dma_enable_channel(DMA1, channel);

}

void usart1_send_dma(char* data, int length) {
	dma1_transmit((uint32_t)data, (uint32_t)&USART1_DR, length, DMA_CHANNEL4);
	usart_enable_tx_dma(USART1);	
}


// void sys_tick_handler(void) {
// 	dma1_transmit_pp((uint32_t)&GPIOA_IDR, (uint32_t)&USART1_DR, 256, DMA_CHANNEL4);
// 	usart_enable_tx_dma(USART1);
// }

int main() {
	rcc_clock_setup_in_hsi_out_24mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_DMA1);
	//gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO3);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	// systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	// systick_set_reload(2400000-1);  // 10 Hz
	// systick_interrupt_enable();
	// systick_counter_enable();


	usart_set_baudrate(USART1, 1000000);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);



	nvic_set_priority(NVIC_DMA1_CHANNEL4_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL4_IRQ);

	dma1_channel4_isr();


	while(1);

	return 0;
}

void dma1_channel4_isr(void) {
	dma1_transmit_pp((uint32_t)&GPIOA_IDR, (uint32_t)&USART1_DR, 65535, DMA_CHANNEL4);	//64k at a time
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);
	usart_enable_tx_dma(USART1);	
}