#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdbool.h>
#include <stdint.h>

// ===== Parallax Position Controller command bases =====
#define CMD_QPOS (0x08u)
#define CMD_QSPD (0x10u)
#define CMD_CLRP (0x28u)
#define CMD_SREV (0x30u)

typedef enum
{
    ENCODER1_IDX = 0,
    ENCODER2_IDX,

    ENCODER_COUNT,
} EncoderIndex;

typedef struct
{
    uint8_t addr;
} Encoder;

typedef enum
{
    ENCODER_RX_HIGH_BYTE_IDX = 0,
    ENCODER_RX_LOW_BYTE_IDX = 1,
    ENCODER_RX_PACKET_SIZE = 2,
} EncoderRxPacketIndex;

void encoder_init(void);

void arm_encoder_dma(void);

void uart_encoder_query(uint8_t cmd, EncoderIndex idx);

void uart_encoder_irq(void);

extern volatile bool g_encoder_rx_done;

extern volatile uint8_t g_encoder_rx_packet[ENCODER_RX_PACKET_SIZE];

#endif // ENCODER_H_