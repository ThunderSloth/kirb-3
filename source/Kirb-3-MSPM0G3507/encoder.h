#ifndef ENCODER_H_
#define ENCODER_H_

/** @file encoder.h
 *  @brief Parallax position-controller polling interface.
 */

#include <stdbool.h>
#include <stdint.h>

/** Parallax Position Controller command bases. */
#define CMD_QPOS (0x08u)
#define CMD_QSPD (0x10u)
#define CMD_CLRP (0x28u)
#define CMD_SREV (0x30u)

/** Software indices for the two encoder addresses on the shared UART bus. */
typedef enum
{
    ENCODER1_IDX = 0,
    ENCODER2_IDX,

    ENCODER_COUNT,
} EncoderIndex;

/** Static encoder address configuration. */
typedef struct
{
    uint8_t addr;
} Encoder;

/** Byte offsets in the two-byte signed count response. */
typedef enum
{
    ENCODER_RX_HIGH_BYTE_IDX = 0,
    ENCODER_RX_LOW_BYTE_IDX = 1,
    ENCODER_RX_PACKET_SIZE = 2,
} EncoderRxPacketIndex;

/** Initialize encoder polling state, DMA receive setup, and schedule timer. */
void encoder_init(void);

/** Mark one encoder poll as due from the scheduler ISR. */
void encoder_schedule(void);

/** Progress the non-blocking encoder query state machine. */
void encoder_service(void);

/** Handle UART DMA-complete interrupts for encoder responses. */
void uart_encoder_irq(void);

/** True while the current DMA response buffer has completed. */
extern volatile bool g_encoder_rx_done;

/** Latest per-encoder packet validity. */
extern volatile bool g_encoder_valid[ENCODER_COUNT];

/** Current in-flight raw response packet. */
extern volatile uint8_t g_encoder_rx_packet[ENCODER_RX_PACKET_SIZE];

/** Last successfully accepted raw response packet for each encoder. */
extern volatile uint8_t g_encoder_last_packet[ENCODER_COUNT]
                                             [ENCODER_RX_PACKET_SIZE];

/** Last accepted signed count for each encoder. */
extern volatile int16_t g_encoder_count[ENCODER_COUNT];

/** Successful response count for each encoder. */
extern volatile uint32_t g_encoder_success_count[ENCODER_COUNT];

/** Timeout/error count for each encoder. */
extern volatile uint32_t g_encoder_error_count[ENCODER_COUNT];

#endif // ENCODER_H_
