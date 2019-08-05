#ifndef PARSE_H
#define PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "object.h"

int parseJSON(char *str,int *flag,Object **data);

#ifdef __cplusplus
}
#endif

#endif