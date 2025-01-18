#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <malloc.h>

#include "protocol/commands/receive.h"
#include "protocol/commands/send.h"
#include "protocol/response.h"
#include "utils/errors.h"

#define PROTOCOL_VERSION 0x00010000 // 1.0.0
#define BUFFER_LENGTH 2024

StatusCode handle() {
    uint8_t buffer[BUFFER_LENGTH];
    int packetLen = read(0, buffer, 2048);

    if (packetLen < HEADER_LENGTH) {
        return LengthError;
    }
    
    packet_header_t* header = parseHeader(buffer);
    if (header == NULL) {
        return MallocError;
    }
    if (header->version != PROTOCOL_VERSION) {
        free(header);
        return VersionError;
    }
    if (header->length == 0) {
        free(header);
        return LengthError;
    }

    if (header->type == SEND) {
        passport_t* passport = parsePassport(&buffer[HEADER_LENGTH-1], header->length);
        if (passport == NULL) {
            free(header);
            return PassportError;
        }
        int code = send(passport);
    } else if (header->type == RECEIVE) {
        uint16_t series = *(uint16_t*)(buffer + HEADER_LENGTH - 1);
        uint32_t number = *(uint32_t*)(buffer + HEADER_LENGTH + 1);
        passport_t* passport = receive(series, number);
        receive_response_t* receive_response = prepareReceiveResponse(*passport);
        write(1, receive_response, sizeof(receive_response_t));
        free(passport);
        free(receive_response);
    } else {
        free(header);
        return TypeError;
    }

    free(header);
    // printf("Version[%d] Type[%d] Length[%d] Checksum[%d]\n", header->version, header->type, header->length, header->checksum);
    return 0;
}

void handleError(StatusCode code) {
    response_t* response = malloc(sizeof(response_t));

    switch (code) {
        response->status = code;
        case MallocError:
            strcpy(response->message, MALLOC_ERROR);
            break;
        case VersionError:
            strcpy(response->message, VERSION_ERROR);
            break;
        case TypeError:
            strcpy(response->message, TYPE_ERROR);
            break;
        case LengthError:
            strcpy(response->message, LENGTH_ERROR);
            break;
        case ChecksumError:
            strcpy(response->message, CHECKSUM_ERROR);
            break;
        case PassportError:
            strcpy(response->message, PASSPORT_ERROR);
            break;
        default:
            strcpy(response->message, UNKNOWN_ERROR);
    }
    write(1, response, sizeof(response_t));
    free(response);
}

#endif