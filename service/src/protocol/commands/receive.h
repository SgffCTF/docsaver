#ifndef RECEIVE_COMMAND_H
#define RECEIVE_COMMAND_H

#include "send.h"
#include "../status.h"

#define RECEIVE_BODY_MIN_LENGTH 16

typedef struct {
    passport_t passport;
    StatusCode status;
    char message[128];
} receive_response_t;

receive_response_t* prepareReceiveResponse(passport_t passport) {
    receive_response_t* response = malloc(sizeof(receive_response_t));
    if (response == NULL) {
        return NULL;
    }

    response->passport = passport;
    response->status = Success;
    strcpy(response->message, RECEIVE_SUCCESS);

    return response;
}

passport_t* receive(uint16_t series, uint32_t number) {
    char filepath[SAVEPATH_LENGTH + FILENAME_LENGTH + 1];
    sprintf(filepath, "%s%04u_%06u", SAVEPATH, series, number);
    filepath[SAVEPATH_LENGTH + FILENAME_LENGTH] = '\x00';

    passport_t* passport = readPassport(filepath);

    return passport;
}

#endif