#pragma once

enum LogLevel {
  kError = 3,
  kWarn = 4,
  kInfo = 6,
  kDebug = 7,
};

/** @brief Sets global log level thresholds.
 * In subsequent calls to Log, only logs with a priority higher than
 * that set here will be logged.
 */
void SetLogLevel(enum LogLevel level);

/** @brief Records a log with the specified priority.
 * If the specified priority is higher than or equal to the thresholds,
 * the log is recorded. Otherwise, it is not recorded.
 * 
 * @param level Log Priority
 * @param format Format string, compatible with printk
 */
int Log(enum LogLevel level, const char *format, ...);
