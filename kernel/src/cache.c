#define CACHE_INST                      (0)
#define CACHE_INST_SIZE                 (16 * 1024)
#define CACHE_INST_LINESIZE             (32)
#define CACHE_DATA                      (1)
#define CACHE_DATA_SIZE                 (8 * 1024)
#define CACHE_DATA_LINESIZE             (16)
// Cache ops described in manual on page 404
#define INDEX_INVALIDATE                (0 << 2)
#define INDEX_LOAD_TAG                  (1 << 2)
#define INDEX_STORE_TAG                 (2 << 2)
#define INDEX_CREATE_DIRTY              (3 << 2)
#define HIT_INVALIDATE                  (4 << 2)
#define HIT_WRITEBACK_INVALIDATE        (5 << 2)
#define HIT_WRITEBACK                   (6 << 2)

#define BUILD_CACHE_OP(o,c)             ((o) | (c))

#define cache_op(addr, op, linesize, length) ({ \
    { \
        void* cur = (void*) ((unsigned long) addr & ~((linesize) - 1)); \
        int count = (int) ((length) + ((addr) - cur)); \
        for (int i = 0; i < count; i += linesize) \
            { asm volatile ("\tcache %0,(%1)\n"::"i" (op), "r" (cur + i)); } \
    } \
})

void data_cache_index_writeback_invalidate(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(INDEX_INVALIDATE, CACHE_DATA), CACHE_DATA_LINESIZE, length);
}

void data_cache_hit_invalidate(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(HIT_INVALIDATE, CACHE_DATA), CACHE_DATA_LINESIZE, length);
}

void data_cache_hit_writeback_invalidate(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(HIT_WRITEBACK_INVALIDATE, CACHE_DATA), CACHE_DATA_LINESIZE, length);
}

void data_cache_hit_writeback(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(HIT_WRITEBACK, CACHE_DATA), CACHE_DATA_LINESIZE, length);
}

void inst_cache_index_invalidate(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(INDEX_INVALIDATE, CACHE_INST), CACHE_INST_LINESIZE, length);
}

void inst_cache_hit_invalidate(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(HIT_INVALIDATE, CACHE_INST), CACHE_INST_LINESIZE, length);
}

void inst_cache_hit_writeback(volatile void* addr, unsigned long length)
{
    cache_op(addr, BUILD_CACHE_OP(HIT_WRITEBACK, CACHE_INST), CACHE_INST_LINESIZE, length);
}
