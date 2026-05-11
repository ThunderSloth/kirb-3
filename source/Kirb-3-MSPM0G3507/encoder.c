#include "encoder.h"

#include "kirb-3.h"
#include "ti/devices/msp/peripherals/hw_uart.h"

/** @file encoder.c
 *  @brief Shared half-duplex UART polling for the Parallax position
 *         controllers.
 */

#define UART_ENCODER_CLEAR_MASK                                                \
    (DL_UART_INTERRUPT_DMA_DONE_RX | DL_UART_INTERRUPT_OVERRUN_ERROR |         \
     DL_UART_INTERRUPT_FRAMING_ERROR | DL_UART_INTERRUPT_PARITY_ERROR |        \
     DL_UART_INTERRUPT_BREAK_ERROR | DL_UART_INTERRUPT_NOISE_ERROR |           \
     DL_UART_INTERRUPT_RX_TIMEOUT_ERROR)

#define ENCODER_WAIT_TX_TIMEOUT (20000u)
#define ENCODER_WAIT_RX_TIMEOUT (120000u)

typedef enum
{
    ENCODER_STATE_IDLE = 0,
    ENCODER_STATE_WAIT_TX_DONE,
    ENCODER_STATE_WAIT_RX_DONE,
} EncoderState;

volatile bool g_encoder_rx_done = false;
volatile bool g_encoder_valid[ENCODER_COUNT] = {false};
volatile uint8_t g_encoder_rx_packet[ENCODER_RX_PACKET_SIZE] = {0u};
volatile uint8_t g_encoder_last_packet[ENCODER_COUNT]
                                      [ENCODER_RX_PACKET_SIZE] = {{0u}};
volatile int16_t g_encoder_count[ENCODER_COUNT] = {0};
volatile uint32_t g_encoder_success_count[ENCODER_COUNT] = {0u};
volatile uint32_t g_encoder_error_count[ENCODER_COUNT] = {0u};

static const Encoder g_encoders[ENCODER_COUNT] = {
    [ENCODER1_IDX] = {.addr = 1u},
    [ENCODER2_IDX] = {.addr = 2u},
};

static volatile EncoderState g_encoder_state = ENCODER_STATE_IDLE;
static volatile EncoderIndex g_encoder_active_idx = ENCODER1_IDX;
static volatile EncoderIndex g_encoder_next_idx = ENCODER1_IDX;
static volatile bool g_encoder_poll_due = false;
static volatile uint32_t g_encoder_wait_countdown = 0u;

/** Enable the MCU-to-encoder UART buffer. */
static void encoder_tx_enable(void)
{
    DL_GPIO_clearPins(ENC_PORT, ENC_TX_N_OE_PIN);
}

/** Disable the MCU-to-encoder UART buffer. */
static void encoder_tx_disable(void)
{
    DL_GPIO_setPins(ENC_PORT, ENC_TX_N_OE_PIN);
}

/** Enable the encoder-to-MCU UART buffer. */
static void encoder_rx_enable(void)
{
    DL_GPIO_clearPins(ENC_PORT, ENC_RX_N_OE_PIN);
}

/** Disable the encoder-to-MCU UART buffer. */
static void encoder_rx_disable(void)
{
    DL_GPIO_setPins(ENC_PORT, ENC_RX_N_OE_PIN);
}

/** Release both sides of the external UART buffer. */
static void encoder_bus_idle(void)
{
    encoder_rx_disable();
    encoder_tx_disable();
}

/** Drain stale bytes before arming DMA for a fresh response. */
static void encoder_flush_rx(void)
{
    while (!DL_UART_Main_isRXFIFOEmpty(UART_ENCODER_INST))
    {
        (void)DL_UART_Main_receiveData(UART_ENCODER_INST);
    }
}

/** Prepare DMA to capture exactly one two-byte encoder response. */
static void encoder_arm_rx_dma(void)
{
    g_encoder_rx_done = false;
    g_encoder_rx_packet[ENCODER_RX_HIGH_BYTE_IDX] = 0u;
    g_encoder_rx_packet[ENCODER_RX_LOW_BYTE_IDX] = 0u;

    DL_DMA_disableChannel(DMA, ENCODER_RX_CHAN_ID);
    encoder_flush_rx();
    DL_UART_Main_clearInterruptStatus(UART_ENCODER_INST,
                                      UART_ENCODER_CLEAR_MASK);

    DL_DMA_setDestAddr(DMA, ENCODER_RX_CHAN_ID,
                       (uint32_t)&g_encoder_rx_packet[0]);
    DL_DMA_setTransferSize(DMA, ENCODER_RX_CHAN_ID, ENCODER_RX_PACKET_SIZE);
    DL_DMA_enableChannel(DMA, ENCODER_RX_CHAN_ID);
}

/** Abort the active transaction and return the bus/state machine to idle. */
static void encoder_recover_transaction(void)
{
    DL_DMA_disableChannel(DMA, ENCODER_RX_CHAN_ID);
    DL_UART_Main_clearInterruptStatus(UART_ENCODER_INST,
                                      UART_ENCODER_CLEAR_MASK |
                                          DL_UART_MAIN_INTERRUPT_EOT_DONE);
    encoder_flush_rx();
    g_encoder_rx_packet[ENCODER_RX_HIGH_BYTE_IDX] = 0u;
    g_encoder_rx_packet[ENCODER_RX_LOW_BYTE_IDX] = 0u;
    encoder_bus_idle();
    g_encoder_rx_done = false;
    g_encoder_state = ENCODER_STATE_IDLE;
    g_encoder_wait_countdown = 0u;
}

/** Begin a QPOS command for one encoder address. */
static void encoder_start_query(EncoderIndex idx)
{
    uint8_t msg;

    if (idx >= ENCODER_COUNT)
    {
        return;
    }

    g_encoder_active_idx = idx;
    msg = (uint8_t)(CMD_QPOS | g_encoders[idx].addr);

    encoder_bus_idle();
    DL_UART_Main_clearInterruptStatus(UART_ENCODER_INST,
                                      UART_ENCODER_CLEAR_MASK |
                                          DL_UART_MAIN_INTERRUPT_EOT_DONE);

    encoder_tx_enable();

    while (DL_UART_Main_isTXFIFOFull(UART_ENCODER_INST))
    {
    }

    DL_UART_Main_transmitData(UART_ENCODER_INST, msg);
    g_encoder_state = ENCODER_STATE_WAIT_TX_DONE;
    g_encoder_wait_countdown = ENCODER_WAIT_TX_TIMEOUT;
}

/** Accept the DMA packet, apply software sign convention, and publish it. */
static void encoder_finish_success(void)
{
    const uint16_t raw =
        ((uint16_t)g_encoder_rx_packet[ENCODER_RX_HIGH_BYTE_IDX] << 8) |
        g_encoder_rx_packet[ENCODER_RX_LOW_BYTE_IDX];
    int16_t count = (int16_t)raw;

    if (g_encoder_active_idx == ENCODER2_IDX)
    {
        count = (int16_t)-count;
    }

    g_encoder_count[g_encoder_active_idx] = count;
    g_encoder_last_packet[g_encoder_active_idx][ENCODER_RX_HIGH_BYTE_IDX] =
        g_encoder_rx_packet[ENCODER_RX_HIGH_BYTE_IDX];
    g_encoder_last_packet[g_encoder_active_idx][ENCODER_RX_LOW_BYTE_IDX] =
        g_encoder_rx_packet[ENCODER_RX_LOW_BYTE_IDX];
    g_encoder_success_count[g_encoder_active_idx]++;
    g_encoder_valid[g_encoder_active_idx] = true;
    g_encoder_state = ENCODER_STATE_IDLE;
    g_encoder_wait_countdown = 0u;
    encoder_bus_idle();
}

/** Count a timeout/error and recover the shared bus. */
static void encoder_handle_timeout(void)
{
    g_encoder_error_count[g_encoder_active_idx]++;
    g_encoder_valid[g_encoder_active_idx] = false;
    encoder_recover_transaction();
}

/** Initialize encoder state, DMA source, bus buffers, and poll scheduler. */
void encoder_init(void)
{
    for (uint32_t i = 0; i < ENCODER_COUNT; i++)
    {
        g_encoder_count[i] = 0;
        g_encoder_valid[i] = false;
        g_encoder_last_packet[i][ENCODER_RX_HIGH_BYTE_IDX] = 0u;
        g_encoder_last_packet[i][ENCODER_RX_LOW_BYTE_IDX] = 0u;
        g_encoder_success_count[i] = 0u;
        g_encoder_error_count[i] = 0u;
    }

    g_encoder_rx_done = false;
    g_encoder_rx_packet[ENCODER_RX_HIGH_BYTE_IDX] = 0u;
    g_encoder_rx_packet[ENCODER_RX_LOW_BYTE_IDX] = 0u;
    g_encoder_state = ENCODER_STATE_IDLE;
    g_encoder_active_idx = ENCODER1_IDX;
    g_encoder_next_idx = ENCODER1_IDX;
    g_encoder_poll_due = true;
    g_encoder_wait_countdown = 0u;

    DL_DMA_setSrcAddr(DMA, ENCODER_RX_CHAN_ID,
                      (uint32_t)(&UART_ENCODER_INST->RXDATA));
    encoder_bus_idle();

    DL_TimerG_clearInterruptStatus(ENC_SCHED_TIM_INST,
                                   DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableInterrupt(ENC_SCHED_TIM_INST,
                              DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_startCounter(ENC_SCHED_TIM_INST);
}

/** Record that the encoder scheduler has made one poll slot available. */
void encoder_schedule(void)
{
    g_encoder_poll_due = true;
}

/** Advance one step of the encoder query/response state machine. */
void encoder_service(void)
{
    switch (g_encoder_state)
    {
    case ENCODER_STATE_IDLE:
        if (!g_encoder_poll_due)
        {
            return;
        }

        g_encoder_poll_due = false;
        encoder_start_query(g_encoder_next_idx);
        g_encoder_next_idx =
            (g_encoder_next_idx == ENCODER1_IDX) ? ENCODER2_IDX : ENCODER1_IDX;
        return;

    case ENCODER_STATE_WAIT_TX_DONE:
        if (DL_UART_Main_isTXFIFOEmpty(UART_ENCODER_INST) &&
            !DL_UART_Main_isBusy(UART_ENCODER_INST))
        {
            DL_UART_Main_clearInterruptStatus(UART_ENCODER_INST,
                                              DL_UART_MAIN_INTERRUPT_EOT_DONE);
            encoder_tx_disable();
            encoder_arm_rx_dma();
            encoder_rx_enable();
            g_encoder_state = ENCODER_STATE_WAIT_RX_DONE;
            g_encoder_wait_countdown = ENCODER_WAIT_RX_TIMEOUT;
            return;
        }
        break;

    case ENCODER_STATE_WAIT_RX_DONE:
        if (g_encoder_rx_done)
        {
            g_encoder_rx_done = false;
            encoder_finish_success();
            return;
        }
        break;

    default:
        encoder_recover_transaction();
        return;
    }

    if (g_encoder_wait_countdown > 0u)
    {
        g_encoder_wait_countdown--;
    }

    if (g_encoder_wait_countdown == 0u)
    {
        encoder_handle_timeout();
    }
}

/** Handle encoder UART interrupts and latch completed DMA receive packets. */
void uart_encoder_irq(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_ENCODER_INST))
    {
    case DL_UART_MAIN_IIDX_DMA_DONE_RX:
        g_encoder_rx_done = true;
        break;
    default:
        break;
    }
}
