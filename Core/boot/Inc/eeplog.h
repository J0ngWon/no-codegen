#pragma once
#include <stdint.h>

#define EELOG_MAGIC 0x45454C47u // 'EELG'
#define EELOG_VER   1u

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t ver;
    uint16_t rec_size;     // 64
    uint16_t max_records;
    uint16_t write_idx;
    uint16_t count;
    uint16_t rsv;
    uint32_t rsv2[12];     // 64B
} eeloghdr_t;

typedef char eeloghdr_size_check[(sizeof(eeloghdr_t) == 64) ? 1 : -1];
