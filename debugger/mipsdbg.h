#ifndef __GDB_H
#define __GDB_H

// change this to one for the debugger to run on every clock cycle

#define EVERY_CLOCK 0

struct architectural_state;
extern void *_db_cache;

void __gdb(struct architectural_state* arch);

#endif
