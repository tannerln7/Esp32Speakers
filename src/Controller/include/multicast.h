//
// Created by LANETAN on 12/19/2023.
//

#ifndef SPEAKERS_MULTICAST_H
#define SPEAKERS_MULTICAST_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>


class multicast : public
BluetoothA2DPSink {
protected:
    void audio_data_callback(const uint8_t *data, uint32_t len) {
        ESP_LODGE(BT_AV_TAG, "%s", __PRETTY_FUNCTION__);
        xQueueSend(audioQueue, , &data, portMAX_DELAY)
    }
};

void audioProcessingTask(void xpvParameter) {
    const uint8_t *data;
    while (1) {
        if (xQeueReceive(audioQueue, &data, portMAX_DELAY)) {
            udp.beginPacketMulticast(...);
            udp.write(data, len);
            udp.endPacket();
        }
    }
}
}


#endif //SPEAKERS_MULTICAST_H
