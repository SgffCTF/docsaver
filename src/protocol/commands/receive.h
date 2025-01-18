#ifndef RECEIVE_COMMAND_H
#define RECEIVE_COMMAND_H

#include "../protocol.h"

#define RECEIVE_BODY_MIN_LENGTH 16

typedef struct {
    packet_header_t header;
    uint16_t series;
    uint32_t number;
    char surname[128];
} receive_request_t;

typedef struct {
    packet_header_t header;
    passport_t passport;
    uint8_t status;
    char message[128];
} receive_response_t;

#endif