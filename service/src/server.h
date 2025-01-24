#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <malloc.h>

#include "protocol/commands/receive.h"
#include "protocol/commands/send.h"
#include "protocol/commands/receive_all.h"
#include "protocol/validators.h"
#include "protocol/status.h"

#define PROTOCOL_VERSION_1_0_0 0x00010000 // 1.0.0
#define PROTOCOL_VERSION_0_1_0 0x00000100 // 0.1.0
#define BODY_LENGTH sizeof(passport_t)

StatusCode handle() {
    uint8_t buffer[HEADER_LENGTH+BODY_LENGTH];

    while (true) {
        int packetLen = read(0, buffer, 2024);

        if (!strncmp(buffer, "exit", 4)) {
            break;
        }
        if (packetLen < HEADER_LENGTH) {
            handleError(LengthError, NULL);
            return 0;
        }
        
        packet_header_t* header = parseHeader(buffer);
        if (header == NULL) {
            handleError(MallocError, NULL);
            return 0;
        }
        if (header->version != PROTOCOL_VERSION_1_0_0 && header->version != PROTOCOL_VERSION_0_1_0) {
            free(header);
            handleError(VersionError, NULL);
            return 0;
        }

        if (header->type == SEND) {
            passport_t* passport = parsePassport(&buffer[HEADER_LENGTH], header->length);
            if (passport == NULL) {
                free(header);
                handleError(LengthError, NULL);
                return 0;
            }
            if (!validateChecksum(&buffer[HEADER_LENGTH], packetLen-HEADER_LENGTH, header->checksum)) {
                free(header);
                free(passport);
                handleError(ChecksumError, NULL);
                return 0;
            }
            int code = validatePassport(passport);
            if (code != 0) {
                free(header);
                handleError(code, passport);
                free(passport);
                return 0;
            }
            send(passport);
            receive_response_t* receive_response = prepareReceiveResponse(*passport);
            write(1, receive_response, sizeof(receive_response_t));
            free(passport);
        } else if (header->type == RECEIVE) {
            uint16_t series = *(uint16_t*)(buffer + HEADER_LENGTH);
            uint32_t number = *(uint32_t*)(buffer + HEADER_LENGTH + 2);
            
            if (!validateChecksum(&buffer[HEADER_LENGTH], packetLen-HEADER_LENGTH, header->checksum)) {
                free(header);
                handleError(ChecksumError, NULL);
                return 0;
            }

            passport_t* passport = receive(series, number);
            if (passport == NULL) {
                free(header);
                handleError(PassportNotFoundError, NULL);
                return 0;
            }

            receive_response_t* receive_response = prepareReceiveResponse(*passport);
            write(1, receive_response, sizeof(receive_response_t));
            free(passport);
            free(receive_response);
        } else if (header->type == RECEIVE_ALL && header->version == PROTOCOL_VERSION_0_1_0) {
            passport_t** passports = NULL;
            uint32_t countPassports = receiveAll(&passports);
            receive_all_response_t* receive_all_response = prepareReceiveAllResponse(passports, countPassports);
            writeReceiveAllResponse(receive_all_response);
            free(receive_all_response);
            for (size_t i = 0; i < countPassports; i++) {
                free(passports[i]);
            }
        } else {
            handleError(TypeError, NULL);
        }

        free(header);
    }
    // printf("Version[%d] Type[%d] Length[%d] Checksum[%d]\n", header->version, header->type, header->length, header->checksum);
    return 0;
}

void handleError(StatusCode code, passport_t* passport) {
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
        case PassportSeriesError:
            strcpy(response->message, PASSPORT_SERIES_ERROR);
            break;
        case PassportNumberError:
            strcpy(response->message, PASSPORT_NUMBER_ERROR);
            break;
        case PassportBirthDateError:
            strcpy(response->message, PASSPORT_BIRTHDATE_ERROR);
            break;
        case PassportClaimDateError:
            strcpy(response->message, PASSPORT_CLAIMDATE_ERROR);
            break;
        case PassportGenderError:
            strcpy(response->message, PASSPORT_GENDER_ERROR);
            break;
        case PassportSurnameError:
            strcpy(response->message, PASSPORT_SURNAME_ERROR);
            response->message[34] = ':';
            strcpy(response->message + 35, passport->surname);
            break;
        case PassportNameError:
            strcpy(response->message, PASSPORT_NAME_ERROR);
            response->message[31] = ':';
            strcpy(response->message + 32, passport->name);
            break;
        case PassportLastnameError:
            strcpy(response->message, PASSPORT_LASTNAME_ERROR);
            response->message[35] = ':';
            strcpy(response->message + 36, passport->lastname);
            break;
        case PassportNotFoundError:
            strcpy(response->message, PASSPORT_NOT_FOUND_ERROR);
            break;
        default:
            strcpy(response->message, UNKNOWN_ERROR);
    }
    write(1, &response->status, sizeof(StatusCode));
    printf(response->message);
    free(response);
}

#endif