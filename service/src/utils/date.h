#ifndef DATE_H
#define DATE_H

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
} date_t;

int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

date_t* parseDate(uint8_t data[]) {
    date_t* result = (date_t*)malloc(sizeof(date_t));
    if (result == NULL) {
        return NULL;
    }

    uint16_t year = *(uint16_t*)data;
    uint8_t month = *(uint8_t*)(data + 2);
    uint8_t day = *(uint8_t*)(data + 3);

    bool isValid = true;
    if (month > 12 || day > 31) {
        isValid = false;
    }
    if ((year % 4 == 0 && month == 2 && day > 29) || daysInMonth[month - 1] < day) {
        isValid = false;
    }

    if (isValid) {
        result->year = year;
        result->month = month;
        result->day = day;
        return result;
    }

    free(result);
    return NULL;
};

#endif