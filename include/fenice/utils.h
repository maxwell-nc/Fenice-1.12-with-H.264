
#ifndef _UTILSH
#define _UTILSH

#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <math.h>
#include <fenice/mediainfo.h>
#include <fenice/types.h>

#define URL_ERROR			-1
#define URL_OK	    	    0
#define URL_BAD	    	    1

#define ERR_NOERROR			 0
#define ERR_GENERIC			-1
#define ERR_NOT_FOUND			-2
#define ERR_PARSE			-3
#define ERR_ALLOC			-4
#define ERR_INPUT_PARAM			-5
#define ERR_NOT_SD			-6
#define ERR_UNSUPPORTED_PT		-7
#define ERR_EOF				-8
#define ERR_FATAL			-9
#define ERR_CONNECTION_CLOSE		-10

	/* message header keywords */
#define HDR_CONTENTLENGTH "Content-Length"
#define HDR_ACCEPT "Accept"
#define HDR_ALLOW "Allow"
#define HDR_BLOCKSIZE "Blocksize"
#define HDR_CONTENTTYPE "Content-Type"
#define HDR_DATE "Date"
#define HDR_REQUIRE "Require"
#define HDR_TRANSPORTREQUIRE "Transport-Require"
#define HDR_SEQUENCENO "SequenceNo"
#define HDR_CSEQ "CSeq"
#define HDR_STREAM "Stream"
#define HDR_SESSION "Session"
#define HDR_TRANSPORT "Transport"
#define HDR_RANGE "Range"
#define HDR_USER_AGENT "User-Agent"

	/* rtsp methods */
#define RTSP_METHOD_MAXLEN 15
#define RTSP_METHOD_DESCRIBE "DESCRIBE"
#define RTSP_METHOD_ANNOUNCE "ANNOUNCE"
#define RTSP_METHOD_GET_PARAMETERS "GET_PARAMETERS"
#define RTSP_METHOD_OPTIONS "OPTIONS"
#define RTSP_METHOD_PAUSE "PAUSE"
#define RTSP_METHOD_PLAY "PLAY"
#define RTSP_METHOD_RECORD "RECORD"
#define RTSP_METHOD_REDIRECT "REDIRECT"
#define RTSP_METHOD_SETUP "SETUP"
#define RTSP_METHOD_SET_PARAMETER "SET_PARAMETER"
#define RTSP_METHOD_TEARDOWN "TEARDOWN"
	/*rtsp method tokens */
#define RTSP_ID_DESCRIBE 0
#define RTSP_ID_ANNOUNCE 1
#define RTSP_ID_GET_PARAMETERS 2
#define RTSP_ID_OPTIONS 3
#define RTSP_ID_PAUSE 4
#define RTSP_ID_PLAY 5
#define RTSP_ID_RECORD 6
#define RTSP_ID_REDIRECT 7
#define RTSP_ID_SETUP 8
#define RTSP_ID_SET_PARAMETER 9
#define RTSP_ID_TEARDOWN 10

	/* SD keywords */
#define SD_STREAM "STREAM"
#define SD_STREAM_END "STREAM_END"
#define SD_FILENAME "FILE_NAME"
#define SD_CLOCK_RATE "CLOCK_RATE"
#define SD_PAYLOAD_TYPE "PAYLOAD_TYPE"
#define SD_AUDIO_CHANNELS "AUDIO_CHANNELS"
#define SD_ENCODING_NAME "ENCODING_NAME"
#define SD_AGGREGATE "AGGREGATE"
#define SD_BIT_PER_SAMPLE "BIT_PER_SAMPLE"
#define SD_SAMPLE_RATE "SAMPLE_RATE"
#define SD_CODING_TYPE "CODING_TYPE"
#define SD_FRAME_LEN "FRAME_LEN"
#define SD_PKT_LEN "PKT_LEN"
#define SD_PRIORITY "PRIORITY"
#define SD_BITRATE "BITRATE"
#define SD_FRAME_RATE "FRAME_RATE"
#define SD_FORCE_FRAME_RATE "FORCE_FRAME_RATE"
#define SD_BYTE_PER_PCKT "BYTE_PER_PCKT"
#define SD_MEDIA_SOURCE "MEDIA_SOURCE"
#define SD_TWIN "TWIN"
#define SD_MULTICAST "MULTICAST"

	/*start CC */
#define SD_LICENSE "LICENSE"
#define SD_RDF "VERIFY"
#define SD_TITLE "TITLE"
#define SD_CREATOR "CREATOR"
	/*end CC */

	/* COMMAND KEYWORDS AND PORT */
	//#define COMMAND_PORT 3000
uint32 random32(int);
	// static uint32 md_32(char *string, int length); // shawill: what?
int parse_url(const char *url, char *server, size_t, unsigned short *port,
	      char *file_name, size_t);
char *alloc_path_name(char *base_path, char *file_path);
float NTP_time(time_t t);
int get_UTC_time(struct tm *t, char *b);
int is_supported_url(char *p);

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif
#define lround(x) (x - 	floor(x) < 0.5) ? floor(x): ceil(x)

#endif
