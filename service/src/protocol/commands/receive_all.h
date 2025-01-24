#ifndef RECEIVE_ALL_COMMAND_H
#define RECEIVE_ALL_COMMAND_H

#include "../protocol.h"
#include "receive.h"
#include "../../utils/file.h"

typedef struct {
    uint32_t countPassports;
    passport_t** passports;
    StatusCode status;
    char message[128];
} receive_all_response_t;

receive_all_response_t* prepareReceiveAllResponse(passport_t** passports, uint32_t countPassports) {
    receive_all_response_t* receive_all_response = malloc(sizeof(receive_all_response_t));

    receive_all_response->countPassports = countPassports;
    receive_all_response->passports = passports;
    receive_all_response->status = Success;
    strcpy(receive_all_response->message, RECEIVE_ALL_SUCCESS);

    return receive_all_response;
}

void writeReceiveAllResponse(receive_all_response_t* response) {
    for (size_t i = 0; i < response->countPassports; i++) {
        write(1, response->passports[i], sizeof(passport_t));
    }
    write(1, &response->status, sizeof(StatusCode));
    fputs(response->message, stdout);
}

uint32_t receiveAll(passport_t*** result) {
    fileinfo_t* files;
    int32_t countFiles = getFiles(SAVEPATH, &files);
    uint32_t countPassports = 0;
    passport_t** passports = NULL;
    for (int32_t i = 0; i < countFiles; i++) {
        passport_t* passport = readPassport(files[i].path);
        free(files[i].path);
        if (passport == NULL) {
            continue;
        }
        countPassports++;
        passports = (passport_t**)realloc(passports, countPassports * sizeof(passport_t));
        passports[countPassports - 1] = passport;
    }

    *result = passports;

    return countPassports;
}

#endif