#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <stdbool.h>

#include "commands/send.h"
#include "status.h"
#include "../../lib/crc/crc.h"

const char BLACKLIST[] = "?!()[]{}^@$;-_=%+`~";

bool validateChecksum(const void* data, size_t size, uint32_t checksum) {
    uint32_t calculatedChecksum = crc32(data, size);
    // printf("%x\n", calculatedChecksum);
    return calculatedChecksum == checksum;
}

bool validateField(char* field, int len) {
    for (int i = 0; i < len; i++) {
        if (strchr(BLACKLIST, field[i]) != NULL) {
            return false;
        }
    }
    return true;
}

int validatePassport(passport_t* passport) {
    if (passport->series > 9999 || passport->series < 1000) {
        return PassportSeriesError;
    }
    if (passport->number > 999999 || passport->number < 100000) {
        return PassportNumberError;
    }
    if (passport->birthDate == NULL) {
        return PassportBirthDateError;
    }
    if (passport->claimDate == NULL) {
        return PassportClaimDateError;
    }
    if (passport->gender != MALE && passport->gender != FEMALE) {
        return PassportGenderError;
    }
    if (!validateField(passport->surname, strlen(passport->surname))) {
        return PassportSurnameError;
    }
    if (!validateField(passport->name, strlen(passport->name))) {
        return PassportNameError;
    }
    if (!validateField(passport->lastname, strlen(passport->lastname))) {
        return PassportLastnameError;
    }
    return 0;
}

#endif