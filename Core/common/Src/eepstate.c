#include <string.h>
#include "eeprom.h"
#include "uart6.h"

extern int at24c256_read(uint16_t mem, uint8_t* out, uint16_t len);
extern int at24c256_write(uint16_t mem, const uint8_t* data, uint16_t len);

static uint32_t calc_xor(const eestate_t* s)
{
    const uint32_t* p = (const uint32_t*)s;
    uint32_t x = 0;
    // -1 : eestate_t->check[xor]
    for (int i = 0; i < (EESTATE_SIZE/4 - 1); i++) x ^= p[i];
    return x;
}

static int is_valid(const eestate_t* s)
{
    if (s->magic != EESTATE_MAGIC) return 0;
    if (s->ver   != EESTATE_VER)   return 0;
    if (s->size  != EESTATE_SIZE)  return 0;
    if (s->check != calc_xor(s))   return 0;
    return 1;
}

static void set_default(eestate_t* s)
{
    memset(s, 0, sizeof(*s));
    s->magic = EESTATE_MAGIC;
    s->ver   = EESTATE_VER;
    s->size  = EESTATE_SIZE;
    s->seq   = 0;
    s->flags = 0;
    s->fail_streak = 0;
    s->threshold   = 0;
    s->boot_count  = 0;
    s->last_crash_seq = 0;
    s->check = calc_xor(s);
}

int eestate_load(eestate_t* out)
{
    eestate_t a, b;
    int ra = at24c256_read(EESTATE_SLOT0_ADDR, (uint8_t*)&a, sizeof(a));
    int rb = at24c256_read(EESTATE_SLOT1_ADDR, (uint8_t*)&b, sizeof(b));

    int va = (ra == 0) && is_valid(&a);
    int vb = (rb == 0) && is_valid(&b);

    if (!va && !vb) {
        set_default(out);
        return 0;
    }
    if (va && !vb) { *out = a; return 0; }
    if (!va && vb) { *out = b; return 0; }

    *out = (a.seq >= b.seq) ? a : b;
    return 0;
}

int eestate_store_next(const eestate_t* in)
{
    eestate_t now;
    eestate_load(&now);

    eestate_t next = *in;
    next.magic = EESTATE_MAGIC;
    next.ver   = EESTATE_VER;
    next.size  = EESTATE_SIZE;
    next.seq   = now.seq + 1;
    next.check = calc_xor(&next);

    // 0,2,4... slot0, 1,3,5... slot1  )
    uint16_t addr = (next.seq & 1u) ? EESTATE_SLOT1_ADDR : EESTATE_SLOT0_ADDR;
    return at24c256_write(addr, (const uint8_t*)&next, sizeof(next));
}

int eestate_boot_step(uint16_t threshold, int crash, uint32_t crash_seq, int* out_enter_recovery)
{
    eestate_t s;
    eestate_load(&s);

    // if(recovery) -> return
    if (s.flags & EESTATE_RECOVERY) {
        if (out_enter_recovery) *out_enter_recovery = 1;
        return 0;
    }

    // if(pending) -> ++
    if (s.flags & EESTATE_PENDING) {
        s.fail_streak++;
    } else {
        s.fail_streak = 0;
    }

    s.boot_count++;
    s.threshold = threshold;

    if (crash) s.last_crash_seq = crash_seq;

    // enter recovery
    if (s.fail_streak >= threshold) {
        s.flags |= EESTATE_RECOVERY;
        s.flags &= ~EESTATE_PENDING; // recovery에서는 pending 의미 없음
        eestate_store_next(&s);
        if (out_enter_recovery) *out_enter_recovery = 1;
        return 0;
    }

    // if(!recovery) -> trial(pending=1)
    s.flags |= EESTATE_PENDING;
    eestate_store_next(&s);

    if (out_enter_recovery) *out_enter_recovery = 0;
    return 0;
}

int eestate_confirm_ok(void)
{
    eestate_t s;
    eestate_load(&s);

    s.flags &= ~EESTATE_PENDING;
    s.flags &= ~EESTATE_RECOVERY;
    s.fail_streak = 0;

    return eestate_store_next(&s);
}

int eestate_clear_recovery(void)
{
    eestate_t s;
    eestate_load(&s);

    s.flags &= ~EESTATE_RECOVERY;
    s.fail_streak = 0;

    return eestate_store_next(&s);
}
