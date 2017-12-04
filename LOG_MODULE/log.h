#ifndef __LOG_H__
#define __LOG_H__

#if LOG_MODULE_ENABLE
#include "stdio.h"
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INF0 3
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_NONE 0

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INF0
#endif

#ifndef LOG_NAME
#define LOG_NAME ""
#endif

#define LOG_PRINTF(A,B,...) printf(A);printf(B);printf(__VA_ARGS__)

#if LOG_LEVEL==LOG_LEVEL_DEBUG
#define LOG_DEBUG(...)        LOG_PRINTF(LOG_NAME,":DEBUG:",__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif


#if (LOG_LEVEL==LOG_LEVEL_INF0)||(LOG_LEVEL==LOG_LEVEL_DEBUG)
#define LOG_INFO(...)         LOG_PRINTF(LOG_NAME,":INF0:",__VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if (LOG_LEVEL==LOG_LEVEL_WARNING)||(LOG_LEVEL==LOG_LEVEL_INF0)||(LOG_LEVEL==LOG_LEVEL_DEBUG)      
#define LOG_WARNING(...)      LOG_PRINTF(LOG_NAME,":WARNING:",__VA_ARGS__)
#else
#define LOG_WARNING(...)
#endif

#if LOG_LEVEL!=LOG_LEVEL_NONE
#define LOG_ERROR(...)        LOG_PRINTF(LOG_NAME,":ERROR:",__VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#endif
#endif
