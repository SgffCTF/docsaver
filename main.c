#include <unistd.h>

#include "src/server.h"
#include "src/utils/status.h"

void setup() {
    alarm(5);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main() {
    setup();

    if (sizeof(PacketType) != 1) {
        fprintf(stderr, "Error: PacketType size is %llu bytes, but should be 1 byte\nPlease compile with -fshort-enums flag\n", sizeof(PacketType));
        return 1;
    }
    
    StatusCode errorCode = handle();
    if (errorCode != Success) {
        handleError(errorCode);
        return 2;
    }

    return 0;
}
