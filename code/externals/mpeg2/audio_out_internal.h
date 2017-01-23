#ifdef LIBA52_DOUBLE
	typedef float convert_t;
#else
	typedef sample_t convert_t;
#endif

#ifdef LIBA52_FIXED
#define CONVERT_LEVEL (1 << 26)
#define CONVERT_BIAS 0
#else
#define CONVERT_LEVEL 1
#define CONVERT_BIAS 384
#endif

	void convert2s16_1(convert_t * f, int16_t * s16);
	void convert2s16_2(convert_t * f, int16_t * s16);
	void convert2s16_3(convert_t * f, int16_t * s16);
	void convert2s16_4(convert_t * f, int16_t * s16);
	void convert2s16_5(convert_t * f, int16_t * s16);
	int channels_multi(int flags);
	void convert2s16_multi(convert_t * f, int16_t * s16, int flags);
	void convert2s16_wav(convert_t * f, int16_t * s16, int flags);
	void s16_swap(int16_t * s16, int channels);

#ifdef WORDS_BIGENDIAN
#define s16_LE(s16,channels) s16_swap (s16, channels)
#define s16_BE(s16,channels) do {} while (0)
#else
#define s16_LE(s16,channels) do {} while (0)
#define s16_BE(s16,channels) s16_swap (s16, channels)
#endif

