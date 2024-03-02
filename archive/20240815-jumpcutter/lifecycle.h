#ifndef _JUMPCUTTER_LIFECYCLE_H
#define _JUMPCUTTER_LIFECYCLE_H

#include "jumpcutter.h"

int jumpcutter_init(jumpcutter_t* j, const char* input_filename, const char* output_filename, jumpcutter_error_t* error);
void jumpcutter_free(jumpcutter_t* j);

#endif