#ifndef COMPAT_H
#define COMPAT_H 1

#if defined(HAVE_CONFIG_H)
 #include "config.h"
#endif

#if defined(_WIN32) || defined(_WIN64)
# ifndef min
#  define min(x, y) ((x) < (y) ? (x) : (y))
# endif
# ifndef max
#  define max(x, y) ((x) > (y) ? (x) : (y))
# endif
#elif defined(HAVE_ALGORITHM)
# include <algorithm>
using std::min;
using std::max;
#else
# error "<algorithm> is missing."
#endif

#if defined(_WIN32) || defined(_WIN64)
#define isnan(x) (((x) != (x)) || (x > 1e11) || (x < -1e11))
#endif

#endif /* COMPAT_H */
