#include "mpeg2.h"
#include "a52.h"
#include "audio_out.h"
#include "video_out.h"
#include <string.h>
static void a52_decode_data(uint8_t * start, uint8_t * end, ao_instance_t* output, a52_state_t** audiostate)
{
	static uint8_t buf[3840];
	static uint8_t * bufptr = buf;
	static uint8_t * bufpos = buf + 7;

	/*
	* sample_rate and flags are static because this routine could
	* exit between the a52_syncinfo() and the ao_setup(), and we want
	* to have the same values when we get back !
	*/

	static int sample_rate;
	static int flags;
	int bit_rate;
	int len;

	while (1) {
		len = end - start;
		if (!len)
			break;
		if (len > bufpos - bufptr)
			len = bufpos - bufptr;
		memcpy(bufptr, start, len);
		bufptr += len;
		start += len;
		if (bufptr == bufpos) {
			if (bufpos == buf + 7) {
				int length;
				length = a52_syncinfo(buf, &flags, &sample_rate, &bit_rate);
				if (!length) {
					for (bufptr = buf; bufptr < buf + 6; bufptr++)
						bufptr[0] = bufptr[1];
					continue;
				}
				bufpos = buf + length;
			}
			else {
				level_t level;
				sample_t bias;
				int i;
				if (output->setup(output, sample_rate, &flags, &level, &bias))
					goto error;
				flags |= A52_ADJUST_LEVEL;
				level = (level_t)(level);
				if (a52_frame((*audiostate), buf, &flags, &level, bias))
					goto error;
				for (i = 0; i < 6; i++) {
					if (a52_block(*audiostate))
						goto error;
					if (output->play(output, flags, a52_samples(*audiostate)))
						goto error;
				}
				bufptr = buf;
				bufpos = buf + 7;
				continue;
			error:
				bufptr = buf;
				bufpos = buf + 7;
			}
		}
	}
}
static void decode_mpeg2(unsigned char * current, unsigned char * end, mpeg2dec_t** mpeg2dec, mpeg2_info_t** info, vo_instance_t * output)
{
	static int total_offset = 0;
	mpeg2_state_t state;
	vo_setup_result_t setup_result;

	mpeg2_buffer((*mpeg2dec), current, end);
	total_offset += end - current;

	*info = (mpeg2_info_t*)mpeg2_info((*mpeg2dec));
	while (1) {
		state = mpeg2_parse((*mpeg2dec));
		switch (state) {
		case STATE_BUFFER:
			return;
		case STATE_SEQUENCE:
			/* might set nb fbuf, convert format, stride */
			/* might set fbufs */
			if (output->setup(output, (*info)->sequence->width,
				(*info)->sequence->height,
				(*info)->sequence->chroma_width,
				(*info)->sequence->chroma_height, &setup_result)) {
				return;
			}
			if (setup_result.convert &&	mpeg2_convert((*mpeg2dec), setup_result.convert, NULL)) {
				return;
			}
			if (output->set_fbuf) {
				uint8_t * buf[3];
				void * id;

				mpeg2_custom_fbuf((*mpeg2dec), 1);
				output->set_fbuf(output, buf, &id);
				mpeg2_set_buf((*mpeg2dec), buf, id);
				output->set_fbuf(output, buf, &id);
				mpeg2_set_buf((*mpeg2dec), buf, id);
			}
			else if (output->setup_fbuf) {
				uint8_t * buf[3];
				void * id;

				output->setup_fbuf(output, buf, &id);
				mpeg2_set_buf((*mpeg2dec), buf, id);
				output->setup_fbuf(output, buf, &id);
				mpeg2_set_buf((*mpeg2dec), buf, id);
				output->setup_fbuf(output, buf, &id);
				mpeg2_set_buf((*mpeg2dec), buf, id);
			}
			mpeg2_skip((*mpeg2dec), (output->draw == NULL));
			break;
		case STATE_PICTURE:
			/* might skip */
			/* might set fbuf */
			if (output->set_fbuf) {
				uint8_t * buf[3];
				void * id;

				output->set_fbuf(output, buf, &id);
				mpeg2_set_buf((*mpeg2dec), buf, id);
			}
			if (output->start_fbuf)
				output->start_fbuf(output, (*info)->current_fbuf->buf,
				(*info)->current_fbuf->id);
			break;
		case STATE_SLICE:
		case STATE_END:
		case STATE_INVALID_END:
			/* draw current picture */
			/* might free frame buffer */
			if ((*info)->display_fbuf) {
				if (output->draw)
					output->draw(output, (*info)->display_fbuf->buf,
					(*info)->display_fbuf->id);
			}
			if (output->discard && (*info)->discard_fbuf)
				output->discard(output, (*info)->discard_fbuf->buf,
				(*info)->discard_fbuf->id);
			break;
		default:
			break;
		}
	}
}

#define DEMUX_PAYLOAD_START 1

#define NEEDBYTES(x)						\
    do {							\
	int missing;						\
								\
	missing = (x) - bytes;					\
	if (missing > 0) {					\
	    if (header == head_buf) {				\
		if (missing <= end - buf) {			\
		    memcpy (header + bytes, buf, missing);	\
		    buf += missing;				\
		    bytes = (x);				\
		} else {					\
		    memcpy (header + bytes, buf, end - buf);	\
		    state_bytes = bytes + end - buf;		\
		    return 0;					\
		}						\
	    } else {						\
		memcpy (head_buf, header, bytes);		\
		state = DEMUX_HEADER;				\
		state_bytes = bytes;				\
		return 0;					\
	    }							\
	}							\
    } while (0)

#define DONEBYTES(x)		\
    do {			\
	if (header != head_buf)	\
	    buf = header + (x);	\
    } while (0)

#define DEMUX_HEADER 0
#define DEMUX_DATA 1
#define DEMUX_SKIP 2
/*
int demux_v(unsigned char * buf, unsigned char * end, int flags, mpeg2dec_t** mpeg2dec, mpeg2_info_t** info, vo_instance_t * output)
{
    unsigned char* _buf, *_end;
    const int demux_pid = 0;
    const int demux_track = 0xe0;
    static int mpeg1_skip_table[16] = {
        0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    
    static int state = DEMUX_SKIP;
    static int state_bytes = 0;
    static uint8_t head_buf[264];
    
    uint8_t * header;
    int bytes;
    int len;
    
    
    if (flags & DEMUX_PAYLOAD_START)
        goto payload_start;
    switch (state) {
        case DEMUX_HEADER:
            if (state_bytes > 0) {
                header = head_buf;
                bytes = state_bytes;
                goto continue_header;
            }
            break;
        case DEMUX_DATA:
            if (demux_pid || (state_bytes > end - buf)) {
                _buf = buf; _end = end;
                decode_mpeg2(buf, end, mpeg2dec, info, output);
                //a52_decode_data(_buf, _end , aoutput, austate);
                state_bytes -= end - buf;
                return 0;
            }
            _buf = buf;_end = end;
            decode_mpeg2(buf, buf + state_bytes, mpeg2dec, info, output);
            //a52_decode_data(_buf, _buf + state_bytes, aoutput, austate);
            buf += state_bytes;
            break;
        case DEMUX_SKIP:
            if (demux_pid || (state_bytes > end - buf)) {
                state_bytes -= end - buf;
                return 0;
            }
            buf += state_bytes;
            break;
    }
    
    while (1) {
        if (demux_pid) {
            state = DEMUX_SKIP;
            return 0;
        }
    payload_start:
        header = buf;
        bytes = end - buf;
    continue_header:
        NEEDBYTES(4);
        if (header[0] || header[1] || (header[2] != 1)) {
            if (demux_pid) {
                state = DEMUX_SKIP;
                return 0;
            }
            else if (header != head_buf) {
                buf++;
                goto payload_start;
            }
            else {
                header[0] = header[1];
                header[1] = header[2];
                header[2] = header[3];
                bytes = 3;
                goto continue_header;
            }
        }
        if (demux_pid) {
            if ((header[3] >= 0xe0) && (header[3] <= 0xef))
                goto pes;
            return 0;
        }
        switch (header[3]) {
            case 0xb9:
                return 1;
            case 0xba:
                NEEDBYTES(5);
                if ((header[4] & 0xc0) == 0x40) {
                    NEEDBYTES(14);
                    len = 14 + (header[13] & 7);
                    NEEDBYTES(len);
                    DONEBYTES(len);
                }
                else if ((header[4] & 0xf0) == 0x20) {
                    NEEDBYTES(12);
                    DONEBYTES(12);
                }
                else {
                    DONEBYTES(5);
                }
                break;
            default:
                if (header[3] == demux_track) {
                pes:
                    NEEDBYTES(7);
                    if ((header[6] & 0xc0) == 0x80) {
                        NEEDBYTES(9);
                        len = 9 + header[8];
                        NEEDBYTES(len);
                        if (header[7] & 0x80) {
                            uint32_t pts, dts;
                            
                            pts = (((header[9] >> 1) << 30) |
                                   (header[10] << 22) | ((header[11] >> 1) << 15) |
                                   (header[12] << 7) | (header[13] >> 1));
                            dts = (!(header[7] & 0x40) ? pts :
                                   (uint32_t)(((header[14] >> 1) << 30) |
                                              (header[15] << 22) |
                                              ((header[16] >> 1) << 15) |
                                              (header[17] << 7) | (header[18] >> 1)));
                            mpeg2_tag_picture((*mpeg2dec), pts, dts);
                        }
                    }
                    else {
                        int len_skip;
                        uint8_t * ptsbuf;
                        
                        len = 7;
                        while (header[len - 1] == 0xff) {
                            len++;
                            NEEDBYTES(len);
                            if (len > 23) {
                                break;
                            }
                        }
                        if ((header[len - 1] & 0xc0) == 0x40) {
                            len += 2;
                            NEEDBYTES(len);
                        }
                        len_skip = len;
                        len += mpeg1_skip_table[header[len - 1] >> 4];
                        NEEDBYTES(len);
                        ptsbuf = header + len_skip;
                        if ((ptsbuf[-1] & 0xe0) == 0x20) {
                            uint32_t pts, dts;
                            
                            pts = (((ptsbuf[-1] >> 1) << 30) |
                                   (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
                                   (ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
                            dts = (((ptsbuf[-1] & 0xf0) != 0x30) ? pts :
                                   (uint32_t)(((ptsbuf[4] >> 1) << 30) |
                                              (ptsbuf[5] << 22) | ((ptsbuf[6] >> 1) << 15) |
                                              (ptsbuf[7] << 7) | (ptsbuf[18] >> 1)));
                            mpeg2_tag_picture((*mpeg2dec), pts, dts);
                        }
                    }
                    DONEBYTES(len);
                    bytes = 6 + (header[4] << 8) + header[5] - len;
                    if (demux_pid || (bytes > end - buf)) {
                        _buf = buf; _end = end;
                        decode_mpeg2(buf, end, mpeg2dec, info, output);
                        //a52_decode_data(_buf, _end, aoutput, austate);
                        state = DEMUX_DATA;
                        state_bytes = bytes - (end - buf);
                        return 0;
                    }
                    else if (bytes > 0) {
                        _buf = buf; _end = end;
                        decode_mpeg2(buf, buf + bytes, mpeg2dec, info, output);
                        //a52_decode_data(_buf, _buf + bytes, aoutput, austate);
                        buf += bytes;
                    }
                }
                else if (header[3] < 0xb9) {
                    DONEBYTES(4);
                }
                else {
                    NEEDBYTES(6);
                    DONEBYTES(6);
                    bytes = (header[4] << 8) + header[5];
                    if (bytes > end - buf) {
                        state = DEMUX_SKIP;
                        state_bytes = bytes - (end - buf);
                        return 0;
                    }
                    buf += bytes;
                }
        }
    }
}

int demux_a(uint8_t * buf, uint8_t * end, int flags, a52_state_t** mepg2dec, ao_instance_t* output)
{
    const int demux_pid = 0;
    const int demux_track = 0x80;
    static int mpeg1_skip_table[16] = {
        0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    static int state = DEMUX_SKIP;
    static int state_bytes = 0;
    static uint8_t head_buf[264];
    
    uint8_t * header;
    int bytes;
    int len;
    
    
    if (flags & DEMUX_PAYLOAD_START)
        goto payload_start;
    switch (state) {
        case DEMUX_HEADER:
            if (state_bytes > 0) {
                header = head_buf;
                bytes = state_bytes;
                goto continue_header;
            }
            break;
        case DEMUX_DATA:
            if (demux_pid || (state_bytes > end - buf)) {
                a52_decode_data(buf, end, output, mepg2dec);
                state_bytes -= end - buf;
                return 0;
            }
            a52_decode_data(buf, buf + state_bytes, output, mepg2dec);
            buf += state_bytes;
            break;
        case DEMUX_SKIP:
            if (demux_pid || (state_bytes > end - buf)) {
                state_bytes -= end - buf;
                return 0;
            }
            buf += state_bytes;
            break;
    }
    
    while (1) {
        if (demux_pid) {
            state = DEMUX_SKIP;
            return 0;
        }
    payload_start:
        header = buf;
        bytes = end - buf;
    continue_header:
        NEEDBYTES(4);
        if (header[0] || header[1] || (header[2] != 1)) {
            if (demux_pid) {
                state = DEMUX_SKIP;
                return 0;
            }
            else if (header != head_buf) {
                buf++;
                goto payload_start;
            }
            else {
                header[0] = header[1];
                header[1] = header[2];
                header[2] = header[3];
                bytes = 3;
                goto continue_header;
            }
        }
        if (demux_pid ) {
            if (header[3] != 0xbd) {
                return -1;
            }
            NEEDBYTES(9);
            if ((header[6] & 0xc0) != 0x80) {
                return -1;
            }
            len = 9 + header[8];
            NEEDBYTES(len);
            DONEBYTES(len);
            bytes = 6 + (header[4] << 8) + header[5] - len;
            if (bytes > end - buf) {
                a52_decode_data(buf, end, output, mepg2dec);
                state = DEMUX_DATA;
                state_bytes = bytes - (end - buf);
                return 0;
            }
            else if (bytes > 0) {
                a52_decode_data(buf, buf + bytes, output, mepg2dec);
                buf += bytes;
            }
        }
        else switch (he
                return 1;
            case 0xba:
                NEEDBYTES(5);
                if ((header[4] & 0xc0) == 0x40) {
                    NEEDBYTES(14);
                    len = 14 + (header[13] & 7);
                    NEEDBYTES(len);
                    DONEBYTES(len);
                }
                else if ((header[4] & 0xf0) == 0x20) {
                    NEEDBYTES(12);
                    DONEBYTES(12);
                }
                else {
                    DONEBYTES(5);
                }
                break;
            case 0xbd:
                NEEDBYTES(7);
                if ((header[6] & 0xc0) == 0x80) {
                    NEEDBYTES(9);
                    len = 10 + header[8];
                    NEEDBYTES(len);
                }
                else {
                    len = 7;
                    while ((header - 1)[len] == 0xff) {
                        len++;
                        NEEDBYTES(len);
                        if (len == 23) {
                            return -1;
                        }
                    }
                    if (((header - 1)[len] & 0xc0) == 0x40) {
                        len += 2;
                        NEEDBYTES(len);
                    }
                    len += mpeg1_skip_table[(header - 1)[len] >> 4] + 1;
                    NEEDBYTES(len);
                }
                if ((header - 1)[len] != demux_track) {
                    DONEBYTES(len);
                    bytes = 6 + (header[4] << 8) + header[5] - len;
                    if (bytes <= 0)
                        continue;
                    goto skip;
                }
                len += 3;
                NEEDBYTES(len);
                DONEBYTES(len);
                bytes = 6 + (header[4] << 8) + header[5] - len;
                if (bytes > end - buf) {
                    a52_decode_data(buf, end, output, mepg2dec);
                    state = DEMUX_DATA;
                    state_bytes = bytes - (end - buf);
                    return 0;
                }
                else if (bytes > 0) {
                    a52_decode_data(buf, buf + bytes, output, mepg2dec);
                    buf += bytes;
                }
                break;
            default:
                if (header[3] < 0xb9) {
                    return -1;
                }
                else {
                    NEEDBYTES(6);
                    DONEBYTES(6);
                    bytes = (header[4] << 8) + header[5];
                skip:
                    if (bytes > end - buf) {
                        state = DEMUX_SKIP;
                        state_bytes = bytes - (end - buf);
                        return 0;
                    }
                    buf += bytes;
                }
        }
    }
}
*/

int demux(unsigned char * buf, unsigned char * end, int flags, mpeg2dec_t** mpeg2dec, mpeg2_info_t** info, vo_instance_t * output,  a52_state_t** a52dec, ao_instance_t* aoutput)
{
    unsigned char* _buf, *_end;
    const int demux_pid = 0;
    const int demux_tracka = 0x80;
    const int demux_track = 0xe0;
    static int mpeg1_skip_table[16] = {
        0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    
    static int state = DEMUX_SKIP;
    static int state_bytes = 0;
    static uint8_t head_buf[264];
    
    uint8_t * header;
    int bytes;
    int len;
    
    
    if (flags & DEMUX_PAYLOAD_START)
        goto payload_start;
    switch (state) {
        case DEMUX_HEADER:
            if (state_bytes > 0) {
                header = head_buf;
                bytes = state_bytes;
                goto continue_header;
            }
            break;
        case DEMUX_DATA:
            if (demux_pid || (state_bytes > end - buf)) {
                _buf = buf; _end = end;
                decode_mpeg2(buf, end, mpeg2dec, info, output);
                a52_decode_data(_buf, _end , aoutput, a52dec);
                state_bytes -= end - buf;
                return 0;
            }
            _buf = buf;_end = end;
            decode_mpeg2(buf, buf + state_bytes, mpeg2dec, info, output);
            a52_decode_data(_buf, _buf + state_bytes, aoutput, a52dec);
            buf += state_bytes;
            break;
        case DEMUX_SKIP:
            if (demux_pid || (state_bytes > end - buf)) {
                state_bytes -= end - buf;
                return 0;
            }
            buf += state_bytes;
            break;
    }
    
    while (1) {
        if (demux_pid) {
            state = DEMUX_SKIP;
            return 0;
        }
    payload_start:
        header = buf;
        bytes = end - buf;
    continue_header:
        NEEDBYTES(4);
        if (header[0] || header[1] || (header[2] != 1)) {
            if (demux_pid) {
                state = DEMUX_SKIP;
                return 0;
            }
            else if (header != head_buf) {
                buf++;
                goto payload_start;
            }
            else {
                header[0] = header[1];
                header[1] = header[2];
                header[2] = header[3];
                bytes = 3;
                goto continue_header;
            }
        }
        if (demux_pid) {
            if ((header[3] >= 0xe0) && (header[3] <= 0xef))
                goto pes;
            if (header[3] != 0xbd) {
                return -1;
            }
            NEEDBYTES(9);
            if ((header[6] & 0xc0) != 0x80) {	/* not mpeg2 */
                return -1;
            }
            len = 9 + header[8];
            NEEDBYTES(len);
            DONEBYTES(len);
            bytes = 6 + (header[4] << 8) + header[5] - len;
            if (bytes > end - buf) {
                a52_decode_data(buf, end, aoutput, a52dec);/////fuck
                state = DEMUX_DATA;
                state_bytes = bytes - (end - buf);
                return 0;
            }
            else if (bytes > 0) {
                a52_decode_data(buf, buf + bytes, aoutput, a52dec);//////fuck
                buf += bytes;
            }
        }
        switch (header[3]) {
            case 0xb9:	/* program end code */
                /* DONEBYTES (4); */
                /* break;         */
                return 1;
            case 0xba:	/* pack header */
                NEEDBYTES(5);
                if ((header[4] & 0xc0) == 0x40) {	/* mpeg2 */
                    NEEDBYTES(14);
                    len = 14 + (header[13] & 7);
                    NEEDBYTES(len);
                    DONEBYTES(len);
                    /* header points to the mpeg2 pack header */
                }
                else if ((header[4] & 0xf0) == 0x20) {	/* mpeg1 */
                    NEEDBYTES(12);
                    DONEBYTES(12);
                    /* header points to the mpeg1 pack header */
                }
                else {
                    DONEBYTES(5);
                }
                break;
                
            case 0xbd:	/* private stream 1 */
                NEEDBYTES(7);
                if ((header[6] & 0xc0) == 0x80) {	/* mpeg2 */
                    NEEDBYTES(9);
                    len = 10 + header[8];
                    NEEDBYTES(len);
                    /* header points to the mpeg2 pes header */
                }
                else {	/* mpeg1 */
                    len = 7;
                    while ((header - 1)[len] == 0xff) {
                        len++;
                        NEEDBYTES(len);
                        if (len == 23) {
                            return -1;
                        }
                    }
                    if (((header - 1)[len] & 0xc0) == 0x40) {
                        len += 2;
                        NEEDBYTES(len);
                    }
                    len += mpeg1_skip_table[(header - 1)[len] >> 4] + 1;
                    NEEDBYTES(len);
                    /* header points to the mpeg1 pes header */
                }
                if ((header - 1)[len] != 0x80) {//////ffffuck
                    DONEBYTES(len);
                    bytes = 6 + (header[4] << 8) + header[5] - len;
                    if (bytes <= 0)
                        continue;
                    goto skip;
                }
                len += 3;
                NEEDBYTES(len);
                DONEBYTES(len);
                bytes = 6 + (header[4] << 8) + header[5] - len;
                if (bytes > end - buf) {
                    a52_decode_data(buf, end, aoutput, a52dec);/////fuck
                    state = DEMUX_DATA;
                    state_bytes = bytes - (end - buf);
                    return 0;
                }
                else if (bytes > 0) {
                    a52_decode_data(buf, buf + bytes, aoutput, a52dec);/////fuck
                    buf += bytes;
                }
                break;
            default:
                if (header[3] == demux_track) {
                pes:
                    NEEDBYTES(7);
                    if ((header[6] & 0xc0) == 0x80) {	/* mpeg2 */
                        NEEDBYTES(9);
                        len = 9 + header[8];
                        NEEDBYTES(len);
                        /* header points to the mpeg2 pes header */
                        if (header[7] & 0x80) {
                            uint32_t pts, dts;
                            
                            pts = (((header[9] >> 1) << 30) |
                                   (header[10] << 22) | ((header[11] >> 1) << 15) |
                                   (header[12] << 7) | (header[13] >> 1));
                            dts = (!(header[7] & 0x40) ? pts :
                                   (uint32_t)(((header[14] >> 1) << 30) |
                                              (header[15] << 22) |
                                              ((header[16] >> 1) << 15) |
                                              (header[17] << 7) | (header[18] >> 1)));
                            mpeg2_tag_picture((*mpeg2dec), pts, dts);
                        }
                    }
                    else {	/* mpeg1 */
                        int len_skip;
                        uint8_t * ptsbuf;
                        
                        len = 7;
                        while (header[len - 1] == 0xff) {
                            len++;
                            NEEDBYTES(len);
                            if (len > 23) {
                                break;
                            }
                        }
                        if ((header[len - 1] & 0xc0) == 0x40) {
                            len += 2;
                            NEEDBYTES(len);
                        }
                        len_skip = len;
                        len += mpeg1_skip_table[header[len - 1] >> 4];
                        NEEDBYTES(len);
                        /* header points to the mpeg1 pes header */
                        ptsbuf = header + len_skip;
                        if ((ptsbuf[-1] & 0xe0) == 0x20) {
                            uint32_t pts, dts;
                            
                            pts = (((ptsbuf[-1] >> 1) << 30) |
                                   (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
                                   (ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
                            dts = (((ptsbuf[-1] & 0xf0) != 0x30) ? pts :
                                   (uint32_t)(((ptsbuf[4] >> 1) << 30) |
                                              (ptsbuf[5] << 22) | ((ptsbuf[6] >> 1) << 15) |
                                              (ptsbuf[7] << 7) | (ptsbuf[18] >> 1)));
                            mpeg2_tag_picture((*mpeg2dec), pts, dts);
                        }
                    }
                    DONEBYTES(len);
                    bytes = 6 + (header[4] << 8) + header[5] - len;
                    if (demux_pid || (bytes > end - buf)) {
                        _buf = buf; _end = end;
                        decode_mpeg2(buf, end, mpeg2dec, info, output);
                        //a52_decode_data(_buf, _end, aoutput, austate);
                        state = DEMUX_DATA;
                        state_bytes = bytes - (end - buf);
                        return 0;
                    }
                    else if (bytes > 0) {
                        _buf = buf; _end = end;
                        decode_mpeg2(buf, buf + bytes, mpeg2dec, info, output);
                        //a52_decode_data(_buf, _buf + bytes, aoutput, austate);
                        buf += bytes;
                    }
                }
                else if (header[3] < 0xb9) {
                    DONEBYTES(4);
                }
                else {
                    NEEDBYTES(6);
                    DONEBYTES(6);
                    bytes = (header[4] << 8) + header[5];
                skip:
                    if (bytes > end - buf) {
                        state = DEMUX_SKIP;
                        state_bytes = bytes - (end - buf);
                        return 0;
                    }
                    buf += bytes;
                }
        }
    }
    return 1;
}
