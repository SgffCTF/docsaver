#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <string.h>

#include "status.h"
#include "../utils/date.h"

const char* SAVEPATH = "data/";
const int SAVEPATH_LENGTH = 5;
const int FILENAME_LENGTH = 11;

typedef enum {
    SEND = 0,
    RECEIVE,
    RECEIVE_ALL
} PacketType;

typedef struct {
    uint32_t version;
    PacketType type;
    uint16_t length;
    uint32_t checksum;
} packet_header_t;

#define HEADER_LENGTH 11

packet_header_t* parseHeader(uint8_t data[]) {
    packet_header_t* header = malloc(sizeof(packet_header_t));
    if (header == NULL) {
        return NULL;
    }

    header->version = *(uint32_t*)data;
    header->type = (PacketType)data[4];
    header->length = *(uint16_t*)(data + 5);
    header->checksum = *(uint32_t*)(data + 7);
    
    return header;
}

typedef enum {
    MALE = 0,
    FEMALE
} Gender;

typedef struct {
    uint16_t series;
    uint32_t number;
    date_t* birthDate;
    date_t* claimDate;
    Gender gender;
    char surname[128];
    char name[128];
    char lastname[128];
    char birthPlace[128];
} passport_t;

passport_t* parsePassport(uint8_t data[], uint32_t length) {
    passport_t* passport = malloc(sizeof(passport_t));
    if (passport == NULL) {
        return NULL;
    }

    passport->series = *(uint16_t*)data;
    passport->number = *(uint32_t*)(data + 2);
    passport->birthDate = parseDate(&data[6]);
    passport->claimDate = parseDate(&data[10]);
    passport->gender = data[14];

    char* surname = &data[15];
    int surnameLen = strlen(surname) + 1;
    char* name = &data[15 + surnameLen];
    int nameLen = strlen(name) + 1;
    char* lastname = &data[15 + surnameLen + nameLen];
    int lastnameLen = strlen(lastname) + 1;
    char* birthPlace = &data[15 + surnameLen + nameLen + lastnameLen];
    int birthPlaceLen = strlen(birthPlace) + 1;

    int passportLen = 2 + 4 + 4 + 4 + 1 + surnameLen + nameLen + lastnameLen + birthPlaceLen;
    if (passportLen != length) {
        free(passport);
        return NULL;
    }

    strcpy(passport->surname, surname);
    strcpy(passport->name, name);
    strcpy(passport->lastname, lastname);
    strcpy(passport->birthPlace, birthPlace);

    return passport;
}

passport_t* readPassport(char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        return NULL;
    }

    uint8_t* bytes = malloc(sizeof(passport_t));
    if (bytes == NULL) {
        fclose(file);
        return NULL;
    }
    size_t bytesRead = fread(bytes, 1, sizeof(passport_t), file);

    fclose(file);

    passport_t* passport = parsePassport(bytes, bytesRead);
    free(bytes);

    return passport;
}

#endif

