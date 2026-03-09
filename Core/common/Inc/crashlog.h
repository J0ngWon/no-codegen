#pragma once
#include <stdint.h>

#define CRASHLOG_MAGIC  (0x434C4F47u) // 'CLOG'
#define CRASHLOG_VER    (1u)

typedef struct {
    uint32_t magic;
    uint32_t ver;
    uint32_t seq;

    //SCB
    uint32_t vtor;
    uint32_t hfsr;
    uint32_t cfsr;
    uint32_t shcsr;
    uint32_t mmfar;
    uint32_t bfar;
    uint32_t afsr;

    uint32_t msp;
    uint32_t psp;
    uint32_t control;

    uint32_t reserved[3];
} crashlog_t;

typedef char crashlog_size_check[(sizeof(crashlog_t) == 64) ? 1 : -1];

//.noinit
extern volatile crashlog_t g_crashlog;

static inline int crashlog_valid(void) {
    return (g_crashlog.magic == CRASHLOG_MAGIC) &&
           (g_crashlog.ver   == CRASHLOG_VER);
}

static inline void crashlog_clear(void) {
    g_crashlog.magic = 0;
    g_crashlog.ver   = 0;
}


