/**
 * @file Logging function for the AB dispatch layer library.
 *
 * @author Ed Hartnett
*/

#include "config.h"
#include "ablogging.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

int ab_log_level = AB_TURN_OFF_LOGGING;

/**
 * This function prints out a message, if the severity of the message
 * is lower than the global ab_log_level. To use it, do something like
 *
 * ab_log(0, "this computer will explode in %d seconds", i);
 *
 * After the first arg (the severity), use the rest like a normal
 * printf statement. Output will appear on stdout.
 *
 * @param severity Print if this is > ab_log_level.
 * @param fmt Format string as from printf statement.
 *
 * @author Ed Hartnett
 */
void 
ab_log(int severity, const char *fmt, ...)
{
#ifdef AB_LOGGING
   va_list argp;
   int t;

   /* If the severity is greater than the log level, we don' care to
      print this message. */
   if (severity > ab_log_level)
      return;

   /* If the severity is zero, this is an error. Otherwise insert that
      many tabs before the message. */
   if (!severity)
      fprintf(stdout, "ERROR: ");
   for (t=0; t<severity; t++)
      fprintf(stdout, "\t");

   /* Print out the variable list of args with vprintf. */
   va_start(argp, fmt);
   vfprintf(stdout, fmt, argp);
   va_end(argp);
   
   /* Put on a final linefeed. */
   fprintf(stdout, "\n");
   fflush(stdout);
#endif /* ifdef AB_LOGGING */
}

/**
 * Use this to set the global log level. Set it to AB_TURN_OFF_LOGGING
 * (-1) to turn off all logging. Set it to 0 to show only errors, and
 * to higher numbers to show more and more logging details.
 *
 * @param new_level The new logging level.
 *
 * @return ::NC_NOERR No error.
 * @author Ed Hartnett
 */
int
ab_set_log_level(int new_level)
{
#ifdef AB_LOGGING
   /* Remember the new level. */
   ab_log_level = new_level;
   LOG((4, "log_level changed to %d", ab_log_level));
#endif /* ifdef AB_LOGGING */   
   return 0;
}



