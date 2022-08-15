#include "driver/adc.h"
#include "guva_s12sd.h"
#include <esp_err.h>
#include <stdio.h>>

#define  
#define ADC_PIN ADC_CHANNEL_0 

void get_uv_values(void)
{
  guva_s12sd_t sensor = guva_s12sd_init_desc(ADC_CHANNEL_0, 3.3, 50);

  int uv;
  if (guva_s12sd_read_uv_index(&sensor, &uv))
  {
    printf("UV Index: %d \n", uv);
    return;
  }
  printf("Failed to read value\n");

}
