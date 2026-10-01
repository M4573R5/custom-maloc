#pragma once

#include <cstddef>
using namespace std;
namespace sys {

    struct BlockHeader {
        size_t size;        // Size of the usable data block
        bool is_free;       // Flag tracking block usage
        BlockHeader* next;  // Pointer tracking the next chunk in our memory map
    };

    class FixedArenaAllocator {
    private:
        void* arena_start{nullptr};   // Start boundary pointer of total mapped memory block
        size_t arena_size{0};         // Total allocated pool ceiling size
        BlockHeader* free_list{nullptr}; // Pointer heading our tracking linked list

        //alignment offsets
        size_t align(size_t size) const {
            return (size + sizeof(max_align_t) - 1) & ~(sizeof(max_align_t) - 1);
        }

    public:
        explicit FixedArenaAllocator(size_t pool_size);
        ~FixedArenaAllocator();

        FixedArenaAllocator(const FixedArenaAllocator&) = delete;
        FixedArenaAllocator& operator=(const FixedArenaAllocator&) = delete;

        void* allocate(size_t size);
        void deallocate(void* ptr);
        void print_memory_map() const;
    };
}
