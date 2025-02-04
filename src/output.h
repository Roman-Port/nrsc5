#pragma once

#include "config.h"

#include <nrsc5.h>

#ifdef HAVE_FAAD2
#include <neaacdec.h>
#endif

#define AUDIO_FRAME_BYTES 8192
#define MAX_PORTS 32
#define MAX_SIG_SERVICES 8
#define MAX_SIG_COMPONENTS 8
#define MAX_LOT_FILES 8
#define LOT_FRAGMENT_SIZE 256
#define MAX_FILE_BYTES 65536
#define MAX_LOT_FRAGMENTS (MAX_FILE_BYTES / LOT_FRAGMENT_SIZE)
#define MAX_STREAM_BYTES 65543

#define AAS_TYPE_STREAM 0
#define AAS_TYPE_PACKET 1
#define AAS_TYPE_LOT    3

enum
{
    SIG_COMPONENT_NONE,
    SIG_COMPONENT_DATA,
    SIG_COMPONENT_AUDIO
};

enum
{
    SIG_SERVICE_NONE,
    SIG_SERVICE_DATA,
    SIG_SERVICE_AUDIO
};

typedef struct
{
    uint16_t port;
    uint8_t type;
    unsigned int service_number;
    uint32_t mime;

    union
    {
        struct
        {
            uint8_t prev[3];
            uint8_t type;
            uint16_t size;
            uint8_t *data;
            unsigned int idx;
        } stream;
        struct
        {
            nrsc5_aas_file_t files[MAX_LOT_FILES];
        } lot;
    };
} aas_port_t;

typedef struct
{
    uint8_t type;
    uint8_t id;

    union
    {
        struct {
            uint16_t port;
            uint16_t service_data_type;
            uint8_t type;
            uint32_t mime;
        } data;
        struct {
            uint8_t port;
            uint8_t type;
            uint32_t mime;
        } audio;
    };
} sig_component_t;

typedef struct
{
    uint8_t type;
    uint16_t number;
    char *name;

    sig_component_t component[MAX_SIG_COMPONENTS];
} sig_service_t;

typedef struct
{
    nrsc5_t *radio;
#ifdef HAVE_FAAD2
    NeAACDecHandle aacdec[MAX_PROGRAMS];
#endif
    aas_port_t ports[MAX_PORTS];
    sig_service_t services[MAX_SIG_SERVICES];
} output_t;

void output_push(output_t *st, uint8_t *pkt, unsigned int len, unsigned int program, unsigned int stream_id);
void output_begin(output_t *st);
void output_reset(output_t *st);
void output_init(output_t *st, nrsc5_t *);
void output_free(output_t *st);
void output_aas_push(output_t *st, uint8_t *psd, unsigned int len);

// Finds the file by the specified port and lot ID. Returns NULL if failed.
nrsc5_aas_file_t* output_find_port_and_lot(output_t* st, uint16_t port_id, uint16_t lot_id);