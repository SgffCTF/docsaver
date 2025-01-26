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
        int packetLen = read(0, buffer, 1024);

        if (!strncmp(buffer, "exit", 4) || packetLen <= 0) {
            break;
        }
        if (packetLen < HEADER_LENGTH) {
            handleError(LengthError, NULL);
            continue;
        }
        
        packet_header_t* header = parseHeader(buffer);
        if (header == NULL) {
            handleError(MallocError, NULL);
            continue;
        }
        if (header->version != PROTOCOL_VERSION_1_0_0 && header->version != PROTOCOL_VERSION_0_1_0) {
            free(header);
            handleError(VersionError, NULL);
            continue;
        }

        if (header->type == SEND) {
            passport_t* passport = parsePassport(&buffer[HEADER_LENGTH], header->length);
            if (passport == NULL) {
                free(header);
                handleError(LengthError, NULL);
                continue;
            }
            if (!validateChecksum(&buffer[HEADER_LENGTH], header->length, header->checksum)) {
                free(header);
                free(passport);
                handleError(ChecksumError, NULL);
                continue;
            }
            int code = validatePassport(passport);
            if (code != 0) {
                free(header);
                handleError(code, passport);
                free(passport);
                continue;
            }
            char* token = send(passport);
            free(passport);

            send_response_t* response = prepareSendResponse(token);
            writeSendResponse(response);
            free(token);
            free(response);
        } else if (header->type == RECEIVE) {
            receive_request_t* request = parseReceiveRequest(&buffer[HEADER_LENGTH]);
            
            if (!validateChecksum(&buffer[HEADER_LENGTH], header->length, header->checksum)) {
                free(header);
                handleError(ChecksumError, NULL);
                continue;
            }

            passport_t* passport = receive(request);
            if (passport == NULL) {
                free(header);
                handleError(PassportNotFoundError, NULL);
                continue;
            }
            free(request);

            receive_response_t* response = prepareReceiveResponse(*passport);
            writeReceiveResponse(response);
            free(passport);
            free(response);
        } else if (header->type == RECEIVE_ALL && header->version == PROTOCOL_VERSION_0_1_0) {
            passport_t** passports = NULL;
            uint32_t countPassports = receiveAll(&passports);
            receive_all_response_t* response = prepareReceiveAllResponse(passports, countPassports);
            writeReceiveAllResponse(response);
            free(response);
            for (size_t i = 0; i < countPassports; i++) {
                free(passports[i]);
            }
        } else {
            handleError(TypeError, NULL);
        }
        // printf("Version[%d] Type[%d] Length[%d] Checksum[%d]\n", header->version, header->type, header->length, header->checksum);
        free(header);
    }
    return 0;
}

#endif