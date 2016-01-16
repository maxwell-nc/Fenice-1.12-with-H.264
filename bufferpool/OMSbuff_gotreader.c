
#include <stdio.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

#define RETURN_ERR	do { \
				OMSbuff_unlock(cons->buffer); \
				return 1; \
			} while (0)

/* ! move current read position
 *
 * \return 1 on error, 0 otherwise
 * */
int32 OMSbuff_gotreader(OMSConsumer * cons)
{
	OMSSlot *last_read;
	OMSSlot *next;

//	TRACE_FUNC();

	OMSbuff_lock(cons->buffer);

	if (OMSbuff_shm_refresh(cons->buffer))
		RETURN_ERR;

	last_read = OMStoSlot(cons->buffer, cons->last_read_pos);
	next = &cons->buffer->slots[cons->read_pos];

	if (!next->refs || (next->slot_seq < cons->last_seq)) {
		// added some slots?
		if (last_read && cons->buffer->slots[last_read->next].refs
		    && (cons->buffer->slots[last_read->next].slot_seq >
			cons->last_seq))
			next = &cons->buffer->slots[last_read->next];
		else
			RETURN_ERR;
	} else if (last_read
		   && (cons->buffer->slots[last_read->next].slot_seq <
		       next->slot_seq))
		next = &cons->buffer->slots[last_read->next];

	next->refs--;

	cons->last_seq = next->slot_seq;

//      if ( msync(next, sizeof(OMSSlot), MS_ASYNC) )
//      if ( msync(cons->buffer->slots, cons->buffer->known_slots * sizeof(OMSSlot), MS_ASYNC) )
//              printf("*** slot msync error\n");

	cons->last_read_pos = OMStoSlotPtr(cons->buffer, next);
	cons->read_pos = next->next;

	// cons->read_pos->refs--;
	OMSbuff_dump(cons, NULL);
	OMSbuff_unlock(cons->buffer);

	// return cons->read_pos;
	return 0;		// next;
}

#undef RETURN_ERR
