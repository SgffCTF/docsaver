#ifndef SEND_COMMAND_H
#define SEND_COMMAND_H

#include <openssl/md5.h>

#include "../protocol.h"

#define SEND_BODY_MIN_LENGTH 19

const char* SAVEPATH = "data/";

typedef struct {
    packet_header_t header;
    passport_t body;
} send_request_t;

typedef struct {
    uint8_t status;
    char message[128];
} send_response_t;

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

int validatePassport(passport_t* passport) {
    if (passport->series > 9999 || passport->series < 1000) {
        return 1;
    }
    if (passport->number > 999999 || passport->number < 100000) {
        return 2;
    }
    if (passport->birthDate == NULL || passport->claimDate == NULL) {
        return 3;
    }
    if (passport->gender != MALE && passport->gender != FEMALE) {
        return 4;
    }
    return 0;
}

char* computeMD5(uint8_t* data, uint32_t serLength, uint8_t* md5String) {
    uint8_t md5Hash[MD5_DIGEST_LENGTH];

    MD5(data, serLength, md5Hash);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&md5String[i * 2], "%02x", md5Hash[i]);
    }
    md5String[MD5_DIGEST_LENGTH * 2] = '\0';

    return md5String;
}

int send(passport_t* passport) {
    int code = validatePassport(passport);
    if (code != 0) {
        return code;
    }

    uint32_t serLength;
    uint8_t* data = serializePassport(passport, &serLength);
    free(passport);

    char* filepath = malloc(strlen(SAVEPATH) + MD5_DIGEST_LENGTH * 2 + 1);
    strcpy(filepath, SAVEPATH);
    char* passportMD5 = computeMD5(data, serLength, filepath + strlen(SAVEPATH));

    FILE* file = fopen(filepath, "wb");
    fwrite(data, 1, serLength, file);
    close(file);

    free(filepath);
    free(data);

    return 0;
}

#endif