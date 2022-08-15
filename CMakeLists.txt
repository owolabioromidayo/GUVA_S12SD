idf_component_register(
    SRCS guva_s12sd.c
    INCLUDE_DIRS .
    REQUIRES log esp_idf_lib_helpers driver esp_adc_cal
)