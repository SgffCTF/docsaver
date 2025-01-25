#ifndef STATUS_H
#define STATUS_H

#include "protocol.h"

typedef enum {
    Success = 0,
    MallocError,
    VersionError,
    TypeError,
    LengthError,
    ChecksumError,
    PassportSeriesError,
    PassportNumberError,
    PassportBirthDateError,
    PassportClaimDateError,
    PassportGenderError,
    PassportSurnameError,
    PassportNameError,
    PassportLastnameError,
    PassportNotFoundError
} StatusCode;

typedef struct {
    StatusCode status;
    char message[128];
} response_t;

const char SEND_SUCCESS[] = "Passport send successfully";
const char RECEIVE_SUCCESS[] = "Passport received successfully";
const char RECEIVE_ALL_SUCCESS[] = "Passports received successfully";

const char MALLOC_ERROR[] = "error: can't malloc";
const char VERSION_ERROR[] = "error: invalid protocol version";
const char LENGTH_ERROR[] = "error: invalid packet length";
const char TYPE_ERROR[] = "error: invalid type";
const char CHECKSUM_ERROR[] = "error: invalid checksum";
const char PASSPORT_SERIES_ERROR[] = "error: invalid series in passport";
const char PASSPORT_NUMBER_ERROR[] = "error: invalid number in passport";
const char PASSPORT_BIRTHDATE_ERROR[] = "error: invalid birth date in passport";
const char PASSPORT_CLAIMDATE_ERROR[] = "error: invalid claim date in passport";
const char PASSPORT_GENDER_ERROR[] = "error: invalid gender in passport";
const char PASSPORT_SURNAME_ERROR[] = "error: invalid surname in passport";
const char PASSPORT_NAME_ERROR[] = "error: invalid name in passport";
const char PASSPORT_LASTNAME_ERROR[] = "error: invalid lastname in passport";
const char PASSPORT_NOT_FOUND_ERROR[] = "error: passport not found";
const char UNKNOWN_ERROR[] = "Error: Unknown error code";

void handleError(StatusCode code, passport_t* passport) {
    response_t* response = malloc(sizeof(response_t));
    response->status = code;

    switch (code) {
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