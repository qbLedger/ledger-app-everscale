#include "os.h"
#include "cx.h"
#include "utils.h"
#include "menu.h"

#include <stdlib.h>


void writeUint32BE(uint32_t val, uint8_t *bytes) {
    bytes[0] = (val >> 24) & 0xFF;
    bytes[1] = (val >> 16) & 0xFF;
    bytes[2] = (val >> 8) & 0xFF;
    bytes[3] = val & 0xFF;
}

void writeUint64BE(uint64_t val, uint8_t *bytes) {
    bytes[0] = (val >> 56) & 0xFF;
    bytes[1] = (val >> 48) & 0xFF;
    bytes[2] = (val >> 40) & 0xFF;
    bytes[3] = (val >> 32) & 0xFF;
    bytes[4] = (val >> 24) & 0xFF;
    bytes[5] = (val >> 16) & 0xFF;
    bytes[6] = (val >> 8) & 0xFF;
    bytes[7] = val & 0xFF;
}

uint16_t readUint16BE(uint8_t *buffer) {
    return (buffer[0] << 8) | (buffer[1]);
}

uint32_t readUint32BE(uint8_t *buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
}

uint64_t readUint64BE(uint8_t *buffer) {
    uint32_t i1 = buffer[3] + (buffer[2] << 8u) + (buffer[1] << 16u) + (buffer[0] << 24u);
    uint32_t i2 = buffer[7] + (buffer[6] << 8u) + (buffer[5] << 16u) + (buffer[4] << 24u);
    return i2 | ((uint64_t) i1 << 32u);
}

uint8_t leading_zeros(uint16_t value) {
    uint8_t lz = 0;
    uint16_t msb = 0x8000;
    for (uint8_t i = 0; i < 16; ++i) {
        if ((value << i) & msb) {
            break;
        }
        ++lz;
    }

    return lz;
}

#define SCRATCH_SIZE 37
uint8_t convert_hex_amount_to_displayable(const uint8_t* amount, uint8_t decimals, uint8_t amount_length, char* out) {
    uint8_t LOOP1 = SCRATCH_SIZE - decimals;
    uint8_t LOOP2 = decimals;
    uint16_t scratch[SCRATCH_SIZE];
    uint8_t offset = 0;
    uint8_t nonZero = 0;
    uint8_t i;
    uint8_t targetOffset = 0;
    uint8_t workOffset;
    uint8_t j;
    uint8_t nscratch = SCRATCH_SIZE;
    uint8_t smin = nscratch - 2;
    uint8_t comma = 0;

    for (i = 0; i < SCRATCH_SIZE; i++) {
        scratch[i] = 0;
    }
    for (i = 0; i < amount_length; i++) {
        for (j = 0; j < 8; j++) {
            uint8_t k;
            uint16_t shifted_in =
                    (((amount[i] & 0xff) & ((1 << (7 - j)))) != 0) ? (short)1
                                                                   : (short)0;
            for (k = smin; k < nscratch; k++) {
                scratch[k] += ((scratch[k] >= 5) ? 3 : 0);
            }
            if (scratch[smin] >= 8) {
                smin -= 1;
            }
            for (k = smin; k < nscratch - 1; k++) {
                scratch[k] =
                        ((scratch[k] << 1) & 0xF) | ((scratch[k + 1] >= 8) ? 1 : 0);
            }
            scratch[nscratch - 1] = ((scratch[nscratch - 1] << 1) & 0x0F) |
                                    (shifted_in == 1 ? 1 : 0);
        }
    }

    for (i = 0; i < LOOP1; i++) {
        if (!nonZero && (scratch[offset] == 0)) {
            offset++;
        } else {
            nonZero = 1;
            out[targetOffset++] = scratch[offset++] + '0';
        }
    }
    if (targetOffset == 0) {
        out[targetOffset++] = '0';
    }
    workOffset = offset;
    for (i = 0; i < LOOP2; i++) {
        unsigned char allZero = 1;
        unsigned char j;
        for (j = i; j < LOOP2; j++) {
            if (scratch[workOffset + j] != 0) {
                allZero = 0;
                break;
            }
        }
        if (allZero) {
            break;
        }
        if (!comma) {
            out[targetOffset++] = '.';
            comma = 1;
        }
        out[targetOffset++] = scratch[offset++] + '0';
    }
    return targetOffset;
}
