#pragma once

#include <arpa/inet.h>
#include <3ds.h>

#define SOCKINPUT_MAXPORT 65535
#define SOCKINPUT_STRLEN (INET_ADDRSTRLEN+6)

bool sockInputFormatFromString(const char* input, struct sockaddr_in* out);
bool sockInputFormatToString(const struct sockaddr_in* in, char* out, size_t size);
bool sockInputAsk(struct sockaddr_in* out, const char* initialStr);
