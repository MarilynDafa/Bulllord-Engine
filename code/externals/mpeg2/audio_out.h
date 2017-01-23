#ifdef __cplusplus
extern "C" {
#endif

#pragma once
typedef struct ao_instance_s ao_instance_t;

struct ao_instance_s {
    int (* setup) (ao_instance_t * instance, int sample_rate, int * flags,
		   level_t * level, sample_t * bias);
    int (* play) (ao_instance_t * instance, int flags, sample_t * samples);
    void (* close) (ao_instance_t * instance);
};

typedef ao_instance_t * ao_open_t (void);

typedef struct ao_driver_s {
    char * name;
    ao_open_t * open;
} ao_driver_t;

/* return NULL terminated array of all drivers */
ao_driver_t * ao_drivers (void);

#ifdef __cplusplus
}
#endif
