
#include <fenice/bufferpool.h>
#include <fenice/log.h>

double OMSbuff_nextts(OMSConsumer * cons)
{
	OMSSlot *last_read;
	OMSSlot *next;

//	TRACE_FUNC();

	OMSbuff_lock(cons->buffer);

	OMSbuff_shm_refresh(cons->buffer);

	last_read = OMStoSlot(cons->buffer, cons->last_read_pos);
	next = &cons->buffer->slots[cons->read_pos];

	if (!next->refs || (next->slot_seq < cons->last_seq)) {
		// added some slots?
		if (last_read && cons->buffer->slots[last_read->next].refs
		    && (cons->buffer->slots[last_read->next].slot_seq >
			cons->last_seq))
			next = &cons->buffer->slots[last_read->next];
		else {
			OMSbuff_unlock(cons->buffer);
			return -1;
		}
	} else if (last_read
		   && (cons->buffer->slots[last_read->next].slot_seq <
		       next->slot_seq))
		next = &cons->buffer->slots[last_read->next];

	OMSbuff_unlock(cons->buffer);

	return next->timestamp;
}
