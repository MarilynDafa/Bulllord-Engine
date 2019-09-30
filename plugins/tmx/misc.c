#include "misc.h"
#include "miniz/miniz.h"
#include "util.h"
Value* newUChar(unsigned char v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeBYTE;
	p->_field.byteVal = v;
	return p;
}
Value* newInt(int v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeINTEGER;
	p->_field.intVal = v;
	return p;
}
Value* newUInt(unsigned int v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeUNSIGNED;
	p->_field.unsignedVal = v;
	return p;
}
Value* newFloat(float v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeFLOAT;
	p->_field.floatVal = v;
	return p;
}
Value* newDouble(double v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeDOUBLE;
	p->_field.doubleVal = v;
	return p;
}
Value* newBool(bool v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeBOOLEAN;
	p->_field.boolVal = v;
	return p;
}
Value* newStr(const char* v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeSTRING;
	p->_field.strVal = strdcopy(v);
	return p;
}

Value* newVMap(BLDictionary* v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeMAP;
	if (v)
	{
		p->_field.mapVal = v;
	}
	else

		p->_field.mapVal = blGenDict();
	return p;
}
Value* newArray(float* v)
{
	Value* p = malloc(sizeof(Value));
	p->_type = TypeVECTOR;
	
		p->_field.arrayVal = v;
	return p;
}

void freeValueMap(BLDictionary* dct)
{
	FOREACH_DICT(Value*, iter, dct)
	{
		if (iter->_type == TypeSTRING)
		{
			StrFree(iter->_field.strVal);
		}
		else if (iter->_type == TypeVECTOR)
			free(iter->_field.arrayVal);
		else if (iter->_type == TypeMAP)
		{
			freeValueMap(iter->_field.mapVal);
		}
		free(iter);
	}
	blDeleteDict(dct);
}
char* strdcopy(const char* str)
{
	if (!str)
	{
		char* p = malloc(1);
		p[0] = 0;
		return p;
	}
	int len = strlen(str);
	char*p = malloc(len + 1);
	strcpy(p, str);
	p[len] = 0;
	return p;
}


unsigned int inflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int outLengthHint)
{
	unsigned int outLength = 0;
	int err = inflateMemoryWithHint2(in, inLength, out, &outLength, outLengthHint);

	if (err != MZ_OK || *out == NULL) {
		if (err == MZ_MEM_ERROR)
		{
			blDebugOutput("ZipUtils: Out of memory while decompressing map data!");
		}
		else
			if (err == MZ_VERSION_ERROR)
			{
				blDebugOutput("ZipUtils: Incompatible zlib version!");
			}
			else
				if (err == MZ_DATA_ERROR)
				{
					blDebugOutput("ZipUtils: Incorrect zlib compressed data!");
				}
				else
				{
					blDebugOutput("ZipUtils: Unknown error while decompressing map data!");
				}

		if (*out) {
			free(*out);
			*out = NULL;
		}
		outLength = 0;
	}

	return outLength;
}
int inflateMemoryWithHint2(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLengthHint)
{
	int err = MZ_OK;

	unsigned int bufferSize = outLengthHint;
	*out = (unsigned char*)malloc(bufferSize);

	mz_stream d_stream; /* decompression stream */
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;

	d_stream.next_in = in;
	d_stream.avail_in = (inLength);
	d_stream.next_out = *out;
	d_stream.avail_out = (bufferSize);

	/* window size to hold 256k */
	if ((err = mz_inflateInit2(&d_stream, 15)) != MZ_OK)
		return err;

	for (;;)
	{
		err = mz_inflate(&d_stream, MZ_NO_FLUSH);

		if (err == MZ_STREAM_END)
		{
			break;
		}

		switch (err)
		{
		case MZ_NEED_DICT:
			err = MZ_DATA_ERROR;
		case MZ_DATA_ERROR:
		case MZ_MEM_ERROR:
			mz_inflateEnd(&d_stream);
			return err;
		}

		// not enough memory ?
		if (err != MZ_STREAM_END)
		{
			*out = (unsigned char*)realloc(*out, bufferSize * 2);

			/* not enough memory, ouch */
			if (!*out)
			{
				blDebugOutput("ZipUtils: realloc failed");
				mz_inflateEnd(&d_stream);
				return MZ_MEM_ERROR;
			}

			d_stream.next_out = *out + bufferSize;
			d_stream.avail_out = (bufferSize);
			bufferSize *= 2;
		}
	}

	*outLength = bufferSize - d_stream.avail_out;
	err = mz_inflateEnd(&d_stream);
	return err;
}
unsigned int ezxml_attru(ezxml_t node, const char* att)
{
	const char* v = ezxml_attr(node, att);
	if (v)
		return strtoul(v, NULL, 10);
	else
		return 0;
}
int ezxml_attrd(ezxml_t node, const char* att)
{
	const char* v = ezxml_attr(node, att);
	if (v)
		return strtol(v, NULL, 10);
	else
		return 0;
}
float ezxml_attrf(ezxml_t node, const char* att)
{
	const char* v = ezxml_attr(node, att);
	if (v)
		return (float)strtod(v, NULL);
	else
		return 0.f;
}
bool ezxml_attrb(ezxml_t node, const char* att, bool def)
{
	const char* v = ezxml_attr(node, att);
	if (v)
	{
		if (v[0] == '0' || v[0] == 'f')
			return FALSE;
		else
			return TRUE;
	}
	else
		return def;
}