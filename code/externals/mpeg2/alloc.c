#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "mpeg2.h"

static void * (* malloc_hook) (unsigned size, mpeg2_alloc_t reason) = NULL;
static int (* free_hook) (void * buf) = NULL;

void * mpeg2_malloc (unsigned size, mpeg2_alloc_t reason)
{
    char * buf;

    if (malloc_hook) {
	buf = (char *) malloc_hook (size, reason);
	if (buf)
	    return buf;
    }

    if (size) {
	buf = (char *) malloc (size + 63 + sizeof (void **));
	if (buf) {
	    char * align_buf;

	    align_buf = buf + 63 + sizeof (void **);
	    align_buf -= (long)align_buf & 63;
	    *(((void **)align_buf) - 1) = buf;
	    return align_buf;
	}
    }
    return NULL;
}

void * _Malloc_hook(unsigned size, mpeg2_alloc_t reason)
{
	void * buf;

	/*
	* Invalid streams can refer to fbufs that have not been
	* initialized yet. For example the stream could start with a
	* picture type onther than I. Or it could have a B picture before
	* it gets two reference frames. Or, some slices could be missing.
	*
	* Consequently, the output depends on the content 2 output
	* buffers have when the sequence begins. In release builds, this
	* does not matter (garbage in, garbage out), but in test code, we
	* always zero all our output buffers to:
	* - make our test produce deterministic outputs
	* - hint checkergcc that it is fine to read from all our output
	*   buffers at any time
	*/
	if ((int)reason < 0) {
		return NULL;
	}
	buf = mpeg2_malloc(size, (mpeg2_alloc_t)-1);
	if (buf && (reason == MPEG2_ALLOC_YUV || reason == MPEG2_ALLOC_CONVERTED))
		memset(buf, 0, size);
	return buf;
}

void mpeg2_free (void * buf)
{
    if (free_hook && free_hook (buf))
	return;

    if (buf)
	free (*(((void **)buf) - 1));
}

void mpeg2_malloc_hooks (void * alloc_func (unsigned, mpeg2_alloc_t),
			 int free_func (void *))
{
    malloc_hook = alloc_func;
    free_hook = free_func;
}
