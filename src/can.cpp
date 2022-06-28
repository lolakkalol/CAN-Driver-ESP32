#include "can.h"

esp_err_t CAN_start() {
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_22, GPIO_NUM_23, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Enable alerts when can controller gets data
    g_config.alerts_enabled = TWAI_ALERT_RX_DATA;

    //Install TWAI driver
    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    if (err != ESP_OK)
        return err;

    //Start TWAI driver
    err = twai_start();
    if (err != ESP_OK)
        return err;
}