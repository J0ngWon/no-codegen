#include <string.h>
#include "crashlog.h"
#include "eeplog.h"
#include "i2c1.h"

#define EE_HDR_ADDR     0x0000u
#define EE_REC_BASE     0x0040u

static void uart_puts_str(const char* s);
static void uart_put_hex32(uint32_t v);

static int eelog_load_hdr(eeloghdr_t* hdr)
{
    int r = at24_read(EE_HDR_ADDR, (uint8_t*)hdr, sizeof(*hdr));
    if (r != 0) return r;

    if (hdr->magic != EELOG_MAGIC || hdr->ver != EELOG_VER || hdr->rec_size != sizeof(crashlog_t)) {

        memset(hdr, 0, sizeof(*hdr));
        hdr->magic = EELOG_MAGIC;
        hdr->ver = EELOG_VER;
        hdr->rec_size = sizeof(crashlog_t);
        hdr->max_records = (uint16_t)((0x8000u - EE_REC_BASE) / sizeof(crashlog_t)); // 511
        hdr->write_idx = 0;
        hdr->count = 0;


        at24_write_page(EE_HDR_ADDR, (const uint8_t*)hdr, sizeof(*hdr));
        return 0;
    }
    return 0;
}

static uint16_t eelog_rec_addr(const eeloghdr_t* hdr, uint16_t idx)
{
    return (uint16_t)(EE_REC_BASE + (uint32_t)idx * hdr->rec_size);
}

static int eelog_append_and_verify(eeloghdr_t* hdr, const crashlog_t* rec)
{
    uint16_t addr = eelog_rec_addr(hdr, hdr->write_idx);


    int r = at24_write_page(addr, (const uint8_t*)rec, sizeof(*rec));
    if (r != 0) return r;


    crashlog_t back;
    r = at24_read(addr, (uint8_t*)&back, sizeof(back));
    if (r != 0) return r;

    if (memcmp(&back, rec, sizeof(back)) != 0) {
        return -99;
    }


    hdr->write_idx = (uint16_t)((hdr->write_idx + 1) % hdr->max_records);
    if (hdr->count < hdr->max_records) hdr->count++;

    r = at24_write_page(EE_HDR_ADDR, (const uint8_t*)hdr, sizeof(*hdr));
    return r;
}

void boot_crashlog_flush_to_eeprom(void)
{
    if (!crashlog_valid()) return;

    eeloghdr_t hdr;
    if (eelog_load_hdr(&hdr) != 0) {
        uart_puts_str("EELOG: header load fail\r\n");
        return;
    }

    uart_puts_str("EELOG: crashlog found, append...\r\n");
    int r = eelog_append_and_verify(&hdr, (const crashlog_t*)&g_crashlog);

    if (r == 0) {
        uart_puts_str("EELOG: verify OK\r\n");
        crashlog_clear();
    } else {
        uart_puts_str("EELOG: verify FAIL, r=0x");
        uart_put_hex32((uint32_t)r);
        uart_puts_str("\r\n");

    }
}

void boot_dump_last_crash(void)
{
    eeloghdr_t hdr;
    if (eelog_load_hdr(&hdr) != 0 || hdr.count == 0) {
        uart_puts_str("EELOG: empty\r\n");
        return;
    }

    uint16_t last = (uint16_t)((hdr.write_idx + hdr.max_records - 1) % hdr.max_records);
    uint16_t addr = eelog_rec_addr(&hdr, last);

    crashlog_t rec;
    if (at24_read(addr, (uint8_t*)&rec, sizeof(rec)) != 0) {
        uart_puts_str("EELOG: read fail\r\n");
        return;
    }

    uart_puts_str("=== EELOG LAST ===\r\n");
    uart_puts_str("idx : 0x"); uart_put_hex32(last); uart_puts_str("\r\n");
    uart_puts_str("VTOR: 0x"); uart_put_hex32(rec.vtor); uart_puts_str("\r\n");
    uart_puts_str("HFSR: 0x"); uart_put_hex32(rec.hfsr); uart_puts_str("\r\n");
    uart_puts_str("CFSR: 0x"); uart_put_hex32(rec.cfsr); uart_puts_str("\r\n");
    uart_puts_str("MMFAR:0x"); uart_put_hex32(rec.mmfar); uart_puts_str("\r\n");
    uart_puts_str("BFAR: 0x"); uart_put_hex32(rec.bfar); uart_puts_str("\r\n");
}
