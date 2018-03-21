/**
 * @file Header file for logging for the AB dispatch layer library.
 *
 * @author Ed Hartnett
*/

#ifndef _ABLOGGING_
#define _ABLOGGING_

#include <stdlib.h>
#include <assert.h>

/* Set the log level. 0 shows only errors, 1 only major messages,
 * etc., to 5, which shows way too much information. */
int ab_set_log_level(int new_level);

/* To log something... */
void ab_log(int severity, const char *fmt, ...);

/* Use this to turn off logging by calling
   ab_log_level(NC_TURN_OFF_LOGGING) */
#define AB_TURN_OFF_LOGGING (-1)

#ifdef AB_LOGGING
#define LOG(e) ab_log e
#else /* AB_LOGGING */
#define LOG(e)
#endif /* AB_LOGGING */

#endif /* _ABLOGGING_ */

