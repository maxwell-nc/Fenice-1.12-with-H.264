
#include <math.h>
#include <fenice/bufferpool.h>

int OMSbuff_sync(OMSAggregate * aggr)
{
	double max_ts = 0;
	double curr_ts, next_ts;
	OMSAggregate *curr;

	for (curr = aggr; curr; curr = curr->next) {
		max_ts =
		    omsbuff_max(max_ts,
				curr->buffer->slots[curr->buffer->control->
						    valid_read_pos].timestamp);
	}

	for (curr = aggr; curr; curr = curr->next) {
		curr_ts =
		    curr->buffer->slots[curr->buffer->control->valid_read_pos].
		    timestamp;
		next_ts =
		    curr->buffer->slots[curr->buffer->
					slots[curr->buffer->control->
					      valid_read_pos].next].timestamp;
		while (fabs(max_ts - curr_ts) > fabs(max_ts - next_ts)) {
			curr->buffer->control->valid_read_pos =
			    curr->buffer->slots[curr->buffer->control->
						valid_read_pos].next;
			curr_ts =
			    curr->buffer->slots[curr->buffer->control->
						valid_read_pos].timestamp;
			next_ts =
			    curr->buffer->slots[curr->buffer->
						slots[curr->buffer->control->
						      valid_read_pos].next].
			    timestamp;
		}
	}

	return 0;
}
