#ifndef CLIENT_H
#define CLIENT_H

#include "../include/arg_parser.h"
#include "../include/logger.h"
#include "../include/ring_buffer.h"

typedef struct {
  ring_buffer_t *rb;
  Options *opts;
  Logger *logger;
} TcpClientArgs;

void *tcp_client_thread(void *arg);

#endif // CLIENT_H