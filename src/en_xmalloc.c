#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int malloc_count = 0;
static int free_count = 0;
static long long int malloc_size = 0;
static long long int free_size = 0;
static pthread_mutex_t s_regptr_mutex = PTHREAD_MUTEX_INITIALIZER;

#define SZ          5000000
#define PTR_FORMAT(p) 2*sizeof(void *), (unsigned long) (p)

static struct {
    const void *ptr;
    const char *file;
    int line;
    size_t size;
} malloc_table[SZ];

static void memfatal(const char *exec_name, const char *context,
    long attempted_size)
{
    fprintf(stderr,
    "%s:%s: Failed to allocate %ld bytes, memory exhausted.\n",
    exec_name, context, attempted_size);

    exit(1);
}

#ifdef DEBUG_MALLOC
#define STATIC_IF_DEBUG static
#else
#define STATIC_IF_DEBUG
#endif

STATIC_IF_DEBUG void* checking_malloc(size_t size)
{
    void *ptr = malloc(size);

    if (NULL == ptr) {
        memfatal(__FUNCTION__, "malloc", size);
    }

    return ptr;
}

STATIC_IF_DEBUG void* checking_calloc(size_t num,
                                      size_t size)
{
    void *ptr = calloc(num, size);
    if (!ptr) {
        memfatal(__FUNCTION__, "calloc", size);
    }

    return ptr;
}

STATIC_IF_DEBUG char* checking_strdup(const char *s)
{
    char *copy;

    copy = strdup(s);
    if (NULL == copy) {
        memfatal(__FUNCTION__, "strdup", 1 + strlen(s));
    }

    return copy;
}

STATIC_IF_DEBUG void checking_free(void *ptr)
{
    assert(ptr != NULL);

    free(ptr);
}

static unsigned long hash_pointer(const void *ptr)
{
    unsigned long key = (unsigned long)ptr;

    key += (key << 12);
    key ^= (key >> 22);
    key += (key << 4);
    key ^= (key >> 9);
    key += (key << 10);
    key ^= (key >> 2);
    key += (key << 7);
    key ^= (key >> 12);

    return key;
}

static inline int ptr_position(const void *ptr)
{
    int i = hash_pointer(ptr) % SZ;

    for (; NULL != malloc_table[i].ptr; i = (i + 1) % SZ) {
        if (malloc_table[i].ptr == ptr) {
            return i;
        }
    }

    return i;
}

void xmalloc_debug_stats(void)
{
#ifdef DEBUG_MALLOC
    int i;
    fprintf(stderr, "\nMalloc Size(bytes): \t%lld\n"
                    "Free size(bytes):\t%lld\n"
                    "Balance size(bytes):\t%lld\n"
                    "Malloc count:\t%d\n"
                    "Free count:\t%d\n"
                    "Balance count:\t%d\n\n",
                    malloc_size,
                    free_size,
                    malloc_size - free_size,
                    malloc_count,
                    free_count,
                    malloc_count - free_count);
    for (i = 0; i < SZ; i++) {
        if (malloc_table[i].ptr != NULL) {
            fprintf(stderr, "leak(%x%0*lx size :%u malloc at %s:%d\n",
                PTR_FORMAT(malloc_table[i].ptr), malloc_table[i].size,
                malloc_table[i].file, malloc_table[i].line);
        }
    }
#endif
}


static void register_ptr(
        const void *ptr,
        size_t size,
        const char *file,
        int line)
{
    int i;

    if (malloc_count - free_count > SZ) {
        fprintf(stderr, "Increase SZ to a larger value and recompile.\n");
        fflush(stderr);

        xmalloc_debug_stats();
        abort();
    }

    i = ptr_position(ptr);
    malloc_table[i].ptr = ptr;
    malloc_table[i].file = file;
    malloc_table[i].line = line;
    malloc_table[i].size = size;
}

static int unregister_ptr(void *ptr)
{
    int i = ptr_position(ptr);

    if (NULL == malloc_table[i].ptr) {
        return 0;
    }

    malloc_table[i].ptr = NULL;
    free_size += malloc_table[i].size;

    for (i = (i + 1) % SZ; NULL != malloc_table[i].ptr; i = (i + 1) % SZ) {
        const void *ptr2 = malloc_table[i].ptr;

        int j = hash_pointer(ptr2) % SZ;
        for (; malloc_table[j].ptr != NULL; j = (j + 1) % SZ)
            if (ptr2 == malloc_table[i].ptr)
                goto count_outer;
        malloc_table[j] = malloc_table[i];
        malloc_table[i].ptr = NULL;
        malloc_table[i].size = 0;
        count_outer:
            ;
    }
    return 1;
}

void* debugging_malloc(
        size_t size,
        const char *source_file,
        int source_line)
{
    void *ptr = checking_malloc(size);

    pthread_mutex_lock(&s_regptr_mutex);
    ++malloc_count;
    malloc_size += size;
    register_ptr(ptr, size, source_file, source_line);
    pthread_mutex_unlock(&s_regptr_mutex);

    return ptr;
}

void *debugging_calloc(size_t numb,
                       size_t size,
                       const char *source_file,
                       int source_line)
{
    void *ptr = checking_calloc(numb, size);

    pthread_mutex_lock(&s_regptr_mutex);
    ++malloc_count;
    malloc_size += (numb * size);
    register_ptr(ptr, numb*size, source_file, source_line);
    pthread_mutex_unlock(&s_regptr_mutex);

    return ptr;
}

char *debugging_strdup(
        const char *s,
        const char *source_file,
        int source_line)
{
    char *copy = checking_strdup(s);
    size_t size = strlen(copy) + 1;

    pthread_mutex_lock(&s_regptr_mutex);
    ++malloc_count;
    malloc_size += size;
    register_ptr(copy, size, source_file, source_line);
    pthread_mutex_unlock(&s_regptr_mutex);

    return copy;
}

void debugging_free(void *ptr,
                    const char *source_file,
                    int source_line)
{
    if (NULL == ptr) {
        fprintf(stderr, "xfree(NULL) at %s:%d\n",
                source_file, source_line);
        fflush(stderr);
        abort();
    }

    pthread_mutex_lock(&s_regptr_mutex);

    if (!unregister_ptr(ptr)) {
        fprintf(stderr, "Bad xfree() at %s:%d\n",
                source_file, source_line);

        pthread_mutex_unlock(&s_regptr_mutex);

        abort();
    }

    ++free_count;

    pthread_mutex_unlock(&s_regptr_mutex);

    checking_free(ptr);
}

