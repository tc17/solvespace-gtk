#ifndef INTTYPES_H
#define INTTYPES_H

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
// Define some useful C99 integer types.
typedef UINT64 uint64_t;
typedef  INT64  int64_t;
typedef UINT32 uint32_t;
typedef  INT32  int32_t;
typedef USHORT uint16_t;
typedef  SHORT  int16_t;
typedef  UCHAR  uint8_t;
typedef   CHAR   int8_t;
#elif defined(HAVE_STDINT_H)
# include <stdint.h>
#else
# error "stdint.h is missing."
#endif

#if defined(_WIN32) || defined(_WIN64)
# include <limits.h>
#else
# if defined(HAVE_LIMITS_H)
#  include <limits.h>
# else
#  error "limits.h is missing."
# endif
#endif

#endif /* INTTYPES_H */
