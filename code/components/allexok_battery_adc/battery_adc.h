#ifdef BOARD_ESP32_S3_ALEKSEI

#ifndef BATTERY_ADC_H
#define BATTERY_ADC_H

#include "esp_adc/adc_oneshot.h"
#include <stdlib.h>
#include <stdint.h>

static adc_oneshot_unit_handle_t adc_handle;
adc_cali_handle_t adc1_cali_chan0_handle = NULL;
bool adc_initialized = false;


/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    if (!calibrated) {
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
    *out_handle = handle;
    return calibrated;
}

static void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
    adc_cali_delete_scheme_curve_fitting(handle);
}

void initADC(void){
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_1, &config));

    //-------------ADC1 Calibration Init---------------//
    example_adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL_1, ADC_ATTEN_DB_12, &adc1_cali_chan0_handle);
}



static int cmp_int(const void *a, const void *b) {
    int ai = *(const int *)a, bi = *(const int *)b;
    return (ai > bi) - (ai < bi);
}

int readBattery(void) {
    if (!adc_initialized) { initADC(); adc_initialized = true; }

    enum { BLOCKS = 7, PER_BLOCK = 41 }; // 287 total reads
    int block_raw[PER_BLOCK];
    long sum_mv = 0;

    for (int b = 0; b < BLOCKS; ++b) {
        for (int i = 0; i < PER_BLOCK; ++i) {
            adc_oneshot_read(adc_handle, ADC_CHANNEL_1, &block_raw[i]);
            // ets_delay_us(200);  // optional
        }
        qsort(block_raw, PER_BLOCK, sizeof(block_raw[0]), cmp_int);
        int median_raw = block_raw[PER_BLOCK / 2];

        int mv = 0;
        adc_cali_raw_to_voltage(adc1_cali_chan0_handle, median_raw, &mv);
        sum_mv += mv;
    }

    int avg_mv = (int)(sum_mv / BLOCKS);
    return avg_mv * 2;
}


#endif
#endif