#include "mbed.h"
#include <ctime>

/**
 * @brief Converts a timestamp string in the format DDMMYYHHMMSS to epoch time.
 * @details This function parses a timestamp string representing date and time
 * and converts it to a `time_t` value (seconds since 1970-01-01 00:00:00 UTC).
 * The year is assumed to be in the 2000s.
 * @param timestamp Pointer to a null-terminated string containing the timestamp in the format DDMMYYHHMMSS.
 * @return The corresponding epoch time as a `time_t` value, using local time.
 */
time_t timestampToEpoch(const char* timestamp);

/**
 * @brief Converts epoch time to a timestamp string in the format DDMMYYHHMMSS.
 * @details This function takes a `time_t` value and formats it into a string representing
 * date and time in the format DDMMYYHHMMSS.
 * @param seconds Epoch time (seconds since 1970-01-01 00:00:00 UTC).
 * @param timestampOut Pointer to the output buffer where the formatted string will be stored.
 * @param len Length of the output buffer (should be at least 13 bytes to store the string plus null terminator).
 */
void epochToTimestamp(time_t seconds, char* timestampOut, size_t len);