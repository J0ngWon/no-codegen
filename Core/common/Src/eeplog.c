#include <string.h>
#include "crashlog.h"
#include "eeprom.h"
#include "i2c1.h"
#include "uart6.h"

#define EE_HDR_ADDR     0x0000u
#define EE_REC_BASE     0x0040u

static int eeloghdr_load(eeloghdr_t* hdr)
{
    int r = at24c256_read(EE_HDR_ADDR, (uint8_t*)hdr, sizeof(*hdr));
    if (r != 0) return r;

    if (hdr->magic != EELOG_MAGIC || hdr->ver != EELOG_VER || hdr->rec_size != sizeof(crashlog_t)) {

        memset(hdr, 0, sizeof(*hdr));
        hdr->magic = EELOG_MAGIC;
        hdr->ver = EELOG_VER;
        hdr->rec_size = sizeof(crashlog_t);
        hdr->max_records = (uint16_t)((0x7F80u - EE_REC_BASE) / sizeof(crashlog_t)); // 511
        hdr->write_idx = 0;
        hdr->count = 0;


       at24c256_write(EE_HDR_ADDR, (const uint8_t*)hdr, sizeof(*hdr));
        return 0;
    }
    return 0;
}

static uint16_t eelog_rec_addr(const eeloghdr_t* hdr, uint16_t idx)
{
    return (uint16_t)(EE_REC_BASE + (uint32_t)idx * hdr->rec_size);
}

static int eelog_append(eeloghdr_t* hdr, const crashlog_t* rec)
{
    uint16_t addr = eelog_rec_addr(hdr, hdr->write_idx);


    int r =at24c256_write(addr, (const uint8_t*)rec, sizeof(*rec));
    if (r != 0) return r;


    crashlog_t back;
    r = at24c256_read(addr, (uint8_t*)&back, sizeof(back));
    if (r != 0) return r;

    if (memcmp(&back, rec, sizeof(back)) != 0) {
        return -3;
    }


    hdr->write_idx = (uint16_t)((hdr->write_idx + 1) % hdr->max_records);
    if (hdr->count < hdr->max_records) hdr->count++;

    r =at24c256_write(EE_HDR_ADDR, (const uint8_t*)hdr, sizeof(*hdr));
    return r;
}

void boot_eeprom(void)
{
    if (!crashlog_valid()) return;

    eeloghdr_t hdr;
    if (eeloghdr_load(&hdr) != 0) {
        uart_puts("EEPLOG: header load fail\r\n");
        return;
    }

    uart_puts("EEPLOG: crashlog found, append...\r\n");
    int r = eelog_append(&hdr, (const crashlog_t*)&g_crashlog);

    if (r == 0) {
        uart_puts("EEPLOG: verify OK\r\n");
        crashlog_clear();
    } else {
        uart_puts("EEPLOG: verify FAIL, r=0x");
        uart_put_hex32((uint32_t)r);
        uart_puts("\r\n");

    }
}

void boot_dump_last_crash(void)
{
    eeloghdr_t hdr;
    if (eelog_load_hdr(&hdr) != 0 || hdr.count == 0) {
        uart_puts("EELOG: empty\r\n");
        return;
    }

    uint16_t last = (uint16_t)((hdr.write_idx + hdr.max_records - 1) % hdr.max_records);
    uint16_t addr = eelog_rec_addr(&hdr, last);

    crashlog_t rec;
    if (at24c256_read(addr, (uint8_t*)&rec, sizeof(rec)) != 0) {
        uart_puts("EELOG: read fail\r\n");
        return;
    }

    uart_puts("=== EELOG LAST ===\r\n");
    uart_puts("idx : 0x"); uart_put_hex32(last); uart_puts("\r\n");
    uart_puts("VTOR: 0x"); uart_put_hex32(rec.vtor); uart_puts("\r\n");
    uart_puts("HFSR: 0x"); uart_put_hex32(rec.hfsr); uart_puts("\r\n");
    uart_puts("CFSR: 0x"); uart_put_hex32(rec.cfsr); uart_puts("\r\n");
    uart_puts("MMFAR:0x"); uart_put_hex32(rec.mmfar); uart_puts("\r\n");
    uart_puts("BFAR: 0x"); uart_put_hex32(rec.bfar); uart_puts("\r\n");
}


