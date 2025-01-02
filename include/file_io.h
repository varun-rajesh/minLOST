#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdint.h>
#include <stdio.h>

#include "parameters.h"
#include "utils.h"

void import_binary_image(uint8_t image[IMAGE_Y][IMAGE_X], const char *filepath);

#endif // FILE_IO_H