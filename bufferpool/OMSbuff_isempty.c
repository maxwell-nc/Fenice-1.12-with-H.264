
#include <stdio.h>

#include <fenice/bufferpool.h>

#define RETURN(x)	do { \
				OMSbuff_unlock(cons->buffer); \
				return x; \
			} while (0)

/*! Check if buffer is empty for a given consumer. 
 *
 * Checks in this function are taken from <tt>OMSbuff_read()</tt> and not
 * optimized
 *
 * \param Consumer to be checked.
 * \return 1 if buffer is empty, 0 if not.
 * \return -1 on error.
 *
 * \see OMSbuff_read
 * */
int OMSbuff_isempty(OMSConsumer * cons)
{
	OMSSlot *last_read;
	OMSSlot *next;

	OMSbuff_lock(cons->buffer);

	OMSbuff_shm_refresh(cons->buffer);

	last_read = OMStoSlot(cons->buffer, cons->last_read_pos);
	next = &cons->buffer->slots[cons->read_pos];

	if (!next->refs || (next->slot_seq < cons->last_seq)) {
		// added some slots?
		if (last_read && cons->buffer->slots[last_read->next].refs
		    && (cons->buffer->slots[last_read->next].slot_seq >
			cons->last_seq))
			RETURN(0);
		else
			RETURN(1);
	} else if (last_read
		   && (cons->buffer->slots[last_read->next].slot_seq <
		       next->slot_seq))
		RETURN(0);

	OMSbuff_unlock(cons->buffer);

	return 0;
}

#undef RETURN
