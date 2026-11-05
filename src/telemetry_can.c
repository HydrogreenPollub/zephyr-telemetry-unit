#include "telemetry_can.h"

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <hydrogreen/can/can_ids.h>
#include <hydrogreen/peripherals/can.h>

LOG_MODULE_REGISTER(telemetry_can, LOG_LEVEL_INF);

#define TELEMETRY_RX_QUEUE_DEPTH 32

K_MSGQ_DEFINE(telemetry_rx_queue, sizeof(struct can_frame), TELEMETRY_RX_QUEUE_DEPTH, 4);
static atomic_t dropped_frames;
static const struct device *can_dev;

static void telemetry_can_rx_callback(const struct device *dev,
                                      struct can_frame *frame,
                                      void *user_data)
{
    ARG_UNUSED(dev);
    struct k_msgq *queue = user_data;
    if (k_msgq_put(queue, frame, K_NO_WAIT) != 0) {
        atomic_inc(&dropped_frames);
    }
}

int telemetry_can_init(void)
{
    can_dev = DEVICE_DT_GET(DT_ALIAS(can));
    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device %s is not ready", can_dev->name);
        return -ENODEV;
    }

    int ret = can_init(can_dev, HYDROGREEN_CAN_BAUD_RATE);
    if (ret != 0) {
        LOG_ERR("Failed to initialize CAN at %u bit/s: %d",
                HYDROGREEN_CAN_BAUD_RATE, ret);
        return ret;
    }

    const struct can_filter filters[] = {
        {.id = 0, .mask = 0, .flags = CAN_FILTER_IDE}, /* all extended IDs */
        {.id = 0, .mask = 0, .flags = 0},              /* all standard IDs */
    };

    for (size_t i = 0; i < ARRAY_SIZE(filters); ++i) {
        ret = can_add_rx_filter(can_dev, telemetry_can_rx_callback,
                                &telemetry_rx_queue, &filters[i]);
        if (ret < 0) {
            LOG_ERR("Failed to install CAN RX filter %u: %d", (unsigned)i, ret);
            return ret;
        }
    }

    LOG_INF("CAN telemetry receiver ready at %u bit/s", HYDROGREEN_CAN_BAUD_RATE);
    return 0;
}

int telemetry_can_get_frame(struct can_frame *frame, k_timeout_t timeout)
{
    if (frame == NULL) {
        return -EINVAL;
    }
    return k_msgq_get(&telemetry_rx_queue, frame, timeout);
}

uint32_t telemetry_can_dropped_frames(void)
{
    return (uint32_t)atomic_get(&dropped_frames);
}
