
#include <stdio.h>
#include <stdlib.h>

#include <fenice/bufferpool.h>
#include <fenice/en_xmalloc.h>
#include <fenice/log.h>

OMSBuffer *OMSbuff_new(uint32 buffer_size)
{
	OMSSlot *slots = NULL;
	OMSBuffer *buffer = NULL;
	OMSControl *control = NULL;
	uint32 index;
	pthread_mutexattr_t mutex_attr;
	
	TRACE_FUNC();

	if (!buffer_size)
		return NULL;

	DEBUGLOGG("buffer_size = %u", buffer_size);
	slots = xnew_array(buffer_size, OMSSlot);
	// *** slots initialization
	for (index = 0; index < buffer_size - 1; index++)
		(slots[index]).next = index + 1;
	(slots[index]).next = 0;	/*end of the list back to the head */

	// control struct allocation
	control = xnew(OMSControl);
	control->write_pos = buffer_size - 1;
	control->valid_read_pos = 0;	// buffer_size-1;

	control->refs = 0;
	control->nslots = buffer_size;

	pthread_mutexattr_init(&mutex_attr);
	pthread_mutex_init(&control->syn, &mutex_attr);

	buffer = xnew(OMSBuffer);
	buffer->type = buff_local;
	*buffer->filename = '\0';
	// buffer->fd = -1;
	// buffer->fd = NULL;
	buffer->known_slots = buffer_size;

	// link all allocated structs
	buffer->slots = slots;
	buffer->control = control;

	return buffer;
}

