#ifndef _EN_XMALLOC_H_
#define _EN_XMALLOC_H_

#include <sys/types.h>

#define DEBUG_MALLOC

#ifndef DEBUG_MALLOC

#define xmalloc_degug_stats

#define xmalloc checking_malloc
#define xcalloc checking_calloc
#define xstrdup checking_strdup
#define xfree checking_free

void *checking_malloc(size_t);
void *checking_calloc(size_t, size_t);
void *checking_strdup(const char *);
void checking_free(void *);

#else

#define xmalloc(s) debugging_malloc(s, __FILE__, __LINE__)
#define xcalloc(n, s) debugging_calloc(n, s, __FILE__, __LINE__)
#define xstrdup(p) debugging_strdup(p, __FILE__, __LINE__)
#define xfree(p) debugging_free(p, __FILE__, __LINE__)

void *debugging_malloc(size_t, const char *, int);
void *debugging_strdup(const char *, const char *, int);
void *debugging_calloc(size_t, size_t, const char *, int);
void *debugging_free(void *, char *, int);

#endif

#define xnew(type) xmalloc(sizeof(type))
#define xnew0(type) xcalloc(1, sizeof(type))
#define xnew_array(size, type) xcalloc(size, sizeof(type))
#define SAFE_XFREE_NULL(p) do {if ((p) != NULL) xfree(p); p = NULL;} while(0)
#define SAVE_XFREE(p) do {xfree(p); p = NULL;} while(0)

void xmalloc_debug_stats(void);

#endif //FENICE_1_12_EN_XMALLOC_H
