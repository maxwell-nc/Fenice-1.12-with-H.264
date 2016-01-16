

#ifndef _DEBUGH
#define _DEBUGH

#include <config.h>
#include <fenice/types.h>


#if ENABLE_VERBOSE
void dump_buffer(char *buffer);
#define VERBOSE
#endif
#if ENABLE_DUMP
int dump_payload(uint8 * data_slot, uint32 data_size, uint8 fname[255]);
#endif


#define DEBUG ENABLE_DEBUG
//      #define POLLED 
//      #define SIGNALED 
#define THREADED
//      #define SELECTED 

#endif
