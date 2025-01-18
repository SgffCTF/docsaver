#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include "protocol/commands/receive.h"
#include "protocol/commands/send.h"
#include "utils/status.h"

#define PROTOCOL_VERSION 0x00010000 // 1.0.0
#define BUFFER_LENGTH 2024

typedef enum {
    Success = 0,
    MallocError,
    VersionError,
    TypeError,
    LengthError,
    ChecksumError,
    PassportError
} StatusCode;

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
        send(passport);
        // TODO: implement send handler
    } else if (header->type == RECEIVE) {
        // TODO: implement receive handler
    } else {
        free(header);
        return TypeError;
    }

    // printf("Version[%d] Type[%d] Length[%d] Checksum[%d]\n", header->version, header->type, header->length, header->checksum);
    return 0;
}

void handleError(StatusCode code) {
    switch (code) {
        case MallocError:
            fputs(MALLOC_ERROR, STDERR_FILENO);
            break;
        case VersionError:
            fputs(VERSION_ERROR, STDERR_FILENO);
            break;
        case TypeError:
            fputs(TYPE_ERROR, STDERR_FILENO);
            break;
        case LengthError:
            fputs(LENGTH_ERROR, STDERR_FILENO);
            break;
        case ChecksumError:
            fputs(CHECKSUM_ERROR, STDERR_FILENO);
            break;
        case PassportError:
            fputs(PASSPORT_ERROR, STDERR_FILENO);
            break;
        default:
            fputs("Error: Unknown error code", STDERR_FILENO);
    }
}

#endif