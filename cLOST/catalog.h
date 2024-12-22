#ifndef catalog_h
#define catalog_h

#include <stdio.h>
#include <string.h>

#include "parameters.h"
#include "types.h"

extern angle_entry_t angle_list[ANGLE_CATALOG_LENGTH];
extern catalog_entry_t star_list[STAR_CATALOG_LENGTH];

void initialize_catalog(const char *angle_filename, const char *star_filename);

#endif