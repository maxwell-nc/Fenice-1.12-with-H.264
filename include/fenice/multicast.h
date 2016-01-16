
#ifndef _MULTICASTH
#define _MULTICASTH

#include <fenice/types.h>
#include <fenice/utils.h>

#define DEFAULT_MULTICAST_ADDRESS "224.124.0.1"
#define DEFAULT_TTL 32

uint32 is_valid_multicast_address(char *addr);
#endif
