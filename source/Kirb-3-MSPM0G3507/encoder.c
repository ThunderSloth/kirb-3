#include "encoder.h"
#include "ti/devices/msp/peripherals/hw_uart.h"
#include "ti_msp_dl_config.h"
#include "uart_debug.h"

#define UART_ENCODER_CLEAR_MASK                                                \
    (DL_UART_INTERRUPT_DMA_DONE_RX | DL_UART_INTERRUPT_OVERRUN_ERROR |         \
     DL_UART_INTERRUPT_FRAMING_ERROR | DL_UART_INTERRUPT_PARITY_ERROR |        \
     DL_UART_INTERRUPT_BREAK_ERROR | DL_UART_INTERRUPT_NOISE_ERROR |           \
     DL_UART_INTERRUPT_RX_TIMEOUT_ERROR)

volatile bool g_encoder_rx_done = false;
volatile uint8_t g_encoder_rx_packet[ENCODER_RX_PACKET_SIZE];

static const Encoder encoders[ENCODER_COUNT] = {
    [ENCODER1_IDX] =
        {
            .addr = 1,
        },
    [ENCODER2_IDX] =
        {
            .addr = 2,
        },
};

void encoder_init(void)
{
    DL_DMA_setSrcAddr(DMA, ENCODER_RX_CHAN_ID,
                      (uint32_t)(&UART_ENCODER_INST->RXDATA));
}

void arm_encoder_dma(void)
{
    g_encoder_rx_done = false;

    DL_DMA_disableChannel(DMA, ENCODER_RX_CHAN_ID);

    // drain FIFO
    while (!DL_UART_Main_isRXFIFOEmpty(UART_ENCODER_INST))
    {
        (void)DL_UART_Main_receiveData(UART_ENCODER_INST);
    }

    DL_UART_Main_clearInterruptStatus(UART_ENCODER_INST,
                                      UART_ENCODER_CLEAR_MASK);

    DL_DMA_setDestAddr(DMA, ENCODER_RX_CHAN_ID,
                       (uint32_t)&g_encoder_rx_packet[0]);
    DL_DMA_setTransferSize(DMA, ENCODER_RX_CHAN_ID, ENCODER_RX_PACKET_SIZE);

    DL_DMA_enableChannel(DMA, ENCODER_RX_CHAN_ID);
}

void uart_encoder_query(uint8_t cmd, EncoderIndex idx)
{
    ;
    if (idx >= ENCODER_COUNT)
        return;

    uint8_t msg = (uint8_t)(cmd | encoders[idx].addr);

    /* Waits until FIFO has space to fill the data */
    while (DL_UART_Main_isTXFIFOFull(UART_ENCODER_INST))
    {
    }
    DL_UART_Main_transmitData(UART_ENCODER_INST, msg);
}

void uart_encoder_irq(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_ENCODER_INST))
    {
    case DL_UART_MAIN_IIDX_DMA_DONE_RX:
        g_encoder_rx_done = true;

        // temp testing code below
        uint16_t data = (g_encoder_rx_packet[ENCODER_RX_HIGH_BYTE_IDX] << 4) +
                        g_encoder_rx_packet[ENCODER_RX_LOW_BYTE_IDX];
        UART_debug_printf("%d", data);

        break;
    default:
        break;
    }
}

/*
volatile uint32_t g_encoder_poll_budget = 0;

void TIMERx_IRQHandler(void)
{
    // clear timer IRQ flag...
    g_encoder_poll_budget++;   // allows one query to be started
}

In main():

while (1) {
    encoder_service();
    // other tasks...
}ncoder_service() behavior

If a transaction is in progress:

check g_encoder_rx_done or timeout and finish it

If idle and g_encoder_poll_budget > 0:

decrement budget

start the next query (round-robin)

Timer at 200 Hz (every 5 ms)

Each tick starts one query alternating encoders:

tick 0: E1 QPOS

tick 1: E2 QPOS
So each encoder gets 100 Hz updates.

Or go 100 Hz timer → each encoder 50 Hz. Both are fine.
The missing piece: timeout + bus recovery

Even if everything works now, you want a timeout so a missing reply doesn’t
stall your state machine forever.

Start a timestamp when you open RX

If now - t_start > timeout_ms:

close OE_RX

mark error

return to IDLE and move on

At 19.2 kbps, a 2-byte reply takes ~1 ms on-wire (plus your “TX delay”), so a
5–10 ms timeout is usually plenty.

Minimal state machine outline

States:

IDLE

WAIT_RX (DMA armed, RX open)

When starting a query:

TX phase

(your OE_TX on)

send command byte

wait for EOT/TX complete (poll flag)

OE_TX off

RX phase

arm_encoder_dma()

OE_RX on

set deadline = now + timeout

In WAIT_RX:

if g_encoder_rx_done:

OE_RX off

parse bytes → int16

store pos + timestamp

state = IDLE

else if timeout:

OE_RX off

state = IDLE (and maybe increment an error counter)



QPOS only

typedef struct {
    int16_t  pos;        // latest raw count
    int16_t  pos_prev;
    uint32_t t_ms;       // timestamp of latest
    uint32_t t_prev_ms;
    float    vel_cps;    // counts per second (computed)
    // optionally: distance, vel filtered, etc.
} EncState;





*/