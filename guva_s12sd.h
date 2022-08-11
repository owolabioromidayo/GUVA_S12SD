#ifndef __GUVA_SI2SD__
#define __GUVA_SI2SD__

#include <stdbool.h>
#include <esp_err.h>
#include <stdlib.h>
#include "drivers/adc.h"
#include "esp_adc_cal.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    adc1_channel_t channel; //adc channel
    float vin;
    int sample_count;
    bool calib_enable;
    esp_adc_cal_characteristics_t calib_chars; //calibration characteristics
} guva_s12sd_t;

/**
 * @brief Initialize device descriptor
 *
 * @param pin port number
 * @param vin Input Voltage
 * @param sample_count Number of Samples to average per reading
 * @return guva_s12sd_t 
 */
guva_s12sd_t guva_s12sd_init_desc(int pin, float vin, int sample_count);

/**
 * @brief Get raw voltage from device
 * *
 * @param dev Device descriptor
 * @param out Raw Voltage 
 * @return `ESP_OK` on success
 */
esp_err_t guva_s12sd_read_analog(guva_s12sd_t *dev, float *out);

/**
 * @brief Get the current UV index reading
 * *
 * @param dev Device descriptor
 * @param out UV Index  
 * @return `ESP_OK` on success
 */
esp_err_t guva_s12sd_read_uv_index(guva_s12sd_t *dev, int *out);


#ifdef __cplusplus
}
#endif


#endif /* __GUVA_SI2SD__ */
