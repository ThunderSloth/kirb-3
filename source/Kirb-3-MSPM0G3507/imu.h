#ifndef IMU_H
#define IMU_H

/** @file imu.h
 *  @brief LSM6-style IMU detection, configuration, and sample cache.
 */

#include <stdbool.h>
#include <stdint.h>

#define IMU_WHO_AM_I_REG (0x0Fu)
#define IMU_WHO_AM_I_EXPECTED (0x6Cu)

/** Last WHO_AM_I value read from the IMU. */
extern volatile uint8_t g_imu_who_am_i;

/** I2C address that successfully responded during detection. */
extern volatile uint8_t g_imu_i2c_addr;

/** True after a usable IMU has been detected. */
extern volatile bool g_imu_present;

/** Set by the IMU data-ready interrupt and consumed by imu_service(). */
extern volatile bool g_imu_sample_ready;

/** Latest raw accelerometer sample, X/Y/Z. */
extern volatile int16_t g_imu_accel_raw[3];

/** Latest raw gyroscope sample, X/Y/Z. */
extern volatile int16_t g_imu_gyro_raw[3];

/** Latest scaled accelerometer sample, milligravity X/Y/Z. */
extern volatile int16_t g_imu_accel_mg[3];

/** Latest scaled gyroscope sample, millidegrees/second X/Y/Z. */
extern volatile int16_t g_imu_gyro_mdps[3];

/** Reset cached state and attempt initial IMU configuration. */
void imu_init(void);

/** Probe both supported I2C addresses and cache WHO_AM_I. */
bool imu_read_who_am_i(void);

/** Configure block-data update, auto-increment, ODR, ranges, and DRDY. */
bool imu_configure_basic(void);

/** Read one accel/gyro sample and update raw and scaled caches. */
bool imu_read_sample(void);

/** Consume data-ready events and refresh the cached IMU sample. */
void imu_service(void);

/** Handle the IMU data-ready GPIO interrupt. */
void imu_gpio_irq(void);

#endif // IMU_H
