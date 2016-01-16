
#include <stdio.h>
#include <stdlib.h>

#include <fenice/bufferpool.h>
#include <fenice/en_xmalloc.h>
#include <fenice/log.h>

/* ! Add and return a new consumer reference to the buffer,
 * \return NULL if an error occurs*/
OMSConsumer *OMSbuff_ref(OMSBuffer * buffer)
{
	OMSConsumer *cons;
	ptrdiff_t i;

	if (!buffer)
		return NULL;

	cons = xnew(OMSConsumer);

	cons->last_read_pos = -1;	// OMStoSlotPtr(buffer, NULL);
	cons->buffer = buffer;
	cons->frames = 0;
	cons->firstts = -1;

	OMSbuff_lock(buffer);
	cons->read_pos = buffer->control->valid_read_pos;	// buffer->slots[buffer->control->valid_read_pos].next;
	cons->last_seq = 0;	// buffer->slots[buffer->control->valid_read_pos].slot_seq;

	if (buffer->slots[cons->read_pos].slot_seq) {
		for (i = cons->read_pos; i != buffer->control->write_pos;
		     i = buffer->slots[i].next)
			buffer->slots[i].refs++;
		buffer->slots[i].refs++;
	}
	// printf("ref at position %d (write_pos @ %d)\n", cons->read_pos, buffer->control->write_pos);
	buffer->control->refs++;

//      if ( msync(buffer->control, sizeof(OMSControl), MS_SYNC) )
//              printf("*** control msync error\n");
	OMSbuff_unlock(buffer);

	DEBUGLOGG("Buffer ref (%d)", buffer->control->refs);

	return cons;
}
