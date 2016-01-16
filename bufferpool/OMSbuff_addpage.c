
#include <fenice/bufferpool.h>
#include <fenice/log.h>

/*\brief internal function used to add a page of slots (of OMSBUFF_MEM_PAGE size).
 * WARNING: the function assumes that the caller (OMSbuff_write or OMSbuff_getslot) locked the buffer mutex
 * \return the first OMSSlot of new added page of slots.
 * */
OMSSlot *OMSbuff_addpage(OMSBuffer * buffer, OMSSlot * prev)
{
	OMSSlot *added;
	unsigned int i;
	ptrdiff_t prev_diff;

	TRACE_FUNC();

	switch (buffer->type) {
		case buff_shm:
			prev_diff = prev - buffer->slots;
			added = OMSbuff_shm_addpage(buffer);
			prev = buffer->slots + prev_diff;

			// last added slot in shm new page is linked to the prev->next in old queue
			buffer->slots[buffer->known_slots - 1].next = prev->next;
			DEBUGLOGG("OMSSlots page added in SHM memory (%u slots)", buffer->known_slots);

			break;
		case buff_local:
			prev_diff = prev - buffer->slots;
			if (!(added = realloc(buffer->slots,
								  (buffer->control->nslots +
								   OMSBUFF_MEM_PAGE) * sizeof(OMSSlot))))
				return NULL;
			buffer->slots = added;

			prev = buffer->slots + prev_diff;
			// init new slots
			for (i = buffer->control->nslots;
				 i < buffer->control->nslots + OMSBUFF_MEM_PAGE - 1; i++) {
				added[i].refs = 0;
				added[i].slot_seq = 0;
				added[i].next = i + 1;
			}
			// last slot
			added[i].refs = 0;
			added[i].slot_seq = 0;
			added[i].next = prev->next;

			added = &added[buffer->control->nslots];
			buffer->control->nslots += OMSBUFF_MEM_PAGE;
			buffer->known_slots = buffer->control->nslots;

			DEBUGLOGG("OMSSlots page added in local bufferpool (%u slots)",
				buffer->known_slots);
			break;
		default:
			return NULL;
			break;
	}

	prev->next = OMStoSlotPtr(buffer, added);

	return added;
}
