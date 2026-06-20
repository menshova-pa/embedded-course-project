#include <stdio.h>

#include <libopencm3/cm3/nvic.h>

#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "filter.h"

// =====================================================
// DEFINES
// =====================================================

#define LED_PORT       GPIOD
#define LED_PIN        GPIO12

// =====================================================
// ADC
// =====================================================

static uint16_t read_adc(uint8_t ch)
{
    uint8_t channels[1] = { ch };

    adc_set_regular_sequence(
        ADC1,
        1,
        channels
    );

    adc_start_conversion_regular(ADC1);

    while (!adc_eoc(ADC1));

    return (uint16_t)adc_read_regular(ADC1);
}

// =====================================================
// TIMER ISR
// =====================================================

void tim2_isr(void)
{
    if (timer_get_flag(TIM2, TIM_SR_UIF)) {

        timer_clear_flag(
            TIM2,
            TIM_SR_UIF
        );

        // ADC
        uint16_t adc_val = read_adc(0);

        // 0..4095 -> -1..1
        float x =
            ((float)adc_val / 4095.0f)
            * 2.0f - 1.0f;

        // FILTER
        float y = iir_filter(x);

        // -1..1 -> 0..1
        y = (y + 1.0f) * 0.5f;

        // CLIPPING
        if (y < 0.0f)
            y = 0.0f;

        if (y > 1.0f)
            y = 1.0f;

        // DAC VALUE
        uint16_t dac_val =
            (uint16_t)(y * 4095.0f);

        // DAC OUTPUT
        dac_load_data_buffer_single(
            DAC1,
            dac_val,
            DAC_ALIGN_RIGHT12,
            DAC_CHANNEL2
        );

        dac_software_trigger(
            DAC1,
            DAC_CHANNEL2
        );

        // LED
        gpio_toggle(
            LED_PORT,
            LED_PIN
        );
    }
}

// =====================================================
// CLOCK
// =====================================================

static void clock_setup(void)
{
    rcc_clock_setup_pll(
        &rcc_hse_8mhz_3v3[
            RCC_CLOCK_3V3_168MHZ
        ]
    );

    rcc_periph_clock_enable(RCC_GPIOD);

    rcc_periph_clock_enable(RCC_GPIOA);

    rcc_periph_clock_enable(RCC_USART2);

    rcc_periph_clock_enable(RCC_ADC1);

    rcc_periph_clock_enable(RCC_DAC);

    rcc_periph_clock_enable(RCC_TIM2);
}

// =====================================================
// GPIO
// =====================================================

static void gpio_setup(void)
{
    // LED
    gpio_mode_setup(
        LED_PORT,
        GPIO_MODE_OUTPUT,
        GPIO_PUPD_NONE,
        LED_PIN
    );

    // ADC PA0
    gpio_mode_setup(
        GPIOA,
        GPIO_MODE_ANALOG,
        GPIO_PUPD_NONE,
        GPIO0
    );

    // DAC PA5
    gpio_mode_setup(
        GPIOA,
        GPIO_MODE_ANALOG,
        GPIO_PUPD_NONE,
        GPIO5
    );

    // USART2 TX -> PA2
    gpio_mode_setup(
        GPIOA,
        GPIO_MODE_AF,
        GPIO_PUPD_NONE,
        GPIO2
    );

    gpio_set_af(
        GPIOA,
        GPIO_AF7,
        GPIO2
    );
}

// =====================================================
// ADC SETUP
// =====================================================

static void adc_setup(void)
{
    adc_power_off(ADC1);

    adc_disable_scan_mode(ADC1);

    adc_set_clk_prescale(
        ADC_CCR_ADCPRE_BY2
    );

    adc_set_sample_time_on_all_channels(
        ADC1,
        ADC_SMPR_SMP_84CYC
    );

    adc_power_on(ADC1);
}

// =====================================================
// DAC SETUP
// =====================================================

static void dac_setup(void)
{
    dac_disable(
        DAC1,
        DAC_CHANNEL2
    );

    dac_disable_waveform_generation(
        DAC1,
        DAC_CHANNEL2
    );

    dac_enable(
        DAC1,
        DAC_CHANNEL2
    );
}


// =====================================================
// TIMER
// =====================================================

static void tim_setup(void)
{
    nvic_enable_irq(
        NVIC_TIM2_IRQ
    );

    rcc_periph_reset_pulse(
        RST_TIM2
    );

    timer_set_mode(
        TIM2,
        TIM_CR1_CKD_CK_INT,
        TIM_CR1_CMS_EDGE,
        TIM_CR1_DIR_UP
    );

    // TIMER CLOCK = 1 MHz
    timer_set_prescaler(
        TIM2,
        (rcc_apb1_frequency * 2)
        / 1000000
    );

    // Fs = 15500 Hz
    timer_set_period(
        TIM2,
        1000000 / 15500
    );

    timer_enable_irq(
        TIM2,
        TIM_DIER_UIE
    );

    timer_enable_counter(
        TIM2
    );
}

// =====================================================
// MAIN
// =====================================================

int main(void)
{
    clock_setup();

    gpio_setup();

    adc_setup();

    dac_setup();

    tim_setup();

   
    while (1) {
    }
}
