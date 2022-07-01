#include "can.h"

// Message reception
twai_message_t rx_message;

esp_err_t config_and_start_twai() {
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

void receive_can_message() {
    // Receives messages and handles them
    if (twai_receive(&rx_message, 0) == ESP_OK) {

    // An example of how the CAN messages can be handled in a super loop
    switch (rx_message.identifier)
    {
    // Examle case where messages with id 0x012's data is printed
    case 0x012:
        //Serial.printf("Message with id %#05x\n", rx_message.identifier);
        //Serial.printf("Data received: ");

        for (int i = 0; i < rx_message.data_length_code; i++) {
        //Serial.printf("%#04x ");
        }

        //Serial.printf("\n\r");
        
        break;

    default:
        break;
    }
    }
}