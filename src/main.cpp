#include "allocator.hpp"
#include <iostream>
using namespace std;
using namespace sys;

struct ModernEntity {
    int id;
    double coordinates[3];
};

int main() {
    cout << "Initializing Pool size: 1024 bytes" << endl << endl;
    
    FixedArenaAllocator mem_pool(1024); //Allocate 1KB
    mem_pool.print_memory_map();

    cout << ">> Allocating object blocks" << endl;
    void* ptr1 = mem_pool.allocate(sizeof(ModernEntity));
    void* ptr2 = mem_pool.allocate(250);

    mem_pool.print_memory_map();

    cout << "<< Deallocating structural components" << endl;
    mem_pool.deallocate(ptr1);
    mem_pool.print_memory_map();

    cout << "<< Deallocating secondary components" << endl;
    mem_pool.deallocate(ptr2);
    mem_pool.print_memory_map();

    return 0;
}
