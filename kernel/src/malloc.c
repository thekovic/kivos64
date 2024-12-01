#include "intdef.h"
#include "system.h"

#define ONE_MB                  (1024 * 1024)
#define RAM_SIZE                (ONE_MB * 8)
#define KERNEL_HEAP_START       (ADDR_TO_KSEG0(1 * ONE_MB))
#define KERNEL_HEAP_END         (ADDR_TO_KSEG0(RAM_SIZE - ONE_MB))

_Static_assert(KERNEL_HEAP_START == 0x80100000, "KERNEL_HEAP_START != 0x80100000");

typedef struct block_info_s
{
    struct block_info_s* prev;
    struct block_info_s* next;
    bool is_free;
    int size;
} block_info_t;

block_info_t* mem_block_list_head;

void malloc_init(void)
{
    mem_block_list_head = (block_info_t*) KERNEL_HEAP_START;
    mem_block_list_head->prev = NULL;
    mem_block_list_head->next = NULL;
    mem_block_list_head->is_free = true;
    mem_block_list_head->size = KERNEL_HEAP_END - KERNEL_HEAP_START - sizeof(block_info_t);
}

void free(void* first_byte)
{
    
}

static block_info_t* find_free_block(size_t numbytes)
{
    for (block_info_t* block = mem_block_list_head; ; block = block->next)
    {
        if (block->is_free && block->size >= (numbytes + sizeof(block_info_t)))
        {
            return block;
        }

        if (block->next == NULL)
        {
            break;
        }
    }

    return NULL;
}

void* malloc(size_t numbytes)
{
    if (numbytes == 0)
    {
        return NULL;
    }

    size_t alignment_offset = (numbytes % 16);
    size_t numbytes_aligned = (alignment_offset != 0) ? numbytes + (16 - alignment_offset) : numbytes;

    block_info_t* block = find_free_block(numbytes_aligned);
    if (!block)
    {
        return NULL;
    }

    void* allocated_mem = ((void*) block) + sizeof(block_info_t);

    block_info_t* new_block = (block_info_t*) (allocated_mem + numbytes_aligned);
    new_block->prev = NULL;
    new_block->next = block;
    new_block->is_free = true;
    new_block->size = block->size - numbytes_aligned - sizeof(block_info_t);

    block->prev = new_block;
    block->size = numbytes_aligned;

    return allocated_mem;
}

void* malloc_uncached(size_t numbytes)
{
    uint32_t mem_uncached = ADDR_TO_KSEG1((uint32_t) malloc(numbytes));
    return (void*) mem_uncached;
}
