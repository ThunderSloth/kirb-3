#include "imu.h"

#include "kirb-3.h"

/** @file imu.c
 *  @brief Blocking I2C register access and sample scaling for the IMU.
 */

#define IMU_I2C_TIMEOUT_COUNT (100000u)
#define IMU_I2C_ADDR_PRIMARY (0x6Au)
#define IMU_I2C_ADDR_SECONDARY (0x6Bu)
#define IMU_CTRL1_XL_REG (0x10u)
#define IMU_CTRL2_G_REG (0x11u)
#define IMU_CTRL3_C_REG (0x12u)
#define IMU_INT1_CTRL_REG (0x0Du)
#define IMU_OUTX_L_G_REG (0x22u)
#define IMU_SAMPLE_BYTES (12u)
#define IMU_CTRL1_XL_104HZ_2G (0x40u)
#define IMU_CTRL2_G_104HZ_250DPS (0x40u)
#define IMU_CTRL3_C_BDU_IF_INC (0x44u)
#define IMU_INT1_CTRL_DRDY_XL (0x01u)
#define IMU_ACCEL_MG_NUM (61)
#define IMU_ACCEL_MG_DEN (1000)
#define IMU_GYRO_MDPS_NUM (875)
#define IMU_GYRO_MDPS_DEN (100)

volatile uint8_t g_imu_who_am_i = 0u;
volatile uint8_t g_imu_i2c_addr = 0u;
volatile bool g_imu_present = false;
volatile bool g_imu_sample_ready = false;
volatile int16_t g_imu_accel_raw[3] = {0};
volatile int16_t g_imu_gyro_raw[3] = {0};
volatile int16_t g_imu_accel_mg[3] = {0};
volatile int16_t g_imu_gyro_mdps[3] = {0};

static bool g_imu_configured = false;

/** Reset I2C controller FIFOs and transfer state after each transaction. */
static void imu_reset_controller_state(void)
{
    DL_I2C_resetControllerTransfer(I2C_IMU_INST);
    DL_I2C_flushControllerTXFIFO(I2C_IMU_INST);
    DL_I2C_flushControllerRXFIFO(I2C_IMU_INST);
}

/** Wait for one I2C controller status bit to become set or clear. */
static bool imu_wait_for_status(uint32_t mask, bool set)
{
    uint32_t timeout = IMU_I2C_TIMEOUT_COUNT;

    while (timeout > 0u)
    {
        const bool status_set =
            (DL_I2C_getControllerStatus(I2C_IMU_INST) & mask) != 0u;

        if (status_set == set)
        {
            return true;
        }

        timeout--;
    }

    return false;
}

/** Wait for the I2C controller to reach idle state. */
static bool imu_wait_for_idle(void)
{
    return imu_wait_for_status(DL_I2C_CONTROLLER_STATUS_IDLE, true);
}

/** Wait for the I2C bus to be released. */
static bool imu_wait_for_bus_not_busy(void)
{
    return imu_wait_for_status(DL_I2C_CONTROLLER_STATUS_BUSY_BUS, false);
}

/** Wait for one received byte or abort on controller error/timeout. */
static bool imu_wait_for_rx_byte(void)
{
    uint32_t timeout = IMU_I2C_TIMEOUT_COUNT;

    while (timeout > 0u)
    {
        if (!DL_I2C_isControllerRXFIFOEmpty(I2C_IMU_INST))
        {
            return true;
        }

        if ((DL_I2C_getControllerStatus(I2C_IMU_INST) &
             DL_I2C_CONTROLLER_STATUS_ERROR) != 0u)
        {
            return false;
        }

        timeout--;
    }

    return false;
}

/** Read one register from the selected IMU I2C address. */
static bool imu_read_register(uint8_t target_addr, uint8_t reg_addr,
                              uint8_t *value)
{
    if (value == 0)
    {
        return false;
    }

    imu_reset_controller_state();

    if (!imu_wait_for_idle())
    {
        return false;
    }

    DL_I2C_fillControllerTXFIFO(I2C_IMU_INST, &reg_addr, 1u);
    DL_I2C_startControllerTransfer(I2C_IMU_INST, target_addr,
                                   DL_I2C_CONTROLLER_DIRECTION_TX, 1u);

    if (!imu_wait_for_bus_not_busy() || !imu_wait_for_idle())
    {
        imu_reset_controller_state();
        return false;
    }

    if ((DL_I2C_getControllerStatus(I2C_IMU_INST) &
         DL_I2C_CONTROLLER_STATUS_ERROR) != 0u)
    {
        imu_reset_controller_state();
        return false;
    }

    DL_I2C_flushControllerTXFIFO(I2C_IMU_INST);

    DL_I2C_startControllerTransfer(I2C_IMU_INST, target_addr,
                                   DL_I2C_CONTROLLER_DIRECTION_RX, 1u);

    if (!imu_wait_for_rx_byte())
    {
        imu_reset_controller_state();
        return false;
    }

    *value = DL_I2C_receiveControllerData(I2C_IMU_INST);

    if (!imu_wait_for_bus_not_busy() || !imu_wait_for_idle())
    {
        imu_reset_controller_state();
        return false;
    }

    if ((DL_I2C_getControllerStatus(I2C_IMU_INST) &
         DL_I2C_CONTROLLER_STATUS_ERROR) != 0u)
    {
        imu_reset_controller_state();
        return false;
    }

    imu_reset_controller_state();
    return true;
}

/** Write one register on the selected IMU I2C address. */
static bool imu_write_register(uint8_t target_addr, uint8_t reg_addr,
                               uint8_t value)
{
    uint8_t tx_data[2];

    tx_data[0] = reg_addr;
    tx_data[1] = value;

    imu_reset_controller_state();

    if (!imu_wait_for_idle())
    {
        return false;
    }

    DL_I2C_fillControllerTXFIFO(I2C_IMU_INST, tx_data, 2u);
    DL_I2C_startControllerTransfer(I2C_IMU_INST, target_addr,
                                   DL_I2C_CONTROLLER_DIRECTION_TX, 2u);

    if (!imu_wait_for_bus_not_busy() || !imu_wait_for_idle())
    {
        imu_reset_controller_state();
        return false;
    }

    if ((DL_I2C_getControllerStatus(I2C_IMU_INST) &
         DL_I2C_CONTROLLER_STATUS_ERROR) != 0u)
    {
        imu_reset_controller_state();
        return false;
    }

    imu_reset_controller_state();
    return true;
}

/** Read a contiguous register block using the IMU auto-increment mode. */
static bool imu_read_bytes(uint8_t target_addr, uint8_t start_reg,
                           uint8_t *data, uint32_t length)
{
    if ((data == 0) || (length == 0u))
    {
        return false;
    }

    imu_reset_controller_state();

    if (!imu_wait_for_idle())
    {
        return false;
    }

    DL_I2C_fillControllerTXFIFO(I2C_IMU_INST, &start_reg, 1u);
    DL_I2C_startControllerTransfer(I2C_IMU_INST, target_addr,
                                   DL_I2C_CONTROLLER_DIRECTION_TX, 1u);

    if (!imu_wait_for_bus_not_busy() || !imu_wait_for_idle())
    {
        imu_reset_controller_state();
        return false;
    }

    if ((DL_I2C_getControllerStatus(I2C_IMU_INST) &
         DL_I2C_CONTROLLER_STATUS_ERROR) != 0u)
    {
        imu_reset_controller_state();
        return false;
    }

    DL_I2C_flushControllerTXFIFO(I2C_IMU_INST);
    DL_I2C_startControllerTransfer(I2C_IMU_INST, target_addr,
                                   DL_I2C_CONTROLLER_DIRECTION_RX,
                                   (uint16_t)length);

    for (uint32_t i = 0u; i < length; i++)
    {
        if (!imu_wait_for_rx_byte())
        {
            imu_reset_controller_state();
            return false;
        }

        data[i] = DL_I2C_receiveControllerData(I2C_IMU_INST);
    }

    if (!imu_wait_for_bus_not_busy() || !imu_wait_for_idle())
    {
        imu_reset_controller_state();
        return false;
    }

    if ((DL_I2C_getControllerStatus(I2C_IMU_INST) &
         DL_I2C_CONTROLLER_STATUS_ERROR) != 0u)
    {
        imu_reset_controller_state();
        return false;
    }

    imu_reset_controller_state();
    return true;
}

/** Unpack one little-endian signed 16-bit sample. */
static int16_t imu_unpack_le_i16(const uint8_t *buf)
{
    return (int16_t)(((uint16_t)buf[1] << 8) | (uint16_t)buf[0]);
}

/** Scale a signed sample with saturation into int16_t output units. */
static int16_t imu_scale_signed(int16_t raw, int32_t num, int32_t den)
{
    const int32_t scaled = ((int32_t)raw * num) / den;

    if (scaled > INT16_MAX)
    {
        return INT16_MAX;
    }

    if (scaled < INT16_MIN)
    {
        return INT16_MIN;
    }

    return (int16_t)scaled;
}

/** Reset IMU caches and attempt initial device configuration. */
void imu_init(void)
{
    bool configured;

    g_imu_who_am_i = 0u;
    g_imu_i2c_addr = 0u;
    g_imu_present = false;
    g_imu_sample_ready = false;
    g_imu_accel_raw[0] = 0;
    g_imu_accel_raw[1] = 0;
    g_imu_accel_raw[2] = 0;
    g_imu_gyro_raw[0] = 0;
    g_imu_gyro_raw[1] = 0;
    g_imu_gyro_raw[2] = 0;
    g_imu_accel_mg[0] = 0;
    g_imu_accel_mg[1] = 0;
    g_imu_accel_mg[2] = 0;
    g_imu_gyro_mdps[0] = 0;
    g_imu_gyro_mdps[1] = 0;
    g_imu_gyro_mdps[2] = 0;
    g_imu_configured = false;
    imu_reset_controller_state();
    configured = imu_configure_basic();

    if (configured)
    {
        g_imu_sample_ready = true;
    }
}

/** Probe supported IMU addresses and cache the responding WHO_AM_I value. */
bool imu_read_who_am_i(void)
{
    uint8_t who_am_i = 0u;

    if (imu_read_register(IMU_I2C_ADDR_PRIMARY, IMU_WHO_AM_I_REG, &who_am_i))
    {
        g_imu_i2c_addr = IMU_I2C_ADDR_PRIMARY;
        g_imu_who_am_i = who_am_i;
        g_imu_present = true;
        return true;
    }

    if (imu_read_register(IMU_I2C_ADDR_SECONDARY, IMU_WHO_AM_I_REG, &who_am_i))
    {
        g_imu_i2c_addr = IMU_I2C_ADDR_SECONDARY;
        g_imu_who_am_i = who_am_i;
        g_imu_present = true;
        return true;
    }

    g_imu_i2c_addr = 0u;
    g_imu_who_am_i = 0u;
    g_imu_present = false;
    return false;
}

/** Configure IMU output data rates, ranges, auto-increment, and DRDY. */
bool imu_configure_basic(void)
{
    if (!imu_read_who_am_i())
    {
        g_imu_configured = false;
        return false;
    }

    if (!imu_write_register(g_imu_i2c_addr, IMU_CTRL3_C_REG,
                            IMU_CTRL3_C_BDU_IF_INC))
    {
        g_imu_configured = false;
        return false;
    }

    if (!imu_write_register(g_imu_i2c_addr, IMU_CTRL1_XL_REG,
                            IMU_CTRL1_XL_104HZ_2G))
    {
        g_imu_configured = false;
        return false;
    }

    if (!imu_write_register(g_imu_i2c_addr, IMU_CTRL2_G_REG,
                            IMU_CTRL2_G_104HZ_250DPS))
    {
        g_imu_configured = false;
        return false;
    }

    if (!imu_write_register(g_imu_i2c_addr, IMU_INT1_CTRL_REG,
                            IMU_INT1_CTRL_DRDY_XL))
    {
        g_imu_configured = false;
        return false;
    }

    g_imu_configured = true;
    return true;
}

/** Read one accel/gyro register block and update raw and scaled samples. */
bool imu_read_sample(void)
{
    uint8_t sample_buf[IMU_SAMPLE_BYTES];

    if (!g_imu_configured)
    {
        if (!imu_configure_basic())
        {
            return false;
        }
    }

    if (!imu_read_bytes(g_imu_i2c_addr, IMU_OUTX_L_G_REG, sample_buf,
                        IMU_SAMPLE_BYTES))
    {
        return false;
    }

    g_imu_gyro_raw[0] = imu_unpack_le_i16(&sample_buf[0]);
    g_imu_gyro_raw[1] = imu_unpack_le_i16(&sample_buf[2]);
    g_imu_gyro_raw[2] = imu_unpack_le_i16(&sample_buf[4]);

    g_imu_accel_raw[0] = imu_unpack_le_i16(&sample_buf[6]);
    g_imu_accel_raw[1] = imu_unpack_le_i16(&sample_buf[8]);
    g_imu_accel_raw[2] = imu_unpack_le_i16(&sample_buf[10]);

    g_imu_gyro_mdps[0] = imu_scale_signed(g_imu_gyro_raw[0], IMU_GYRO_MDPS_NUM,
                                          IMU_GYRO_MDPS_DEN);
    g_imu_gyro_mdps[1] = imu_scale_signed(g_imu_gyro_raw[1], IMU_GYRO_MDPS_NUM,
                                          IMU_GYRO_MDPS_DEN);
    g_imu_gyro_mdps[2] = imu_scale_signed(g_imu_gyro_raw[2], IMU_GYRO_MDPS_NUM,
                                          IMU_GYRO_MDPS_DEN);

    g_imu_accel_mg[0] = imu_scale_signed(g_imu_accel_raw[0], IMU_ACCEL_MG_NUM,
                                         IMU_ACCEL_MG_DEN);
    g_imu_accel_mg[1] = imu_scale_signed(g_imu_accel_raw[1], IMU_ACCEL_MG_NUM,
                                         IMU_ACCEL_MG_DEN);
    g_imu_accel_mg[2] = imu_scale_signed(g_imu_accel_raw[2], IMU_ACCEL_MG_NUM,
                                         IMU_ACCEL_MG_DEN);

    return true;
}

/** Consume one pending IMU data-ready event from the foreground loop. */
void imu_service(void)
{
    if (!g_imu_sample_ready)
    {
        return;
    }

    g_imu_sample_ready = false;

    if (!imu_read_sample())
    {
        g_imu_configured = false;
    }
}

/** Latch an IMU data-ready GPIO interrupt for foreground servicing. */
void imu_gpio_irq(void)
{
    const uint32_t imu_status =
        DL_GPIO_getEnabledInterruptStatus(IMU_PORT, IMU_INT1_PIN);

    if ((imu_status & IMU_INT1_PIN) == 0u)
    {
        return;
    }

    g_imu_sample_ready = true;
    DL_GPIO_clearInterruptStatus(IMU_PORT, IMU_INT1_PIN);
}
