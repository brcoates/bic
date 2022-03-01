#include <stdio.h>
#include "include/log.h"

void log_unexpected(char* expected, char* actual) {
    fprintf(stderr, "Unexpected identifier, expected %s, got %s\n", expected, actual);
}