#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include "filter.h"

static void clock_setup(void)
{
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_84MHZ]);

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_ADC1);
    rcc_periph_clock_enable(RCC_DAC);
    rcc_periph_clock_enable(RCC_TIM2);
}

static void gpio_setup(void)
{
    // ADC PA0
    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);

    // DAC PA4
    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);
}

static void adc_setup(void)
{
    adc_power_off(ADC1);

    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY2);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_56CYC);

    adc_power_on(ADC1);
}

static void dac_setup(void)
{
    dac_enable(CHANNEL_1);
}

static void tim2_setup(void)
{
    timer_set_prescaler(TIM2, 84 - 1); // 1 MHz
    timer_set_period(TIM2, 64);        // ≈ 15.5 кГц

    timer_enable_irq(TIM2, TIM_DIER_UIE);
    nvic_enable_irq(NVIC_TIM2_IRQ);

    timer_enable_counter(TIM2);
}

void tim2_isr(void)
{
    if (timer_get_flag(TIM2, TIM_SR_UIF)) {
        timer_clear_flag(TIM2, TIM_SR_UIF);

        // --- ADC ---
        adc_start_conversion_regular(ADC1);
        while (!adc_eoc(ADC1));
        uint16_t adc_val = adc_read_regular(ADC1);

        // --- Нормализация ---
        float x = ((float)adc_val - 2048.0f) / 2048.0f;

        // --- ФИЛЬТР ---
        float y = iir_filter(x);

        // --- В DAC ---
        int32_t dac_val = (int32_t)(y * 2048.0f + 2048.0f);

        if (dac_val > 4095) dac_val = 4095;
        if (dac_val < 0) dac_val = 0;

        dac_load_data_buffer_single(dac_val, RIGHT12, CHANNEL_1);
    }
}

int main(void)
{
    clock_setup();
    gpio_setup();
    adc_setup();
    dac_setup();
    tim2_setup();

    while (1);
}
