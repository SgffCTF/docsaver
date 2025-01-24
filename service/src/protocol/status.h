#ifndef STATUS_H
#define STATUS_H

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

#endif