#ifndef SEND_COMMAND_H
#define SEND_COMMAND_H

#include <stdlib.h>
#include <openssl/md5.h>

#include "../protocol.h"
#include "../status.h"

#define TOKEN_LENGTH 16

typedef struct {
    StatusCode status;
    char message[128];
    char* token;
} send_response_t;

send_response_t* prepareSendResponse(char* token) {
    send_response_t* response = malloc(sizeof(send_response_t));
    if (response == NULL) {
        return NULL;
    }

    response->status = Success;
    strcpy(response->message, SEND_SUCCESS);
    response->token = token;

    return response;
}

void writeSendResponse(send_response_t* response) {
    write(1, &response->status, sizeof(StatusCode));
    write(1, response->message, strlen(response->message) + 1);
    write(1, response->token, TOKEN_LENGTH + 1);
}

uint8_t* serializePassport(passport_t* passport, uint32_t* serLength) {
    uint32_t surnameLen = strlen(passport->surname) + 1;
    uint32_t nameLen = strlen(passport->name) + 1;
    uint32_t lastnameLen = strlen(passport->lastname) + 1;
    uint32_t birthPlaceLen = strlen(passport->birthPlace) + 1;

    *serLength = 2 + 4 + 4 + 4 + 1 + surnameLen + nameLen + lastnameLen + birthPlaceLen;
    uint8_t* data = malloc(*serLength);
    if (data == NULL) {
        return NULL;
    }

    uint8_t* ptr = data;

    memcpy(ptr, &passport->series, sizeof(passport->series));
    ptr += sizeof(passport->series);

    memcpy(ptr, &passport->number, sizeof(passport->number));
    ptr += sizeof(passport->number);

    memcpy(ptr, &passport->birthDate->year, sizeof(passport->birthDate->year));
    ptr += sizeof(passport->birthDate->year);
    *ptr = passport->birthDate->month;
    ptr++;
    *ptr = passport->birthDate->day;
    ptr++;

    memcpy(ptr, &passport->claimDate->year, sizeof(passport->claimDate->year));
    ptr += sizeof(passport->claimDate->year);
    *ptr = passport->claimDate->month;
    ptr++;
    *ptr = passport->claimDate->day;
    ptr++;

    *ptr = (uint8_t)passport->gender;
    ptr++;

    memcpy(ptr, passport->surname, surnameLen);
    ptr += surnameLen;

    memcpy(ptr, passport->name, nameLen);
    ptr += nameLen;

    memcpy(ptr, passport->lastname, lastnameLen);
    ptr += lastnameLen;

    memcpy(ptr, passport->birthPlace, birthPlaceLen);
    ptr += birthPlaceLen;

    return data;
}

char* generateToken() {
    char* token = malloc(TOKEN_LENGTH + 1);
    for (size_t i = 0; i < TOKEN_LENGTH; i++) {
        token[i] = 97 + rand() % 26;
    }
    token[TOKEN_LENGTH] = '\x00';
    return token;
}

char* send(passport_t* passport) {
    char filepath[SAVEPATH_LENGTH + FILENAME_LENGTH + TOKEN_LENGTH + 3];
    char* token = generateToken();

    sprintf(filepath, "%s%04u_%06u_%16s", SAVEPATH, passport->series, passport->number, token);
    filepath[SAVEPATH_LENGTH + FILENAME_LENGTH + TOKEN_LENGTH + 2] = '\x00';

    uint32_t serLength;
    uint8_t* data = serializePassport(passport, &serLength);

    FILE* file = fopen(filepath, "wb");
    fwrite(data, 1, serLength, file);
    fclose(file);

    free(data);

    return token;
}

#endif