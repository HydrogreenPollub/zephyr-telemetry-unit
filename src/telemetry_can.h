#ifndef TELEMETRY_CAN_H
#define TELEMETRY_CAN_H

#include <stdint.h>
#include <zephyr/drivers/can.h>
#include <zephyr/kernel.h>

int telemetry_can_init(void);
int telemetry_can_get_frame(struct can_frame *frame, k_timeout_t timeout);
uint32_t telemetry_can_dropped_frames(void);

#endif
