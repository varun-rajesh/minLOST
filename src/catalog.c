#include "catalog.h"

angle_entry_t angle_list[ANGLE_CATALOG_LENGTH] = {
#include ANGLE_CATALOG_OUTPUT
};

catalog_entry_t star_list[STAR_CATALOG_LENGTH] = {
#include STAR_CATALOG_OUTPUT
};