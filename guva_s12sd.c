#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include <esp_idf_lib_helpers.h>


#include "guva_s12sd.h"

#define VOLTAGE_CUTOFF_LOW 0 
#define VOLTAGE_CUTOFF_HIGH 1100
#define VOLTAGE_MULT_FACTOR 1.0

static const char *TAG = "guva_s12sd";

static bool init_adc(guva_s12sd_t *dev)
{
    esp_err_t ret;
    bool calib_enable = false;

    ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
    if (ret == ESP_ERR_NOT_SUPPORTED) 
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    else if (ret == ESP_ERR_INVALID_VERSION) 
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    else if (ret == ESP_OK) 
    {
        calib_enable = true;
        // MEASURING RANGES FOR ADC_ATTEN_DB_11  150 mV ~ 2450 mV. We only need 0 - 1000mV for our sensor.
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_10, 0, &dev->calib_chars);
        ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_10));
        ESP_ERROR_CHECK(adc1_config_channel_atten(dev->channel, ADC_ATTEN_DB_11));
    }
    else 
        ESP_LOGE(TAG, "Invalid arg");
    
    return calib_enable;
}


guva_s12sd_t guva_s12sd_init_desc(int pin, float vin, int sample_count)
{
    guva_s12sd_t dev; 
    dev.channel = pin;
    dev.vin = vin;
    dev.sample_count = sample_count;

    return dev;
}



esp_err_t guva_s12sd_read_analog(guva_s12sd_t *dev, float *out)
{
    bool calib_enable = init_adc(dev);

    uint32_t voltage_readings = 0;
    int curr_voltage_reading_raw = 0;
    uint32_t curr_voltage_reading = 0;
    
    for (int i = 0; i <dev->sample_count; ++i)
        adc1_get_raw(dev->channel); //take garbage readings, warm up ADC, sleep

    if(!calib_enable)
        return 0;

    for (int i = 0; i < dev->sample_count; ++i)
    {
        curr_voltage_reading_raw = adc1_get_raw(dev->channel);
        curr_voltage_reading =  esp_adc_cal_raw_to_voltage(curr_voltage_reading_raw, &dev->calib_chars);
        
        if ( VOLTAGE_CUTOFF_LOW <= curr_voltage_reading && curr_voltage_reading <= VOLTAGE_CUTOFF_HIGH)
            voltage_readings += curr_voltage_reading;
        else 
            i--;    
    }
    
    float avg_voltage = (VOLTAGE_MULT_FACTOR * voltage_readings) / (dev->sample_count);
    *out = avg_voltage; 
    return 1;

}

esp_err_t guva_s12sd_read_uv_index(guva_s12sd_t *dev, int *out)
{
    float voltage = 0;
    esp_err_t ret = guva_s12sd_read_analog(dev, &voltage);
    if(ret){
        voltage = voltage / 1024.0;
        int uv_index = voltage / 0.1;

        *out = uv_index;
        return 1;
    }

    return 0;
}