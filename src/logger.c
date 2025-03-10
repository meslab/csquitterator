#include "../include/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * @brief Get the string representation of a log level
 *
 * @param level The log level
 * @return const char* The string representation of the log level
 */
const char *log_level_str(LogLevel level) {
  switch (level) {
  case LOG_DEBUG:
    return "DEBUG";
  case LOG_INFO:
    return "INFO";
  case LOG_WARNING:
    return "WARNING";
  default:
    return "ERROR";
  }
}

/**
 * @brief Parse a string to a log level
 *
 * @param level_str The string representation of the log level
 * @return LogLevel The log level
 */
LogLevel parse_log_level(const char *level_str) {
  if (strcmp(level_str, "DEBUG") == 0 || strcmp(level_str, "D") == 0)
    return LOG_DEBUG;
  if (strcmp(level_str, "INFO") == 0 || strcmp(level_str, "I") == 0)
    return LOG_INFO;
  if (strcmp(level_str, "WARNING") == 0 || strcmp(level_str, "W") == 0)
    return LOG_WARNING;
  return LOG_ERROR;
}

/**
 * @brief Log a message to the appropriate destination
 *
 * @param logger The logger
 * @param level The log level
 * @param message The message to log
 */
void log_message(Logger *logger, const LogLevel level, const char *message) {
  if (level < logger->level)
    return;

  FILE *dest = (level == LOG_ERROR)
                   ? (logger->err_log ? logger->err_log : stderr)
                   : (logger->out_log ? logger->out_log : stdout);

  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char time_buf[20]; // YYYY-MM-DD HH:MM:SS
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

  fprintf(dest, "[%s] [%s] %s\n", time_buf, log_level_str(level), message);
  fflush(dest);
}

/**
 * @brief Initialize the logger
 *
 * @param logger The logger
 * @param opts The options
 * @return int 0 on success, -1 on failure
 */
int init_logger(Logger *logger, const Options *opts) {
  logger->err_log = NULL;
  logger->out_log = NULL;
  logger->level = LOG_ERROR;

  if (!opts)
    return -1; // Ensure opts is valid

  // Set the log level
  logger->level = parse_log_level(opts->log_level ? opts->log_level : "ERROR");

  // Open log files if paths are provided
  if (opts->err_log) {
    logger->err_log = fopen(opts->err_log, "a");
    if (!logger->err_log) {
      perror("Failed to open error log file");
      return -1;
    }
  }

  if (opts->out_log) {
    logger->err_log = fopen(opts->out_log, "a");
    if (!logger->out_log) {
      perror("Failed to open output log file");
      if (logger->err_log)
        fclose(logger->err_log);
      return -1;
    }
  }

  return 0; // Success
}

/**
 * @brief Close the logger
 *
 * @param logger The logger
 */
void close_logger(Logger *logger) {
  if (logger->err_log)
    fclose(logger->err_log);
  if (logger->out_log)
    fclose(logger->out_log);
  logger->err_log = logger->out_log = NULL;
}

/// @brief Log an error message
/// @param logger  The logger
/// @param message The message to log
void log_error(Logger *logger, const char *message) {
  log_message(logger, LOG_ERROR, message);
}

/// @brief Log an info message
/// @param logger  The logger
/// @param message The message to log
void log_info(Logger *logger, const char *message) {
  log_message(logger, LOG_INFO, message);
}

/// @brief Log a warning message
/// @param logger  The logger
/// @param message The message to log
void log_warning(Logger *logger, const char *message) {
  log_message(logger, LOG_WARNING, message);
}

/// @brief Log a debug message
/// @param logger  The logger
/// @param message The message to log
void log_debug(Logger *logger, const char *message) {
  log_message(logger, LOG_DEBUG, message);
}

/// @brief Log an info message with a formatted string
/// @param logger The logger
/// @param format The format string
/// @param    ... The format arguments
void log_info_formatted(Logger *logger, const char *format, ...) {
  if (LOG_INFO < logger->level)
    return;

  char log_buffer[LOG_MESSAGE_MAX_LENGTH];
  va_list args;
  va_start(args, format);
  vsnprintf(log_buffer, LOG_MESSAGE_MAX_LENGTH, format, args);
  va_end(args);

  log_message(logger, LOG_INFO, log_buffer);
}

/// @brief Log a debug message with a formatted string
/// @param logger The logger
/// @param format The format string
/// @param    ... The format arguments
void log_debug_formatted(Logger *logger, const char *format, ...) {
  if (LOG_DEBUG < logger->level)
    return;

  char log_buffer[LOG_MESSAGE_MAX_LENGTH];
  va_list args;
  va_start(args, format);
  vsnprintf(log_buffer, LOG_MESSAGE_MAX_LENGTH, format, args);
  va_end(args);

  log_message(logger, LOG_DEBUG, log_buffer);
}