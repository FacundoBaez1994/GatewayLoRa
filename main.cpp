/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "arm_book_lib.h"
#include "string.h"
#include "tracker.h"


//=====[Main function, the program entry point after power on or reset]========

int main() {
    tracker tracker;
    while (true) {
        tracker.update();
    }
}



