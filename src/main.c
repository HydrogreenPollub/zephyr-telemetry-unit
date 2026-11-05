#include "telemetry_can.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(telemetry, LOG_LEVEL_INF);

int main(void)
{
    int ret = telemetry_can_init();
    if (ret != 0) {
        LOG_ERR("CAN telemetry initialization failed: %d", ret);
        return ret;
    }

    LOG_INF("Telemetry receiver started; waiting for CAN frames");

    struct can_frame frame;
    while (true) {
        ret = telemetry_can_get_frame(&frame, K_SECONDS(1));
        if (ret == 0) {
            LOG_INF("CAN RX id=0x%08x %s dlc=%u data=%02x %02x %02x %02x %02x %02x %02x %02x",
                    frame.id,
                    (frame.flags & CAN_FRAME_IDE) ? "extended" : "standard",
                    frame.dlc,
                    frame.data[0], frame.data[1], frame.data[2], frame.data[3],
                    frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
        } else {
            LOG_INF("CAN telemetry: no frame received in the last second (dropped=%u)",
                    telemetry_can_dropped_frames());
        }
    }
}
