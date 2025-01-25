#ifndef RECEIVE_COMMAND_H
#define RECEIVE_COMMAND_H

#include "send.h"
#include "../status.h"

#define RECEIVE_BODY_MIN_LENGTH 16

typedef struct {
    uint16_t series;
    uint32_t number;
    char token[TOKEN_LENGTH + 1];
} receive_request_t;

receive_request_t* parseReceiveRequest(uint8_t data[]) {
    receive_request_t* request = malloc(sizeof(receive_request_t));
    request->series = *(uint16_t*)data;
    request->number = *(uint32_t*)(data + 2);
    strncpy(request->token, data + 6, TOKEN_LENGTH);
    request->token[TOKEN_LENGTH] = '\x00';

    return request;
}

typedef struct {
    StatusCode status;
    char message[128];
    passport_t passport;
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

void writeReceiveResponse(receive_response_t* response) {
    write(1, &response->status, 1);
    write(1, response->message, strlen(response->message) + 1);
    writePassport(&response->passport);
}

passport_t* receive(receive_request_t* request) {
    char filepath[SAVEPATH_LENGTH + FILENAME_LENGTH + TOKEN_LENGTH + 3];
    sprintf(filepath, "%s%04u_%06u_%s", SAVEPATH, request->series, request->number, request->token);
    filepath[SAVEPATH_LENGTH + FILENAME_LENGTH + TOKEN_LENGTH + 2] = '\x00';

    passport_t* passport = readPassport(filepath);

    return passport;
}

#endif