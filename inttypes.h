#ifndef INTTYPES_H
#define INTTYPES_H

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#if defined(_WIN32) || defined(_WIN64)
typedef unsigned __int64 QWORD;
typedef signed __int64 SQWORD;
typedef signed long SDWORD;
typedef signed short SWORD;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
#elif defined(HAVE_STDINT_H)
# include <stdint.h>
typedef uint64_t QWORD;
typedef int64_t SQWORD;
typedef int32_t SDWORD;
typedef int16_t SWORD;
typedef uint32_t DWORD;
typedef uint8_t BYTE;
#else
# error "stdint.h is missing."
#endif

#if !defined(_WIN32) && !defined(_WIN64)
# if defined(HAVE_LIMITS_H)
#  include <limits.h> // for INT_MAX
# else
#  error "limits.h is missing."
# endif
#endif


#endif /* INTTYPES_H */
