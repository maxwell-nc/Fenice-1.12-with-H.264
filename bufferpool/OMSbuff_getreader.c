
#include <stdio.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

/*! \return the next slot in the buffer and do NOT move current read position.
 * NULL if buffer is empty
 * */
OMSSlot *OMSbuff_getreader(OMSConsumer * cons)
{
	OMSSlot *last_read;
	OMSSlot *next;

//	TRACE_FUNC();

	OMSbuff_lock(cons->buffer);

	// TODO: if it fails?
	OMSbuff_shm_refresh(cons->buffer);

//	DEBUGLOGG("cons->last_read_pos = %d", cons->last_read_pos);
//	DEBUGLOGG("cons->read_pos = %d", cons->read_pos);

	last_read = OMStoSlot(cons->buffer, cons->last_read_pos);
	next = &cons->buffer->slots[cons->read_pos];

	if (!next->refs || (next->slot_seq < cons->last_seq)) {
		// added some slots?
		if (last_read && cons->buffer->slots[last_read->next].refs &&
		    (cons->buffer->slots[last_read->next].slot_seq >
		     cons->last_seq))
			next = &cons->buffer->slots[last_read->next];
		else {
			OMSbuff_unlock(cons->buffer);
			return NULL;
		}
	} else if (last_read
		   && (cons->buffer->slots[last_read->next].slot_seq <
		       next->slot_seq))
		next = &cons->buffer->slots[last_read->next];

	if (cons->firstts == -1)
		cons->firstts = next->timestamp;

	OMSbuff_unlock(cons->buffer);
	return next;
}
