
#ifndef _SERV_PREFSH
#define _SERV_PREFSH

#include <config.h>

	/* Please note (1): PROCESS_COUNT must be >=1
	 */
	/* Please note (2):
	   MAX_CONNECTION must be an integral multiple of MAX_PROCESS
	 */
#define MAX_PROCESS	1	/*number of fork */
#define MAX_CONNECTION	100	/*rtsp connection */
#define ONE_FORK_MAX_CONNECTION ((int)(MAX_CONNECTION/MAX_PROCESS))	/*rtsp connection for one fork */

/* Moved to config.h. Now in FENICE_MAX_SESSION_DEFAULT
 * Was:
 * 
 * default max session after which i need to send a redirect
 * #define DEFAULT_MAX_SESSION 100
 *
*/

typedef enum _pref_id {
	// Preferences
	PREFS_ROOT,
	PREFS_TCP_PORT,
	PREFS_SCTP_PORT,
	PREFS_MAX_SESSION,
	PREFS_LOG,
	PREFS_HOSTNAME,
	// Insert other preferences before this one
	PREFS_LAST,
	// For prefs_use_default()
	PREFS_ALL
} pref_id;

	// Sync with the first preference in enum _pref_id
#define PREFS_FIRST PREFS_ROOT

typedef enum _pref_type {
	INTEGER,
	STRING
} pref_type;

typedef struct _pref_record {
	pref_id id;		// identifies the preference
	pref_type type;		// for selecting the right way to allocate and accessing data
	char tag[256];		// for storing the tag to be used when parsing the config file
	void *data;		// store the pointer to data
} pref_record;

#define SET_STRING_DATA(PREF_ID, PREF_DATA) \
		if (prefs[(PREF_ID)].data) free(prefs[(PREF_ID)].data); \
		prefs[(PREF_ID)].data = strdup(PREF_DATA);

#define SET_INTEGER_DATA(PREF_ID, PREF_DATA) \
		if (prefs[(PREF_ID)].data) free(prefs[(PREF_ID)].data); \
		prefs[(PREF_ID)].data = malloc(sizeof(int)); \
		*((int *) prefs[(PREF_ID)].data) = (PREF_DATA);

// Functions
void prefs_init(char *fileconf);
void *get_pref(pref_id id);
void prefs_use_default(pref_id id);

// Macros (for compatibility with old prefs.h
#define prefs_get_serv_root() ((char *) get_pref(PREFS_ROOT))
#define prefs_get_port() (*((int *) get_pref(PREFS_TCP_PORT)))
#define prefs_get_sctp_port() (*((int *) get_pref(PREFS_SCTP_PORT)))
#define prefs_get_max_session() (*((int *) get_pref(PREFS_MAX_SESSION)))
#define prefs_get_log() ((char *) get_pref(PREFS_LOG))
#define prefs_get_hostname() ((char *) get_pref(PREFS_HOSTNAME))

#endif
