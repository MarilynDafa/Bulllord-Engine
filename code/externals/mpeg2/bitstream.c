#include "mpgconfig.h"

#include <inttypes.h>

#include "a52.h"
#include "a52_internal.h"
#include "bitstream.h"

#define BUFFER_SIZE 4096

void a52_bitstream_set_ptr (a52_state_t * state, uint8_t * buf)
{
    int align;

    align = (long)buf & 3;
    state->buffer_start = (uint32_t *) (buf - align);
    state->bits_left = 0;
    state->current_word = 0;
    bitstream_get (state, align * 8);
    bitstream_get_2 (state, 0);	/* pretend function is used - keep gcc happy */
}

static inline void bitstream_fill_current (a52_state_t * state)
{
    uint32_t tmp;

    tmp = *(state->buffer_start++);
    state->current_word = swab32 (tmp);
}

/*
 * The fast paths for _get is in the
 * bitstream.h header file so it can be inlined.
 *
 * The "bottom half" of this routine is suffixed _bh
 *
 * -ah
 */

uint32_t a52_bitstream_get_bh (a52_state_t * state, uint32_t num_bits)
{
    uint32_t result;

    num_bits -= state->bits_left;
    result = ((state->current_word << (32 - state->bits_left)) >>
	      (32 - state->bits_left));

    bitstream_fill_current (state);

    if (num_bits != 0)
	result = (result << num_bits) | (state->current_word >> (32 - num_bits));

    state->bits_left = 32 - num_bits;

    return result;
}

int32_t a52_bitstream_get_bh_2 (a52_state_t * state, uint32_t num_bits)
{
    int32_t result;

    num_bits -= state->bits_left;
    result = ((((int32_t)state->current_word) << (32 - state->bits_left)) >>
	      (32 - state->bits_left));

    bitstream_fill_current(state);

    if (num_bits != 0)
	result = (result << num_bits) | (state->current_word >> (32 - num_bits));
	
    state->bits_left = 32 - num_bits;

    return result;
}
