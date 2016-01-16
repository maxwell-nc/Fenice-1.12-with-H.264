
#include <stdlib.h>
#include <fenice/mpeg.h>
#include <fenice/utils.h>

int free_MPV(void *stat)
{
	/*
	   static_MPEG_video *s;

	   s=(static_MPEG_video *) stat;
	   if(s==NULL)
	   return ERR_ALLOC;
	   free(s);
	 */
	free(stat);
	return ERR_NOERROR;
}
