
#include <fenice/intnet.h>

int downgrade_L16(RTP_session * changing_session)
{
	return priority_decrease(changing_session);
}
