
#include <stdio.h>
#include <string.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

/* ! Read the next slot in buffer.
 *
 * Read the next slot in the buffer fill the parameters given as input and move
 * current read position.
 *
 * \param cons is the consumer.
 * \param timestamp return position for timestamp value of slot read.
 * \param marker return position for marker value of slot read.
 * \param data position of where to copy data filed of the read slot.
 * \param data_size input value for size of <tt>data</tt> buffer and return
 * value for size of effective size of read data.
 *
 * \return -1 on error, 1 if slot data size is bigger thar size of buffer
 * provided and then it was not possible to copy all the data, 0 otherwise.
 * 
 * */
// OMSSlot *OMSbuff_read(OMSConsumer *cons)

int32 OMSbuff_read(
		OMSConsumer * cons,
		uint32 * timestamp,
		uint8 * marker,
		uint8 * data,
		uint32 * data_size)
{
	OMSSlot *last_read;
	OMSSlot *next;
	uint32 cpy_size;

	TRACE_FUNC();

	OMSbuff_lock(cons->buffer);

	OMSbuff_shm_refresh(cons->buffer);

	last_read = OMStoSlot(cons->buffer, cons->last_read_pos);	//we use OMStoSlot 'cause last_read could be NULL
	next = &cons->buffer->slots[cons->read_pos];

	if (!next->refs || (next->slot_seq < cons->last_seq)) {
		// added some slots?
		if (last_read && cons->buffer->slots[last_read->next].refs
		    && (cons->buffer->slots[last_read->next].slot_seq >
			cons->last_seq))
			next = &cons->buffer->slots[last_read->next];
		else {
			OMSbuff_unlock(cons->buffer);
			return -1;	// NULL;
		}
	} else if (last_read
		   && (cons->buffer->slots[last_read->next].slot_seq <
		       next->slot_seq))
		next = &cons->buffer->slots[last_read->next];

	cpy_size = omsbuff_min(*data_size, next->data_size);

	next->refs--;

	cons->last_seq = next->slot_seq;
	// cons->read_pos = next;

	cons->read_pos = next->next;

	// cons->read_pos->refs--;
	// fill input parameters
	*timestamp = next->timestamp;
	*marker = next->marker;
	memcpy(data, next->data, cpy_size);
	*data_size = cpy_size;

//      if ( msync(next, sizeof(OMSSlot), MS_ASYNC) )
//      if ( msync(cons->buffer->slots, cons->buffer->known_slots * sizeof(OMSSlot), MS_ASYNC) )
//              printf("*** msync error\n");
	OMSbuff_unlock(cons->buffer);

	cons->last_read_pos = OMStoSlotPtr(cons->buffer, next);

	// return cons->read_pos;
	return (cpy_size == next->data_size) ? 0 : 1;	// next;          
}
