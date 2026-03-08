#pragma once
#include <stdint.h>

#define EELOG_MAGIC 0x45454C47u // 'EELG'
#define EELOG_VER   1u

#define EE_SIZE_BYTES   (0x8000u)
#define EE_PAGE_SIZE    (64u)

#define EESTATE_MAGIC  (0x45535441u) // 'ESTA'
#define EESTATE_VER    (1u)

#define EESTATE_SLOT0_ADDR (0x7F80u)
#define EESTATE_SLOT1_ADDR (0x7FC0u)
#define EESTATE_SIZE       (64u)

// flags
#define EESTATE_PENDING  (1u << 0)  // trial boot in progress
#define EESTATE_RECOVERY (1u << 1)  // recovery lock (선택)

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

void boot_eeprom(void);
void boot_dump_last_crash(void);

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t ver;
    uint16_t size;         // 64

    uint32_t seq;
    uint32_t flags;        // pending/recovery

    uint16_t fail_streak;
    uint16_t threshold;

    uint32_t boot_count;
    uint32_t last_crash_seq;

    uint32_t reserved[8];
    uint32_t check;         // XOR check
} eestate_t;

typedef char eestate_size_check[(sizeof(eestate_t) == EESTATE_SIZE) ? 1 : -1];


int  eestate_load(eestate_t* out);
int  eestate_store_next(const eestate_t* in);
int  eestate_boot_step(uint16_t threshold, int crash_present, uint32_t crash_seq, int* out_enter_recovery);
int  eestate_confirm_ok(void);
int  eestate_clear_recovery(void);


