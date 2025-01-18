#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdint.h>
#include <string.h>
#include <malloc.h>

typedef enum {
    Success = 0,
    MallocError,
    VersionError,
    TypeError,
    LengthError,
    ChecksumError,
    PassportError
} StatusCode;

typedef struct {
    StatusCode status;
    char message[128];
} response_t;

uint8_t* serializeResponse(response_t* data) {
    uint8_t* response = malloc(sizeof(response_t));
    if (response == NULL) {
        return NULL;
    }

    *response = data->status;
    strcpy(response + 1, data->message);
    *(1 + response + strlen(data->message)) = '\x00';

    return response;
}

#endif