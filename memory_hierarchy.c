/*************************************************************************************|
|   1. YOU ARE NOT ALLOWED TO SHARE/PUBLISH YOUR CODE (e.g., post on piazza or online)|
|   2. Fill main.c and memory_hierarchy.c files                                       |
|   3. Do not use any other .c files neither alter main.h or parser.h                 |
|   4. Do not include any other library files                                         |
|*************************************************************************************/
#include "mipssim.h"

/// @students: declare cache-related structures and variables here

struct cache_block {
    int tag;   // tag bits that tell you if its the correct data
    int idx;   //index bits
    int valid; // valid bit either 1 or 0
    int data[4];   // data duuh
};

int address_size = 32;
int address_offset= 4;
int address_idx = 0;
int address_tag = 0;
int numLines = 0;
struct cache_block *cache;
struct cache_block block;

void memory_state_init(struct architectural_state* arch_state_ptr) {
    arch_state_ptr->memory = (uint32_t *) malloc(sizeof(uint32_t) * MEMORY_WORD_NUM);
    memset(arch_state_ptr->memory, 0, sizeof(uint32_t) * MEMORY_WORD_NUM);

    if(cache_size == 0){
        // CACHE DISABLED
        memory_stats_init(arch_state_ptr, 0); // WARNING: we initialize for no cache 0
    }
    else if (cache_size >=16) {
        // CACHE ENABLED
        // if(cache_size%16!=0){
        //   assert(0);
        // }
        numLines = (int)(cache_size / 16);
        address_idx = (int) ceil(log2(numLines));
        address_tag = address_size - address_idx - address_offset;
        printf("Cache Address tag size = %u, idx size = %u, offset size = %u\n", address_tag,address_idx,address_offset);
        //malloc would be used here to create the cache itself
        //create cache
        //cache1 = (cache) malloc( sizeof( struct cache ) );
        int ram_aloc = sizeof(block) * numLines;
        printf("Ram used :%u\n",ram_aloc);
        cache =  malloc(ram_aloc);
        assert(cache != NULL);
        for(int i = 0; i < numLines; i++)
        {
           cache[i].valid = 0;
           cache[i].idx = 0;
           cache[i].tag = 0;
           printf("Valid Cache: %u\n",cache[i].valid);
        }
        memory_stats_init(arch_state_ptr, address_tag);
        /// @students: memory_stats_init(arch_state_ptr, X); <-- fill # of tag bits for cache 'X' correctly
    }
    else {
      assert(0);
    }

}

// returns data on memory[address / 4]
int memory_read (int address){

    arch_state.mem_stats.lw_total++;
    check_address_is_word_aligned(address);

    if(cache_size == 0){
        // CACHE DISABLED
        return (int) arch_state.memory[address / 4];
    }else{
        // CACHE ENABLED#
        //printf("Address= %i \n",address );

        int offset = (get_piece_of_a_word(address, 0, address_offset)/4);
        int idx = get_piece_of_a_word(address,address_offset,address_idx);
        int tag = get_piece_of_a_word(address,address_offset+address_idx,address_tag);
        //printf("Read: acc tag: %i, acc idx: %i, acc offset: %i\n",tag,idx,offset );

         if(cache[idx].valid == 0 || cache[idx].tag != tag ){
           //printf("Miss Read\n");
            cache[idx].valid = 1;
            cache[idx].tag = tag;


            for(int j=0; j<4;j++){
              cache[idx].data[j] =  (int) arch_state.memory[(address / 4)-(offset)+j];
              //printf("cache boi %i\n",cache[idx].data[j]);
            }
            //printf("return boi %i\n",cache[idx].data[offset]);
            return cache[idx].data[offset];
          }


          else if((cache[idx].valid == 1) && (cache[idx].tag == tag)){
            arch_state.mem_stats.lw_cache_hits++;
            //printf("Hit Read\n");
            return cache[idx].data[offset];
          }
          else{
            assert(0);
          }
        /// @students: your implementation must properly increment: arch_state_ptr->mem_stats.lw_cache_hits
    }
    //return 0;
}

// writes data on memory[address / 4]
void memory_write(int address, int write_data){
    //printf("Address= %i \n",address );
    arch_state.mem_stats.sw_total++;
    check_address_is_word_aligned(address);
    int offset = (get_piece_of_a_word(address, 0, address_offset)/4);
    int idx = get_piece_of_a_word(address,address_offset,address_idx);
    int tag = get_piece_of_a_word(address,address_offset+address_idx,address_tag);
    printf("Write: acc tag: %i, acc idx: %i, acc offset: %i\n",tag,idx,offset );


    if(cache_size == 0){
        // CACHE DISABLED
        arch_state.memory[address / 4] = (uint32_t) write_data;
    }else{
        // CACHE ENABLED

        if((cache[idx].tag == tag) && cache[idx].valid ==1){
          arch_state.mem_stats.sw_cache_hits++;
          printf("Hit Write\n");
          cache[idx].valid =1 ;
          arch_state.memory[address / 4] = (uint32_t) write_data;
          int j=0;
          cache[idx].data[j] =  (int) arch_state.memory[(address / 4)-(offset)+j];
            //printf("cache boi %i\n",cache[idx].data[j]);


        }else{

        arch_state.memory[address / 4] = (uint32_t) write_data;
        printf("Miss Write\n");
        }
        /// @students: your implementation must properly increment: arch_state_ptr->mem_stats.sw_cache_hits
    }
}
