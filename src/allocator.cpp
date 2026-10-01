#include "allocator.hpp"
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
using namespace std;

namespace sys {
    FixedArenaAllocator::FixedArenaAllocator(size_t pool_size) {
        arena_size = align(pool_size);
        arena_start = mmap(nullptr,arena_size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,-1,0);

        if (arena_start == MAP_FAILED) {
            cerr << "[** CRITICAL] Memory initialization failed." << endl;
            arena_start = nullptr;
            return;
        }

        free_list = reinterpret_cast<BlockHeader*>(arena_start);
        free_list->size = arena_size - sizeof(BlockHeader);
        free_list->is_free = true;
        free_list->next = nullptr;
    }

    FixedArenaAllocator::~FixedArenaAllocator() {
        if (arena_start) munmap(arena_start, arena_size);
    }

    void* FixedArenaAllocator::allocate(size_t size) {
        size_t requested_size = align(size);
        BlockHeader* current = free_list;

        while (current) {
            if (current->is_free && current->size >= requested_size) {
                if (current->size >= requested_size + sizeof(BlockHeader) + sizeof(max_align_t)) {
                    char* next_block_addr = reinterpret_cast<char*>(current) + sizeof(BlockHeader) + requested_size;
                    BlockHeader* split_block = reinterpret_cast<BlockHeader*>(next_block_addr);
                    
                    split_block->size = current->size - requested_size - sizeof(BlockHeader);
                    split_block->is_free = true;
                    split_block->next = current->next;

                    current->size = requested_size;
                    current->next = split_block;
                }

                current->is_free = false;
                return reinterpret_cast<void*>(reinterpret_cast<char*>(current) + sizeof(BlockHeader));
            }
            current = current->next;
        }

        cerr << "[WARNING] Allocator Out of Memory: " << requested_size << " bytes.\n";
        return nullptr;
    }

    void FixedArenaAllocator::deallocate(void* ptr) {
        if (!ptr) return;
        
        BlockHeader* header = reinterpret_cast<BlockHeader*>(reinterpret_cast<char*>(ptr) - sizeof(BlockHeader));
        header->is_free = true;

        BlockHeader* current = free_list;
        while (current) {
            // re-check current position to merge adjacent segments
            if (current->is_free && current->next && current->next->is_free) {
                current->size += sizeof(BlockHeader) + current->next->size;
                current->next = current->next->next;
                continue; 
            }
            current = current->next;
        }
    }

    void FixedArenaAllocator::print_memory_map() const {
        cout << endl << "--- Memory Allocation Map ---" << endl;
        BlockHeader* current = free_list;
        int block_index = 0;
        while (current) {
            cout << "Block [" << block_index++ << "]" << endl 
                << "Header Addr: " << current << endl
                << "Block Capacity: " << current->size << " bytes" << endl
                << "Status: " << (current->is_free ? "FREE" : "ALLOCATED") << endl 
                << "-----------------------------" << endl;
            current = current->next;
        }
        cout << endl;
    }
}
